//
// HAPServer.cpp
// Homekit
//
//  Created on: 08.08.2017
//      Author: michael
//

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include "HAPBonjour.hpp"

#if HAP_WIFI_MODE_DEFAULT == 1
#include "../WiFiCredentials.hpp"
#endif

#if HAP_DEBUG
#include "esp_system.h"
#endif

#elif defined( CORE_TEENSY )
#include <Entropy.h>
#endif

#include <string>
#include <algorithm>
#include "HAPServer.hpp"
#include "HAPLogging.hpp"
#include "HAPHelper.hpp"

#include "HAPDeviceID.hpp"

#if HAP_ENABLE_WIFI_BUTTON
#include "HAPButton.hpp"
#endif

#include "HAPEncryption.hpp"



#include "concat.h"

#if HAP_SERVER_USE_INTEGRATED_KNX
#include <knx.h>
#include "plugins/HAPPluginKNX/HAPPluginKNXMemory.hpp"
#endif



#include <mbedtls/version.h>
#include <mbedtls/entropy_poll.h>
#include "m_X25519.h"
#include "m_ed25519.h"
#include "m_chacha20_poly1305.h"
#include "m_hkdf.h"

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)


#ifndef HAP_HEAP_LOG_INTERVAL
#define HAP_HEAP_LOG_INTERVAL 1000
#endif


#if HAP_PRINT_QRCODE
#include "qrcode.h"
#include "HAPSVG.hpp"
#endif


#include "HAPPrint.hpp"


static const char HTTP_200[]  					= "HTTP/1.1 200 OK\r\n";
static const char HTTP_204[]  					= "HTTP/1.1 204 No Content\r\n\r\n";
static const char HTTP_207[]  					= "HTTP/1.1 207 Multi-Status\r\n";
static const char HTTP_400[]  					= "HTTP/1.1 400 Bad Request\r\n";

static const char HTTP_CONTENT_TYPE_HAPJSON[]  	= "Content-Type: application/hap+json\r\n";
static const char HTTP_CONTENT_TYPE_TLV8[]  	= "Content-Type: application/pairing+tlv8\r\n";

static const char HTTP_KEEP_ALIVE[] 		 	= "Connection: keep-alive\r\n";
static const char HTTP_TRANSFER_ENCODING[] 		= "Transfer-Encoding: chunked\r\n";

static const char HTTP_CRLF[]  					= "\r\n";
static const char EVENT_200[]  					= "EVENT/1.0 200 OK\r\n";



//
// init static variables
//
EventManager HAPServer::_eventManager;
bool HAPServer::_isConnected = false;

#if defined(CORE_TEENSY)
HAP_MDNS_TXT HAPServer::_hapMdnsTxt;
#endif

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPServer::HAPServer(uint16_t port, uint8_t maxClients)
:  _server(port)
, __HOMEKIT_SIGNATURE("\x25\x48\x4f\x4d\x45\x4b\x49\x54\x5f\x45\x53\x50\x33\x32\x5f\x46\x57\x25")
{
	_port = port;


#if HAP_DEBUG
	_previousMillisHeap = 0;

#endif

	_accessorySet = new HAPAccessorySet();

#if HAP_ENABLE_WEBSERVER
	_webserver = new HAPWebServer();
	_webserver->setAccessorySet(_accessorySet);
#endif

	_hapsrp = nullptr;

	_homekitStarted = false;
	_isInPairingMode = false;
	_homekitFailedLoginAttempts = 0;

#if HAP_ENABLE_WIFI_BUTTON
	_taskButtonHandle = NULL;
#endif
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPServer::~HAPServer() {
	// TODO Auto-generated destructor stub
	delete _accessorySet;

#if HAP_ENABLE_WEBSERVER
	delete _webserver;
#endif

	if (_hapsrp) delete _hapsrp;

}



#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::begin(bool resume) {
	LOG_V("Begin\n");
	// Generate DeviceID
	uint8_t *baseMac = HAPDeviceID::mac();

	if (resume == false){

		LOG_N("Starting %s v%s [%s] %s\n", HOMEKIT_PROJECT, hap.versionString(), HOMEKIT_GIT_REV, HOMEKIT_GIT_BRANCH);
		LOG_N("compiled at %s\n", HOMEKIT_COMPILE_TIME);
		LOG_N("Feature Rev.: %s\n", HOMEKIT_FEATURE_REV);
		LOG_N("Loglevel: %d\n", LOG_LEVEL);

		LOG_DESC();

#if defined(CORE_TEENSY)
		LOG_I("Initialize entropy ...");
		Entropy.Initialize();
		LOGRAW_I("OK\n");
#endif

		//
		// Configuration
		//
		LOG_I("Setup configuration ...");
		auto callback = std::bind(&HAPServer::updateConfig, this);
		_configuration.registerCallbackUpdate(callback);
		_configuration.begin();
		LOGRAW_I("OK\n");


#if HAP_RESET_CONFIGURATION

		LOG_I("Reseting configuration ...");
		_configuration.reset();
		_configuration.getAccessoryConfig()->save();
		LOGRAW_I("OK\n");
#endif

#if HAP_RESET_PAIRINGS
		LOG_I("Reseting pairings ...");
		_configuration.getAccessoryConfig()->clearPairings();
		_configuration.getAccessoryConfig()->save();
		LOGRAW_I("OK\n");
#endif

		LOG_I("Loading configuration ...");
		if (_configuration.load() == false){
			LOG_E("ERROR: Could not load configuration! -> Setting defaults\n");
			// _configuration.formatFlash();

			_configuration.setDefaults();
			_configuration.save();
		} else {
			LOGRAW_I("OK\n");
		}

#if HAP_DEBUG
		_configuration.toJson(*LOGDEVICE);
		LOGDEVICE->println();
#endif

		_accessorySet->setConfiguration(_configuration.getAccessoryConfig());


#if HAP_ENABLE_PIXEL_INDICATOR
		// ToDo: Pixel Indicator
		_pixelIndicator.begin();
		// // show pixel indicator color
		// LOGDEVICE->print("WIFI MODE: ");
		// LOGDEVICE->println(_config.config()["wifi"]["mode"].as<uint8_t>());
		_pixelIndicator.setColor(_wifi.getColorForMode((enum HAP_WIFI_MODE)_configuration.getWiFiConfig()->mode));
#endif

#if HAP_ENABLE_WIFI_BUTTON
		// Button Task
		xTaskCreatePinnedToCore(
			taskButtonRead
			,  "TaskButton"   		// A name just for humans
			,  2048  		  		// This stack size can be checked & adjusted
			,  (void*)this			// parameter of the task
			,  0  					// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
			,  &_taskButtonHandle 	// task handle
			,  CONFIG_ARDUINO_RUNNING_CORE == 0 ? 1 : 0
			);
#endif

#if HAP_DEBUG

		LOG_D("Device Information:\n");
		char deviceId[18] = {0, };
		HAPDeviceID::deviceID(deviceId);

		char chipId[18] = {0, };
		HAPDeviceID::chipID(chipId);

		LOG_D("   %-14s: %s\n", "Device ID", deviceId);
		LOG_D("   %-14s: %s\n", "Chip ID", chipId);
		LOG_D("   %-14s: %s\n", "Endian", IS_BIG_ENDIAN ? "BIG" : "little" );


		LOG_D("Versions:\n");
		LOG_D("   %-14s: %.2f\n", "Teensyduino", TEENSYDUINO / 100.0f);

		char mbedtlsVersion[32];
		mbedtls_version_get_string_full(mbedtlsVersion);
		LOG_D("   %-14s: %s\n", "mbedTLS", mbedtlsVersion);

		LOG_D("Fakegato:\n");
		LOG_D("   %-14s: %d\n", "Interval", HAP_FAKEGATO_INTERVAL );
		LOG_D("   %-14s: %d\n", "Size", HAP_FAKEGATO_BUFFER_SIZE );

		LOG_D("Storage:\n");
#if HAP_USE_EEPROM
		LOG_D("   %-14s: %s\n", "Type", "EEPROM" );
#elif HAP_USE_PREFERENCES
		LOG_D("   %-14s: %s\n", "Type", "Preferences" );
#elif HAP_USE_SPIFFS_CONFIGURATION
		LOG_D("   %-14s: %s\n", "Type", "SPIFFS (External Flash)" );
#endif


		// LogD(F("\nDevice Information"), true);
		// LogD(F("==================================================="), true);
		// LogD(F("Device ID:    ") + HAPDeviceID::deviceID(), true);
		// LogD(F("Chip ID:      ") + HAPDeviceID::chipID(), true);


		// char mbedtlsVersion[32];
		// mbedtls_version_get_string_full(mbedtlsVersion);

		// LogD("", true);
// 		LogD(F("Versions:"), true);
// #if defined(ARDUINO_ARCH_ESP32)
// 		LogD("   SDK:       " + String(ESP.getSdkVersion()), true);
// #endif
// 		LogD(F("   mbedtls:   ") + String(mbedtlsVersion), true);


#if defined(ARDUINO_ARCH_ESP32)
		LogD("", true);
		LogD("MAC address:  " + WiFi.macAddress(), true);


		esp_chip_info_t chip_info;
		esp_chip_info(&chip_info);

		LogD("", true);
		LogD("ESP32:", true);
		LogD("   features:  " + String(chip_info.features), true);
		LogD("   cores:     " + String(chip_info.cores), true);
		LogD("   revision:  " + String(chip_info.revision), true);

		LogD("", true);
		LogD("Flash chip:", true);
		LogD("   size:      " + String(ESP.getFlashChipSize()), true);
		LogD("   speed:     " + String(ESP.getFlashChipSpeed()), true);
		LogD("   mode:      " + String(ESP.getFlashChipMode()), true);

		LogD("Main stack:   ", false);
		LogD(String(CONFIG_MAIN_TASK_STACK_SIZE), true);
#endif

// 		LOGRAW_D("OK\n");
// 		LogD(F("Endian:       "), false);
// 		LogD(IS_BIG_ENDIAN ? F("BIG") : F("little"), true);
// 		LOGRAW_D("OK\n");

// 		LogD(F("Storage:"), true);
// 		LogD(F("   type:      "), false);
// #if HAP_USE_EEPROM
// 		LogD(F("EEPROM"), true);
// #elif HAP_USE_PREFERENCES
// 		LogD(F("Preferences"), true);
// #elif HAP_USE_SPIFFS_CONFIGURATION
// 		LogD(F("SPIFFS (External Flash)"), true);
// #endif
// 		LOGRAW_D("OK\n");

// 		LogD(F("Fakegato:"), true);
// 		LogD(F("   interval:  "), false);
// 		LogD(String(HAP_FAKEGATO_INTERVAL), true);
// 		LogD(F("   size:      "), false);
// 		LogD(String(HAP_FAKEGATO_BUFFER_SIZE), true);

// #if defined (CORE_TEENSY)
// 		HAPLogger::printTeensyInfo();
// #endif
		// LogD(F("==================================================="), true);

#if defined(ARDUINO_ARCH_ESP32)
		HAPHelper::getPartionTableInfo();
#endif

#endif

#if HAP_RESET_EEPROM
		LOG_I("Reset EEPROM - Delete pairings ...");
		_accessorySet->getPairings()->resetEEPROM();
		LOGRAW_I("OK\n");
#endif

#if HAP_WEBSERVER_USE_SPIFFS
		SPIFFS.begin();
#if HAP_DEBUG
    	LOG_D("Listing files of SPIFFS:");
    	HAPServer::listDir(SPIFFS, "/", 0);
#endif
#endif

	}


#if (HAP_WEBSERVER_USE_SSL && HAP_ENABLE_KEYSTORE)
	//
	// keystore
	//
	LogI(F("Loading Keystore from partition "), false);
	LogI(String(_configuration.getKeystoreConfig()->keystore) + " ...", false);
	if (_keystore.begin(_configuration.getKeystoreConfig()->keystore, HAP_KEYSTORE_STORAGE_LABEL) == false){
		LogE(F("ERROR: Failed to start keystore!"), true);
	} else {
		if (_keystore.isValid() == false) {
			LogE(F("ERROR: Keystore is not valid! - Trying alternate keystore"), true);
			_keystore.end();
			if (_keystore.begin(_keystore.getAlternatePartition(), HAP_KEYSTORE_STORAGE_LABEL) == false){
				LogE(F("ERROR: Failed to start keystore!- Restarting now!"), true);
				// ESP.restart();
			} else {
				if (_keystore.isValid() == false) {
					LogE(F("ERROR: Alternate keystore is not valid! - Trying reboot"), true);
					_keystore.end();
					// ESP.restart();
				} else {
					LogI("Setting keystore to " + String(_keystore.getCurrentPartition()), true);
					_configuration.getKeystoreConfig()->setKeystore(_keystore.getCurrentPartition());
					_configuration.save();
				}
			}
		} else {
			LogI(" OK - Using keystore with containerId " + String(_keystore.getContainerId()), true);
		}
	}

#endif



	// Hostname
	char* hostname = (char*) malloc(sizeof(char) * strlen(HAP_HOSTNAME_PREFIX) + 8);
	sprintf(hostname, "%s-%02X%02X%02X", HAP_HOSTNAME_PREFIX, baseMac[3], baseMac[4], baseMac[5]);

#if defined(CORE_TEENSY)
	// start the Ethernet connection:
	LOG_I("Initialize Ethernet with DHCP ...");
	if (Ethernet.begin(baseMac, HAP_ETHERNET_TIMEOUT) == 0) {
		LOGRAW_E("ERROR - Failed to configure Ethernet using DHCP\n");
		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			LOGRAW_E("ERROR - Ethernet shield was not found.  Sorry, can't run without hardware. :(\n");
			while (true) {
				delay(1); // do nothing, no point running without Ethernet hardware
			}
		}
		if (Ethernet.linkStatus() == LinkOFF) {
			LOGRAW_E("ERROR - Ethernet cable is not connected!\n");
		}
		// try to congifure using IP address instead of DHCP:
		// Ethernet.begin(mac, ip, myDns);
	} else {
		LOGRAW_I("OK\n");
		LOG_I("Assigned IP: ");
		LOGDEVICE->println(Ethernet.localIP());
		_isConnected = true;
	}
#endif

#if HAP_ENABLE_WIFI

	// WiFi
	_wifi.begin(_configuration.getWiFiConfig(), std::bind(&HAPServer::begin, this, std::placeholders::_1), hostname);

	// if captive portal, return here
	if (_configuration.getWiFiConfig()->mode == (uint8_t)HAP_WIFI_MODE_AP){
		free(hostname);
		return true;
	}

	// Wait until wifi is online
	for (;;){
		if (_wifi.isProvisioned()){
			_isConnected = true;
			break;
		}
		delay(100);
	}
#endif

	// ToDo: Pixel Indicator
#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.off();
#endif

	//
	// Timesettings
	//
	// ToDo: Timezone to config
	// _time.begin(CEST, CET);
	_time.begin();

	LOG_I("Set time to: %s\n", _time.timestring());

	_configuration.getPlatformConfig()->setRefTime(_time.timestamp());
	_time.setRefTime(_configuration.getPlatformConfig()->refTime());
	// _configuration.getPlatformConfig()->setRefTime(1601846922);
	LOG_D("Set reftime to: %lu\n", _time.refTime());

	//
	// Pairings
	//
	LOG_I("Loading pairings ...OK\n");
	LOG_I("Loaded %d pairings from storage\n", _accessorySet->numberOfPairings());

// #if HAP_DEBUG_HOMEKIT
// 	_accessorySet->getPairings()->print();
// #endif


	LOG_I("Setup accessory ...");
	_accessorySet->setModelName(hostname);
	_accessorySet->setAccessoryType(HAPAccessoryType::Bridge);
	_accessorySet->setPinCode(HAP_PIN_CODE);
	_accessorySet->begin();
	LOGRAW_I("OK\n");


	//
	// Event Manager
	//
	LOG_I( "Adding listener to event manager ...");

	// // Notifications
  	// listenerNotificaton.mObj = this;
  	// listenerNotificaton.mf = &HAPServer::handleEvents;
	// _eventManager.addListener( EventManager::kEventNotifyController, &listenerNotificaton );

	// UpdateConfigNumber
	listenerUpdateConfigNumber.mObj = this;
  	listenerUpdateConfigNumber.mf = &HAPServer::handleEventUpdateConfigNumber;
	_eventManager.addListener( EventManager::kEventIncrementConfigNumber, &listenerUpdateConfigNumber );

	// kEventUpdatedConfig
	listenerConfigUpdated.mObj = this;
  	listenerConfigUpdated.mf = &HAPServer::handleEventUpdatedConfig;
	_eventManager.addListener( EventManager::kEventUpdatedConfig, &listenerConfigUpdated );

	// kEventRebootNow
	listenerRebootNow.mObj = this;
  	listenerRebootNow.mf = &HAPServer::handleEventRebootNow;
	_eventManager.addListener( EventManager::kEventRebootNow, &listenerRebootNow );


	// kEventConfigReset
	listenerConfigReset.mObj = this;
  	listenerConfigReset.mf = &HAPServer::handleEventConfigReset;
	_eventManager.addListener( EventManager::kEventConfigReset, &listenerConfigReset );


	// kEventDeleteAllPairings
	listenerDeleteAllPairings.mObj = this;
  	listenerDeleteAllPairings.mf = &HAPServer::handleEventDeleteAllPairings;
	_eventManager.addListener( EventManager::kEventRemoveAllPairings, &listenerDeleteAllPairings );

	LOGRAW_I("OK\n");

	//
	// Generate setupID and xmi uri
	//
	_accessorySet->generateSetupID();
	LOG_D("Homekit X-HM URI: %s\n", _accessorySet->xhm());
	LOG_D("Homekit setup hash: %s\n", _accessorySet->setupHash());

	//
	// QR Code generation
	//

#if HAP_PRINT_QRCODE || HAP_DEBUG_QRCODE_SVG

	/*
	 * Generate QR Code
	 */
	QRCode qrCode;
	uint8_t qrcodeData[qrcode_getBufferSize(3)];
	qrcode_initText(&qrCode, qrcodeData, 3, ECC_HIGH, _accessorySet->xhm());

#if HAP_PRINT_QRCODE
	for (uint8_t y = 0; y < qrCode.size; y++) {
		// Left quiet zone
		LOGDEVICE->print("        ");
		// Each horizontal module
		for (uint8_t x = 0; x < qrCode.size; x++) {
            // Print each module (UTF-8 \u2588 is a solid block)
			LOGDEVICE->print(qrcode_getModule(&qrCode, x, y) ? "\u2588\u2588": "  ");
		}
		LOGDEVICE->println("");
	}
#endif

#if HAP_DEBUG_QRCODE_SVG
	LOGDEVICE->println("SVG:");
	HAPSVG::drawQRCode(LOGDEVICE, &qrCode);
	LOGDEVICE->println("");
#endif


#endif


	//
	// Starting KNX
	//
#if HAP_SERVER_USE_INTEGRATED_KNX

	LOG_I("Starting integrated knx ...");
    // Transmitter is connected to Arduino Pin #10
    ArduinoPlatform::SerialDebug = &Serial;
	randomSeed(millis());

    // read adress table, association table, groupobject table and parameters from eeprom
    knx.readMemory();

#if HAP_DEBUG
	printHex("knx configuration ", knx.paramData(0), ((ETS_HK_CHANNEL_PARAMETER_SIZE * HAP_PLUGIN_KNX_MAX_DEVICES) + 1 ));
#endif

	LOGRAW_I("OK\n");
    LOG_I("KNX is configured: %d\n", knx.configured());

    // is the led active on HIGH or low? Default is LOW
    knx.ledPinActiveOn(HIGH);
	knx.buttonPin(0);

	knx.start();
#endif





	//
	// Starting Webserver
	//
#if HAP_ENABLE_WEBSERVER
	LOG_I("Starting webserver ...");
	if (_configuration.getWebServerConfig()->enabled){


		// #if HAP_API_ADMIN_MODE
		// 	// Get hap accessories
		// 	_webserver.setCallbackApiAccessories(std::bind(&HAPServer::callbackGetAccessories, this));
		// #endif

		// #if HAP_DEBUG
		// 	_webserver.setCallbackApiDebugHapClients(std::bind(&HAPServer::callbackApiHapClients, this));
		// #endif
		_webserver->setAccessorySet(_accessorySet);
		_webserver->setConfig(&_configuration);
		_webserver->setEventManager(&_eventManager);

#if (HAP_WEBSERVER_USE_SSL && HAP_ENABLE_KEYSTORE)
		_webserver->setKeystore(&_keystore);
#endif

		_webserver->begin();
		LOGRAW_I("OK\n");
	} else {
		LOGRAW_W("DISABLED\n");
	}
#endif

  	//
  	// Loading plugins
  	//
  	LOG_I( "Loading plugins ...\n");

	auto &factory = HAPPluginFactory::Instance();
    std::vector<std::string> names = factory.names();

    for(std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it) {
    	//LOGDEVICE->println(*it);
		LOG_HEAP(0, 0);
    	auto plugin = factory.getPlugin(*it);

		LOG_I("   - %s [v%s] of type %d : ", plugin->name(), plugin->version(), plugin->type());

		plugin->setAccessorySet(_accessorySet);
		plugin->setFakeGatoFactory(&_fakeGatoFactory);
		plugin->addEventListener(&_eventManager);

		// Load configuration from NVS here and pass it to the plugins
		size_t pluginConfigSize = _configuration.getDataLengthForPlugin(plugin->name());
		HAPConfigurationPlugin* pluginConfig = new HAPConfigurationPlugin();

		_configuration.getDataForPlugin(plugin->name(), pluginConfig, pluginConfigSize);
		plugin->setConfiguration(pluginConfig);
		// delete pluginConfig;

		// plugin->setConfig(_config.config()["plugins"][plugin->name()]);

#if HAP_DEBUG
		plugin->configToJson(*LOGDEVICE);
		LOGDEVICE->println();
#endif

		if ( plugin->isEnabled()) {
			LOGRAW_I("ENABLED\n");

			if (plugin->begin()) {

				HAPAccessory *accessory = plugin->initAccessory();

				if (accessory != nullptr){
					_accessorySet->addAccessory(accessory);
				}

#if HAP_ENABLE_WEBSERVER
				if (_configuration.getWebServerConfig()->enabled){
					std::vector<HAPWebServerPluginNode*> vector = plugin->getResourceNodes();
					if (!vector.empty()) {
						for (auto node : vector) {
							_webserver->registerPluginNode(node);
						}
					}
				}
#endif

				_plugins.push_back(std::move(plugin));

			} else {
				LOGRAW_E("ERROR: Failed to start plugin\n");
				plugin->enable(false);
			}

    	} else {
    		LOGRAW_W("Disabled\n");
    	}
	}

	//
	// Starting HAP server
	//
	LOG_I("Starting HAP server ...");
	_server.begin();

#if defined(ARDUINO_ARCH_ESP32)
	_server.setNoDelay(true);
#endif
	LOGRAW_I("OK\n");

	//
	// Bonjour
	//
	// Set up mDNS responder:
	// - first argument is the domain name, in this example
	//   the fully-qualified domain name is "esp8266.local"
	// - second argument is the IP address to advertise
	//   we send our IP address on the WiFi network
	LOG_I("Advertising bonjour service ...");
#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.begin(_accessorySet->modelName())) {
		LOG_E( "ERROR; Starting mDNS responder failed!\n");
		return false;
	}

	// Add service to MDNS-SD
	// mDNSExt.addService("_hap", "_tcp", _port);


	mDNSExt.enableHomekit(_port, HAPDeviceID::deviceID(), _accessorySet->modelName(), _accessorySet->accessoryTypeAsInt(), _accessorySet->setupHash(), "1.0");

#if HAP_ENABLE_WEBSERVER
	if (_configuration.getWebServerConfig()->enabled){
		mDNSExt.addService("http", "_tcp", _configuration.getWebServerConfig()->port);
	}
#endif
#elif defined( CORE_TEENSY )
	MDNS.begin("T41", 2);
	// MDNS.setServiceName(_accessorySet->modelName());
	MDNS.addService("_hap._tcp", _port, HomekitTXTRecord);
#endif

	LOGRAW_I("OK\n");


#if HAP_ENABLE_UPDATE_OTA
	if (_configuration.getOTAConfig()->enabled){
		//
		// Starting Arduino OTA
		//
		LOG_I( "Starting Arduino OTA ...");
		_updater.setConfig(_configuration.getOTAConfig());
		_updater.begin(hostname);
		LOGRAW_I("OK\n");
	}
#endif


#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
		LOG_E( "ERROR: Advertising mDNS service txt failed!\n");
		return false;
	}
#else
	if ( !updateServiceTxt() ){
		LOG_E("ERROR: Advertising mDNS service failed!\n");
		return false;
	}
#endif


#if HAP_DEBUG

	{
		uint8_t buffer[1024];
		HAPPrintBuffered bufferedPrint(*LOGDEVICE, buffer, 1024);
		// serializeJson(doc, eStream);
		_accessorySet->printTo(bufferedPrint);
		bufferedPrint.flush();

		LOGDEVICE->println();
	}

	LOG_HEAP(0, 0);

#endif

	// queue event
	_eventManager.queueEvent(EventManager::kEventHomekitStarted, HAPEvent());

	//
	// Show event listerners
	//
  	LOG_D("Number of event listeners: %d\n", _eventManager.numListeners());


	// Handle any events that are in the queue
	_eventManager.processAllEvents();

	_homekitStarted = true;
	if ( !updateServiceTxt() ){
		LOG_E("ERROR: Updating mDNS service failed!\n");
	}

	//
	// Startup completed
	//
	LOG_I("Homekit has started successfully!\n");
	if (!_accessorySet->isPaired()) {
		LOG_I("Homekit pin code: %s\n", _accessorySet->pinCode());
	}

#if HAP_ENABLE_WEBSERVER
	if (_configuration.getWebServerConfig()->enabled){
		LOG_I("Webinterface available at: ");
		if (HAP_WEBSERVER_USE_SSL) {
			LOGRAW_I("https://");
		} else {
			LOGRAW_I("http://");
		}
		LOGRAW_I("%s\n", hostname);
	}
#endif

// #if HAP_ENABLE_UPDATE_WEB
// 	if (_config.config()["update"]["web"]["enabled"]){
// 		if ( _updater.checkUpdateAvailable() ) {
// 			LogI("Online pdate available: " + _updater.onlineVersion() , true);
// 		} else {
// 			LogV( F("No update available"), true);
// 		}
// 	}
// #endif

	free(hostname);

#if HAP_WEBSERVER_USE_SSL
#if HAP_ENABLE_KEYSTORE
	// Close Keystore
	_keystore.end();
#endif
#endif



#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.confirmWithColor(HAPColorGreen);
#endif
	return true;
}


// ToDo: Rewrite and fix memory leaks !!!
bool HAPServer::updateServiceTxt() {
	LOG_V("updateServiceTxt\n");
#if defined(ARDUINO_ARCH_ESP32)

	uint8_t *baseMac = HAPDeviceID::generateID();


	char valIdentifier[18 + 1];
	sprintf(valIdentifier, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	mdns_txt_item_t hapTxtData[] {
		{.key = "id", .value = (const char*) valIdentifier},
		{.key = "md", .value = (const char*) _accessorySet->modelName()},
		{.key = "ci", .value = (const char*) _accessorySet->accessoryType()},
		{.key = "c#", .value = (const char*) _accessorySet->configurationNumber},
		{.key = "ff", .value = (const char*) 0},
		{.key = "pv", .value = (const char*) HOMEKIT_PROTOKOL_VERSION},
		{.key = "sf", .value = (const char*) !_accessorySet->isPaired()},
		{.key = "s#", .value = (const char*) 1},
		{.key = "sh", .value = (const char*) _accessorySet->setupHash()},
	};
	return mDNSExt.addServiceTxtSet((char*)"_hap", "_tcp", 9, hapTxtData);

#elif defined( CORE_TEENSY )

	uint8_t *baseMac = HAPDeviceID::mac();
	char valIdentifier[18 + 1] = {'\0', };
	sprintf(valIdentifier, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	_hapMdnsTxt.setId(valIdentifier);									// Device ID
	_hapMdnsTxt.setModellDescription(_accessorySet->modelName());		// Model name of the accessory (e.g. ”Device1,1”). Required.
	_hapMdnsTxt.setConfigNumber(_accessorySet->configurationNumber);	// Current configuration number. Required.
	_hapMdnsTxt.setFf(0);												// Pairing Feature flags (e.g. ”0x3” for bits 0 and 1). Required if non-zero.
	_hapMdnsTxt.setSf(!_accessorySet->isPaired());						// Status flags
	_hapMdnsTxt.setPv(HOMEKIT_PROTOKOL_VERSION);						// Protocol Version
	_hapMdnsTxt.setStateNumber(1);										// Current state number. Required. - This must have a value of ”1”.
	_hapMdnsTxt.setCi(_accessorySet->accessoryTypeAsInt());					// Accessory Category Identifier. Required.
	_hapMdnsTxt.setSh(_accessorySet->setupHash());						// Setup Hash

	return true;
#endif
}


#if defined ( CORE_TEENSY )
const fnet_mdns_txt_key_t* HAPServer::HomekitTXTRecord(){
	static fnet_mdns_txt_key_t _hapTXTRecord[] = {
		{.key_name = (char*)"id", .key_value = (char*)_hapMdnsTxt.id},
		{.key_name = (char*)"md", .key_value = (char*)_hapMdnsTxt.md},
		{.key_name = (char*)"ci", .key_value = (char*)_hapMdnsTxt.ci},
		{.key_name = (char*)"c#", .key_value = (char*)_hapMdnsTxt.cN},
		{.key_name = (char*)"ff", .key_value = (char*)_hapMdnsTxt.ff},
		{.key_name = (char*)"pv", .key_value = (char*)_hapMdnsTxt.pv},
		{.key_name = (char*)"sf", .key_value = (char*)_hapMdnsTxt.sf},
		{.key_name = (char*)"s#", .key_value = (char*)_hapMdnsTxt.sN},
		{.key_name = (char*)"sh", .key_value = (char*)_hapMdnsTxt.sh},
		{0,0}
	};

	return _hapTXTRecord;
}
#endif


void HAPServer::handle() {

#if HAP_SERVER_USE_INTEGRATED_KNX
	knx.loop();
#endif


#if HAP_DEBUG
	// Free Heap every interval ms
	if ( millis() - _previousMillisHeap >= HAP_HEAP_LOG_INTERVAL) {
	    // save the last time you blinked the LED
	    _previousMillisHeap = millis();
		LOG_HEAP(_clients.size(), _eventManager.getNumEventsInQueue());
	}
#endif



#if HAP_ENABLE_PIXEL_INDICATOR
	LOG_V("Handle pixel indicator\n");
	_pixelIndicator.handle();
#endif


#if HAP_ENABLE_WIFI
	if ( (_wifi.captiveInitialized()) && (_configuration.getWiFiConfig()->mode == (uint8_t)HAP_WIFI_MODE_AP) ){
		_wifi.handle();
		return;
	}
#endif

	//
	// Handle existing clients
	//
	LOG_V("Handle existing clients ...");
	for (auto& hapClient : _clients) {

		// Connected
		if (hapClient->client.connected()) {

			// Available
			unsigned long timeout = 15;
			unsigned long previousMillis = millis();
			while ( millis() - previousMillis < timeout) {

				if (hapClient->client.available()) {
					hapClient->state = HAPClientState::Available;
					handleClientState(hapClient);
					break;
				}

				// Idle
				hapClient->state = HAPClientState::Idle;
			}


		} else {
			// Disconnected
			hapClient->state = HAPClientState::Disconnected;
			handleClientState(hapClient);
		}

		// LogV( "HAPClient state " + hapClient.getClientState(), true );
	}
	LOGRAW_V("OK\n");

	//
	// Handle new clients
	//
#if defined(ARDUINO_ARCH_ESP32)
	WiFiClient client = _server.available();
#elif defined( CORE_TEENSY)

	LOG_V("Checking for new clients ...");
	EthernetClient client = _server.available();
#endif
	if (client) {
		// LOG_V("Handle new clients\n");

		HAPClient* hapClient = new HAPClient();

		// New client connected
		hapClient->client = client;
		hapClient->state = HAPClientState::Connected;

		handleClientState(hapClient);
	}
	LOGRAW_V("OK\n");

	//
	// Handle Webserver
	//
#if HAP_ENABLE_WEBSERVER
	if (_configuration.getWebServerConfig()->enabled){
		// LOG_V("Handle webserver\n");
		_webserver->handle();
	}
#endif

	//
	// Handle Arduino OTA
	//
#if HAP_ENABLE_UPDATE_WEB || HAP_ENABLE_UPDATE_OTA
	if (_configuration.getOTAConfig()->enabled){
		// LOG_V("Handle OTA\n");
		_updater.handle();
	}
#endif

	//
	// Handle plugins
	//
	LOG_V("Handling plugins ...");
	for (auto& plugin : _plugins) {
		if (plugin->isEnabled()) {
			plugin->handle();
		}
	}
	LOGRAW_V("OK\n");

	//
	// Handle fakeGatos
	//
	LOG_V("Handling Fakegato ...");
	_fakeGatoFactory.handle();
	LOGRAW_V("OK\n");

	//
	// Handle any events that are in the queue
	//
	LOG_V("Handling Events ...");
	processEvents();
	LOGRAW_V("OK\n");
	// _eventManager.processAllEvents();

}







void HAPServer::handleClientDisconnect(HAPClient* hapClient) {
	LOG_V("handleClientDisconnect\n");
	if (hapClient == nullptr) return;

	for ( int i = 0; i < _clients.size(); i++ ) {
        if (_clients[i] == hapClient ) {
			delete _clients[i];
			_clients.erase(_clients.begin() + i);
			break;
		}
    }

}

void HAPServer::handleClientState(HAPClient* hapClient) {
	LOG_V("handleClientState\n");
	if ((hapClient->state == HAPClientState::Disconnected) || (!hapClient->client.connected())){
		handleClientDisconnect( hapClient );
		return;
	}

#if defined( ARDUINO_ARCH_ESP32 )
	LogD( "Handle state for client [" + hapClient->client.remoteIP().toString() + "] connected", true );
#elif defined( CORE_TEENSY )
	LOG_D("Handle state for client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] ");
#endif

	if (hapClient->state == HAPClientState::Connected) {
		LOGRAW_D("connected\n");
		_clients.push_back(std::move(hapClient));

		hapClient->state = HAPClientState::Idle;
	}

	if (hapClient->state == HAPClientState::Idle) {
		LOGRAW_D("idle\n");
		if (hapClient->client.available()) {
			hapClient->state = HAPClientState::Available;
		}
	}

	if (hapClient->state == HAPClientState::Available) {
		LOGRAW_D("available: %d\n", hapClient->client.available());
		handleClientAvailable(hapClient);
	} else if (hapClient->state == HAPClientState::AllPairingsRemoved) {
		LOG_D("all pairings removed\n");
		handleAllPairingsRemoved();
	}


}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleAllPairingsRemoved(){
	LOG_V("handleAllPairingsRemoved\n");
	LOG_D("Handle all pairings removed ...");
	for (int i=0; i < _clients.size(); i++){

		if (_clients[i]->client.connected()){
#if defined( ARDUINO_ARCH_ESP32 )
			LogD("\nClosing connection to client [" + _clients[i].client.remoteIP().toString() + "]", true);
#elif defined( CORE_TEENSY )
			LOGRAW_V("Closing connection to client [");
			LOGDEVICE->print(_clients[i]->client.remoteIP());
			LOGRAW_V("]\n");
#endif
			_clients[i]->client.stop();
		}

		_clients[i]->state = HAPClientState::Disconnected;
	}
	LOGRAW_D("OK\n");

	LOG_V("END handleAllPairingsRemoved\n");
}

void HAPServer::handleClientAvailable(HAPClient* hapClient) {
	LOG_V("handleClientAvailable\n");

	LOG_D("Handle client available [enrypted: %d]\n", hapClient->isEncrypted());

	ReadBufferingClient bufferedClient{hapClient->client, 1024 + 16};

	if (hapClient->isEncrypted()) {
		processIncomingEncryptedRequest( hapClient, &bufferedClient );
	} else {
		processIncomingRequest( hapClient, &bufferedClient );
	}

	// clear request
	hapClient->clear();

	if ( !hapClient->client.connected() ) {
		hapClient->state = HAPClientState::Disconnected;
	}

	// Update client state *print*
	handleClientState( hapClient );


}


void HAPServer::processIncomingEncryptedRequest(HAPClient* hapClient, ReadBufferingClient* bufferedClient){


	LOG_V("processIncomingEncryptedRequest\n");

	//
    // Each HTTP message is split into frames no larger than 1024 bytes
    //
    // Each frame has the following format:
    //
    // < 2:    AAD for little endian length of encrypted data (n) in bytes>
    // < n:    encrypted data according to AEAD algorithm, up to 1024 bytes>
    // <16:    authTag according to AEAD algorithm>
    //


	//String bodyData = "";

	while ( bufferedClient->available() )	{

		//
	    // AAD
	    //
	    // < 2:    AAD for little endian length of encrypted data (n) in bytes>
	    // uint8_t aad[HAP_AAD_LENGTH];
	    // aad[0] = (uint8_t) (length >> 8);   // Get upper byte of 16-bit var;
	    // aad[1] = (uint8_t) length;          // Get lower byte of 16-bit var;
		uint8_t AAD[HAP_ENCRYPTION_AAD_SIZE];
		bufferedClient->readBytes(AAD, HAP_ENCRYPTION_AAD_SIZE);

		uint16_t trueLength = ((uint16_t)AAD[1] << 8) | AAD[0];

		// LOGDEVICE->printf("AAD: %02X%02X - %d\n", AAD[0], AAD[1], trueLength);
		// LOGDEVICE->printf("availableSize: %d\n", availableSize);
		int availableSize = bufferedClient->available() - HAP_ENCRYPTION_HMAC_SIZE;	// 16 is the size of the HMAC
		// LogD("\nNeed " + String(trueLength) + " bytes and have " + String(availableSize) + " bytes", true);

		while (trueLength > availableSize){
			// The packet is bigger than the available data; wait till more comes in
			availableSize = bufferedClient->available() - HAP_ENCRYPTION_HMAC_SIZE;	// 16 is the size of the HMAC
			delay(1);
		}



		//
		// cipherText
		uint8_t cipherText[trueLength];
		bufferedClient->readBytes(cipherText, trueLength);

		// LOGDEVICE->print("trueLength: ");
		// LOGDEVICE->println(trueLength);
		// HAPHelper::arrayPrint(cipherText, trueLength);

		//
		// hmac
		uint8_t hmac[HAP_ENCRYPTION_HMAC_SIZE];	// 16 is the size of the HMAC
		//LogD("Need " + String(HAP_ENCRYPTION_HMAC_SIZE) + " bytes and have " + String(availableSize) + " bytes", true);

		availableSize = bufferedClient->available();

		while ( HAP_ENCRYPTION_HMAC_SIZE > availableSize ) {
			// The packet is bigger than the available data; wait till more comes in
			availableSize = bufferedClient->available();
			delay(1);
		}

		bufferedClient->readBytes(hmac, HAP_ENCRYPTION_HMAC_SIZE);

		uint8_t plainText[trueLength + 1];

		//
	    // nonce
	    //
	    // The 32-bit fixed-common part of the 96-bit ( 12? ) nonce
	    // is all zeros: 00 00 00 00.
	    //
	    // length 8
	    //
	    // < n:    encrypted data according to AEAD algorithm, up to 1024 bytes>
	    //
	    // Needs to be incremented each time it is called after the 1st 4 bytes

		uint8_t nonce[HAP_ENCRYPTION_NONCE_SIZE] = { 0, };
    	nonce[4] = hapClient->encryptionContext.decryptCount % 256;
    	nonce[5] = hapClient->encryptionContext.decryptCount++ / 256;


		HAPEncryption::decrypt(hmac, plainText, cipherText, trueLength, AAD, 2, nonce, HAP_ENCRYPTION_NONCE_SIZE, hapClient->encryptionContext.decryptKey);

		plainText[trueLength] = '\0';

#if HAP_DEBUG_HOMEKIT_REQUEST
		LOGDEVICE->println("plaintext:");
		LOGDEVICE->println((char*) plainText);
#endif


		int bodyDataLen = 0;
		uint8_t *bodyData;
		parseRequest(hapClient, (char*)plainText, trueLength, &bodyData, &bodyDataLen);

#if HAP_DEBUG_HOMEKIT_REQUEST
		LOGARRAY_D("bodyData", bodyData, bodyDataLen);
#endif

		handlePath(hapClient, bodyData, bodyDataLen);

		if (bodyDataLen > 0){
			free(bodyData);
		}
	}

}


bool HAPServer::handlePath(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen){
	LOG_V("handlePath\n");

	bool validPath = false;
	// /accessories
	if ( (hapClient->request.path == "/accessories") && (hapClient->request.method == HAPServerMethod::Get) ) {
		validPath = true;
		handleAccessories( hapClient );
	}

	// /characteristics
	else if ( hapClient->request.path == "/characteristics" ) {

		// GET
		if ( hapClient->request.method == HAPServerMethod::Get ) {
			validPath = true;
			handleCharacteristicsGet( hapClient );
		}
		// PUT
		else if ( (hapClient->request.method == HAPServerMethod::Put) && (hapClient->request.contentType == "application/hap+json") ) {
			validPath = true;
			// char bodyDataStr[bodyDataLen + 1];
			// strncpy(bodyDataStr, (char*)bodyData, bodyDataLen);

			handleCharacteristicsPut( hapClient, bodyData, bodyDataLen );
		}

	} else if ( hapClient->request.path == "/pairings" ) {
		// POST
		if ( ( hapClient->request.method == HAPServerMethod::Post ) && (hapClient->request.contentType == "application/pairing+tlv8") ) {
			validPath = true;
			handlePairingsPost( hapClient, bodyData,  bodyDataLen);
		}
	}


	if (validPath == false) {


#if defined( ARDUINO_ARCH_ESP32 )
		LogE(F("Not yet implemented! Client [") + hapClient->client.remoteIP().toString() + "] ", false);
#elif defined( CORE_TEENSY )
		LOG_E("Not yet implemented! Client [");
		LOGDEVICE->print(hapClient->client.remoteIP());
		LOGRAW_E("] requested the following URI\n");
#endif
		LOG_E(" - method: %d\n", hapClient->request.method);
		LOG_E(" - path: %s\n", hapClient->request.path.c_str());
		LOG_E(" - content-type: %s\n", hapClient->request.contentType.c_str());

		hapClient->client.stop();
		return false;
	}


	
	return true;
}


void HAPServer::parseRequest(HAPClient* hapClient, const char* msg, size_t msg_len, uint8_t** out, int* outLen){
	LOG_V("parseRequest\n");
	int curPos = 0;

	for (int i = 0; i < msg_len; i++) {
    		//LOGDEVICE->print(msg[i]);
		if ( msg[i] == '\r' && msg[i + 1] == '\n' ) {

			char cLine[i - curPos + 1];
			memcpy(cLine, msg + curPos, i - curPos);
			cLine[i - curPos] = '\0';

			processIncomingLine(hapClient, cLine, i - curPos);

			i++;

			if (i - curPos == 2) {
				curPos = i + 1;
				break;
			}
			curPos = i + 1;
		}

	}

	//LOGDEVICE->printf(">>> curPos: %d\n", curPos);

	if (curPos + hapClient->request.contentLength == msg_len) {
		//bodyData += String(msg).substring(curPos);
		int siz = msg_len - curPos;

		if (siz > 0) {
			*out = (uint8_t*)malloc(sizeof(uint8_t) * siz);
			memcpy(*out, &msg[curPos], siz);
		}

		*outLen = siz;
	} else {
		LOG_W("WARNING: Size mismatch while parsing request\n");
	}

	//return bodyData;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::sendErrorTLV(HAPClient* hapClient, uint8_t state, uint8_t error){
	LOG_V("sendErrorTLV\n");
	TLV8 response;
	response.encode(HAPTLVType::State, 1, state);
	response.encode(HAPTLVType::Error, 1, error);

	sendResponse(hapClient, &response);
	response.clear();


	hapClient->clear();
	hapClient->client.stop();

	_homekitFailedLoginAttempts++;
	_isInPairingMode = false;

	hapClient->state = HAPClientState::Disconnected;

	_eventManager.queueEvent(EventManager::kEventErrorOccurred, HAPEvent());
}


void HAPServer::processIncomingRequest(HAPClient* hapClient, ReadBufferingClient* bufferedClient){

	LOG_V("Process incoming request\n");

	size_t curLineBufferSize = 2048;
	char curLine[curLineBufferSize];
	size_t curLineCount = 0;

	while (bufferedClient->available()){

		const byte b = bufferedClient->read();

		if ( (char) b == '\n' ) {
			// if the current line is blank, you got two newline characters in a row.
			// that's the end of the client HTTP request, so send a response:
			if (curLineCount == 0) {


#if HAP_DEBUG_HOMEKIT
				// Handle data
				LOG_D("request: %s\n", hapClient->request.toString().c_str());
#endif

				// /identify
				if ( (hapClient->request.path == "/identify") && (hapClient->request.method == HAPServerMethod::Post) ) {
					handleIdentify(hapClient);
					hapClient->state = HAPClientState::Idle;
				}

				// has content
				else if ( hapClient->request.contentLength > 0) {
					// encode tlv8
					if ( hapClient->request.contentType == "application/pairing+tlv8" )  {


						if ( !encode(hapClient, bufferedClient) ) {
							LOG_E("ERROR: Decoding pairing request failed!\n");

							sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorType::Unknown);
							return;
						}


						// pair-setup M1
						if ( (hapClient->request.path == "/pair-setup" ) && (hapClient->pairState == HAPPairingState::M1) ) {


#if HAP_ALLOW_PAIRING_WHILE_PAIRED == 0

							if (_accessorySet->isPaired() == true) {
								// accessory is already paired
								LOG_E( "ERROR: Accessory is already paired!\n");
								sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorCode::Authentication);
								return;
							} else if (_isInPairingMode == true) {
								// accessory is in pairing mode
								sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorCode::Busy);
								return;
							} else if (_homekitFailedLoginAttempts >= 100) {
								// accessory has more than 100 failed attempts
								sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorCode::MaxTries);
								return;
							} else {
#endif
								if (!handlePairSetupM1( hapClient ) ) {
									LOG_E("ERROR: Pair-setup failed at M1!\n");
									hapClient->clear();
									hapClient->client.stop();
									hapClient->state = HAPClientState::Disconnected;
								}
#if HAP_ALLOW_PAIRING_WHILE_PAIRED == 0
							}
#endif
						}

						// pair-setup M3
						else if ( (hapClient->request.path == "/pair-setup" ) && (hapClient->pairState == HAPPairingState::M3) ) {
							if (!handlePairSetupM3( hapClient ) ) {
								LOG_E("ERROR: Pair-setup failed at M3!\n");
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAPClientState::Disconnected;
							}
						}

						// pair-setup M5
						else if ( (hapClient->request.path == "/pair-setup" ) && (hapClient->pairState == HAPPairingState::M5) ) {
							if ( !handlePairSetupM5( hapClient ) ) {
								LOG_E("ERROR: Pair-setup failed at M5!\n");
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAPClientState::Disconnected;
							}
						}

						// pair-verify M1
						if ( (hapClient->request.path == "/pair-verify" ) && (hapClient->verifyState == HAPVerifyState::M1) ) {
							if ( !handlePairVerifyM1( hapClient ) ) {
								LOG_E("ERROR: Pair-verify failed at M1!\n");
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAPClientState::Disconnected;
							}
						}

						// pair-verify M3
						else if ( (hapClient->request.path == "/pair-verify" ) && (hapClient->verifyState == HAPVerifyState::M3) ) {
							if ( !handlePairVerifyM3( hapClient ) ) {
								LOG_E("ERROR: Pair-verify failed at M3!\n");
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAPClientState::Disconnected;
							}
						}
					}
				}

				// curLine = "";
				memset(curLine, 0, curLineBufferSize);
				curLineCount = 0;


				return;
			} else {  // if you got a newline, then clear currentLine:
				// Handle lines
				processIncomingLine(hapClient, curLine, curLineCount);
				// curLine = "";
				memset(curLine, 0, curLineBufferSize);
				curLineCount = 0;
			}
		} else if ( (char) b != '\r') {  	// if you got anything else but a carriage return character,
			curLine[curLineCount++] = (char) b;      		// add it to the end of the currentLine
		}

	}


	hapClient->state = HAPClientState::Idle;

}


void HAPServer::processPathParameters(HAPClient* hapClient, const char* line, size_t lineLength, int curPos){

	LOG_V("Process path parameters\n");

	//int index = line.indexOf("?", curPos);
	int index = HAPHelper::indexOf(line, lineLength, '?', curPos);

	if ( index == -1) {
		// no ? in request
		int len = HAPHelper::indexOf(line, lineLength, ' ', curPos); //line.indexOf(" ", curPos)

		// line.substring(curPos, line.indexOf(" ", curPos));
		char requestPath[ len + 1 - curPos];
		memcpy( requestPath, &line[curPos], len - curPos);
		requestPath[len - curPos] = '\0';

		hapClient->request.path = requestPath; // line.substring(curPos, line.indexOf(" ", curPos));
		hapClient->request.params = std::map<std::string, std::string>();
	} else {
		// hapClient->request.path = line.substring(curPos, index);
		char requestPath[ index - curPos ];
		memcpy( requestPath, &line[curPos], index);
		requestPath[index - curPos] = '\0';

		hapClient->request.path = requestPath;

		curPos = index + 1;

		// String paramStr = line.substring(curPos, line.indexOf(" ", curPos));

		int len = HAPHelper::indexOf(line, lineLength, ' ', curPos); //line.indexOf(" ", curPos)
		char paramString[ len - curPos + 1 ];
		memcpy( paramString, &line[curPos], len - curPos);
		paramString[len - curPos] = '\0';


		char* tokens = paramString;
		char *p = paramString;

		while ((p = strsep (&tokens, "&\n"))) {
			char *key = strtok (p, "=");
			char *value = NULL;
			if (key && (value = strtok (NULL, "="))){
				hapClient->request.params[key] = value;
			}
		}
	}
}


void HAPServer::processIncomingLine(HAPClient* hapClient, const char* line, size_t lineLength){

	LOG_V("Process incoming line\n");

	// Print Line
#if HAP_DEBUG_HOMEKIT_REQUEST
	if (lineLength > 0) LOGDEVICE->println(line);
#endif

	int curPos = 0;

	// Method
	if ( HAPHelper::startsWith(line, "POST ") ) {
		hapClient->request.method = HAPServerMethod::Post;
		curPos = 5;
		// Path
		processPathParameters( hapClient, line, lineLength, curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	} else if ( HAPHelper::startsWith(line, "GET ") ) {
		hapClient->request.method = HAPServerMethod::Get;
		curPos = 4;
		// Path
		processPathParameters( hapClient, line, lineLength, curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	} else if ( HAPHelper::startsWith(line, "PUT ") ) {
		hapClient->request.method = HAPServerMethod::Put;
		curPos = 4;
		// Path
		processPathParameters( hapClient, line, lineLength, curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	} else if ( HAPHelper::startsWith(line, "DELETE ") ) {
		hapClient->request.method = HAPServerMethod::Delete;
		curPos = 7;
		// Path
		processPathParameters( hapClient, line, lineLength, curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	}

	if (lineLength == 0) {
		//LOGDEVICE->println("END OF HEADERS!!!");



	} else {

		char lowLine[16];
		for (int i =0; i < 16; i++){
			lowLine[i] = tolower(line[i]);
		}
		lowLine[15] = '\0';

		// LOGDEVICE->println("lowLine:" + String(lowLine));

		// Content Type
		if (HAPHelper::startsWith(lowLine, "content-type:")){
			curPos = 13;
			if (line[curPos] == ' '){
				curPos++;
			}

			char contentType[ strlen(line) - curPos + 1];
			memcpy( contentType, &line[curPos], strlen(line) - curPos);
			contentType[strlen(line) - curPos] = '\0';

			// LOGDEVICE->println("content-type:*" + String(contentType) + "*");
			hapClient->request.contentType = contentType;
		}

		// Content Length
		else if (HAPHelper::startsWith(lowLine, "content-length:")){
			curPos = 15;
			if (line[curPos] == ' '){
				curPos++;
			}

			char contentLength[ strlen(line) - curPos + 1];
			memcpy( contentLength, &line[curPos], strlen(line) - curPos);
			contentLength[strlen(line) - curPos] = '\0';

			// LOGDEVICE->println("content-type:*" + String(contentType) + "*");
			hapClient->request.contentLength = atoi(contentLength);
		}
	}
}


/**
 * @brief
 *
 *
 * @param hapClient
 * @return true
 * @return false
 */
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::encode(HAPClient* hapClient, ReadBufferingClient* bufferedClient){

	LOG_V("encode\n");
	uint16_t written = 0;
	bool success = false;

	// Method not supported :(
	if ( bufferedClient->peek() == 0x00) {
		bufferedClient->read();
//		LOGDEVICE->println(c, HEX);
		bufferedClient->read();
//		LOGDEVICE->println(c, HEX);
		bufferedClient->read();
//		LOGDEVICE->println(c, HEX);
		hapClient->request.contentLength = hapClient->request.contentLength - 3;
	}

	// Reset pairing state
	hapClient->pairState = HAPPairingState::Reserved;

	while (bufferedClient->available()) {            	// loop while the client's connected

		if ( TLV8::isValidTLVType( bufferedClient->peek()) ) {

			uint8_t type = bufferedClient->read();
			uint8_t length = bufferedClient->read();

			uint8_t data[length];
			bufferedClient->readBytes(data, length);

#if HAP_DEBUG_TLV8
			LOG_D("------------------------------------------");
			LOG_D(" - type: %x\n", type);
			LOG_D(" - length: %d\n", length);
			LOGARRAY_D("value", data, length);
#endif


			if (type == HAPTLVType::State) {

				if (hapClient->request.path == "/pair-verify") {
					hapClient->verifyState = static_cast<HAPVerifyState::Type>(data[0]);
				} else
					hapClient->pairState = static_cast<HAPPairingState::Type>(data[0]);
			}


			written += length + 2;


			if (!hapClient->request.tlv.encode(type, length, data)) {
				LOG_E( "ERROR: Encoding TLV data failed!\n");
				success = false;
				break;
			}

			if ( written == hapClient->request.contentLength ) {

#if HAP_DEBUG_TLV8
				hapClient->request.tlv.print();
#endif
				success = true;

				break;
			}

		} else {


#if HAP_DEBUG_TLV8
			uint8_t read = bufferedClient->read();
			LOG_W("WARNING: Invalid TLV8 type: "));
			LOGRAW_W("%s\n",(char*)read);
#else
			bufferedClient->read();
#endif

		}

	}

	hapClient->state = HAPClientState::Idle;
	return success;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleIdentify(HAPClient* hapClient){
	LOG_D("Handle /identify ...");

	HAPCharacteristic<bool>* c = reinterpret_cast< HAPCharacteristic<bool> *>(_accessorySet->getCharacteristicOfType(_accessorySet->aid(), HAPCharacteristicType::Identify));

	WriteBufferingClient bufferedWifiClient{hapClient->client, 256};

	if ( !isPaired() ) {
		// Send 204
		bufferedWifiClient.write( HTTP_204 );
		bufferedWifiClient.write( HTTP_CRLF );

		if (c != NULL){
			c->setValue(true);
		}

	} else {
		// Send 400
		bufferedWifiClient.write( HTTP_400 );

		bufferedWifiClient.write( HTTP_CONTENT_TYPE_HAPJSON );
		bufferedWifiClient.write( HTTP_CRLF );

		bufferedWifiClient.print( F("Content-Length: 21") );
		bufferedWifiClient.write( HTTP_CRLF );
		bufferedWifiClient.write( HTTP_CRLF );

		bufferedWifiClient.print( F("{ \"status\" : -70401 }") );
		bufferedWifiClient.write( HTTP_CRLF );
	}

	bufferedWifiClient.write( HTTP_CRLF );
	bufferedWifiClient.flush();

	hapClient->clear();

	if (c != NULL){
		c->setValue(false);
	}

	LOGRAW_D("OK\n");
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::send(HAPClient* hapClient, const char* httpStatus, const uint8_t* data, const size_t length, const HAPEncryptionMode::Type mode, const char* contentType){
	LOG_V("send\n");
	if (httpStatus == HTTP_204) {
		send204(hapClient);
		return true;
	}

	uint8_t buf[HAP_PRINT_ENCRYPTED_BUFFER_SIZE];
	HAPPrintEncrypted eStream(hapClient->client, buf, HAP_PRINT_ENCRYPTED_BUFFER_SIZE);

	eStream.setEncryptCount(hapClient->encryptionContext.encryptCount);
	eStream.setKey(hapClient->encryptionContext.encryptKey);

	eStream.begin();

	eStream.print(httpStatus);

	// Content-Type
	eStream.print(F("Content-Type: "));
	eStream.print(contentType);
	eStream.print(HTTP_CRLF);


	// Keep Alive
	if ( httpStatus != EVENT_200 ) {
		eStream.print( HTTP_KEEP_ALIVE );
	}

	if ( mode == HAPEncryptionMode::Plain ) {
		eStream.print(F("Content-Length: "));
		eStream.print(length);
		eStream.print(HTTP_CRLF);

		// end header
		eStream.print(HTTP_CRLF);

	} else if (mode == HAPEncryptionMode::PlainChunked) {
		eStream.print( HTTP_TRANSFER_ENCODING );
		eStream.print(HTTP_CRLF);

		// begin chunk
		eStream.begin();
	} else {
		eStream.print( HTTP_TRANSFER_ENCODING );
		eStream.print(HTTP_CRLF);

		// end header
		eStream.endHeader();
	}

	if (length > 0) {
		// Body start
		eStream.write(data, length);
	}


	eStream.end();

	if ( ( mode == HAPEncryptionMode::Plain ) || (mode == HAPEncryptionMode::PlainChunked) ) {
		return 0;
	}

	hapClient->encryptionContext.encryptCount = eStream.encryptCount();

	return 0;

}

bool HAPServer::send(HAPClient* hapClient, const char* httpStatus, const JsonDocument& doc, const HAPEncryptionMode::Type mode, const char* contentType){

	LOG_V("send\n");
	if (httpStatus == HTTP_204) {
		send204(hapClient);
		return true;
	}

#if 0
	if (mode == HAPEncryptionMode::Encrypt) {
		size_t jsonLength = measureJson(doc);

		String response = "";
		response += httpStatus;

		// Keep Alive
		if ( httpStatus != EVENT_200 ) {
			response += HTTP_KEEP_ALIVE;
		}

		// Content Type
		response += F("Content-Type: ");
		response += contentType;
		response += HTTP_CRLF;

		// Content Length
		response += F("Content-Length: ");
		response += String(jsonLength);
		response += HTTP_CRLF;

		// end header
		response += HTTP_CRLF;

		// body
		serializeJson(doc, response);

#if HAP_DEBUG_HOMEKIT
		//LOGDEVICE->write(_buffer - l, chunk_size);
		LOGARRAY_D("response", response.c_str(), response.length());
#endif

		uint8_t* encrypted = nullptr;
		int encryptedLen = 0;
		encrypted = HAPEncryption::encrypt((uint8_t*)response.c_str(), response.length(), &encryptedLen, hapClient->encryptionContext.encryptKey, hapClient->encryptionContext.encryptCount++);
		if (encryptedLen == 0) {
			LOG_E("ERROR: Encrpyting response failed!\n");

			hapClient->clear();
			return false;
		} else {
			// LogD(F("OK"), true);
		}

		WriteBufferingClient bufferedWifiClient{hapClient->client, HAP_BUFFER_CLIENT_SIZE};
		size_t bytesSend = bufferedWifiClient.write(encrypted, encryptedLen);
		bufferedWifiClient.flush();  // <- OPTIONAL

		free(encrypted);

		if (bytesSend == encryptedLen) return true;
	}

	else if (mode == HAPEncryptionMode::EncryptChunked) {
#else
	if (mode == HAPEncryptionMode::EncryptChunked || mode == HAPEncryptionMode::Encrypt) {
#endif
		uint8_t buf[HAP_PRINT_ENCRYPTED_BUFFER_SIZE];
		HAPPrintEncrypted eStream(hapClient->client, buf, HAP_PRINT_ENCRYPTED_BUFFER_SIZE);

		eStream.setEncryptCount(hapClient->encryptionContext.encryptCount);
		eStream.setKey(hapClient->encryptionContext.encryptKey);

		// eStream.setPayloadSize(jsonLength);

		eStream.begin();

		eStream.print(httpStatus);

		// Content-Type
		eStream.print(F("Content-Type: "));
		eStream.print(contentType);
		eStream.print(HTTP_CRLF);


		// Keep Alive
		if ( httpStatus != EVENT_200 ) {
			eStream.print( HTTP_KEEP_ALIVE );
		}

		eStream.print( HTTP_TRANSFER_ENCODING );
		eStream.print(HTTP_CRLF);

		// end header
		eStream.endHeader();

		// Body start
		serializeJson(doc, eStream);

		eStream.end();
		hapClient->encryptionContext.encryptCount = eStream.encryptCount();
	}

#if 0
	else if (mode == HAPEncryptionMode::PlainChunked) {
		uint8_t buffer[HAP_BUFFER_CLIENT_SIZE];
		HAPPrintChunked chunk(hapClient->client, buffer, HAP_BUFFER_CLIENT_SIZE);

		chunk.print(HTTP_200);

		// Content-Type
		chunk.print(F("Content-Type: "));
		chunk.print(contentType);
		chunk.print(HTTP_CRLF);

		// Keep Alive
		if ( httpStatus != EVENT_200 ) {
			chunk.print( HTTP_KEEP_ALIVE );
		}

		// Encoding chunked
		chunk.print( HTTP_TRANSFER_ENCODING );

		// end header
		chunk.print(HTTP_CRLF);


		// begin 1st chunk
		chunk.begin();

		// Body start
		serializeJson(doc, chunk);

		// end last chunk
		chunk.end();

	}

	else if (mode == HAPEncryptionMode::Plain){
		WriteBufferingClient bufferedWifiClient{hapClient->client, HAP_BUFFER_CLIENT_SIZE};

		size_t jsonLength = measureJson(doc);
		bufferedWifiClient.print(HTTP_200);

		// Content-Type
		bufferedWifiClient.print(F("Content-Type: "));
		bufferedWifiClient.print(contentType);
		bufferedWifiClient.print(HTTP_CRLF);

		// Keep Alive
		if ( httpStatus != EVENT_200 ) {
			bufferedWifiClient.print( HTTP_KEEP_ALIVE );
		}

		// Encoding chunked
		bufferedWifiClient.print(F("Content-Length: ") );
		bufferedWifiClient.print(F(jsonLength));

		// end header
		bufferedWifiClient.print(HTTP_CRLF);

		serializeJson(doc, bufferedWifiClient);
		bufferedWifiClient.print(HTTP_CRLF);

		bufferedWifiClient.print(HTTP_CRLF);

		bufferedWifiClient.flush();  // <- OPTIONAL
	}
#endif

	hapClient->clear();
	return 0;
}

bool HAPServer::send204(HAPClient* hapClient){
	LOG_V("send204\n");
	// LogD(F("\nEncrpyting response ..."), false);

	uint8_t* encrypted = nullptr;
	int encryptedLen = 0;
	encrypted = HAPEncryption::encrypt((uint8_t*)HTTP_204, strlen(HTTP_204), &encryptedLen, hapClient->encryptionContext.encryptKey, hapClient->encryptionContext.encryptCount++);
    if (encryptedLen == 0) {
    	LOG_E("ERROR: Encrpyting response failed!\n");

		hapClient->clear();
    	return false;
    } else {
		// LogD(F("OK"), true);
    }

// #if defined( ARDUINO_ARCH_ESP32 )
// 	LogD("\n>>> Sending " + String(encryptedLen) + " bytes encrypted response to client [" + hapClient->client.remoteIP().toString() + "] ...", false);
// #elif defined( CORE_TEENSY )
// 	LogD(F("\n>>> Sending ") + String(encryptedLen) + F(" bytes encrypted response to client ["), false);
// 	LOGDEVICE->print(hapClient->client.remoteIP());
// 	LogD("] ...", false);
// #endif

	WriteBufferingClient bufferedWifiClient{hapClient->client, 64};
	size_t bytesSend = bufferedWifiClient.write(encrypted, encryptedLen);
	bufferedWifiClient.flush();  // <- OPTIONAL

	free(encrypted);

	if (bytesSend == encryptedLen){
		// LogD( F("OK"), true);
	} else {
		LOG_E("ERROR: Could not send all bytes %d/%d\n", bytesSend, encryptedLen);
		return false;
	}
	return true;
}


/**
 * @brief Sends a TLV8 formatted response to the hapClient
 * 		  Used for pairing and verify
 *
 * // ToDo: Check if buffer is required!
 * // ToDo: Move to hapClient
 *
 * @param hapClient Pointer to hapClient
 * @param response  The tlv8 response to send
 * @param chunked 	Seend in chunked mode
 * @return true     Return true on success
 * @return false 	Return false on error
 */
void HAPServer::sendResponse(HAPClient* hapClient, TLV8* response){

	// uint8_t tlvdata[response->size()];
	// size_t outlen = 0;
	// response->decode(tlvdata, &outlen);
	// send(hapClient, HTTP_200, tlvdata, outlen, HAP_ENCRYPTION_MODE_PLAIN_CHUNKED, "application/pairing+tlv8");

	uint8_t buffer[HAP_BUFFER_CLIENT_SIZE];
	HAPPrintChunked chunk(hapClient->client, buffer, HAP_BUFFER_CLIENT_SIZE);

	chunk.print(HTTP_200);

	// Content-Type
	chunk.print( HTTP_CONTENT_TYPE_TLV8 );

	// Keep Alive
	chunk.print( HTTP_KEEP_ALIVE );

	// Encoding chunked
	chunk.print( HTTP_TRANSFER_ENCODING );

	// end header
	chunk.print(HTTP_CRLF);


	// begin 1st chunk
	chunk.begin();

	uint8_t tlvdata[response->size()];
	size_t outlen = 0;
	response->decode(tlvdata, &outlen);

	size_t bytesSent = chunk.write(tlvdata, outlen);

	// end last chunk
	chunk.end();

	LOG_D("Sent %d/%d bytes.\n", bytesSent, outlen);

	response->clear();

	hapClient->clear();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::handlePairSetupM1(HAPClient* hapClient){

	LOG_I("Homekit PIN: %s\n", _accessorySet->pinCode());


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + F("] -> /pair-setup Step 1/4 ..."), false);
#elif defined( CORE_TEENSY )
	// LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] -> /pair-setup Step 1/4 ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pair-setup Step 1/4 ...");
#endif


	_eventManager.queueEvent(EventManager::kEventPairingStep1, HAPEvent());

	_isInPairingMode = true;
	TLV8 response;

	// generate keys if not stored
	if (!isPaired()) {
		// _longTermContext = (struct HAPLongTermContext*) calloc(1, sizeof(struct HAPLongTermContext));
		// if (_longTermContext == NULL) {
		// 	LogE( F("[ERROR] Initializing struct _longTermContext failed!"), true);
		// 	return false;
		// }


		// ToDo: Saving only LTSK = not the private Key but some random nummbers...
		// _longTermContext->LTSK = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_BYTES);
		// _longTermContext->LTSKLength = ED25519_BYTES;


		// size_t olen;
		// mbedtls_hardware_poll(NULL, _longTermContext->LTSK, ED25519_BYTES, &olen);


#if defined( CORE_TEENSY )

		for (int i = 0; i < ED25519_BYTES; i++){
			_accessorySet->LTSK()[i] = (uint8_t)Entropy.random(255);

			// uint32_t u32 = Entropy.random();

			// _accessorySet->LTSK()[i + 3] = (uint8_t)u32;
    		// _accessorySet->LTSK()[i + 2] = (uint8_t)(u32 >>= 8);
    		// _accessorySet->LTSK()[i + 1] = (uint8_t)(u32 >>= 8);
    		// _accessorySet->LTSK()[i + 0] = (uint8_t)(u32 >>= 8);
			// i = i + 3;
		}
#else
		esp_fill_random(_accessorySet->LTSK(), ED25519_BYTES);
#endif

		// _longTermContext->privateKey = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_PRIVATE_KEY_LENGTH);
		// _longTermContext->privateKeyLength = ED25519_PRIVATE_KEY_LENGTH;


		// _longTermContext->publicKey = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_PUBLIC_KEY_LENGTH);
		// _longTermContext->publicKeyLength = ED25519_PUBLIC_KEY_LENGTH;

		ed25519_key_generate(_accessorySet->LTPK(), _accessorySet->LTSK());




#if HAP_DEBUG_HOMEKIT
		LOGARRAY_D("LTPK", _accessorySet->LTPK(), ED25519_PUBLIC_KEY_LENGTH);
		LOGARRAY_D("LTSK", _accessorySet->LTSK(), ED25519_PRIVATE_KEY_LENGTH);
#endif
	}

 	LOG_V("Initializing srp ...");

	// ToDo: Check new HAPSRP
	if (_hapsrp == nullptr) _hapsrp = new HAPSRP();

	_hapsrp->begin("Pair-Setup");

	if (_hapsrp->data->session != NULL) delete _hapsrp->data->session;

	_hapsrp->data->session = _hapsrp->newSession(HAPHashAlgorithm::Sha512, HAPSRP::SRP_NG_3072, NULL,NULL);
	// _hapsrp->data->session = session;

#if HAP_DEBUG_SRP
	HAPHelper::mpi_print("N", &_hapsrp->data->session->ng->N);
	HAPHelper::mpi_print("g", &_hapsrp->data->session->ng->g);
#endif

	LOGRAW_V("OK\n");


	LOG_V("Create srp verification key ...");
	_hapsrp->data->len_s = SRP_SALT_LENGTH;
	_hapsrp->createSaltedVerificationKey1(_hapsrp->data->session,
										"Pair-Setup",
										(const uint8_t*)_accessorySet->pinCode(), strlen(_accessorySet->pinCode()),
										&_hapsrp->data->bytes_s, _hapsrp->data->len_s,
										&_hapsrp->data->bytes_v, &_hapsrp->data->len_v
										);

	if (_hapsrp->data->bytes_s == NULL || _hapsrp->data->bytes_v == NULL) {
		LOGRAW_E("Failed to create SRP verifier!\n");

		// srp_session_delete(_srp->session);

		sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorType::Unknown);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		return false;
	}
	LOGRAW_V("OK\n");

#if HAP_DEBUG_SRP
	LOGARRAY_D("salt", _hapsrp->data->bytes_s, _hapsrp->data->len_s);
#endif

	if (_hapsrp->data->keys == NULL) {
		LOG_V("Calculating srp public key ...");
		_hapsrp->data->keys = _hapsrp->createKeyPair(_hapsrp->data->session, _hapsrp->data->bytes_v, _hapsrp->data->len_v, &_hapsrp->data->bytes_B, &_hapsrp->data->len_B);
		if (_hapsrp->data->keys == NULL) {

			LOGRAW_E("Failed to calculate srp public key!\n");
			sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorType::Unknown);

			if (_hapsrp) delete _hapsrp;
			_hapsrp = nullptr;
			return false;
		}
		LOGRAW_V("OK\n");
	}

#if HAP_DEBUG_SRP
	LOGARRAY_D("pubKey", _hapsrp->data->bytes_B, _hapsrp->data->len_B);
#endif

	LOG_V("Sending response ...");

	response.encode(HAPTLVType::State, 1, HAPPairingState::M2);
	response.encode(HAPTLVType::Salt, SRP_SALT_LENGTH, _hapsrp->data->bytes_s);
	response.encode(HAPTLVType::PublicKey, SRP_PUBLIC_KEY_LENGTH, _hapsrp->data->bytes_B);

	sendResponse(hapClient, &response);

	response.clear();
	hapClient->clear();
	LOGRAW_V("OK\n");


	LOGRAW_D("OK\n");

	return true;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::handlePairSetupM3(HAPClient* hapClient) {


#if defined( ARDUINO_ARCH_ESP32 )
	LogD( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-setup Step 2/4 ...", false);
#elif defined( CORE_TEENSY )
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pair-setup Step 2/4 ...");
#endif

	_eventManager.queueEvent(EventManager::kEventPairingStep3, HAPEvent());

	LOG_V("Decoding TLV ...");

	size_t decodedLen = 0;
	uint8_t device_public_key[hapClient->request.tlv.size(HAPTLVType::PublicKey)];

	hapClient->request.tlv.decode(HAPTLVType::PublicKey, device_public_key, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E("ERROR: Invalid payload: no client public key!\n");

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAPPairingState::M4, HAPErrorType::Authentication);

		return false;
	}
	LOGRAW_V("OK\n");


	LOG_V("Generating proof ...");
#if HAP_DEBUG_SRP
	LOGARRAY_D("v", _hapsrp->data->bytes_v, _hapsrp->data->len_v);
#endif

	_hapsrp->data->verifier = _hapsrp->newVerifier1(_hapsrp->data->session,
											_hapsrp->data->username,
											true,
											_hapsrp->data->bytes_s,
											_hapsrp->data->len_s,
											_hapsrp->data->bytes_v,
											_hapsrp->data->len_v,
											device_public_key, decodedLen,
											NULL, NULL, _hapsrp->data->keys
										);




    uint8_t proof[hapClient->request.tlv.size(HAPTLVType::Proof)];
	hapClient->request.tlv.decode(HAPTLVType::Proof, proof, &decodedLen);

	if (decodedLen == 0) {
    	LOGRAW_E("ERROR: Invalid payload: no device proof");

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAPPairingState::M4, HAPErrorType::Authentication);
    	return false;
    }

	LOGRAW_V("OK\n");


	LOG_V("Verifying device proof ...");
	if (decodedLen != _hapsrp->length(_hapsrp->data->session) ){

		LOGRAW_E("ERROR: Client SRP proof does not match session hash length: %d/%d\n", decodedLen, _hapsrp->length(_hapsrp->data->session));

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAPPairingState::M4, HAPErrorType::Authentication);
		return false;
	}

#if HAP_DEBUG_SRP
	// Server keys
	HAPHelper::mpi_print("_hapsrp->data->keys->B = PubKey", &(_hapsrp->data->keys->B));
	HAPHelper::mpi_print("_hapsrp->data->keys->b = Private Key ?", &(_hapsrp->data->keys->b));

	LOGARRAY_D("_hapsrp->data->bytes_v", _hapsrp->data->bytes_v, _hapsrp->data->len_v);

	// salt
	LOGARRAY_D("_hapsrp->data->bytes_s = SALT", (uint8_t*)_hapsrp->data->bytes_s, _hapsrp->data->len_s);
	LOGARRAY_D("_hapsrp->data->bytes_b", (uint8_t*)_hapsrp->data->bytes_B, _hapsrp->data->len_B);

	// proof
	LOGARRAY_D("_hapsrp->data->verifier->M", _hapsrp->data->verifier->M, SHA512_DIGEST_LENGTH);
	LOGARRAY_D("proof", proof, SHA512_DIGEST_LENGTH);


	LOGARRAY_D("_hapsrp->data->username", (uint8_t*)_hapsrp->data->username, strlen(_hapsrp->data->username));
	LOGARRAY_D("password", (uint8_t*)_accessorySet->pinCode(), strlen(_accessorySet->pinCode()));
#endif

	if (_hapsrp->verifySession(_hapsrp->data->verifier, proof, NULL) == false) {
        LOGRAW_E("ERROR: Failed to verify client SRP proof!\n");

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAPPairingState::M4, HAPErrorType::Authentication);
        return false;
	}

    LOGRAW_V("OK\n");



    LOG_V("Generating accessory proof ...");

	const uint8_t *acc_srp_proof = _hapsrp->getVerifierHAMK(_hapsrp->data->verifier);
	if (acc_srp_proof == NULL) {
		LOGRAW_E("ERROR: srp_verifier_get_HAMK failed!\n");

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAPPairingState::M4, HAPErrorType::Authentication);
        return false;
	}
	LOGRAW_V("OK\n");

    LOG_V("Sending response ...");
	TLV8 response;
    response.encode(HAPTLVType::State, 1, HAPPairingState::M4);
    response.encode(HAPTLVType::Proof, SRP_PROOF_LENGTH, acc_srp_proof);

#if HAP_DEBUG_TLV8
	response.print();
#endif

	sendResponse(hapClient, &response);
	LOGRAW_V("OK\n");

	response.clear();

	hapClient->clear();

	LOGRAW_D("OK\n");
    return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::handlePairSetupM5(HAPClient* hapClient) {

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-setup Step 3/4 ...", false);
#elif defined( CORE_TEENSY )
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pair-setup Step 3/4 ...");
#endif

	_eventManager.queueEvent(EventManager::kEventPairingStep3, HAPEvent());


	int err_code = 0;
	TLV8 response;

	LOG_V("Getting verifier session key ...");

	int srp_key_length = 0;
	uint8_t srp_key[_hapsrp->getVerifierSessionKeyLength(_hapsrp->data->verifier)] = {0,};

	memcpy(srp_key, _hapsrp->getVerifierSessionKey(_hapsrp->data->verifier, &srp_key_length), srp_key_length);
	if (srp_key_length == 0) {
		LOGRAW_E("ERROR: Failed to get verifier session key!\n");

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;

		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
		return false;
	}
	LOGRAW_V("OK\n");


	LOG_V("Clearing SRP ...");
	if (_hapsrp != nullptr) {
		delete _hapsrp;
		_hapsrp = nullptr;
	}
	LOGRAW_V("OK\n");

    LOG_V("Decoding TLV values ...");
	// uint8_t *encrypted_tlv = hapClient->request.tlv.decode(HAPTLVType::EncryptedData);

	size_t decodedLen = 0;
	size_t encryptedTLVLen = hapClient->request.tlv.size(HAPTLVType::EncryptedData);

	uint8_t encryptedTLV[encryptedTLVLen] = {0,};
	hapClient->request.tlv.decode(HAPTLVType::EncryptedData, encryptedTLV, &decodedLen);


	if (decodedLen == 0) {
        LOGRAW_E("ERROR: Decrypting TLV failed!\n");

		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
    	return false;
    }
    LOGRAW_V("OK\n");

    LOG_V("Get HKDF key ...");
	uint8_t subtlv_key[HKDF_KEY_LEN] = {0,};
	err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_ENCRYPT, srp_key, SRP_SESSION_KEY_LENGTH, subtlv_key);

	if (err_code != 0) {
        LOGRAW_E("ERROR: Failed to get HKDF key\n");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
        return false;
    }
    LOGRAW_V("OK\n");

    LOG_V("Decrypting chacha20_poly1305 ...");
    // uint8_t *subtlv = (uint8_t*) malloc(sizeof(uint8_t) * encrypted_tlv_len);

	size_t decryptedLen = encryptedTLVLen - CHACHA20_POLY1305_AUTH_TAG_LENGTH;
	uint8_t subtlv[decryptedLen];

#if HAP_DEBUG_HOMEKIT
	LOGARRAY_D("subtlv_key", subtlv_key, HKDF_KEY_LEN);
	LOGARRAY_D("encryptedTLV", encryptedTLV, encryptedTLVLen);
#endif
	err_code = chacha20_poly1305_decrypt(CHACHA20_POLY1305_TYPE_PS05, subtlv_key, NULL, 0, encryptedTLV, encryptedTLVLen, subtlv);

    if (err_code != 0) {
        LOGRAW_E("ERROR: Decrypting CHACHA20_POLY1305_TYPE_PS05 failed! Reason: %d\n");

#if HAP_DEBUG_HOMEKIT
		LOGARRAY_D("subtlv", subtlv, decryptedLen);
#endif
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
        return false;
    }
    LOGRAW_V("OK\n");

#if HAP_DEBUG_HOMEKIT
	LOGARRAY_D("subtlv", subtlv, decryptedLen);
#endif

	TLV8 encTLV;
	encTLV.encode(subtlv, decryptedLen);

#if HAP_DEBUG_TLV8
	encTLV.print();
#endif

    uint8_t ios_devicex[HKDF_KEY_LEN];
    hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_CONTROLLER, srp_key, SRP_SESSION_KEY_LENGTH, ios_devicex);

    uint8_t ios_device_pairing_id_len 	= encTLV.size(HAPTLVType::Identifier);
	uint8_t ios_device_pairing_id[ios_device_pairing_id_len];
	encTLV.decode(HAPTLVType::Identifier, ios_device_pairing_id, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E("ERROR: TLV decoding identifier failed\n");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		encTLV.clear();
		response.clear();
		return false;
	}

	uint8_t  ios_device_ltpk_len 		= encTLV.size(HAPTLVType::PublicKey);
	uint8_t ios_device_ltpk[ios_device_ltpk_len];

	encTLV.decode(HAPTLVType::PublicKey, ios_device_ltpk, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E("ERROR: TLV decoding public key failed\n");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		encTLV.clear();
		response.clear();
		return false;
	}

    uint8_t  ios_device_signature_len 	= encTLV.size(HAPTLVType::Signature);
	uint8_t ios_device_signature[ios_device_signature_len];
	encTLV.decode(HAPTLVType::Signature, ios_device_signature, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E("ERROR: TLV decoding signature failed\n");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);

		LOGARRAY_E("signature:", ios_device_signature, ios_device_signature_len);


#if HAP_DEBUG_TLV8
		encTLV.print();
#endif

		encTLV.clear();
		response.clear();
		return false;
	}


    int ios_device_info_len = 0;
    uint8_t* ios_device_info = concat3(ios_devicex, sizeof(ios_devicex),
            ios_device_pairing_id, ios_device_pairing_id_len,
            ios_device_ltpk, ios_device_ltpk_len,
            &ios_device_info_len);


    LOG_V("Verifying ED25519 ...");
	int verified = ed25519_verify(ios_device_signature, ios_device_info, ios_device_info_len, ios_device_ltpk);



    concat_free(ios_device_info);

	if (verified < 0) {
        LOGRAW_E("ERROR: Verification failed\n");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		encTLV.clear();
		response.clear();
        return false;
	}
	LOGRAW_V("OK\n");



	encTLV.clear();

	LOG_HEAP(_clients.size(), _eventManager.getNumEventsInQueue());

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-setup Step 4/4 ...", true);
#elif defined( CORE_TEENSY )
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pair-setup Step 4/4 ...");
#endif

	_eventManager.queueEvent(EventManager::kEventPairingStep4, HAPEvent());


	//  _acc_m6_subtlv(srp_key, ps->acc_id, ps->keys.public, ps->keys.private, &acc_subtlv, &acc_subtlv_length);
	uint8_t accessoryx[HKDF_KEY_LEN] = {0,};
	hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_ACCESSORY, srp_key, SRP_SESSION_KEY_LENGTH,
            accessoryx);


    int acc_info_len = 0;

	char deviceId[18] = {'\0', };
	HAPDeviceID::deviceID(deviceId);

    uint8_t* acc_info = concat3(accessoryx, sizeof(accessoryx),
            (uint8_t*)deviceId, 17,
            _accessorySet->LTPK(), ED25519_PUBLIC_KEY_LENGTH, &acc_info_len);

    LOG_V("Verifying signature ...");
    uint8_t acc_signature[ED25519_SIGN_LENGTH] = {0,};

	ed25519_sign(acc_signature, acc_info, acc_info_len, _accessorySet->LTSK(), _accessorySet->LTPK());

    concat_free(acc_info);

    if (err_code != 0) {
        LOGRAW_E("ERROR: Verify signature failed! Reason: %d\n", err_code);
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
        return false;
	}
	LOGRAW_V("OK\n");


	// Encrypt data
	TLV8 subTLV;
	subTLV.encode(HAPTLVType::Identifier, 17, (uint8_t*)deviceId );
	subTLV.encode(HAPTLVType::PublicKey, ED25519_PUBLIC_KEY_LENGTH, _accessorySet->LTPK());
	subTLV.encode(HAPTLVType::Signature, ED25519_SIGN_LENGTH, acc_signature);


	size_t tlv8Len = subTLV.size();
	uint8_t tlv8Data[tlv8Len];
	size_t written = 0;

	subTLV.decode(tlv8Data, &written);
	if (written == 0) {
		LOGRAW_E("ERROR: Failed to decode subtlv8!\n");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
		subTLV.clear();
		return false;
	}



#if HAP_DEBUG_TLV8
	subTLV.print();
	//HAPHelper::arrayPrint(tlv8Data, tlv8Len);
#endif

	subTLV.clear();

	uint8_t encryptedData[tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH];

	LOG_V("Getting session key ...");
	err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_ENCRYPT, srp_key, SRP_SESSION_KEY_LENGTH, subtlv_key);
	if (err_code != 0) {
        LOGRAW_E("ERROR: Getting session key failed! Reason %d\n", err_code);
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();
        return false;
	}
	LOGRAW_V("OK\n");


	LOG_V("Encrypting Data ...");
	err_code = chacha20_poly1305_encrypt(CHACHA20_POLY1305_TYPE_PS06, subtlv_key, NULL, 0, tlv8Data, tlv8Len, encryptedData);

	if (err_code != 0) {
        LOGRAW_E("ERROR: Verify signature failed");
		sendErrorTLV(hapClient, HAPPairingState::M6, HAPErrorType::Authentication);
		response.clear();

        return false;
	}
	LOGRAW_V("OK\n");


	LOG_V("Sending response ...");
	response.encode(HAPTLVType::State, 1, HAPPairingState::M6);
	response.encode(HAPTLVType::EncryptedData, tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH, encryptedData);

#if HAP_DEBUG_TLV8
	response.print();
#endif

	sendResponse(hapClient, &response);
	LOGRAW_V("OK\n");

	response.clear();


	// Save to Pairings as admin
	LOG_V("Saving pairing ...");
	_accessorySet->addPairing(ios_device_pairing_id, ios_device_ltpk, true);
	LOGRAW_V("OK\n");


	LOG_V( "Updating mDNS ...");
#if defined (ARDUINO_ARCH_ESP32)
	mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber);
	LOGRAW_V("OK\n");
#else
	if ( !updateServiceTxt() ){
		LOG_E("ERROR: Updating mDNS service failed!\n");
	}
#endif

	LOGRAW_D("OK\n");
#if defined( ARDUINO_ARCH_ESP32 )
	LogI("Pairing with client [" + hapClient->client.remoteIP().toString() + "] complete!", true);
#elif defined( CORE_TEENSY )
	LOG_I("Pairing with client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_I("] -> complete!\n");
#endif

	// ToDo: set timeout for resetting to false automatically?
	_isInPairingMode = false;

	_eventManager.queueEvent(EventManager::kEventPairingComplete, HAPEvent());


	hapClient->clear();

	LOG_HEAP(_clients.size(), _eventManager.getNumEventsInQueue());

    return true;
}

/**
 * handlePairVerifyM1
 */
bool HAPServer::handlePairVerifyM1(HAPClient* hapClient){

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-verify Step 1/2 ...", false);
#elif defined( CORE_TEENSY )
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pair-verify Step 1/2 ...\n");
#endif

	// _eventManager.queueEvent(EventManager::kEventVerifyStep1, HAPEvent());

	int err_code = 0;

	if ( !isPaired() ) {
		LOG_E("ERROR: Attempt to verify unpaired accessory!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Unknown);

		hapClient->clear();
		return false;
	}

	hapClient->encryptionContext.decryptCount = 0;
	hapClient->encryptionContext.encryptCount = 0;

	LOG_V("Generating accessory curve25519 keys ...");

	uint8_t acc_curve_public_key[CURVE25519_SECRET_LENGTH] = {0,};		// my_key_public

	// Create new public key from accessory's LTSK
	err_code = X25519_scalarmult_base(acc_curve_public_key, _accessorySet->LTSK());
	if (err_code < 0) {
		LOGRAW_E("ERROR: X25519_scalarmult_base failed! Reason: %d\n", err_code);
		sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Unknown);

		hapClient->clear();
		return false;
	}
	else {
		LOGRAW_V("OK\n");
	}

	uint8_t ios_device_curve_key_len = hapClient->request.tlv.size(HAPTLVType::PublicKey);
	if (ios_device_curve_key_len == 0){
		LOGRAW_E("ERROR: ios_device_curve_key_len is 0!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Unknown);

		hapClient->clear();
		return false;
	}


	uint8_t ios_device_curve_key[ios_device_curve_key_len];
	size_t decodedLen = 0;
	hapClient->request.tlv.decode(HAPTLVType::PublicKey, ios_device_curve_key, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E( "ERROR: PAIR-VERIFY M1 - HAPTLVType::EncryptedData failed!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Authentication);

		hapClient->clear();
		return false;
	}

#if HAP_DEBUG_HOMEKIT

	LOGARRAY_D("acc_curve_public_key", acc_curve_public_key, CURVE25519_SECRET_LENGTH);
	LOGARRAY_D("_accessorySet->LTSK()", _accessorySet->LTSK(), HAP_PAIRINGS_LTSK_LENGTH);
	LOGARRAY_D("ios_device_curve_key", ios_device_curve_key, ios_device_curve_key_len);

#endif

	// shared_secret
	LOG_V("Generating Curve25519 shared secret ...");
	uint8_t sharedSecret[CURVE25519_SECRET_LENGTH] = {0,};

	if (X25519_scalarmult(sharedSecret, _accessorySet->LTSK(), ios_device_curve_key) < 0) {
		LOGRAW_E("ERROR: X25519_scalarmult failed!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Authentication);

		hapClient->clear();
		return false;
	}
	LOGRAW_V("OK\n");


	LOG_V("Generating signature ...");

	char deviceId[18] = {'\0', };
	HAPDeviceID::deviceID(deviceId);


	int acc_info_len;
	uint8_t* acc_info = concat3(acc_curve_public_key, CURVE25519_SECRET_LENGTH,
		(uint8_t*)deviceId, 17,
		ios_device_curve_key, ios_device_curve_key_len,
		&acc_info_len);


	uint8_t acc_signature[ED25519_SIGN_LENGTH] = {0,};
	ed25519_sign(acc_signature, acc_info, acc_info_len, _accessorySet->LTSK(), _accessorySet->LTPK());
	concat_free(acc_info);
	LOGRAW_V("OK\n");


	// Encrypt data

	LOG_V("Encoding into TLV ...");
	TLV8 *subTLV = new TLV8();
	subTLV->encode(HAPTLVType::Identifier, 17, (uint8_t*)deviceId  );
	subTLV->encode(HAPTLVType::Signature, ED25519_SIGN_LENGTH, acc_signature);

	size_t tlv8Len = subTLV->size();
	uint8_t tlv8Data[tlv8Len];
	size_t written = 0;

	subTLV->decode(tlv8Data, &written);
	if (written == 0) {
		LOGRAW_E("ERROR: Failed to decode subtlv8!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Authentication);

		subTLV->clear();
		delete subTLV;

		return false;
	}

#if HAP_DEBUG_TLV8
	subTLV->print();
	//HAPHelper::arrayPrint(tlv8Data, tlv8Len);
#endif
	LOGRAW_V("OK\n");


	LOG_V("Generating proof ...");
    uint8_t sessionKey[HKDF_KEY_LEN] = {0,};   		// session_key
    err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_VERIFY_ENCRYPT, sharedSecret, CURVE25519_SECRET_LENGTH, sessionKey);
	if (err_code != 0) {
        LOGRAW_E("ERROR: Get HKDF key failed! Reason: %d\n", err_code);
        sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Authentication);

		subTLV->clear();
		delete subTLV;

        return false;
	}
	LOGRAW_V("OK\n");



	LOG_V("Encrypting data ...");
	// uint8_t* encryptedData;
	// encryptedData = (uint8_t*)malloc(sizeof(uint8_t) * (tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH));
	uint8_t encryptedData[tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH];


	err_code = chacha20_poly1305_encrypt(CHACHA20_POLY1305_TYPE_PV02, sessionKey, NULL, 0, tlv8Data, tlv8Len, encryptedData);
	if (err_code != 0) {
        LOGRAW_E("ERROR: Encrypting failed! Reason: %d\n", err_code);
        sendErrorTLV(hapClient, HAPVerifyState::M2, HAPErrorType::Authentication);

		subTLV->clear();
		delete subTLV;

        return false;
	}
	LOGRAW_V("OK\n");

	LOG_V("Saving context ...");
	memcpy(hapClient->verifyContext.secret, sharedSecret, HKDF_KEY_LEN);
	memcpy(hapClient->verifyContext.accessoryLTPK, acc_curve_public_key, ED25519_PUBLIC_KEY_LENGTH);
	memcpy(hapClient->verifyContext.deviceLTPK, ios_device_curve_key, ED25519_PUBLIC_KEY_LENGTH);
	LOGRAW_V("OK\n");


	LOG_V("Sending response ...");
	TLV8 response;
	response.encode(HAPTLVType::State, 1, HAPVerifyState::M2);
	response.encode(HAPTLVType::PublicKey, CURVE25519_SECRET_LENGTH, acc_curve_public_key);
	response.encode(HAPTLVType::EncryptedData, tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH, encryptedData);


#if HAP_DEBUG_TLV8
	response.print();
	//HAPHelper::arrayPrint(encryptedData, tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH);
#endif


	sendResponse(hapClient, &response);
	LOGRAW_V("OK\n");
	response.clear();

	subTLV->clear();
	delete subTLV;

	hapClient->clear();

	LOGRAW_D("OK\n");
	return true;
}


bool HAPServer::handlePairVerifyM3(HAPClient* hapClient){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-verify Step 2/2 ...", false);
#elif defined( CORE_TEENSY )
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pair-verify Step 2/2 ...");
#endif



	// _eventManager.queueEvent(EventManager::kEventVerifyStep2, HAPEvent());

	int err_code = 0;

	int encryptedDataLen = hapClient->request.tlv.size(HAPTLVType::EncryptedData);
	size_t decodedLen = 0;

	uint8_t encryptedData[encryptedDataLen];
	hapClient->request.tlv.decode(HAPTLVType::EncryptedData, encryptedData, &decodedLen);


	if (decodedLen == 0) {
		LOGRAW_E("ERROR: PAIR-VERIFY M3 - HAPTLVType::EncryptedData failed!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		return false;
	}

	LOG_V("Generating decrpytion key ...");
	uint8_t subtlv_key[HKDF_KEY_LEN] = {0,};
	err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_VERIFY_ENCRYPT, hapClient->verifyContext.secret, HKDF_KEY_LEN, subtlv_key);
	if (err_code != 0) {
		LOGRAW_E("ERROR: PAIR-VERIFY M3 - hkdf_key_get failed!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		return false;
	}
	LOGRAW_V("OK\n");



	LOG_V("Decrypting data ...");
	size_t decryptedLen = encryptedDataLen - CHACHA20_POLY1305_AUTH_TAG_LENGTH;
	uint8_t subtlvData[decryptedLen] = {0,};

	// LOGDEVICE->println("chacha20_poly1305_decrypt");
	// LOGDEVICE->send_now();

#if HAP_DEBUG_HOMEKIT
	LOGARRAY_D("subtlv_key", subtlv_key, HKDF_KEY_LEN);
	LOGARRAY_D("encryptedData", encryptedData, decryptedLen);
	LOGARRAY_D("mac", encryptedData + decryptedLen, CHACHA20_POLY1305_AUTH_TAG_LENGTH);
#endif

	err_code = chacha20_poly1305_decrypt(CHACHA20_POLY1305_TYPE_PV03, subtlv_key, NULL, 0, encryptedData, encryptedDataLen, subtlvData);
	if (err_code != 0) {
		LOGRAW_E("ERROR: Decrypting failed: Reason: %d\n", err_code);

#if HAP_DEBUG_HOMEKIT
		// LOGARRAY_D("subtlv_key", subtlv_key, HKDF_KEY_LEN);
		// LOGARRAY_D("encryptedData", encryptedData, encryptedDataLen - 16);
		// LOGARRAY_D("mac", encryptedData + (encryptedDataLen - 16), 16);
		LOGARRAY_D("subtlvData", subtlvData, decryptedLen);
#endif

		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		return false;
	}
	LOGRAW_V("OK\n");

#if HAP_DEBUG_TLV8
	LOGARRAY_D("subtlvData", subtlvData, decryptedLen);
#endif

	TLV8 subTlv;
	subTlv.encode(subtlvData, decryptedLen);

// #if HAP_DEBUG
// 	LogD("subTLV: ", true);
// 	subTlv.print();
// #endif


	uint8_t ios_device_pairing_id_len 	= subTlv.size(HAPTLVType::Identifier);
	uint8_t ios_device_pairing_id[ios_device_pairing_id_len];
	subTlv.decode(HAPTLVType::Identifier, ios_device_pairing_id, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E("ERROR: HAPTLVType::Identifier failed!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		subTlv.clear();
		return false;
	}


#if HAP_DEBUG_HOMEKIT
	LOG_D("Looking up iOS device LTPK for client\n");
	LOGARRAY_D("ios_device_pairing_id", ios_device_pairing_id, ios_device_pairing_id_len);
#endif

	uint8_t* ios_device_ltpk = _accessorySet->getKeyForPairingWithId(ios_device_pairing_id);
	// err_code = _accessorySet->getPairings()->getKey(ios_device_pairing_id, ios_device_ltpk);

	if (err_code == -1) {
		LOGRAW_E( "ERROR: No iOS Device LTPK found!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		subTlv.clear();
		return false;
	}

#if HAP_DEBUG_HOMEKIT
	LOG_D("Found LTPK\n");
	LOGARRAY_D("ios_device_ltpk", ios_device_ltpk, ED25519_PUBLIC_KEY_LENGTH);
#endif

	uint8_t ios_device_signature_len = subTlv.size(HAPTLVType::Signature);

	uint8_t ios_device_signature[ios_device_signature_len];
	subTlv.decode(HAPTLVType::Signature, ios_device_signature, &decodedLen);

	if (decodedLen == 0) {
		LOGRAW_E( "ERROR: PAIR-VERIFY M3 - HAPTLVType::EncryptedData failed!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		subTlv.clear();
		return false;
	}

#if HAP_DEBUG_HOMEKIT
	LOG_D("Found Signature\n");
	LOGARRAY_D("ios_device_signature", ios_device_signature, ios_device_signature_len);
#endif

#if HAP_DEBUG_HOMEKIT
	LOG_D("pairingIdIsAdmin: %d\n", _accessorySet->pairingIdIsAdmin(ios_device_pairing_id));
#endif


	int ios_device_info_len = 0;
    uint8_t* ios_device_info = concat3(hapClient->verifyContext.deviceLTPK, ED25519_PUBLIC_KEY_LENGTH,
            ios_device_pairing_id, ios_device_pairing_id_len,
            hapClient->verifyContext.accessoryLTPK, ED25519_PUBLIC_KEY_LENGTH, &ios_device_info_len);



    LOG_V( "Verifying Signature ...");
	int verified = ed25519_verify(ios_device_signature, ios_device_info, ios_device_info_len, ios_device_ltpk);


    concat_free(ios_device_info);
	if (verified < 0) {
        LOGRAW_E( "ERROR: Signature verification failed\n");
        sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		subTlv.clear();
        return false;
	}

	LOGRAW_V("OK\n");


	LOG_V( "Generating session keys ...");
    err_code = hkdf_key_get(HKDF_KEY_TYPE_CONTROL_READ, hapClient->verifyContext.secret, CURVE25519_SECRET_LENGTH, hapClient->encryptionContext.encryptKey);
	if (err_code != 0) {
		LOGRAW_E( "ERROR: HKDF encrpytion key not available!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		subTlv.clear();
		return false;
	}



	err_code = hkdf_key_get(HKDF_KEY_TYPE_CONTROL_WRITE, hapClient->verifyContext.secret, CURVE25519_SECRET_LENGTH, hapClient->encryptionContext.decryptKey);
	if (err_code != 0) {
		LOGRAW_E( "ERROR: HKDF decryption key not available!\n");
		sendErrorTLV(hapClient, HAPVerifyState::M4, HAPErrorType::Authentication);
		subTlv.clear();
		return false;
	}
	LOGRAW_V("OK\n");

	// ToDo: FREE CONTEXT ??

	LOG_V("Sending response ...");
	TLV8 response;
	response.encode(HAPTLVType::State, 1, HAPVerifyState::M4);

#if HAP_DEBUG_TLV8
	response.print();
#endif

	sendResponse(hapClient, &response);
	LOGRAW_V("OK\n");

	subTlv.clear();
	response.clear();

	hapClient->clear();

	// following messages from this client will be encrypted
	hapClient->setEncryped(true);
	hapClient->setId(ios_device_pairing_id);
	hapClient->setAdmin(_accessorySet->pairingIdIsAdmin(ios_device_pairing_id));

	LOGRAW_D("OK\n");

#if defined( ARDUINO_ARCH_ESP32 )
	LogI("Verification with client [" + hapClient->client.remoteIP().toString() + "] complete!", true);
#elif defined( CORE_TEENSY )
	LOG_I("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_I("] -> Pair verify complete!\n");
#endif

	_eventManager.queueEvent(EventManager::kEventVerifyComplete, HAPEvent());

	return true;
}


void HAPServer::handleAccessories(HAPClient* hapClient) {


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( "Handle [" + hapClient->client.remoteIP().toString() + "] -> /accessories ...", false);
#elif defined( CORE_TEENSY )
	// LogV( "Handle [" + String(hapClient->client.remoteIP()) + "] -> /accessories ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /accessories ...");

#endif


#if HAP_DEBUG_HOMEKIT
	// LogD(_accessorySet->describe(), true);
	// serializeJson(doc, Serial);
#endif

	// DynamicJsonDocument doc(32768);
	// JsonArray jsonAccessories = doc.createNestedArray("accessories");
	// _accessorySet->toJson(jsonAccessories);

	// doc.shrinkToFit();

	// send(hapClient, HTTP_200, doc, HAP_ENCRYPTION_MODE_ENCRYPT, "application/hap+json");

	// String accD;
	// serializeJson(doc, accD);
	// sendEncrypt(hapClient, HTTP_200, _accessorySet->describe(), true);


	uint8_t buf[HAP_PRINT_ENCRYPTED_BUFFER_SIZE];
	HAPPrintEncrypted eStream(hapClient->client, buf, HAP_PRINT_ENCRYPTED_BUFFER_SIZE);

	eStream.setEncryptCount(hapClient->encryptionContext.encryptCount);
	eStream.setKey(hapClient->encryptionContext.encryptKey);

	eStream.begin();

	eStream.print(HTTP_200);

	// Content-Type
	eStream.print(F("Content-Type: "));
	eStream.print(F("application/hap+json"));
	eStream.print(HTTP_CRLF);

	// Keep Alive
	eStream.print( HTTP_KEEP_ALIVE );
	eStream.print( HTTP_TRANSFER_ENCODING );

	eStream.print(HTTP_CRLF);
	eStream.endHeader();

	_accessorySet->printTo(eStream);

	eStream.end();

	hapClient->encryptionContext.encryptCount = eStream.encryptCount();

	hapClient->state = HAPClientState::Idle;


	hapClient->clear();
	LOGRAW_D("OK\n");
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsList(HAPClient* hapClient){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings list ...", false);
#elif defined( CORE_TEENSY )
	//LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings list ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pairings list ...");
#endif

	if (hapClient->isAdmin() == false){
		LOG_E("ERROR: Non-Admin controllers are not allowed to call this method!\n");
		sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorType::Authentication);
		return;
	}

	TLV8 response;
	response.encode(HAPTLVType::State, 1, HAPPairingState::M2);

	// for (int i=0; i<_accessorySet->getPairings()->size(); i++){
	for (int i=0; i<_accessorySet->numberOfPairings(); i++){

#if HAP_HOMEKIT_PYTHON_COMPATIBLE == 0
		if (i > 0) {
			response.addSeperator();
		}
#endif

		// response.encode(HAPTLVType::Identifier, HAP_PAIRINGS_ID_LENGTH, _accessorySet->getPairings()->pairings[i].id);
		// response.encode(HAPTLVType::PublicKey, HAP_PAIRINGS_LTPK_LENGTH, _accessorySet->getPairings()->pairings[i].key);
		// response.encode(HAPTLVType::Permissions, 1, _accessorySet->getPairings()->pairings[i].isAdmin);

		response.encode(HAPTLVType::Identifier, HAP_PAIRINGS_ID_LENGTH, _accessorySet->getPairingAtIndex(i)->id);
		response.encode(HAPTLVType::PublicKey, HAP_PAIRINGS_LTPK_LENGTH, _accessorySet->getPairingAtIndex(i)->id);
		response.encode(HAPTLVType::Permissions, 1, _accessorySet->getPairingAtIndex(i)->isAdmin);
	}

#if HAP_DEBUG_TLV8
	response.print();
#endif

	//sendResponse(hapClient, &response);
	uint8_t data[response.size()];
	size_t length = 0;

	response.decode(data, &length);
	// sendEncrypt(hapClient, HTTP_200, data, length, true, "application/pairing+tlv8");
	send(hapClient, HTTP_200, data, length, HAPEncryptionMode::EncryptChunked, "application/pairing+tlv8");

	response.clear();

	hapClient->clear();

	hapClient->state = HAPClientState::Idle;

	LOGRAW_D("OK\n");
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsAdd(HAPClient* hapClient, const uint8_t* identifier, const uint8_t* publicKey, bool isAdmin){

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings add ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings add ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pairings add ...");
#endif


	if (hapClient->isAdmin() == false){
		LOG_E("ERROR: Non-Admin controllers are not allowed to call this method!\n");
		sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorType::Authentication);
		return;
	}

	TLV8 response;
	response.encode(HAPTLVType::State, 1, HAPPairingState::M2);

	if (_accessorySet->numberOfPairings() >= HAP_PAIRINGS_MAX) {
		response.encode(HAPTLVType::Error, 1, HAPErrorType::MaxPeers);
	} else {
		_accessorySet->addPairing(identifier, publicKey, isAdmin);
	}

#if HAP_DEBUG_TLV8
	response.print();
#endif

	// sendResponse(hapClient, &response);
	uint8_t data[response.size()];
	size_t length = 0;

	response.decode(data, &length);
	// sendEncrypt(hapClient, HTTP_200, data, length, true, "application/pairing+tlv8");
	send(hapClient, HTTP_200, data, length, HAPEncryptionMode::EncryptChunked, "application/pairing+tlv8");

	response.clear();

	hapClient->clear();

	hapClient->state = HAPClientState::Idle;
	LOGRAW_D("OK\n");
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsRemove(HAPClient* hapClient, const uint8_t* identifier){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings remove ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings remove ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pairings remove ...");
#endif


	bool removeItsOwnPairings = false;

	// id identifier is controllers id, then disonnect
	if (memcmp(identifier, hapClient->getId(), HAP_PAIRINGS_ID_LENGTH) == 0) {
		LOG_V("Remove its own pairing\n");
		removeItsOwnPairings = true;
	}

	// ToDo:
	// According to the spec, only admin controllers can delete pairings.
	//
	// But this will deny removing ones own pairings
	// i will allow this
	//
	if ( (hapClient->isAdmin() == false) && (removeItsOwnPairings == false) ) {
		LOG_E("ERROR: Non-Admin controllers are not allowed to call this method!");
		sendErrorTLV(hapClient, HAPPairingState::M2, HAPErrorType::Authentication);
		return;
	}

	TLV8 response;
	response.encode(HAPTLVType::State, 1, HAPPairingState::M2);


	// if not paired
	LOG_V("Removing pairings ...");
	_accessorySet->removePairing(identifier);
	LOGRAW_V("OK\n");

	// send response


#if HAP_DEBUG_TLV8
	response.print();
#endif

	uint8_t data[response.size()];
	size_t length = 0;

	response.decode(data, &length);
	//sendEncrypt(hapClient, HTTP_200, data, length, true, "application/pairing+tlv8");
	send(hapClient,  HTTP_200, data, length, HAPEncryptionMode::EncryptChunked, "application/pairing+tlv8");

	response.clear();


	if (removeItsOwnPairings) {
		hapClient->client.stop();
	}


	hapClient->clear();

	// tear down all other pairings if admin removed
	// and update mdns
	if (!_accessorySet->isPaired()) {

		hapClient->state = HAPClientState::AllPairingsRemoved;
		_eventManager.queueEvent(EventManager::kEventAllPairingsRemoved, HAPEvent());

		// update mdns
#if defined( ARDUINO_ARCH_ESP32)
		if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
			LogE( "ERROR: Updating HAP service txt failed!", true);
			return;
		}
#else
		if ( !updateServiceTxt() ){
			LOG_E("ERROR: Updating mDNS service failed!\n");
			return;
		}
#endif

	}

	LOGRAW_D("OK\n");
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsPost(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen){



#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> /pairings ...");
#endif


	TLV8 tlv;
	tlv.encode(bodyData, bodyDataLen);

#if HAP_DEBUG_TLV8
	tlv.print();
#endif

	// TLV8Entry *entry = tlv.searchType(HAPTLVType::Method); // 0x01
	// HAP_TLV_PAIR_TYPE method = (HAP_TLV_PAIR_TYPE) entry->value[0];

	TLV8Entry *entry = tlv.searchType(HAPTLVType::Method);
	HAPPairingType::Type method = (HAPPairingType::Type) entry->value[0];

	if (method == HAPPairingType::Add) {

		TLV8Entry *entryIdentifier = tlv.searchType(HAPTLVType::Identifier); // 0x01
		TLV8Entry *entryPublicKey = tlv.searchType(HAPTLVType::PublicKey); // 0x03
		TLV8Entry *entryAdmin = tlv.searchType(HAPTLVType::Permissions); // 0x0b

		handlePairingsAdd(hapClient, entryIdentifier->value, entryPublicKey->value, *(entryAdmin->value) );

	} else if (method == HAPPairingType::Remove) {
		TLV8Entry *entryIdentifier = tlv.searchType(HAPTLVType::Identifier); // 0x01
		handlePairingsRemove(hapClient, entryIdentifier->value);
	} else if (method == HAPPairingType::List) {
		handlePairingsList(hapClient);
	}

	tlv.clear();

	LOGRAW_D("OK\n");
}


void HAPServer::handleCharacteristicsGet(HAPClient* hapClient){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] -> GET /characteristics ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] -> GET /characteristics ...", false);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> GET /characteristics ...");
#endif

	std::string idStr = hapClient->request.params["id"];

	bool hasParamMeta = false;
	bool hasParamPerms = false;
	bool hasParamEvent = false;
	bool hasParamType = false;
	LOGRAW_D("OK\n");

	for (const auto &p : hapClient->request.params) {

#if HAP_DEBUG
    	LOG_V("param: %s - %s\n", p.first.c_str(), p.second.c_str());
#endif
		if (p.first == "meta" && p.second == "1"){
			hasParamMeta = true;
		} else if (p.first == "perms" && p.second == "1") {
			hasParamPerms = true;
		} else if (p.first == "ev" && p.second == "1"){
			hasParamEvent = true;
		} else if (p.first == "type" && p.second == "1"){
			hasParamType = true;
		}
	}


	//String result = "[";
	DynamicJsonDocument responseRoot(2048);
	JsonArray jsonCharacteristics = responseRoot.createNestedArray("characteristics");

	bool errorOccured = false;
	int32_t errorCode = 0;

	do {
		int curPos = 0;
		// int endIndex = idStr.indexOf(",");

		int endIndex = HAPHelper::indexOf(idStr, ",");


		if (endIndex == -1){
			endIndex = idStr.length();
		}

		std::string keyPair = idStr.substr(curPos, endIndex);

		// int equalIndex = keyPair.indexOf(".");
		int equalIndex = HAPHelper::indexOf(keyPair, ".");

		uint8_t aid = atoi(keyPair.substr(0, equalIndex).c_str());
		uint8_t iid = atoi(keyPair.substr(equalIndex + 1).c_str());

		JsonObject jsonCharacteristic = jsonCharacteristics.createNestedObject();
		jsonCharacteristic["aid"] = aid;

		HAPCharacteristicBase* characteristic = _accessorySet->getCharacteristic(aid, iid);
		if (characteristic) {
			if (characteristic->readable()){

				// characteristic->valueGetFunctionCall();

				characteristic->toJson(jsonCharacteristic, hasParamType, hasParamPerms, hasParamEvent, hasParamMeta);

			} else {
				jsonCharacteristic["iid"] = iid;
				jsonCharacteristic["status"] = HAPErrorCode::WriteOnlyRead;
				errorOccured = true;
			}

		} else {
			jsonCharacteristic["iid"] = iid;
			jsonCharacteristic["status"] = HAPErrorCode::ResourceNotFound;
			errorCode = -1;
			errorOccured = true;
		}

		if (endIndex < idStr.length()) {
			idStr = idStr.substr(endIndex + 1);
		} else {
			break;
		}

	} while ( idStr.length() > 0 );

	// String response;
	// serializeJson(responseRoot, response);

#if HAP_DEBUG_HOMEKIT
	serializeJson(responseRoot, *LOGDEVICE);
#endif

	if (errorCode == -1){
		// Accessory not found
		// sendEncrypt(hapClient, HTTP_400, response, true);
		send(hapClient, HTTP_400, responseRoot, HAPEncryptionMode::Encrypt);
	} else if (errorOccured == false) {
		// everything ok
		// String response;
		// serializeJson(responseRoot, response);
		// sendEncrypt(hapClient, HTTP_200, response, true);

		send(hapClient, HTTP_200, responseRoot, HAPEncryptionMode::Encrypt);
	} else if (errorOccured == true) {
		// partial ok
    	// sendEncrypt(hapClient, HTTP_207, response, true);
		send(hapClient, HTTP_207, responseRoot, HAPEncryptionMode::Encrypt);
	}

	LOGRAW_D("OK\n");
	hapClient->state = HAPClientState::Idle;

}

void HAPServer::handleCharacteristicsPut(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("Handle client [") + hapClient->client.remoteIP().toString() + "] ->  PUT /characteristics ...", true);
#elif defined( CORE_TEENSY )
	// LogV( F("Handle client [") + String(hapClient->client.remoteIP()) + "] ->  PUT /characteristics ...", true);
	LOG_D("Handle client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] -> PUT /characteristics ...");
#endif

	DynamicJsonDocument root(2048);
	DeserializationError error = deserializeJson(root, bodyData, bodyDataLen);

	if (error) {
    	LOG_E("ERROR: Parsing put characteristics request failed!\n");
		// ToDo Send Error Response to client
    	return;
  	}

#if HAP_DEBUG
	serializeJson(root, *LOGDEVICE);
	LOGDEVICE->println();
#endif

	int s = root[F("characteristics")].as<JsonArray>().size();
	DynamicJsonDocument responseRoot( JSON_ARRAY_SIZE(s) + JSON_OBJECT_SIZE(1) + s * JSON_OBJECT_SIZE(4) );
	JsonArray responseArray = responseRoot.createNestedArray(F("characteristics"));

	bool errorOccured = false;


	for( auto jc : root[F("characteristics")].as<JsonArray>()) {

    	int aid = jc[F("aid")].as<int>();
    	int iid = jc[F("iid")].as<int>();



		HAPCharacteristicBase* character = _accessorySet->getCharacteristic(aid, iid);

		JsonObject jsonNewChr = responseArray.createNestedObject();
		jsonNewChr[F("aid")] = aid;

		if (character) {

			bool isEvent = false;

			if (jc.as<JsonObject>().containsKey(F("ev"))){
				isEvent = true;
			}

			if (isEvent) {
				if (character->notifiable() ) {
					character->toJson(jsonNewChr);

					hapClient->subscribe(aid, iid, jc[F("ev")].as<bool>());

					if (jc[F("ev")].as<bool>()) {
						struct HAPEvent event = HAPEvent(hapClient, aid, iid);
						_eventManager.queueEvent( EventManager::kEventNotifyController, event);
					}

				} else {
					// char has no event permission
					LOG_W("WARNING: Resource notify not permitted for characteristic %d.%d\n", aid, iid);
					jsonNewChr[F("iid")] = iid;
					jsonNewChr[F("status")] = HAPErrorCode::NoNotification;
					errorOccured = true;
				}
			} else {

				if (character->writable() ) {
					character->valueFromString(jc["value"].as<const char*>());
					// Add to jsonCharacteristics array
					character->toJson(jsonNewChr, false, false, false, false, false);
				} else {
					LOG_W("WARNING: Resource not writable for characteristic %d.%d\n", aid, iid);
					jsonNewChr[F("iid")] = iid;
					jsonNewChr[F("status")] = HAPErrorCode::ReadOnlyWrite;
					errorOccured = true;
				}
			}

		} else {
			LOG_E("ERROR: Resource not found for characteristic %d.%d\n", aid, iid);
			jsonNewChr[F("iid")] = iid;
			jsonNewChr[F("status")] = HAPErrorCode::ResourceNotFound;
			errorOccured = true;
		}
	}

	if (errorOccured){
		// String response;
		// serializeJson(responseRoot, response);
		// sendEncrypt(hapClient, HTTP_207, response, false);
		send(hapClient, HTTP_207, responseRoot, HAPEncryptionMode::EncryptChunked);
	} else {
		// sendEncrypt(hapClient, HTTP_204, "", false);
		send(hapClient, HTTP_204, "", 0, HAPEncryptionMode::EncryptChunked);
	}

	hapClient->state = HAPClientState::Idle;
	LOGRAW_D("OK\n");

}

void HAPServer::handleEventUpdateConfigNumber( int eventCode, struct HAPEvent eventParam ){
	LOG_V("handleEventUpdateConfigNumber\n");
	_accessorySet->configurationNumber++;
	// updateServiceTxt();

	if (_homekitStarted) {
#if defined( ARDUINO_ARCH_ESP32)
		if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
			LogE( F("ERROR: Updating HAP service txt failed!"), true);
			return;
		}
#else
		if ( !updateServiceTxt() ){
			LOG_E("ERROR: Updating mDNS service failed!\n");
			return;
		}
	}

#endif
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleEventRebootNow(int eventCode, struct HAPEvent eventParam){
	LOG_C("*********************************************************\n");
	LOG_C("*                                                       *\n");
	LOG_C("*                 !!! Rebooting now !!!                 *\n");
	LOG_C("*                                                       *\n");
	LOG_C("*********************************************************\n");

	delay(2000);

#if defined( ARDUINO_ARCH_ESP32)
	ESP.restart();
#elif defined( CORE_TEENSY )
	// ToDo: Add reboot
	SCB_AIRCR = 0x05FA0004;

#endif

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleEventConfigReset(int eventCode, struct HAPEvent eventParam){

	LOG_I("Delete config!\n");

    _configuration.setDefaults();
    _configuration.save();

#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.blinkWithColor(CRGB::Orange, 5);
#endif
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleEventDeleteAllPairings(int eventCode, struct HAPEvent eventParam){

	LOG_I("Delete all pairings!\n");

	_accessorySet->removeAllPairings();

#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.blinkWithColor(CRGB::Red, 5);
#endif

#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
		LOG_E("ERROR: Advertising HAP service failed!\n");
		return;
	}
#else
	if ( !updateServiceTxt() ){
		LOG_E("ERROR: Updating mDNS service failed!\n");
		return;
	}
#endif
}

void HAPServer::processEvents(){

	uint16_t noOfEvents = _eventManager.getNumEventCodeInQueue(EventManager::kEventNotifyController, EventManager::kHighPriority) + _eventManager.getNumEventCodeInQueue(EventManager::kEventNotifyController, EventManager::kLowPriority);
	uint16_t noOfEventsToSend = 0;

	struct HAPEvent evParams[noOfEvents];

	while (!_eventManager.isEventQueueEmpty(EventManager::kHighPriority)){
		int evCode;
		struct HAPEvent evParam;
		if (_eventManager.popEvent(&evCode, &evParam, EventManager::kHighPriority)){
			if (evCode == EventManager::kEventNotifyController) {
				evParams[noOfEventsToSend++] = evParam;
			}
			_eventManager.processEvent(evCode, evParam);
		}
	}

	while (!_eventManager.isEventQueueEmpty(EventManager::kLowPriority)){
		int evCode;
		struct HAPEvent evParam;
		if (_eventManager.popEvent(&evCode, &evParam, EventManager::kLowPriority)){
			if (evCode == EventManager::kEventNotifyController) {
				evParams[noOfEventsToSend++] = evParam;
			}
			_eventManager.processEvent(evCode, evParam);
		}
	}

	if (_clients.size() > 0 && noOfEventsToSend > 0){
		LOG_V("Number of connected clients: %d\n", _clients.size());
		LOG_V("Number of notification events: %d\n", noOfEvents);

		for (auto& hapClient : _clients) {

			const size_t bufferSize = 1024;
			DynamicJsonDocument root(bufferSize);

			JsonArray jsonCharacteristics = root.createNestedArray("characteristics");

			bool isSubcribedToAtLeastOne = false;

			for (int i=0; i < noOfEventsToSend; i++){

				int aid = evParams[i].aid;
				int iid = evParams[i].iid;


				if ( hapClient->isSubscribed(aid, iid) ) {
					HAPCharacteristicBase* character = _accessorySet->getCharacteristic(aid, iid);

					if (character) {

						LOG_D("Handle event %d for accessory %d.%d\n", EventManager::kEventNotifyController, aid, iid);

						JsonObject chr = jsonCharacteristics.createNestedObject();
						chr["aid"] = aid;
						character->toJson(chr, false, false, false, false, false, false);

						// LOGDEVICE->print("event json: ");
						// serializeJsonPretty(chr, Serial);
						// LOGDEVICE->println();

						isSubcribedToAtLeastOne = true;
					} else {
						LOG_W("WARNING: Not notifiable event %d for accessory %d.%d\n", EventManager::kEventNotifyController, aid, iid);
					}
				}
			}

			// serializeJson(root, response);

			if (isSubcribedToAtLeastOne) {
#if HAP_DEBUG
				LOG_D("Event response: ");
				serializeJson(root, *LOGDEVICE);
				LOGRAW_D("\n");
#endif
				sendEvent(hapClient, root);
			}
		}
	}
}


bool HAPServer::sendEvent(HAPClient* hapClient, const JsonDocument& response){

	LOG_V("Send events\n");
#if defined( ARDUINO_ARCH_ESP32 )
	LogD("Sending event to client [" + hapClient->client.remoteIP().toString() + "] ...", false);
#elif defined( CORE_TEENSY )
	LOG_D("Sending event to client [");
	LOGDEVICE->print(hapClient->client.remoteIP());
	LOGRAW_D("] ...");

#endif

	if ( hapClient->client.connected() ){
		// sendEncrypt(hapClient, EVENT_200, response, true);
		send(hapClient, EVENT_200, response, HAPEncryptionMode::EncryptChunked);
		LOGRAW_D("OK\n");
		return true;
	} else {
		hapClient->state = HAPClientState::Disconnected;
	}

	LOG_W("WARNING: No client available to send the event to!");
	return false;

}

bool HAPServer::isPaired(){
	return _accessorySet->isPaired();
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::__setFirmware(const char* name, const char* version, const char* rev) {

	if (strlen(name) + 1 - 10 > MAX_FIRMWARE_NAME_LENGTH || strlen(version) + 1 - 10 > MAX_FIRMWARE_VERSION_LENGTH) {
		LOG_E("ERROR: Either the name or version string is too long\n");
		return;  // never reached, here for clarity
	}

	// Remove flags
	char ver[20];
	strncpy(ver, version + 5, strlen(version) - 5);
	ver[strlen(version) - 5] = '\0';
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::__setBrand(const char* brand) {

	if (strlen(brand) + 1 - 10 > MAX_BRAND_LENGTH) {
		LOG_E("ERROR: The brand string is too long\n");
		return;  // never reached, here for clarity
	}

	strncpy(_brand, brand + 5, strlen(brand) - 10);
	_brand[strlen(brand) - 10] = '\0';
}


HAPAccessorySet* HAPServer::getAccessorySet(){
	return _accessorySet;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::updateConfig(){
	// ToDo: Rewrite config update handling
	LOG_D("Updating configuration ...");

	// ToDo: Update plugins and reinit
	// for (auto& plugin : _plugins) {
    // 	plugin->setConfig(_config.config()["plugins"][plugin->name()]);
	// }

	LOGRAW_D("OK\n");
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleEventUpdatedConfig(int eventCode, struct HAPEvent eventParam){

	// ToDo: find error
	LOG_I("Handle update config event");
	const size_t capacity = HAP_ARDUINOJSON_BUFFER_SIZE;
    DynamicJsonDocument doc(capacity);

#if HAP_DEBUG_CONFIG
	LOGDEVICE->println("before merging:");
	_configuration.toJson(*LOGDEVICE);
#endif

	LOGRAW_D("OK\n");

// 	// HAPHelper::mergeJson(doc, _config.config());
// 	// doc.remove("plugins");

// 	JsonObject plugins = doc.createNestedObject("plugins");

//   	for (auto & plugin : _plugins) {
// 		//if (plugin->isEnabled()) {
//         	plugins[plugin->name()] = plugin->getConfig();
// 		//}
// 	}

// 	_config.setConfig(doc);

// #if HAP_DEBUG_CONFIG
// 	LOGDEVICE->println("after merging:");
// 	_config.prettyPrintTo(Serial);
// #endif

// 	_config.save();

	updateConfig();
}


#if HAP_DEBUG && HAP_WEBSERVER_USE_SPIFFS
void HAPServer::listDir(FS &fs, const char * dirname, uint8_t levels) {
    LogD("Listing directory: ", false);
	LogD(dirname, true);

    File root = fs.open(dirname);
    if(!root){
        LogE(F("ERROR: Failed to open directory"), true);
        return;
    }

    if(!root.isDirectory()){
        LogW("WaRNING: ", false);
		LogW(dirname, false);
		LogW(" is not a directory", true);
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            LogD("  DIR : ", false);
            LogD(file.name(), true);
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            LogD("  FILE: ", false);
            LogD(file.name(), false);
            LogD("\tSIZE: ", false);
            LogD(file.size(), true);
        }
        file = root.openNextFile();
    }
}
#endif

#if HAP_ENABLE_WIFI_BUTTON
void HAPServer::callbackClick(){
	LOGDEVICE->print("CALLBACK CLICK! - ");
	LOGDEVICE->print("current wifi mode: ");
	LOGDEVICE->println((uint8_t)_wifi.getNextMode());

#if HAP_ENABLE_PIXEL_INDICATOR
	CRGB col = _wifi.getColorForMode(_wifi.getNextMode());
#endif

	_configuration.getWiFiConfig()->setWifiMode((uint8_t)_wifi.getNextMode());
	_eventManager.queueEvent( EventManager::kEventUpdatedConfig, HAPEvent());

#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.confirmWithColor(col);
#endif
}

void HAPServer::callbackDoubleClick(){
	LOGDEVICE->print("CALLBACK DOUBLE CLICK! - ");
	LOGDEVICE->print("Set default wifi mode: ");
	LOGDEVICE->println(HAP_WIFI_MODE_DEFAULT);

	_configuration.getWiFiConfig()->setWifiMode((uint8_t)HAP_WIFI_MODE_DEFAULT);
	_eventManager.queueEvent( EventManager::kEventUpdatedConfig, HAPEvent());

#if HAP_ENABLE_PIXEL_INDICATOR
	CRGB col = _wifi.getColorForMode((HAP_WIFI_MODE)HAP_WIFI_MODE_DEFAULT);
	_pixelIndicator.confirmWithColor(col);
#endif

}

void HAPServer::callbackHold(){
	LOGDEVICE->print("CALLBACK HOLD! - ");
	LOGDEVICE->println("Delete config");

	_eventManager.queueEvent( EventManager::kEventConfigReset, HAPEvent());
}

void HAPServer::callbackLongHold(){
	LOGDEVICE->print("CALLBACK LONG HOLD! - ");
	LOGDEVICE->println("Delete all pairings");

	_eventManager.queueEvent( EventManager::kEventRemoveAllPairings, HAPEvent());
}



void HAPServer::taskButtonRead(void* pvParameters){

	HAPServer* foo = reinterpret_cast<HAPServer*>(pvParameters);
	HAPButton button;

	auto callbackClick = std::bind(&HAPServer::callbackClick, foo);
	button.setCallbackClick(callbackClick);

	auto callbackDoubleClick = std::bind(&HAPServer::callbackDoubleClick, foo);
	button.setCallbackDoubleClick(callbackDoubleClick);

	auto callbackHold = std::bind(&HAPServer::callbackHold, foo);
	button.setCallbackHold(callbackHold);

	auto callbackLongHold = std::bind(&HAPServer::callbackLongHold, foo);
	button.setCallbackLongHold(callbackLongHold);


	for(;;){
		button.dispatchEvents();
		vTaskDelay(1);
	}
}
#endif



HAPServer hap;
