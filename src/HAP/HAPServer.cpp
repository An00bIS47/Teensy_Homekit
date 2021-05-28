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

#include <WString.h>
#include <algorithm>
#include "HAPServer.hpp"
#include "HAPLogger.hpp"
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

#if HAP_PRINT_QRCODE
#include "qrcode.h"
#include "HAPSVG.hpp"
#endif

// ToDo: Remove #include <StreamUtils.h> ?
// #include <StreamUtils.h>
#include "HAPPrint.hpp"

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

	_stopEvents = false;
	_stopPlugins = false;

	_hapsrp = nullptr;


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

	// Generate DeviceID
	uint8_t *baseMac = HAPDeviceID::generateID();

	if (resume == false){

		//
		// Logging
		//
		HAPLogger::setPrinter(&Serial);
		HAPLogger::setLogLevel(HAP_LOGLEVEL);


		//
		// Configuration
		//
		LogI(F("Loading configuration ..."), false);
		auto callback = std::bind(&HAPServer::updateConfig, this);
		// _config.registerCallbackUpdate(callback);
		// _config.begin();

		_configuration.registerCallbackUpdate(callback);
		_configuration.begin();



#if HAP_RESET_CONFIGURATION
		_configuration.reset();
		_configuration.getAccessoryConfig()->save();
#endif

#if HAP_RESET_PAIRINGS
		_configuration.getAccessoryConfig()->clearPairings();
		_configuration.getAccessoryConfig()->save();
#endif

		if (_configuration.load() == false){
			LogE(F("ERROR: Could not load configuration! -> Setting defaults ..."), true);

			// _configuration.formatFlash();

			_configuration.setDefaults();
			_configuration.save();
		} else {
			LogI(F("OK"), true);
		}

#if HAP_DEBUG
		_configuration.toJson(Serial);
#endif


		HAPLogger::setLogLevel(_configuration.getPlatformConfig()->logLevel());
		_accessorySet->setConfiguration(_configuration.getAccessoryConfig());

#if HAP_ENABLE_PIXEL_INDICATOR
		// ToDo: Pixel Indicator
		_pixelIndicator.begin();
		// // show pixel indicator color
		// Serial.print("WIFI MODE: ");
		// Serial.println(_config.config()["wifi"]["mode"].as<uint8_t>());
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

		HAPLogger::printInfo();

		LogD(F("\nDevice Information"), true);
		LogD(F("==================================================="), true);
		LogD(F("Device ID:    ") + HAPDeviceID::deviceID(), true);
		LogD(F("Chip ID:      ") + HAPDeviceID::chipID(), true);


		char mbedtlsVersion[32];
		mbedtls_version_get_string_full(mbedtlsVersion);

		// LogD("", true);
		LogD(F("Versions:"), true);
#if defined(ARDUINO_ARCH_ESP32)
		LogD("   SDK:       " + String(ESP.getSdkVersion()), true);
#endif
		LogD(F("   mbedtls:   ") + String(mbedtlsVersion), true);


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

		LogD(F(""), true);
		LogD(F("Endian:       "), false);
		LogD(IS_BIG_ENDIAN ? F("BIG") : F("little"), true);
		LogD(F(""), true);

		LogD(F("Storage:"), true);
		LogD(F("   type:      "), false);
#if HAP_USE_EEPROM
		LogD(F("EEPROM"), true);
#elif HAP_USE_PREFERENCES
		LogD(F("Preferences"), true);
#elif HAP_USE_SPIFFS_CONFIGURATION
		LogD(F("SPIFFS (External Flash)"), true);
#endif
		LogD(F(""), true);

		LogD(F("Fakegato:"), true);
		LogD(F("   interval:  "), false);
		LogD(String(HAP_FAKEGATO_INTERVAL), true);
		LogD(F("   size:      "), false);
		LogD(String(HAP_FAKEGATO_BUFFER_SIZE), true);

#if defined (CORE_TEENSY)
		HAPLogger::printTeensyInfo();
#endif
		LogD(F("==================================================="), true);

#if defined(ARDUINO_ARCH_ESP32)
		HAPHelper::getPartionTableInfo();
#endif

#endif

#if HAP_RESET_EEPROM
		LogW(F("Reset EEPROM - Delete pairings ..."), false);
		_accessorySet->getPairings()->resetEEPROM();
		LogW(F("OK"), true);
#endif

#if HAP_WEBSERVER_USE_SPIFFS
		SPIFFS.begin();
#if HAP_DEBUG
    	LogD(F("Listing files of SPIFFS:"), true);
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
	LogI(F("Initialize Ethernet with DHCP ..."), false);
	if (Ethernet.begin(baseMac, HAP_ETHERNET_TIMEOUT) == 0) {
		LogE(F("ERROR - Failed to configure Ethernet using DHCP"), true);
		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			LogE(F("ERROR - Ethernet shield was not found.  Sorry, can't run without hardware. :("), true);
			while (true) {
				delay(1); // do nothing, no point running without Ethernet hardware
			}
		}
		if (Ethernet.linkStatus() == LinkOFF) {
			LogE(F("ERROR - Ethernet cable is not connected."), true);
		}
		// try to congifure using IP address instead of DHCP:
		// Ethernet.begin(mac, ip, myDns);
	} else {
		LogI(F("  DHCP assigned IP: "), false);
		// LogI(Ethernet.localIP(), true);
		Serial.println(Ethernet.localIP());
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



	_time.setTimeZone(1);

#if HAP_ENABLE_NTP
	LogI( F("Starting NTP client ..."), false);
	if (_isConnected) {
		if (_time.beginNTP()){
			LogI( F("OK"), true);
		} else {
			LogE( F("ERROR - Setting time from NTP failed!"), true);
		}
#if defined( CORE_TEENSY )
		_time.setCallbackGetTime(HAPTime::getNTPTime);

#endif
		LogI( F("OK"), true);
	}
#endif /* HAP_ENABLE_NTP */
	_time.begin();

	LogI(F("Set time to: ") + _time.timeString(), true);
	_configuration.getPlatformConfig()->setRefTime(_time.timestamp());
	// _configuration.getPlatformConfig()->setRefTime(1601846922);
	_time.setReftime(_configuration.getPlatformConfig()->refTime());
	LogI(F("Current refTime is: ") + String(_time.refTime()), true);
	LogI(F("Current refTime is: ") + String(HAPTime::refTime()), true);
	LogI(F("Current t_offset is: ") + String(_time.getTOffset()), true);

	LogI(F("Loading pairings ..."), false);
	LogI(F("OK"), true);
	LogI(F("Loaded ") + String(_accessorySet->numberOfPairings()) + F(" pairings from EEPROM"), true);

#if HAP_DEBUG
	// _accessorySet->getPairings()->print();
#endif


// 	if ( isPaired() ){
// 		LogD("Loading long term keys ...", false);
// 		_longTermContext = (struct HAPLongTermContext*) calloc(1, sizeof(struct HAPLongTermContext));
// 		if (_longTermContext == NULL) {
// 			LogE( F("[ERROR] Initializing struct _longTermContext failed!"), true);
// 			free(hostname);
// 			return false;
// 		}

// 		_longTermContext->publicKey = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_PUBLIC_KEY_LENGTH);
// 		_longTermContext->publicKeyLength = ED25519_PUBLIC_KEY_LENGTH;

// 		// _longTermContext->privateKey = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_PRIVATE_KEY_LENGTH);
// 		// _longTermContext->privateKeyLength = ED25519_PRIVATE_KEY_LENGTH;

// 		_longTermContext->LTSK = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_BYTES);
// 		_longTermContext->LTSKLength = ED25519_BYTES;


// 	 	_accessorySet->getPairings()->loadKeys(_longTermContext->publicKey, _longTermContext->LTSK);


// // #if HAP_DEBUG
// //  		Serial.println("_longTermContext->publicKey: ");
// //  		HAPHelper::arrayPrint(_longTermContext->publicKey, ED25519_PUBLIC_KEY_LENGTH);

// //  		Serial.println("_longTermContext->LTSK: ");
// //  		HAPHelper::arrayPrint(_longTermContext->LTSK, ED25519_PRIVATE_KEY_LENGTH);
// // #endif


// 		LogD(F("OK"), true);
// 	}

	LogI( F("Setup accessory ..."), false);
	_accessorySet->setModelName(hostname);
	_accessorySet->setAccessoryType(HAP_ACCESSORY_TYPE_BRIDGE);
	_accessorySet->setPinCode(HAP_PIN_CODE);
	_accessorySet->begin();
	LogI(F("OK"), true);


	//
	// Event Manager
	//
	LogI( F("Adding listener to event manager ..."), false);
	// Incoming
  	listenerNotificaton.mObj = this;
  	listenerNotificaton.mf = &HAPServer::handleEvents;
	_eventManager.addListener( EventManager::kEventNotifyController, &listenerNotificaton );


	// _evtMgr.addListener( );


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



	LogI( F("OK"), true);

	/*
	 * Generate setupID and xmi uri
	 */
	_accessorySet->generateSetupID();


	LogD(F("Homekit X-HM URI: ") + String(_accessorySet->xhm()), true);
	LogD(F("Homekit setup hash: ") + String(_accessorySet->setupHash()), true);


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
		Serial.print("        ");
		// Each horizontal module
		for (uint8_t x = 0; x < qrCode.size; x++) {
            // Print each module (UTF-8 \u2588 is a solid block)
			Serial.print(qrcode_getModule(&qrCode, x, y) ? "\u2588\u2588": "  ");
		}
		Serial.println("");
	}
#endif

#if HAP_DEBUG_QRCODE_SVG
	Serial.println("SVG:");
	HAPSVG::drawQRCode(&Serial, &qrCode);
	Serial.println("");
#endif


#endif


	//
	// Starting KNX
	//
#if HAP_SERVER_USE_INTEGRATED_KNX

	LogI("Starting integrated knx ...", false);
    // Transmitter is connected to Arduino Pin #10
    ArduinoPlatform::SerialDebug = &Serial;
	randomSeed(millis());

    // read adress table, association table, groupobject table and parameters from eeprom
    knx.readMemory();

#if HAP_DEBUG
	printHex("knx configuration ", knx.paramData(0), ((ETS_HK_CHANNEL_PARAMETER_SIZE * HAP_PLUGIN_KNX_MAX_DEVICES) + 1 ));
#endif

	LogI(" OK", true);
    LogI("KNX is configured: ", false);
    LogI((uint8_t)knx.configured(), true);

    // is the led active on HIGH or low? Default is LOW
    knx.ledPinActiveOn(HIGH);
	knx.buttonPin(0);

	knx.start();
#endif





	//
	// Starting Webserver
	//
#if HAP_ENABLE_WEBSERVER
	LogI("Starting webserver ...", false);
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
		LogI(" OK", true);
	} else {
		LogI(" DISABLED", true);
	}
#endif


  	//
  	// Loading fakegato factory
  	//
	// Setting Reference Time to FakeGato
	// LogI( "Setting EVE reference time ...", false);
	// _fakeGatoFactory.setRefTime(_configuration.getPlatformConfig()->refTime());
	// LogI(" OK", true);


  	//
  	// Loading plugins
  	//
  	LogI( F("Loading plugins ..."), true);

	auto &factory = HAPPluginFactory::Instance();
    std::vector<String> names = factory.names();

    for(std::vector<String>::iterator it = names.begin(); it != names.end(); ++it) {
    	//Serial.println(*it);
		Heap(0, 0);
    	auto plugin = factory.getPlugin(*it);

		LogI(F("   - ") + plugin->name(), false);
		LogD(F(" (v") + String(plugin->version()) + ")", false);
		LogD(F(" of type: ") + String(plugin->type()), false);

		plugin->setAccessorySet(_accessorySet);
		plugin->setFakeGatoFactory(&_fakeGatoFactory);
		plugin->addEventListener(&_eventManager);

		// Load configuration from NVS here and pass it to the plugins
		size_t pluginConfigSize = _configuration.getDataLengthForPlugin(plugin->name().c_str());
		HAPConfigurationPlugin* pluginConfig = new HAPConfigurationPlugin();

		_configuration.getDataForPlugin(plugin->name().c_str(), pluginConfig, pluginConfigSize);
		plugin->setConfiguration(pluginConfig);
		// delete pluginConfig;

		// plugin->setConfig(_config.config()["plugins"][plugin->name()]);


		if ( plugin->isEnabled()) {
			LogI(F(": ENABLED"), true);

			plugin->configToJson(Serial);

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
				LogE(F(": DISABLED"), true);
			}

    	} else {
    		LogE(F(": DISABLED"), true);
    	}
	}

	//
	// Starting HAP server
	//
	LogI(F("Starting HAP server ..."), false);
	_server.begin();

#if defined(ARDUINO_ARCH_ESP32)
	_server.setNoDelay(true);
#endif
	LogI(F("OK"), true);

	//
	// Bonjour
	//
	// Set up mDNS responder:
	// - first argument is the domain name, in this example
	//   the fully-qualified domain name is "esp8266.local"
	// - second argument is the IP address to advertise
	//   we send our IP address on the WiFi network
	LogI(F("Advertising bonjour service ..."), false);
#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.begin(_accessorySet->modelName())) {
		LogE( "ERROR; Starting mDNS responder failed!", true);
		return false;
	}

	// Add service to MDNS-SD
	// mDNSExt.addService("_hap", "_tcp", _port);


	mDNSExt.enableHomekit(_port, HAPDeviceID::deviceID(), _accessorySet->modelName(), _accessorySet->accessoryType(), _accessorySet->setupHash(), "1.0");

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

	LogI( F("OK"), true);


#if HAP_ENABLE_UPDATE_OTA
	if (_configuration.getOTAConfig()->enabled){
		//
		// Starting Arduino OTA
		//
		LogI( "Starting Arduino OTA ...", false);
		_updater.setConfig(_configuration.getOTAConfig());
		_updater.begin(hostname);
		LogI( F("OK"), true);
	}
#endif


#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
		LogE( "ERROR: Updating HAP service txt failed!", true);
		return false;
	}
#else
	if ( !updateServiceTxt() ){
		LogE( F("ERROR: Advertising HAP service failed!"), true);
		return false;
	}
#endif


#if HAP_DEBUG

	{
		uint8_t buffer[1024];
		HAPPrintBuffered bufferedPrint(Serial, buffer, 1024);
		// serializeJson(doc, eStream);
		_accessorySet->printTo(bufferedPrint);
		bufferedPrint.flush();

		Serial.println();
	}

	Heap(0, 0);

	// {
	// 	HAPPrintBase64Encoded b64Stream(Serial);
	// 	_accessorySet->printTo(b64Stream);
	// 	b64Stream.flush();

	// 	Serial.println();
	// 	Heap(0, 0);
	// }

	// {
	// 	char buffer[1031];
	// 	HAPPrintChunked chunkedStream(Serial, buffer, 1031);
	// 	chunkedStream.begin();

	// 	HAPPrintBase64Encoded b64Stream(chunkedStream);
	// 	_accessorySet->printTo(b64Stream);
	// 	b64Stream.flush();

	// 	chunkedStream.end();

	// 	Serial.println();
	// 	Heap(0, 0);
	// }


#endif


	stopEvents(false);

	// queue event
	_eventManager.queueEvent(EventManager::kEventHomekitStarted, HAPEvent());

	//
	// Show event listerners
	//
  	LogD( F("Number of event listeners:  "), false );
  	LogD( String(_eventManager.numListeners()), true );


	//
	// Startup completed
	//
	LogI(F("Homekit has started successfully!"), true);
	if (!_accessorySet->isPaired()) {
		LogI(F("Homekit pin code: "), false);
		LogI(_accessorySet->pinCode(), true);
	}

#if HAP_ENABLE_WEBSERVER
	if (_configuration.getWebServerConfig()->enabled){
		LogI("Webinterface available at: ", false);
		if (HAP_WEBSERVER_USE_SSL) {
			LogI("https://", false);
		} else {
			LogI("http://", false);
		}
		LogI(String(hostname), true);
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

	// Handle any events that are in the queue
	_eventManager.processEvent();

#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.confirmWithColor(HAPColorGreen);
#endif

	return true;
}


// ToDo: Rewrite and fix memory leaks !!!
bool HAPServer::updateServiceTxt() {

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

	uint8_t *baseMac = HAPDeviceID::generateID();
	char valIdentifier[18 + 1];
	sprintf(valIdentifier, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	_hapMdnsTxt.setId(valIdentifier);									// Device ID
	_hapMdnsTxt.setModellDescription(_accessorySet->modelName());		// Model name of the accessory (e.g. ”Device1,1”). Required.
	_hapMdnsTxt.setConfigNumber(_accessorySet->configurationNumber);	// Current configuration number. Required.
	_hapMdnsTxt.setFf(0);												// Pairing Feature flags (e.g. ”0x3” for bits 0 and 1). Required if non-zero.
	_hapMdnsTxt.setSf(!_accessorySet->isPaired());						// Status flags
	_hapMdnsTxt.setPv(HOMEKIT_PROTOKOL_VERSION);						// Protocol Version
	_hapMdnsTxt.setStateNumber(1);										// Current state number. Required. - This must have a value of ”1”.
	_hapMdnsTxt.setCi(_accessorySet->accessoryType());					// Accessory Category Identifier. Required.
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
	if ( millis() - _previousMillisHeap >= 1000) {
	    // save the last time you blinked the LED
	    _previousMillisHeap = millis();
	    Heap(_clients.size(), _eventManager.getNumEventsInQueue());

		// ToDo: remove
		// Serial.print("Task Button Handle: ");
		// Serial.println(_taskButtonHandle == NULL ? "NULL" : "not NULL");
		// if (_taskButtonHandle != NULL){
		// 	Serial.print("Task State: ");
		// 	Serial.println(eTaskGetState(_taskButtonHandle));
		// }

	}
#endif



#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.handle();
#endif


#if HAP_ENABLE_WIFI
	if ( (_wifi.captiveInitialized()) && (_configuration.getWiFiConfig()->mode == (uint8_t)HAP_WIFI_MODE_AP) ){
		_wifi.handle();
		return;
	}
#endif

	// Handle existing clients
	for (auto& hapClient : _clients) {

		// Connected
		if (hapClient->client.connected()) {

			// Available
			unsigned long timeout = 15;
			unsigned long previousMillis = millis();
			while ( millis() - previousMillis < timeout) {

				if (hapClient->client.available()) {
					hapClient->state = HAP_CLIENT_STATE_AVAILABLE;
					handleClientState(hapClient);
					break;
				}

				// Idle
				hapClient->state = HAP_CLIENT_STATE_IDLE;
			}


		} else {
			// Disconnected
			hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
			handleClientState(hapClient);
		}

		// LogV( "HAPClient state " + hapClient.getClientState(), true );
	}


	// Handle new clients
#if defined(ARDUINO_ARCH_ESP32)
	WiFiClient client = _server.available();
#elif defined( CORE_TEENSY)

	EthernetClient client = _server.available();
#endif
	if (client) {

		HAPClient* hapClient = new HAPClient();

		// New client connected
		hapClient->client = client;
		hapClient->state = HAP_CLIENT_STATE_CONNECTED;

		handleClientState(hapClient);
	}

	// Handle Webserver
#if HAP_ENABLE_WEBSERVER
	if (_configuration.getWebServerConfig()->enabled){
		_webserver->handle();
	}
#endif

	// Handle Arduino OTA
#if HAP_ENABLE_UPDATE_WEB || HAP_ENABLE_UPDATE_OTA
	if (_configuration.getOTAConfig()->enabled){
		_updater.handle();
	}
#endif

	// Handle plugins
	if (!_stopPlugins){
		for (auto& plugin : _plugins) {
			if (plugin->isEnabled()) {
				plugin->handle();
			}
		}
	}

	//
	// Handle fakeGatos
	_fakeGatoFactory.handle();

	// Handle any events that are in the queue
	_eventManager.processEvent();
}







void HAPServer::handleClientDisconnect(HAPClient* hapClient) {

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

	if ((hapClient->state == HAP_CLIENT_STATE_DISCONNECTED) || (!hapClient->client.connected())){
		handleClientDisconnect( hapClient );
		return;
	}

#if defined( ARDUINO_ARCH_ESP32 )
	LogD( ">>> client [" + hapClient->client.remoteIP().toString() + "] connected", true );
#elif defined( CORE_TEENSY )
	LogD( F(">>> client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogD(F("] "), false);
#endif

	if (hapClient->state == HAP_CLIENT_STATE_CONNECTED) {
		LogD(F("connected"), true);
		_clients.push_back(std::move(hapClient));

		hapClient->state = HAP_CLIENT_STATE_IDLE;
	}

	if (hapClient->state == HAP_CLIENT_STATE_IDLE) {
		LogD(F("idle"), true);
		if (hapClient->client.available()) {
			hapClient->state = HAP_CLIENT_STATE_AVAILABLE;
		}
	}

	if (hapClient->state == HAP_CLIENT_STATE_AVAILABLE) {
		LogD(F("available: "), false);
		LogD(hapClient->client.available(), true);
		handleClientAvailable(hapClient);
	} else if (hapClient->state == HAP_CLIENT_STATE_ALL_PAIRINGS_REMOVED) {
		LogD(F("all pairings removed"), true);
		handleAllPairingsRemoved();
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleAllPairingsRemoved(){
	LogV( F("<<< Handle all pairings removed ..."), false);
	for (int i=0; i < _clients.size(); i++){

		if (_clients[i]->client.connected()){
#if defined( ARDUINO_ARCH_ESP32 )
			LogD("\nClosing connection to client [" + _clients[i].client.remoteIP().toString() + "]", true);
#elif defined( CORE_TEENSY )
			LogD(F("\nClosing connection to client ["),false);
			Serial.print(_clients[i]->client.remoteIP());
			LogD(F("]"), true);
#endif
			_clients[i]->client.stop();
		}

		_clients[i]->state = HAP_CLIENT_STATE_DISCONNECTED;
	}
	LogV(F("OK"), true);
}

void HAPServer::handleClientAvailable(HAPClient* hapClient) {
	String curLine = "";

	LogD(F("<<< Handle client available [enrypted:") + String(hapClient->isEncrypted()) + "]" , true);

	ReadBufferingClient bufferedClient{hapClient->client, 1024 + 16};

	if (hapClient->isEncrypted()) {
		processIncomingEncryptedRequest( hapClient, &bufferedClient );
	} else {
		processIncomingRequest( hapClient, &bufferedClient, curLine );
	}

#if HAP_DEBUG_HOMEKIT
	if (curLine != "")
		LogD(curLine, true);
#endif

	// clear request

	hapClient->clear();

	if ( !hapClient->client.connected() ) {
		hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
	}

	// Update client state *print*
	handleClientState( hapClient );
}


void HAPServer::processIncomingEncryptedRequest(HAPClient* hapClient, ReadBufferingClient* bufferedClient){


	LogD( F("<<< Handle encrypted request ..."), false);

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

		// Serial.printf("AAD: %02X%02X - %d\n", AAD[0], AAD[1], trueLength);
		// Serial.printf("availableSize: %d\n", availableSize);
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

		// Serial.print("trueLength: ");
		// Serial.println(trueLength);
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
		Serial.println("plaintext:");
		Serial.println((char*) plainText);
#endif


		int bodyDataLen = 0;
		uint8_t *bodyData;
		parseRequest(hapClient, (char*)plainText, trueLength, &bodyData, &bodyDataLen);

#if HAP_DEBUG_HOMEKIT_REQUEST
		HAPHelper::array_print("bodyData", bodyData, bodyDataLen);
#endif

		handlePath(hapClient, bodyData, bodyDataLen);

		if (bodyDataLen > 0){
			free(bodyData);
		}
	}
}


bool HAPServer::stopEvents(){
	return _stopEvents;
}


void HAPServer::stopEvents(bool value) {

#if HAP_DEBUG
	if (value) {
		LogD(F("<<< Stopping Events"), true);
	} else {
		LogD(F(">>> Starting Events"), true);
	}
#endif
	_stopEvents = value;
}


bool HAPServer::handlePath(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen){

	bool validPath = false;
	// /accessories
	if ( (hapClient->request.path == "/accessories") && (hapClient->request.method == METHOD_GET) ) {
		validPath = true;
		handleAccessories( hapClient );
	}

	// /characteristics
	else if ( hapClient->request.path == "/characteristics" ) {

		// GET
		if ( hapClient->request.method == METHOD_GET ) {
			validPath = true;
			handleCharacteristicsGet( hapClient );
		}
		// PUT
		else if ( (hapClient->request.method == METHOD_PUT) && (hapClient->request.contentType == "application/hap+json") ) {
			validPath = true;
			// char bodyDataStr[bodyDataLen + 1];
			// strncpy(bodyDataStr, (char*)bodyData, bodyDataLen);

			handleCharacteristicsPut( hapClient, String((char*)bodyData) );
		}

	} else if ( hapClient->request.path == "/pairings" ) {
		// POST
		if ( ( hapClient->request.method == METHOD_POST ) && (hapClient->request.contentType == "application/pairing+tlv8") ) {
			validPath = true;
			handlePairingsPost( hapClient, bodyData,  bodyDataLen);
		}
	}


	if (validPath == false) {


#if defined( ARDUINO_ARCH_ESP32 )
		LogE(F("Not yet implemented! >>> client [") + hapClient->client.remoteIP().toString() + "] ", false);
#elif defined( CORE_TEENSY )
		LogE(F("Not yet implemented! >>> client ["), false);
		Serial.print(hapClient->client.remoteIP());
		LogE(F("]"), true);
#endif
		LogE(F(" - method: ") + String(hapClient->request.method), true);
		LogE(F(" - path: ") + String(hapClient->request.path), true);
		LogE(F(" - Content-Type: ") + String(hapClient->request.contentType), true);


		hapClient->client.stop();
		return false;
	}

	// Moved to handle functions
	//hapClient->state = CLIENT_STATE_IDLE;
	return true;
}


void HAPServer::parseRequest(HAPClient* hapClient, const char* msg, size_t msg_len, uint8_t** out, int* outLen){

	int curPos = 0;
	for (int i = 0; i < msg_len; i++) {
    		//Serial.print(decrypted[i]);
		if ( msg[i] == '\r' && msg[i + 1] == '\n' ) {

			String curLine = String(msg).substring(curPos, i);
			processIncomingLine(hapClient, curLine);
			i++;

			if (i - curPos == 2) {
				curPos = i + 1;
				break;
			}
			curPos = i + 1;
		}

	}

	//Serial.printf(">>> curPos: %d\n", curPos);

	if (curPos + hapClient->request.contentLength == msg_len) {
		//bodyData += String(msg).substring(curPos);
		int siz = msg_len - curPos;

		if (siz > 0) {
			*out = (uint8_t*)malloc(sizeof(uint8_t) * siz);
			memcpy(*out, &msg[curPos], siz);
		}

		*outLen = siz;
	} else {
		LogW(F("Size mismatch"), true);
	}

	//return bodyData;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::sendErrorTLV(HAPClient* hapClient, uint8_t state, uint8_t error){
	TLV8 response;
	response.encode(HAP_TLV_STATE, 1, state);
	response.encode(HAP_TLV_ERROR, 1, error);

	sendResponse(hapClient, &response);
	response.clear();


	hapClient->clear();
	hapClient->client.stop();

	_homekitFailedLoginAttempts++;
	_isInPairingMode = false;

	hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;

	_eventManager.queueEvent(EventManager::kEventErrorOccurred, HAPEvent());

	stopEvents(false);
}


void HAPServer::processIncomingRequest(HAPClient* hapClient, ReadBufferingClient* bufferedClient, String& curLine){

	while (bufferedClient->available()){

		const byte b = bufferedClient->read();

		if ( (char) b == '\n' ) {
			// if the current line is blank, you got two newline characters in a row.
			// that's the end of the client HTTP request, so send a response:
			if (curLine.length() == 0) {


#if HAP_DEBUG_HOMEKIT
				// Handle data
				LogD( F("request: "), false);
				LogD(hapClient->request.toString(), true);
#endif

				// /identify
				if ( (hapClient->request.path == "/identify") && (hapClient->request.method == METHOD_POST) ) {
					handleIdentify(hapClient);
					hapClient->state = HAP_CLIENT_STATE_IDLE;
				}

				// has content
				else if ( hapClient->request.contentLength > 0) {
					// encode tlv8
					if ( hapClient->request.contentType == "application/pairing+tlv8" )  {


						if ( !encode(hapClient, bufferedClient) ) {
							LogE( F("ERROR: Decoding pairing request failed!"), true);

							sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_UNKNOWN);
							return;
						}


						// pair-setup M1
						if ( (hapClient->request.path == "/pair-setup" ) && (hapClient->pairState == HAP_PAIR_STATE_M1) ) {


#if HAP_ALLOW_PAIRING_WHILE_PAIRED == 0

							if (_accessorySet->isPaired() == true) {
								// accessory is already paired
								LogE( "ERROR: Accessory is already paired!", true);
								sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_AUTHENTICATON);
								return;
							} else if (_isInPairingMode == true) {
								// accessory is in pairing mode
								sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_BUSY);
								return;
							} else if (_homekitFailedLoginAttempts >= 100) {
								// accessory has more than 100 failed attempts
								sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_MAX_TRIES);
								return;
							} else {
#endif
								if (!handlePairSetupM1( hapClient ) ) {
									LogE( F("ERROR: Pair-setup failed at M1!"), true);
									hapClient->clear();
									hapClient->client.stop();
									stopEvents(false);
									hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
								}
#if HAP_ALLOW_PAIRING_WHILE_PAIRED == 0
							}
#endif
						}

						// pair-setup M3
						else if ( (hapClient->request.path == "/pair-setup" ) && (hapClient->pairState == HAP_PAIR_STATE_M3) ) {
							if (!handlePairSetupM3( hapClient ) ) {
								LogE( F("ERROR: Pair-setup failed at M3!"), true);
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
								stopEvents(false);
							}
						}

						// pair-setup M5
						else if ( (hapClient->request.path == "/pair-setup" ) && (hapClient->pairState == HAP_PAIR_STATE_M5) ) {
							if ( !handlePairSetupM5( hapClient ) ) {
								LogE( F("ERROR: Pair-setup failed at M5!"), true);
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
								stopEvents(false);
							}
						}

						// pair-verify M1
						if ( (hapClient->request.path == "/pair-verify" ) && (hapClient->verifyState == HAP_VERIFY_STATE_M1) ) {
							if ( !handlePairVerifyM1( hapClient ) ) {
								LogE( F("ERROR: Pair-verify failed at M1!"), true);
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
								stopEvents(false);
							}
						}

						// pair-verify M3
						else if ( (hapClient->request.path == "/pair-verify" ) && (hapClient->verifyState == HAP_VERIFY_STATE_M3) ) {
							if ( !handlePairVerifyM3( hapClient ) ) {
								LogE( F("ERROR: Pair-verify failed at M3!"), true);
								hapClient->clear();
								hapClient->client.stop();
								hapClient->state = HAP_CLIENT_STATE_DISCONNECTED;
								stopEvents(false);
							}
						}
					}
				}

				curLine = "";


				return;
			} else {  // if you got a newline, then clear currentLine:
				// Handle lines
				processIncomingLine(hapClient, curLine);
				curLine = "";
			}
		} else if ( (char) b != '\r') {  	// if you got anything else but a carriage return character,
			curLine += (char) b;      		// add it to the end of the currentLine
		}

	}


	hapClient->state = HAP_CLIENT_STATE_IDLE;

}


void HAPServer::processPathParameters(HAPClient* hapClient, const char* line, size_t lineLength, int curPos){

	
	
	// Serial.print("constLine: ");
	// Serial.println(constLine);
	// Serial.print("curPos: ");
	// Serial.println(curPos);


	//int index = line.indexOf("?", curPos);
	int index = HAPHelper::indexOf(line, lineLength, '?', curPos);

	if ( index == -1) {
		// no ? in request
		int len = HAPHelper::indexOf(line, lineLength, ' ', curPos); //line.indexOf(" ", curPos)

		// line.substring(curPos, line.indexOf(" ", curPos));
		char requestPath[ len + 1 - curPos];
		memcpy( requestPath, &line[curPos], len - curPos);
		requestPath[len - curPos] = '\0';

		// Serial.println("len: " + String(len));
		// Serial.println("REFERENCE len: " + String(line.indexOf(" ", curPos)));
		// Serial.println("requestPath: " + String(requestPath));
		// Serial.println("REFERENCE requestPath: " + line.substring(curPos, line.indexOf(" ", curPos)));

		hapClient->request.path = requestPath; // line.substring(curPos, line.indexOf(" ", curPos));
		hapClient->request.params = std::map<String, String>();
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

		// Serial.println("paramString size: " + String(len - curPos + 1));
		// Serial.println("len: " + String(len));
		// Serial.println("REFERENCE len: " + String(line.indexOf(" ", curPos)));
		// Serial.println("paramString: " + String(paramString));
		// Serial.println("strlen(paramString): " + String(strlen(paramString)));
		// Serial.println("REFERENCE paramString: " + paramStr);

		char* tokens = paramString;
		char *p = paramString;

		while ((p = strsep (&tokens, "&\n"))) {
			char *key = strtok (p, "=");
			char *value = NULL;
			if (key && (value = strtok (NULL, "="))){

				hapClient->request.params[key] = value;
			}
		}




		// do {
		// 	curPos = 0;
		// 	int endIndex = paramStr.indexOf("&");
		// 	if (endIndex == -1){
		// 		endIndex = paramStr.length();
		// 	}

		// 	String keyPair = paramStr.substring(curPos, endIndex);
		// 	// Serial.printf("tmp: %s\n", keyPair.c_str());

		// 	int equalIndex = keyPair.indexOf("=");

		// 	// Serial.print("key: ");
		// 	// Serial.print(keyPair.substring(0, equalIndex));
		// 	// Serial.print(" - value: ");
		// 	// Serial.println(keyPair.substring(equalIndex + 1));
		// 	// Serial.send_now();


		// 	hapClient->request.params[keyPair.substring(0, equalIndex)] = keyPair.substring(equalIndex + 1);

		// 	if (endIndex < paramStr.length()) {
		// 		paramStr = paramStr.substring(endIndex + 1);
		// 	} else {
		// 		break;
		// 	}

		// } while ( paramStr.length() > 0 );
	}
}


void HAPServer::processIncomingLine(HAPClient* hapClient, String& line){

	// Print Line
#if HAP_DEBUG_HOMEKIT_REQUEST
	if (line != "") Serial.println(line);
#endif

	int curPos = 0;

	// Method
	if ( line.startsWith("POST ") ) {
		hapClient->request.method = METHOD_POST;
		curPos = 5;
		// Path
		processPathParameters( hapClient, line.c_str(), line.length(), curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	} else if ( line.startsWith("GET ") ) {
		hapClient->request.method = METHOD_GET;
		curPos = 4;
		// Path
		processPathParameters( hapClient, line.c_str(), line.length(), curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	} else if ( line.startsWith("PUT ") ) {
		hapClient->request.method = METHOD_PUT;
		curPos = 4;
		// Path
		processPathParameters( hapClient, line.c_str(), line.length(), curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	} else if ( line.startsWith("DELETE ") ) {
		hapClient->request.method = METHOD_DELETE;
		curPos = 7;
		// Path
		processPathParameters( hapClient, line.c_str(), line.length(), curPos);
		//hapClient->request.path = line.substring(curPos, line.indexOf(" ", curPos));
	}

	if (line.length() == 0) {
		//Serial.println("END OF HEADERS!!!");



	} else {

		String orgLine = line;
		line.toLowerCase();

		// Content Type
		if ( line.startsWith("content-type:") ) {
			curPos = 13;
			String strValue = orgLine.substring(curPos);
			strValue.trim();
			hapClient->request.contentType = strValue;
		}

		// Content Length
		else if ( line.startsWith("content-length:") ) {
			curPos = 15;
			String strValue = orgLine.substring(curPos);
			strValue.trim();
			hapClient->request.contentLength = strValue.toInt();
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

	uint16_t written = 0;
	bool success = false;

	// Method not supported :(
	if ( bufferedClient->peek() == 0x00) {
		bufferedClient->read();
//		Serial.println(c, HEX);
		bufferedClient->read();
//		Serial.println(c, HEX);
		bufferedClient->read();
//		Serial.println(c, HEX);
		hapClient->request.contentLength = hapClient->request.contentLength - 3;
	}

	// Reset pairing state
	hapClient->pairState = HAP_PAIR_STATE_RESERVED;

	while (bufferedClient->available()) {            	// loop while the client's connected

		if ( TLV8::isValidTLVType( bufferedClient->peek()) ) {

			uint8_t type = bufferedClient->read();
			uint8_t length = bufferedClient->read();

			uint8_t data[length];
			bufferedClient->readBytes(data, length);

#if HAP_DEBUG_TLV8
			LogD( F("------------------------------------------"), true );
			LogD(F("type:    ") + String(type, HEX), true);
			LogD(F("length:  ") + String(length), true);
			HAPHelper::array_print("value", data, length);
#endif


			if (type == HAP_TLV_STATE) {

				if (hapClient->request.path == "/pair-verify") {
					hapClient->verifyState = static_cast<HAP_VERIFY_STATE>(data[0]);
				} else
					hapClient->pairState = static_cast<HAP_PAIR_STATE>(data[0]);
			}


			written += length + 2;


			if (!hapClient->request.tlv.encode(type, length, data)) {
				LogE( "ERROR: Encoding TLV data failed!", true );
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
			LogW( F("WARNING: Invalid TLV8 type: "), false );
			LogW((char*)read, true);
#else
			bufferedClient->read();
#endif

		}

	}

	hapClient->state = HAP_CLIENT_STATE_IDLE;
	return success;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleIdentify(HAPClient* hapClient){
	LogI( F("<<< Handle /identify: "), true );

	HAPCharacteristicT<bool>* c = reinterpret_cast< HAPCharacteristicT<bool> *>(_accessorySet->getCharacteristicOfType(_accessorySet->aid(), HAP_CHARACTERISTIC_IDENTIFY));

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
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::send(HAPClient* hapClient, const String httpStatus, const uint8_t* data, const size_t length, const enum HAP_ENCRYPTION_MODE mode, const char* contentType){

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

	if ( mode == HAP_ENCRYPTION_MODE_PLAIN ) {
		eStream.print(F("Content-Length: "));
		eStream.print(length);
		eStream.print(HTTP_CRLF);

		// end header
		eStream.print(HTTP_CRLF);

	} else if (mode == HAP_ENCRYPTION_MODE_PLAIN_CHUNKED) {
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

	if ( ( mode == HAP_ENCRYPTION_MODE_PLAIN ) || (mode == HAP_ENCRYPTION_MODE_PLAIN_CHUNKED) ) {
		return 0;
	}

	hapClient->encryptionContext.encryptCount = eStream.encryptCount();

	return 0;

}

bool HAPServer::send(HAPClient* hapClient, const String httpStatus, const JsonDocument& doc, const enum HAP_ENCRYPTION_MODE mode, const char* contentType){


	if (httpStatus == HTTP_204) {
		send204(hapClient);
		return true;
	}

#if 0
	if (mode == HAP_ENCRYPTION_MODE_ENCRYPT) {
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
		//Serial.write(_buffer - l, chunk_size);
		HAPHelper::array_print("response", response.c_str(), response.length());
#endif

		uint8_t* encrypted = nullptr;
		int encryptedLen = 0;
		encrypted = HAPEncryption::encrypt((uint8_t*)response.c_str(), response.length(), &encryptedLen, hapClient->encryptionContext.encryptKey, hapClient->encryptionContext.encryptCount++);
		if (encryptedLen == 0) {
			LogE(F("ERROR: Encrpyting response failed!"), true);

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

	else if (mode == HAP_ENCRYPTION_MODE_ENCRYPT_CHUNKED) {
#else
	if (mode == HAP_ENCRYPTION_MODE_ENCRYPT_CHUNKED || mode == HAP_ENCRYPTION_MODE_ENCRYPT) {
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
	else if (mode == HAP_ENCRYPTION_MODE_PLAIN_CHUNKED) {
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

	else if (mode == HAP_ENCRYPTION_MODE_PLAIN){
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

	// LogD(F("\nEncrpyting response ..."), false);

	uint8_t* encrypted = nullptr;
	int encryptedLen = 0;
	encrypted = HAPEncryption::encrypt((uint8_t*)HTTP_204, strlen(HTTP_204), &encryptedLen, hapClient->encryptionContext.encryptKey, hapClient->encryptionContext.encryptCount++);
    if (encryptedLen == 0) {
    	LogE(F("ERROR: Encrpyting response failed!"), true);

		hapClient->clear();
    	return false;
    } else {
		// LogD(F("OK"), true);
    }

// #if defined( ARDUINO_ARCH_ESP32 )
// 	LogD("\n>>> Sending " + String(encryptedLen) + " bytes encrypted response to client [" + hapClient->client.remoteIP().toString() + "] ...", false);
// #elif defined( CORE_TEENSY )
// 	LogD(F("\n>>> Sending ") + String(encryptedLen) + F(" bytes encrypted response to client ["), false);
// 	Serial.print(hapClient->client.remoteIP());
// 	LogD("] ...", false);
// #endif

	WriteBufferingClient bufferedWifiClient{hapClient->client, 64};
	size_t bytesSend = bufferedWifiClient.write(encrypted, encryptedLen);
	bufferedWifiClient.flush();  // <- OPTIONAL

	free(encrypted);

	if (bytesSend == encryptedLen){
		// LogD( F("OK"), true);
	} else {
		// LogE( F("ERROR: Could not send all bytes! ") + String(bytesSend) + "/" + String(encryptedLen), true);
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

	LogV(F("\nSent ") + String(bytesSent) + F(" bytes"), true);

	response->clear();

	hapClient->clear();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::handlePairSetupM1(HAPClient* hapClient){

	LogI(F("Homekit PIN: "), false);
	LogI(String(_accessorySet->pinCode()), true);


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + F("] -> /pair-setup Step 1/4 ..."), false);
#elif defined( CORE_TEENSY )
	// LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] -> /pair-setup Step 1/4 ...", false);
	LogV( F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV( F("] -> /pair-setup Step 1/4 ..."), false);
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
		Entropy.Initialize();

		for (int i = 0; i < ED25519_BYTES; i++){
			_accessorySet->LTSK()[i] = Entropy.randomByte();
		}
#else
		esp_fill_random(_accessorySet->LTSK(), ED25519_BYTES);
#endif

		// _longTermContext->privateKey = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_PRIVATE_KEY_LENGTH);
		// _longTermContext->privateKeyLength = ED25519_PRIVATE_KEY_LENGTH;


		// _longTermContext->publicKey = (uint8_t*) malloc(sizeof(uint8_t) * ED25519_PUBLIC_KEY_LENGTH);
		// _longTermContext->publicKeyLength = ED25519_PUBLIC_KEY_LENGTH;

		ed25519_key_generate(_accessorySet->LTPK(), _accessorySet->LTSK());

		LogD(F("OK"), true);


#if HAP_DEBUG_HOMEKIT
		HAPHelper::array_print("LTPK", _accessorySet->LTPK(), ED25519_PUBLIC_KEY_LENGTH);
		HAPHelper::array_print("LTSK", _accessorySet->LTSK(), ED25519_PRIVATE_KEY_LENGTH);
#endif
	}

 	LogD(F("Initializing srp ..."), false);

	// ToDo: Check new HAPSRP
	if (_hapsrp == nullptr) _hapsrp = new HAPSRP();

	_hapsrp->begin("Pair-Setup");

	if (_hapsrp->data->session != NULL) delete _hapsrp->data->session;

	_hapsrp->data->session = _hapsrp->newSession(SRP_SHA512, HAPSRP::SRP_NG_3072, NULL,NULL);
	// _hapsrp->data->session = session;

#if HAP_DEBUG_SRP
	HAPHelper::mpi_print("N", &_hapsrp->data->session->ng->N);
	HAPHelper::mpi_print("g", &_hapsrp->data->session->ng->g);
#endif

	LogD(F("OK"), true);



	_hapsrp->data->len_s = SRP_SALT_LENGTH;
	_hapsrp->createSaltedVerificationKey1(_hapsrp->data->session,
										"Pair-Setup",
										(const uint8_t*)_accessorySet->pinCode(), strlen(_accessorySet->pinCode()),
										&_hapsrp->data->bytes_s, _hapsrp->data->len_s,
										&_hapsrp->data->bytes_v, &_hapsrp->data->len_v
										);

	if (_hapsrp->data->bytes_s == NULL || _hapsrp->data->bytes_v == NULL) {
		LogE(F("Failed to create SRP verifier "), true);

		// srp_session_delete(_srp->session);

		sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_UNKNOWN);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		return false;
	}

#if HAP_DEBUG_SRP
	HAPHelper::array_print("salt", _hapsrp->data->bytes_s, _hapsrp->data->len_s);
#endif

	if (_hapsrp->data->keys == NULL) {
		LogD(F("Calculating srp public key"), true);
		_hapsrp->data->keys = _hapsrp->createKeyPair(_hapsrp->data->session, _hapsrp->data->bytes_v, _hapsrp->data->len_v, &_hapsrp->data->bytes_B, &_hapsrp->data->len_B);
		if (_hapsrp->data->keys == NULL) {

			LogE(F("Failed to calculate srp public key "), true);
			sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_UNKNOWN);

			if (_hapsrp) delete _hapsrp;
			_hapsrp = nullptr;
			return false;
		}
	}

#if HAP_DEBUG_SRP
	HAPHelper::array_print("pubKey", _hapsrp->data->bytes_B, _hapsrp->data->len_B);
#endif

	LogD(F("Sending response ..."), false);

	response.encode(HAP_TLV_STATE, 1, HAP_PAIR_STATE_M2);
	response.encode(HAP_TLV_SALT, SRP_SALT_LENGTH, _hapsrp->data->bytes_s);
	response.encode(HAP_TLV_PUBLIC_KEY, SRP_PUBLIC_KEY_LENGTH, _hapsrp->data->bytes_B);

	sendResponse(hapClient, &response);

	response.clear();
	hapClient->clear();

	LogD(F("OK"), true);

	return true;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::handlePairSetupM3(HAPClient* hapClient) {


#if defined( ARDUINO_ARCH_ESP32 )
	LogD( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-setup Step 2/4 ...", false);
#elif defined( CORE_TEENSY )
	LogD( F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pair-setup Step 2/4 ..."), false);
#endif

	_eventManager.queueEvent(EventManager::kEventPairingStep3, HAPEvent());

	LogV( F("\nDecoding TLV ..."), false);

	size_t decodedLen = 0;
	uint8_t device_public_key[hapClient->request.tlv.size(HAP_TLV_PUBLIC_KEY)];

	hapClient->request.tlv.decode(HAP_TLV_PUBLIC_KEY, device_public_key, &decodedLen);

	if (decodedLen == 0) {
		LogE(F("ERROR: Invalid payload: no client public key"), true);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M4, HAP_ERROR_AUTHENTICATON);

		return false;
	}
	LogV(F("OK"), true);


	LogV( F("Generating proof ..."), false);
#if HAP_DEBUG_SRP
	HAPHelper::array_print("v", _hapsrp->data->bytes_v, _hapsrp->data->len_v);
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




    uint8_t proof[hapClient->request.tlv.size(HAP_TLV_PROOF)];
	hapClient->request.tlv.decode(HAP_TLV_PROOF, proof, &decodedLen);

	if (decodedLen == 0) {
    	LogE(F("ERROR: Invalid payload: no device proof"), true);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M4, HAP_ERROR_AUTHENTICATON);
    	return false;
    }

	LogV(F("OK"), true);


	LogV( F("Verifying device proof ..."), false);
	if (decodedLen != _hapsrp->length(_hapsrp->data->session) ){

		LogE(F("ERROR: Client SRP proof does not match session hash length: ") + String(decodedLen) + F("/") + String(_hapsrp->length(_hapsrp->data->session)), true);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M4, HAP_ERROR_AUTHENTICATON);
		return false;
	}

#if HAP_DEBUG_SRP
	// Server keys
	HAPHelper::mpi_print("_hapsrp->data->keys->B = PubKey", &(_hapsrp->data->keys->B));
	HAPHelper::mpi_print("_hapsrp->data->keys->b = Private Key ?", &(_hapsrp->data->keys->b));

	HAPHelper::array_print("_hapsrp->data->bytes_v", _hapsrp->data->bytes_v, _hapsrp->data->len_v);

	// salt
	HAPHelper::array_print("_hapsrp->data->bytes_s = SALT", (uint8_t*)_hapsrp->data->bytes_s, _hapsrp->data->len_s);
	HAPHelper::array_print("_hapsrp->data->bytes_b", (uint8_t*)_hapsrp->data->bytes_B, _hapsrp->data->len_B);

	// proof
	HAPHelper::array_print("_hapsrp->data->verifier->M", _hapsrp->data->verifier->M, SHA512_DIGEST_LENGTH);
	HAPHelper::array_print("proof", proof, SHA512_DIGEST_LENGTH);


	HAPHelper::array_print("_hapsrp->data->username", (uint8_t*)_hapsrp->data->username, strlen(_hapsrp->data->username));
	HAPHelper::array_print("password", (uint8_t*)_accessorySet->pinCode(), strlen(_accessorySet->pinCode()));
#endif

	if (_hapsrp->verifySession(_hapsrp->data->verifier, proof, NULL) == false) {
        LogE(F("ERROR: Failed to verify client SRP proof"), true);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M4, HAP_ERROR_AUTHENTICATON);
        return false;
	}

    LogV(F("OK"), true);



    LogV(F("Generating accessory proof ..."), false);

	const uint8_t *acc_srp_proof = _hapsrp->getVerifierHAMK(_hapsrp->data->verifier);
	if (acc_srp_proof == NULL) {
		LogE(F("ERROR: srp_verifier_get_HAMK failed"), true);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M4, HAP_ERROR_AUTHENTICATON);
        return false;
	}
	LogV(F("OK"), true);

    LogV(F("Sending response ..."), false);
	TLV8 response;
    response.encode(HAP_TLV_STATE, 1, HAP_PAIR_STATE_M4);
    response.encode(HAP_TLV_PROOF, SRP_PROOF_LENGTH, acc_srp_proof);

#if HAP_DEBUG_TLV8
	response.print();
#endif

	sendResponse(hapClient, &response);
	LogV(F("OK"), true);

	response.clear();

	hapClient->clear();

	LogD(F("OK"), true);
    return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPServer::handlePairSetupM5(HAPClient* hapClient) {

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-setup Step 3/4 ...", false);
#elif defined( CORE_TEENSY )
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pair-setup Step 3/4 ..."), false);
#endif

	_eventManager.queueEvent(EventManager::kEventPairingStep3, HAPEvent());


	int err_code = 0;
	TLV8 response;


	int srp_key_length = 0;
	uint8_t srp_key[_hapsrp->getVerifierSessionKeyLength(_hapsrp->data->verifier)] = {0,};

	memcpy(srp_key, _hapsrp->getVerifierSessionKey(_hapsrp->data->verifier, &srp_key_length), srp_key_length);
	if (srp_key_length == 0) {
		LogE(F("ERROR: Failed to get verifier session key!"), true);

		if (_hapsrp) delete _hapsrp;
		_hapsrp = nullptr;

		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();
		return false;
	}


	LogV(F("\nClearing SRP ..."), false);
	if (_hapsrp != nullptr) {
		delete _hapsrp;
		_hapsrp = nullptr;
	}
	LogV(F("OK"), true);


    LogV(F("\nDecoding TLV values ..."), false);
	// uint8_t *encrypted_tlv = hapClient->request.tlv.decode(HAP_TLV_ENCRYPTED_DATA);

	size_t decodedLen = 0;
	size_t encryptedTLVLen = hapClient->request.tlv.size(HAP_TLV_ENCRYPTED_DATA);

	uint8_t encryptedTLV[encryptedTLVLen] = {0,};
	hapClient->request.tlv.decode(HAP_TLV_ENCRYPTED_DATA, encryptedTLV, &decodedLen);


	if (decodedLen == 0) {
        LogE(F("ERROR: Decrypting TLV failed!"), true);

		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();
    	return false;
    }
    LogV(F("OK"), true);

    LogV(F("Get HKDF key ..."), false);
	uint8_t subtlv_key[HKDF_KEY_LEN] = {0,};
	err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_ENCRYPT, srp_key, SRP_SESSION_KEY_LENGTH, subtlv_key);

	if (err_code != 0) {
        LogE(F("ERROR: Failed to get HKDF key"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();
        return false;
    }
    LogV(F("OK"), true);

    LogV(F("Decrypting chacha20_poly1305 ..."), false);
    // uint8_t *subtlv = (uint8_t*) malloc(sizeof(uint8_t) * encrypted_tlv_len);

	size_t decryptedLen = encryptedTLVLen - CHACHA20_POLY1305_AUTH_TAG_LENGTH;
	uint8_t subtlv[decryptedLen];

#if HAP_DEBUG_HOMEKIT
	HAPHelper::array_print("subtlv_key", subtlv_key, HKDF_KEY_LEN);
	HAPHelper::array_print("encryptedTLV", encryptedTLV, encryptedTLVLen);
#endif
	err_code = chacha20_poly1305_decrypt(CHACHA20_POLY1305_TYPE_PS05, subtlv_key, NULL, 0, encryptedTLV, encryptedTLVLen, subtlv);

    if (err_code != 0) {
        LogE(F("ERROR: Decrypting CHACHA20_POLY1305_TYPE_PS05 failed! Reason: ") + String(err_code), true);

#if HAP_DEBUG_HOMEKIT
		HAPHelper::array_print("subtlv", subtlv, decryptedLen);
#endif
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();
        return false;
    }
    LogD( F("OK"), true);

#if HAP_DEBUG_HOMEKIT
	HAPHelper::array_print("subtlv", subtlv, decryptedLen);
#endif

	TLV8 encTLV;
	encTLV.encode(subtlv, decryptedLen);

#if HAP_DEBUG_TLV8
	encTLV.print();
#endif

    uint8_t ios_devicex[HKDF_KEY_LEN];
    hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_CONTROLLER, srp_key, SRP_SESSION_KEY_LENGTH, ios_devicex);

    uint8_t ios_device_pairing_id_len 	= encTLV.size(HAP_TLV_IDENTIFIER);
	uint8_t ios_device_pairing_id[ios_device_pairing_id_len];
	encTLV.decode(HAP_TLV_IDENTIFIER, ios_device_pairing_id, &decodedLen);

	if (decodedLen == 0) {
		LogE( F("ERROR: TLV decoding identifier failed"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		encTLV.clear();
		response.clear();
		return false;
	}

	uint8_t  ios_device_ltpk_len 		= encTLV.size(HAP_TLV_PUBLIC_KEY);
	uint8_t ios_device_ltpk[ios_device_ltpk_len];

	encTLV.decode(HAP_TLV_PUBLIC_KEY, ios_device_ltpk, &decodedLen);

	if (decodedLen == 0) {
		LogE( F("ERROR: TLV decoding public key failed"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		encTLV.clear();
		response.clear();
		return false;
	}

    uint8_t  ios_device_signature_len 	= encTLV.size(HAP_TLV_SIGNATURE);
	uint8_t ios_device_signature[ios_device_signature_len];
	encTLV.decode(HAP_TLV_SIGNATURE, ios_device_signature, &decodedLen);

	if (decodedLen == 0) {
		LogE( F("ERROR: TLV decoding signature failed"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);

		HAPHelper::array_print("signature:", ios_device_signature, ios_device_signature_len);


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


    LogV(F("Verifying ED25519 ..."), false);
	int verified = ed25519_verify(ios_device_signature, ios_device_info, ios_device_info_len, ios_device_ltpk);



    concat_free(ios_device_info);

	if (verified < 0) {
        LogE(F("ERROR: Verification failed"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		encTLV.clear();
		response.clear();
        return false;
	}
	LogV( F("OK"), true);




	encTLV.clear();

	Heap(_clients.size(), _eventManager.getNumEventsInQueue());

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-setup Step 4/4 ...", true);
#elif defined( CORE_TEENSY )
	LogV(F(F("<<< Handle client [")), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pair-setup Step 4/4 ..."), false);
#endif

	_eventManager.queueEvent(EventManager::kEventPairingStep4, HAPEvent());


	//  _acc_m6_subtlv(srp_key, ps->acc_id, ps->keys.public, ps->keys.private, &acc_subtlv, &acc_subtlv_length);
	uint8_t accessoryx[HKDF_KEY_LEN] = {0,};
	hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_ACCESSORY, srp_key, SRP_SESSION_KEY_LENGTH,
            accessoryx);


    int acc_info_len = 0;

    uint8_t* acc_info = concat3(accessoryx, sizeof(accessoryx),
            (uint8_t*)HAPDeviceID::deviceID().c_str(), 17,
            _accessorySet->LTPK(), ED25519_PUBLIC_KEY_LENGTH, &acc_info_len);

    LogV( F("\nVerifying signature ..."), false);
    uint8_t acc_signature[ED25519_SIGN_LENGTH] = {0,};

	ed25519_sign(acc_signature, acc_info, acc_info_len, _accessorySet->LTSK(), _accessorySet->LTPK());

    concat_free(acc_info);

    if (err_code != 0) {
        LogE(F("ERROR: Verify signature failed! Reason: ") + String(err_code), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();
        return false;
	}
	LogV(F("OK"), true);

	// Encrypt data
	TLV8 subTLV;
	subTLV.encode(HAP_TLV_IDENTIFIER, 17, (uint8_t*)HAPDeviceID::deviceID().c_str()  );
	subTLV.encode(HAP_TLV_PUBLIC_KEY, ED25519_PUBLIC_KEY_LENGTH, _accessorySet->LTPK());
	subTLV.encode(HAP_TLV_SIGNATURE, ED25519_SIGN_LENGTH, acc_signature);


	size_t tlv8Len = subTLV.size();
	uint8_t tlv8Data[tlv8Len];
	size_t written = 0;

	subTLV.decode(tlv8Data, &written);
	if (written == 0) {
		LogE(F("ERROR: Failed to decode subtlv8!"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
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

	LogV(F("Getting session key ..."), false);
	err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_SETUP_ENCRYPT, srp_key, SRP_SESSION_KEY_LENGTH, subtlv_key);
	if (err_code != 0) {
        LogE(F("ERROR: Getting session key failed! Reason ") + String(err_code), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();
        return false;
	}
	LogV(F("OK"), true);


	LogV(F("Encrypting Data ..."), false);
	err_code = chacha20_poly1305_encrypt(CHACHA20_POLY1305_TYPE_PS06, subtlv_key, NULL, 0, tlv8Data, tlv8Len, encryptedData);

	if (err_code != 0) {
        LogE(F("ERROR: Verify signature failed"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M6, HAP_ERROR_AUTHENTICATON);
		response.clear();

        return false;
	}
	LogV(F("OK"), true);


	LogV(F("Sending response ..."), false);
	response.encode(HAP_TLV_STATE, 1, HAP_PAIR_STATE_M6);
	response.encode(HAP_TLV_ENCRYPTED_DATA, tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH, encryptedData);

#if HAP_DEBUG_TLV8
	response.print();
#endif

	sendResponse(hapClient, &response);
	LogV(F("OK"), true);

	response.clear();


	// Save to Pairings as admin
	LogD(F("Saving pairing ..."), false);
	_accessorySet->addPairing(ios_device_pairing_id, ios_device_ltpk, true);
	LogV( F("OK"), true);


	LogV( F("Updating mDNS ..."), false);
#if defined (ARDUINO_ARCH_ESP32)
	mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber);
	LogV(F("OK"), true);
#else
	updateServiceTxt();
#endif


#if defined( ARDUINO_ARCH_ESP32 )
	LogI(">>> Pairing with client [" + hapClient->client.remoteIP().toString() + "] complete!", true);
#elif defined( CORE_TEENSY )
	LogI(F(">>> Pairing with client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogI(F("] -> complete!"), true);
#endif

	// ToDo: set timeout for resetting to false automatically?
	_isInPairingMode = false;

	_eventManager.queueEvent(EventManager::kEventPairingComplete, HAPEvent());


	hapClient->clear();

	//stopEvents(false);

	Heap(_clients.size(), _eventManager.getNumEventsInQueue());

    return true;
}

/**
 * handlePairVerifyM1
 */
bool HAPServer::handlePairVerifyM1(HAPClient* hapClient){

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-verify Step 1/2 ...", false);
#elif defined( CORE_TEENSY )
	LogV( F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pair-verify Step 1/2 ..."), false);
#endif

	// _eventManager.queueEvent(EventManager::kEventVerifyStep1, HAPEvent());

	int err_code = 0;

	if ( !isPaired() ) {
		LogW(F("\nWARNING: Attempt to verify unpaired accessory!\n"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_UNKNOWN);

		hapClient->clear();
		return false;
	}

	hapClient->encryptionContext.decryptCount = 0;
	hapClient->encryptionContext.encryptCount = 0;

	LogD(F("\nGenerating accessory curve25519 keys ..."), false);

	uint8_t acc_curve_public_key[CURVE25519_SECRET_LENGTH] = {0,};		// my_key_public

	// ToDo: Fix this! This will stay constant 0x00
	// ?? This will stay at 00 ?
	// What key to use here?
	// int r = crypto_ed25519_generate(key);
	// uint8_t acc_curve_private_key[CURVE25519_SECRET_LENGTH] = {0,};	// my_key

	// Create new public key from accessory's LTSK
	err_code = X25519_scalarmult_base(acc_curve_public_key, _accessorySet->LTSK());
	if (err_code < 0) {
		LogE(F("ERROR: X25519_scalarmult_base failed! Reason: ") + String(err_code), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_UNKNOWN);

		hapClient->clear();
		return false;
	}
	else {
		LogD(F("OK"), true);
	}

	uint8_t ios_device_curve_key_len = hapClient->request.tlv.size(HAP_TLV_PUBLIC_KEY);
	if (ios_device_curve_key_len == 0){
		LogE(F("ERROR: ios_device_curve_key_len is 0!"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_UNKNOWN);

		hapClient->clear();
		return false;
	}


	uint8_t ios_device_curve_key[ios_device_curve_key_len];
	size_t decodedLen = 0;
	hapClient->request.tlv.decode(HAP_TLV_PUBLIC_KEY, ios_device_curve_key, &decodedLen);

	if (decodedLen == 0) {
		LogE( F("ERROR: PAIR-VERIFY M1 - HAP_TLV_ENCRYPTED_DATA failed "), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_AUTHENTICATON);

		hapClient->clear();
		return false;
	}

#if HAP_DEBUG_HOMEKIT

	HAPHelper::array_print("acc_curve_public_key", acc_curve_public_key, CURVE25519_SECRET_LENGTH);
	HAPHelper::array_print("_accessorySet->LTSK()", _accessorySet->LTSK(), HAP_PAIRINGS_LTSK_LENGTH);
	HAPHelper::array_print("ios_device_curve_key", ios_device_curve_key, ios_device_curve_key_len);

#endif

	// shared_secret
	LogD(F("Generating Curve25519 shared secret ..."), false);
	uint8_t sharedSecret[CURVE25519_SECRET_LENGTH] = {0,};

	if (X25519_scalarmult(sharedSecret, _accessorySet->LTSK(), ios_device_curve_key) < 0) {
		LogE( F("ERROR: X25519_scalarmult failed"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_AUTHENTICATON);

		hapClient->clear();
		return false;
	}
	LogD( F("OK"), true);

	LogD(F("Generating signature ..."), false);
	int acc_info_len;
	uint8_t* acc_info = concat3(acc_curve_public_key, CURVE25519_SECRET_LENGTH,
		(uint8_t*)HAPDeviceID::deviceID().c_str(), 17,
		ios_device_curve_key, ios_device_curve_key_len,
		&acc_info_len);


	uint8_t acc_signature[ED25519_SIGN_LENGTH] = {0,};
	ed25519_sign(acc_signature, acc_info, acc_info_len, _accessorySet->LTSK(), _accessorySet->LTPK());
	concat_free(acc_info);
	LogD(F("OK"), true);


	// Encrypt data

	LogD(F("Encoding into TLV ..."), false);
	TLV8 *subTLV = new TLV8();
	subTLV->encode(HAP_TLV_IDENTIFIER, 17, (uint8_t*)HAPDeviceID::deviceID().c_str()  );
	subTLV->encode(HAP_TLV_SIGNATURE, ED25519_SIGN_LENGTH, acc_signature);

	size_t tlv8Len = subTLV->size();
	uint8_t tlv8Data[tlv8Len];
	size_t written = 0;

	subTLV->decode(tlv8Data, &written);
	if (written == 0) {
		LogE(F("ERROR: Failed to decode subtlv8!"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_AUTHENTICATON);

		subTLV->clear();
		delete subTLV;

		return false;
	}

#if HAP_DEBUG_TLV8
	subTLV->print();
	//HAPHelper::arrayPrint(tlv8Data, tlv8Len);
#endif
	LogD(F("OK"), true);


	LogD(F("Generating proof ..."), false);
    uint8_t sessionKey[HKDF_KEY_LEN] = {0,};   		// session_key
    err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_VERIFY_ENCRYPT, sharedSecret, CURVE25519_SECRET_LENGTH, sessionKey);
	if (err_code != 0) {
        LogE(F("ERROR: Get HKDF key failed! Reason: ") + String(err_code), true);
        sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_AUTHENTICATON);

		subTLV->clear();
		delete subTLV;

        return false;
	}
	LogD(F("OK"), true);



	LogD(F("Encrypting data ..."), false);
	// uint8_t* encryptedData;
	// encryptedData = (uint8_t*)malloc(sizeof(uint8_t) * (tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH));
	uint8_t encryptedData[tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH];


	err_code = chacha20_poly1305_encrypt(CHACHA20_POLY1305_TYPE_PV02, sessionKey, NULL, 0, tlv8Data, tlv8Len, encryptedData);
	if (err_code != 0) {
        LogE(F("ERROR: Encrypting failed! Reason: ") + String(err_code), true);
        sendErrorTLV(hapClient, HAP_VERIFY_STATE_M2, HAP_ERROR_AUTHENTICATON);

		subTLV->clear();
		delete subTLV;

        return false;
	}
	LogD( F("OK"), true);

	LogD(F("Saving context ..."), false);
	memcpy(hapClient->verifyContext.secret, sharedSecret, HKDF_KEY_LEN);
	memcpy(hapClient->verifyContext.accessoryLTPK, acc_curve_public_key, ED25519_PUBLIC_KEY_LENGTH);
	memcpy(hapClient->verifyContext.deviceLTPK, ios_device_curve_key, ED25519_PUBLIC_KEY_LENGTH);
	LogD( F("OK"), true);


	LogD( F("Sending response ..."), false);
	TLV8 response;
	response.encode(HAP_TLV_STATE, 1, HAP_VERIFY_STATE_M2);
	response.encode(HAP_TLV_PUBLIC_KEY, CURVE25519_SECRET_LENGTH, acc_curve_public_key);
	response.encode(HAP_TLV_ENCRYPTED_DATA, tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH, encryptedData);


#if HAP_DEBUG_TLV8
	response.print();
	//HAPHelper::arrayPrint(encryptedData, tlv8Len + CHACHA20_POLY1305_AUTH_TAG_LENGTH);
#endif


	sendResponse(hapClient, &response);
	LogD( F("OK"), true);
	response.clear();

	subTLV->clear();
	delete subTLV;

	hapClient->clear();

	LogI(F("OK"), true);

	return true;
}


bool HAPServer::handlePairVerifyM3(HAPClient* hapClient){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> /pair-verify Step 2/2 ...", false);
#elif defined( CORE_TEENSY )
	LogV(F("Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pair-verify Step 2/2 ..."), false);
#endif



	// _eventManager.queueEvent(EventManager::kEventVerifyStep2, HAPEvent());

	int err_code = 0;

	int encryptedDataLen = hapClient->request.tlv.size(HAP_TLV_ENCRYPTED_DATA);
	size_t decodedLen = 0;

	uint8_t encryptedData[encryptedDataLen];
	hapClient->request.tlv.decode(HAP_TLV_ENCRYPTED_DATA, encryptedData, &decodedLen);


	if (decodedLen == 0) {
		LogE(F("ERROR: PAIR-VERIFY M3 - HAP_TLV_ENCRYPTED_DATA failed "), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		return false;
	}

	LogD(F("\nGenerating decrpytion key ..."), false);
	uint8_t subtlv_key[HKDF_KEY_LEN] = {0,};
	err_code = hkdf_key_get(HKDF_KEY_TYPE_PAIR_VERIFY_ENCRYPT, hapClient->verifyContext.secret, HKDF_KEY_LEN, subtlv_key);
	if (err_code != 0) {
		LogE( F("ERROR: PAIR-VERIFY M3 - hkdf_key_get failed"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		return false;
	}
	LogD(F("OK"), true);



	LogD(F("Decrypting data ..."), false);
	size_t decryptedLen = encryptedDataLen - CHACHA20_POLY1305_AUTH_TAG_LENGTH;
	uint8_t subtlvData[decryptedLen] = {0,};

	// Serial.println("chacha20_poly1305_decrypt");
	// Serial.send_now();

#if HAP_DEBUG_HOMEKIT
	HAPHelper::array_print("subtlv_key", subtlv_key, HKDF_KEY_LEN);
	HAPHelper::array_print("encryptedData", encryptedData, decryptedLen);
	HAPHelper::array_print("mac", encryptedData + decryptedLen, CHACHA20_POLY1305_AUTH_TAG_LENGTH);
#endif

	err_code = chacha20_poly1305_decrypt(CHACHA20_POLY1305_TYPE_PV03, subtlv_key, NULL, 0, encryptedData, encryptedDataLen, subtlvData);
	if (err_code != 0) {
		LogE(F("[ERROR] Decrypting failed: Reason: ") + String(err_code), true);

#if HAP_DEBUG_HOMEKIT
		// HAPHelper::array_print("subtlv_key", subtlv_key, HKDF_KEY_LEN);
		// HAPHelper::array_print("encryptedData", encryptedData, encryptedDataLen - 16);
		// HAPHelper::array_print("mac", encryptedData + (encryptedDataLen - 16), 16);
		HAPHelper::array_print("subtlvData", subtlvData, decryptedLen);
#endif

		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		return false;
	}
	LogD( F("OK"), true);

#if HAP_DEBUG_TLV8
	HAPHelper::array_print("subtlvData", subtlvData, decryptedLen);
#endif

	TLV8 subTlv;
	subTlv.encode(subtlvData, decryptedLen);

// #if HAP_DEBUG
// 	LogD("subTLV: ", true);
// 	subTlv.print();
// #endif


	uint8_t ios_device_pairing_id_len 	= subTlv.size(HAP_TLV_IDENTIFIER);
	uint8_t ios_device_pairing_id[ios_device_pairing_id_len];
	subTlv.decode(HAP_TLV_IDENTIFIER, ios_device_pairing_id, &decodedLen);

	if (decodedLen == 0) {
		LogE(F("ERROR: HAP_TLV_IDENTIFIER failed!"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		subTlv.clear();
		return false;
	}


#if HAP_DEBUG_HOMEKIT
	LogD(F("Looking up iOS device LTPK for client: "), true);
	HAPHelper::array_print("ios_device_pairing_id", ios_device_pairing_id, ios_device_pairing_id_len);
#endif

	uint8_t* ios_device_ltpk = _accessorySet->getKeyForPairingWithId(ios_device_pairing_id);
	// err_code = _accessorySet->getPairings()->getKey(ios_device_pairing_id, ios_device_ltpk);

	if (err_code == -1) {
		LogE( F("ERROR: No iOS Device LTPK found!"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		subTlv.clear();
		return false;
	}

#if HAP_DEBUG_HOMEKIT
	LogD("Found LTPK: ", true);
	HAPHelper::array_print("ios_device_ltpk", ios_device_ltpk, ED25519_PUBLIC_KEY_LENGTH);
#endif

	uint8_t ios_device_signature_len = subTlv.size(HAP_TLV_SIGNATURE);

	uint8_t ios_device_signature[ios_device_signature_len];
	subTlv.decode(HAP_TLV_SIGNATURE, ios_device_signature, &decodedLen);

	if (decodedLen == 0) {
		LogE( F("ERROR: PAIR-VERIFY M3 - HAP_TLV_ENCRYPTED_DATA failed "), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		subTlv.clear();
		return false;
	}

#if HAP_DEBUG_HOMEKIT
	LogD(F("Found Signature: "), true);
	HAPHelper::array_print("ios_device_signature", ios_device_signature, ios_device_signature_len);
#endif

#if HAP_DEBUG_HOMEKIT
	LogD(F("pairingIdIsAdmin: "), false);
	LogD(_accessorySet->pairingIdIsAdmin(ios_device_pairing_id), true);
#endif


	int ios_device_info_len = 0;
    uint8_t* ios_device_info = concat3(hapClient->verifyContext.deviceLTPK, ED25519_PUBLIC_KEY_LENGTH,
            ios_device_pairing_id, ios_device_pairing_id_len,
            hapClient->verifyContext.accessoryLTPK, ED25519_PUBLIC_KEY_LENGTH, &ios_device_info_len);



    LogD( F("Verifying Signature ..."), false);
	int verified = ed25519_verify(ios_device_signature, ios_device_info, ios_device_info_len, ios_device_ltpk);


    concat_free(ios_device_info);
	if (verified < 0) {
        LogE( F("ERROR: Signature verification failed"), true);
        sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		subTlv.clear();
        return false;
	}

	LogD( F("OK"), true);


	LogD( F("Generating session keys ..."), false);
    err_code = hkdf_key_get(HKDF_KEY_TYPE_CONTROL_READ, hapClient->verifyContext.secret, CURVE25519_SECRET_LENGTH, hapClient->encryptionContext.encryptKey);
	if (err_code != 0) {
		LogE( F("ERROR: HKDF encrpytion key not available!"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		subTlv.clear();
		return false;
	}



	err_code = hkdf_key_get(HKDF_KEY_TYPE_CONTROL_WRITE, hapClient->verifyContext.secret, CURVE25519_SECRET_LENGTH, hapClient->encryptionContext.decryptKey);
	if (err_code != 0) {
		LogE( F("ERROR: HKDF decryption key not available!"), true);
		sendErrorTLV(hapClient, HAP_VERIFY_STATE_M4, HAP_ERROR_AUTHENTICATON);
		subTlv.clear();
		return false;
	}
	LogD( F("OK"), true);

	// ToDo: FREE CONTEXT ??

	LogD( F("Sending response ..."), true);
	TLV8 response;
	response.encode(HAP_TLV_STATE, 1, HAP_VERIFY_STATE_M4);

#if HAP_DEBUG_TLV8
	response.print();
#endif

	sendResponse(hapClient, &response);

	subTlv.clear();
	response.clear();

	hapClient->clear();

	// following messages from this client will be encrypted
	hapClient->setEncryped(true);
	hapClient->setId(ios_device_pairing_id);
	hapClient->setAdmin(_accessorySet->pairingIdIsAdmin(ios_device_pairing_id));

	LogV(F("OK"), true);

#if defined( ARDUINO_ARCH_ESP32 )
	LogI(">>> Verification with client [" + hapClient->client.remoteIP().toString() + "] complete!", true);
#elif defined( CORE_TEENSY )
	LogI(F("Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogI(F("] -> Pair verify complete!"), true);
#endif

	_eventManager.queueEvent(EventManager::kEventVerifyComplete, HAPEvent());

	return true;
}


void HAPServer::handleAccessories(HAPClient* hapClient) {


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( "<<< Handle [" + hapClient->client.remoteIP().toString() + "] -> /accessories ...", false);
#elif defined( CORE_TEENSY )
	// LogV( "<<< Handle [" + String(hapClient->client.remoteIP()) + "] -> /accessories ...", false);
	LogV(F("Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /accessories ..."), false);

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

	LogV(F("OK"), true);
	hapClient->state = HAP_CLIENT_STATE_IDLE;


	hapClient->clear();
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsList(HAPClient* hapClient){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings list ...", false);
#elif defined( CORE_TEENSY )
	//LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings list ...", false);
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pairings list ..."), false);
#endif

	if (hapClient->isAdmin() == false){
		LogE(F("ERROR: Non-Admin controllers are not allowed to call this method!"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_AUTHENTICATON);
		return;
	}

	TLV8 response;
	response.encode(HAP_TLV_STATE, 1, HAP_PAIR_STATE_M2);

	// for (int i=0; i<_accessorySet->getPairings()->size(); i++){
	for (int i=0; i<_accessorySet->numberOfPairings(); i++){

#if HAP_HOMEKIT_PYTHON_COMPATIBLE == 0
		if (i > 0) {
			response.addSeperator();
		}
#endif

		// response.encode(HAP_TLV_IDENTIFIER, HAP_PAIRINGS_ID_LENGTH, _accessorySet->getPairings()->pairings[i].id);
		// response.encode(HAP_TLV_PUBLIC_KEY, HAP_PAIRINGS_LTPK_LENGTH, _accessorySet->getPairings()->pairings[i].key);
		// response.encode(HAP_TLV_PERMISSIONS, 1, _accessorySet->getPairings()->pairings[i].isAdmin);

		response.encode(HAP_TLV_IDENTIFIER, HAP_PAIRINGS_ID_LENGTH, _accessorySet->getPairingAtIndex(i)->id);
		response.encode(HAP_TLV_PUBLIC_KEY, HAP_PAIRINGS_LTPK_LENGTH, _accessorySet->getPairingAtIndex(i)->id);
		response.encode(HAP_TLV_PERMISSIONS, 1, _accessorySet->getPairingAtIndex(i)->isAdmin);
	}

#if HAP_DEBUG_TLV8
	response.print();
#endif

	//sendResponse(hapClient, &response);
	uint8_t data[response.size()];
	size_t length = 0;

	response.decode(data, &length);
	// sendEncrypt(hapClient, HTTP_200, data, length, true, "application/pairing+tlv8");
	send(hapClient, HTTP_200, data, length, HAP_ENCRYPTION_MODE_ENCRYPT, "application/pairing+tlv8");

	response.clear();

	hapClient->clear();

	hapClient->state = HAP_CLIENT_STATE_IDLE;

	LogV(F("OK"), true);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsAdd(HAPClient* hapClient, const uint8_t* identifier, const uint8_t* publicKey, bool isAdmin){

#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings add ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings add ...", false);
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pairings add ..."), false);
#endif


	if (hapClient->isAdmin() == false){
		LogE(F("ERROR: Non-Admin controllers are not allowed to call this method!"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_AUTHENTICATON);
		return;
	}

	TLV8 response;
	response.encode(HAP_TLV_STATE, 1, HAP_PAIR_STATE_M2);

	if (_accessorySet->numberOfPairings() >= HAP_PAIRINGS_MAX) {
		response.encode(HAP_TLV_ERROR, 1, HAP_ERROR_MAX_PEERS);
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
	send(hapClient, HTTP_200, data, length, HAP_ENCRYPTION_MODE_ENCRYPT, "application/pairing+tlv8");

	response.clear();

	hapClient->clear();

	hapClient->state = HAP_CLIENT_STATE_IDLE;
	LogV(F("OK"), true);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsRemove(HAPClient* hapClient, const uint8_t* identifier){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings remove ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings remove ...", false);
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pairings remove ..."), false);
#endif


	bool removeItsOwnPairings = false;

	// id identifier is controllers id, then disonnect
	if (memcmp(identifier, hapClient->getId(), HAP_PAIRINGS_ID_LENGTH) == 0) {
		LogD(F("Remove its own pairing"), true);
		removeItsOwnPairings = true;
	}

	// ToDo:
	// According to the spec, only admin controllers can delete pairings.
	//
	// But this will deny removing ones own pairings
	// i will allow this
	//
	if ( (hapClient->isAdmin() == false) && (removeItsOwnPairings == false) ) {
		LogE(F("ERROR: Non-Admin controllers are not allowed to call this method!"), true);
		sendErrorTLV(hapClient, HAP_PAIR_STATE_M2, HAP_ERROR_AUTHENTICATON);
		return;
	}

	TLV8 response;
	response.encode(HAP_TLV_STATE, 1, HAP_PAIR_STATE_M2);


	// if not paired
	LogD(F("Removing pairings ..."), false);
	_accessorySet->removePairing(identifier);
	LogD(F("OK"), true);

	// send response


#if HAP_DEBUG_TLV8
	response.print();
#endif

	uint8_t data[response.size()];
	size_t length = 0;

	response.decode(data, &length);
	//sendEncrypt(hapClient, HTTP_200, data, length, true, "application/pairing+tlv8");
	send(hapClient,  HTTP_200, data, length, HAP_ENCRYPTION_MODE_ENCRYPT, "application/pairing+tlv8");

	response.clear();


	if (removeItsOwnPairings) {
		hapClient->client.stop();
	}


	hapClient->clear();

	// tear down all other pairings if admin removed
	// and update mdns
	if (!_accessorySet->isPaired()) {

		hapClient->state = HAP_CLIENT_STATE_ALL_PAIRINGS_REMOVED;
		_eventManager.queueEvent(EventManager::kEventAllPairingsRemoved, HAPEvent());

		// update mdns
#if defined( ARDUINO_ARCH_ESP32)
		if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
			LogE( "ERROR: Updating HAP service txt failed!", true);
			return;
		}
#else
		if ( !updateServiceTxt() ){
			LogE( F("ERROR: Advertising HAP service failed!"), true);
			return;
		}
#endif

	}

	LogV(F("OK"), true);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handlePairingsPost(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen){



#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> POST /pairings ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] -> POST /pairings ...", false);
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> /pairings ..."), false);
#endif


	TLV8 tlv;
	tlv.encode(bodyData, bodyDataLen);

#if HAP_DEBUG_TLV8
	tlv.print();
#endif

	// TLV8Entry *entry = tlv.searchType(HAP_TLV_METHOD); // 0x01
	// HAP_TLV_PAIR_TYPE method = (HAP_TLV_PAIR_TYPE) entry->value[0];

	TLV8Entry *entry = tlv.searchType(HAP_TLV_METHOD);
	HAP_TLV_PAIR_TYPE method = (HAP_TLV_PAIR_TYPE) entry->value[0];

	if (method == HAP_TLV_PAIR_ADD) {

		TLV8Entry *entryIdentifier = tlv.searchType(HAP_TLV_IDENTIFIER); // 0x01
		TLV8Entry *entryPublicKey = tlv.searchType(HAP_TLV_PUBLIC_KEY); // 0x03
		TLV8Entry *entryAdmin = tlv.searchType(HAP_TLV_PERMISSIONS); // 0x0b

		handlePairingsAdd(hapClient, entryIdentifier->value, entryPublicKey->value, *(entryAdmin->value) );

	} else if (method == HAP_TLV_PAIR_REMOVE) {
		TLV8Entry *entryIdentifier = tlv.searchType(HAP_TLV_IDENTIFIER); // 0x01
		handlePairingsRemove(hapClient, entryIdentifier->value);
	} else if (method == HAP_TLV_PAIR_LIST) {
		handlePairingsList(hapClient);
	}

	tlv.clear();

	LogV(F("OK"), true);
}


void HAPServer::handleCharacteristicsGet(HAPClient* hapClient){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] -> GET /characteristics ...", false);
#elif defined( CORE_TEENSY )
	// LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] -> GET /characteristics ...", false);
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> GET /characteristics ..."), false);
#endif

	String idStr = hapClient->request.params["id"];
	//LogE(idStr, true);

	bool hasParamMeta = false;
	bool hasParamPerms = false;
	bool hasParamEvent = false;
	bool hasParamType = false;
	LogD(F(""), true);

	for (const auto &p : hapClient->request.params) {

#if HAP_DEBUG
    	LogD(F("param: ") + p.first + F(" - ") + p.second, true);
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
		int endIndex = idStr.indexOf(",");
		if (endIndex == -1){
			endIndex = idStr.length();
		}

		String keyPair = idStr.substring(curPos, endIndex);

		uint16_t equalIndex = keyPair.indexOf(".");

		uint8_t aid = keyPair.substring(0, equalIndex).toInt();
		uint8_t iid = keyPair.substring(equalIndex + 1).toInt();


		JsonObject jsonCharacteristic = jsonCharacteristics.createNestedObject();
		jsonCharacteristic["aid"] = aid;

		HAPCharacteristicBase* characteristic = _accessorySet->getCharacteristic(aid, iid);
		if (characteristic) {
			if (characteristic->readable()){

				// characteristic->valueGetFunctionCall();

				characteristic->toJson(jsonCharacteristic, hasParamType, hasParamPerms, hasParamEvent, hasParamMeta);

			} else {
				jsonCharacteristic["iid"] = iid;
				jsonCharacteristic["status"] = String(HAP_STATUS_WRITEONLY_READ);
				errorOccured = true;
			}

		} else {
			jsonCharacteristic["iid"] = iid;
			jsonCharacteristic["status"] = String(HAP_STATUS_RESOURCE_NOT_FOUND);
			errorCode = -1;
			errorOccured = true;
		}

		if (endIndex < idStr.length()) {
			idStr = idStr.substring(endIndex + 1);
		} else {
			break;
		}

	} while ( idStr.length() > 0 );

	// String response;
	// serializeJson(responseRoot, response);

#if HAP_DEBUG_HOMEKIT
	serializeJson(responseRoot, Serial);
#endif

	if (errorCode == -1){
		// Accessory not found
		// sendEncrypt(hapClient, HTTP_400, response, true);
		send(hapClient, HTTP_400, responseRoot, HAP_ENCRYPTION_MODE_ENCRYPT);
	} else if (errorOccured == false) {
		// everything ok
		// String response;
		// serializeJson(responseRoot, response);
		// sendEncrypt(hapClient, HTTP_200, response, true);

		send(hapClient, HTTP_200, responseRoot, HAP_ENCRYPTION_MODE_ENCRYPT);
	} else if (errorOccured == true) {
		// partial ok
    	// sendEncrypt(hapClient, HTTP_207, response, true);
		send(hapClient, HTTP_207, responseRoot, HAP_ENCRYPTION_MODE_ENCRYPT);
	}

	LogV(F("OK"), true);
	hapClient->state = HAP_CLIENT_STATE_IDLE;

}

void HAPServer::handleCharacteristicsPut(HAPClient* hapClient, String body){


#if defined( ARDUINO_ARCH_ESP32 )
	LogV( F("<<< Handle client [") + hapClient->client.remoteIP().toString() + "] ->  PUT /characteristics ...", true);
#elif defined( CORE_TEENSY )
	// LogV( F("<<< Handle client [") + String(hapClient->client.remoteIP()) + "] ->  PUT /characteristics ...", true);
	LogV(F("<<< Handle client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] -> PUT /characteristics ..."), false);
#endif

	DynamicJsonDocument root(2048);
	DeserializationError error = deserializeJson(root, body);

	if (error) {
    	LogE(F("ERROR: Parsing put characteristics request failed!"), true);
		// ToDo Send Error Response to client
    	return;
  	}

#if HAP_DEBUG
	serializeJson(root, Serial);
	Serial.println();
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
						struct HAPEvent event = HAPEvent(hapClient, aid, iid, character->valueString());
						_eventManager.queueEvent( EventManager::kEventNotifyController, event);
					}

				} else {
					// char has no event permission
					LogW(F("WARNING: Resource notify not permitted for characteristic ") + String(aid) + F(".") + String(iid), true);
					jsonNewChr[F("iid")] = iid;
					jsonNewChr[F("status")] = String(HAP_STATUS_NO_NOTIFICATION);
					errorOccured = true;
				}
			} else {

				if (character->writable() ) {
					character->valueFromString(jc["value"].as<String>());
					// Add to jsonCharacteristics array
					character->toJson(jsonNewChr);
				} else {
					LogW(F("WARNING: Resource not writable for characteristic ") + String(aid) + F(".") + String(iid), true);
					jsonNewChr[F("iid")] = iid;
					jsonNewChr[F("status")] = String(HAP_STATUS_READONLY_WRITE);
					errorOccured = true;
				}
			}


		} else {
			LogE(F("ERROR: Resource not found for characteristic ") + String(aid) + F(".") + String(iid), true);
			jsonNewChr[F("iid")] = iid;
			jsonNewChr[F("status")] = String(HAP_STATUS_RESOURCE_NOT_FOUND);
			errorOccured = true;
		}
	}

	if (errorOccured){
		// String response;
		// serializeJson(responseRoot, response);
		// sendEncrypt(hapClient, HTTP_207, response, false);
		send(hapClient, HTTP_207, responseRoot, HAP_ENCRYPTION_MODE_ENCRYPT);
	} else {
		// sendEncrypt(hapClient, HTTP_204, "", false);
		send(hapClient, HTTP_204, "", 0, HAP_ENCRYPTION_MODE_ENCRYPT);
	}

	hapClient->state = HAP_CLIENT_STATE_IDLE;
	LogV(F("OK"), true);

}

void HAPServer::handleEventUpdateConfigNumber( int eventCode, struct HAPEvent eventParam ){
	_accessorySet->configurationNumber++;
	// updateServiceTxt();
#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
		LogE( F("ERROR: Updating HAP service txt failed!"), true);
		return;
	}
#else
	if ( !updateServiceTxt() ){
		LogE( F("ERROR: Advertising HAP service failed!"), true);
		return;
	}
#endif
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleEventRebootNow(int eventCode, struct HAPEvent eventParam){
	LogW(F("*********************************************************"), true);
	LogW(F("*                                                       *"), true);
	LogW(F("*                 !!! Rebooting now !!!                 *"), true);
	LogW(F("*                                                       *"), true);
	LogW(F("*********************************************************"), true);

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

	LogI(F("Delete config!"), true);

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

	LogI( F("Delete all pairings!"), true);

	_accessorySet->removeAllPairings();

#if HAP_ENABLE_PIXEL_INDICATOR
	_pixelIndicator.blinkWithColor(CRGB::Red, 5);
#endif

#if defined( ARDUINO_ARCH_ESP32)
	if (!mDNSExt.updateHomekitTxt(_accessorySet->isPaired(), _accessorySet->configurationNumber)){
		LogE( "ERROR: Updating HAP service txt failed!", true);
		return;
	}
#else
	if ( !updateServiceTxt() ){
		LogE( F("ERROR: Advertising HAP service failed!"), true);
		return;
	}
#endif
}


void HAPServer::handleEvents( int eventCode, struct HAPEvent eventParam )
{

	// Stopping events
	if (stopEvents() == true) {
		return;
	}

	if (_clients.size() > 0){
		int count = 0;
		int totalEvents = _eventManager.getNumEventsInQueue();
		int noOfEvents = _eventManager.getNumEventCodeInQueue(eventCode);
		struct HAPEvent evParams[noOfEvents + 1];
		int addedToHomekitEvent = 0;

		// add
		if (eventCode == EventManager::kEventNotifyController) {
			evParams[addedToHomekitEvent++] = eventParam;
		}

		while (!_eventManager.isEventQueueEmpty()){

			int evCode;
			struct HAPEvent evParam;
			if (_eventManager.popEvent(&evCode, &evParam)){

				if (evCode == EventManager::kEventNotifyController) {


					evParams[addedToHomekitEvent++] = evParam;
				} else {
					// Add again to queue
					_eventManager.queueEvent(evCode, evParam);
				}
				count++;

				if (count == totalEvents + 1){
					break;
				}
			}
		}

		for (auto& hapClient : _clients) {

			const size_t bufferSize = 512;
			DynamicJsonDocument root(bufferSize);

			JsonArray jsonCharacteristics = root.createNestedArray("characteristics");
			String response = "";

			bool isSubcribedToAtLeastOne = false;

			for (int i=0; i < addedToHomekitEvent; i++){

				int aid = evParams[i].aid;
				int iid = evParams[i].iid;


				if ( hapClient->isSubscribed(aid, iid) ) {
					HAPCharacteristicBase* character = _accessorySet->getCharacteristic(aid, iid);

					if (character) {

						LogD(F(">>> Handle event - code: ") + String(eventCode) + F(" aid: ") + String(aid) + F(" iid: ") + String(iid) + F(" - value: "), false);
    					LogD(evParams[i].value, true);

						JsonObject chr = jsonCharacteristics.createNestedObject();
						chr["aid"] = aid;
						character->toJson(chr, false, false, false, false, false, false);

						// Serial.print("event json: ");
						// serializeJsonPretty(chr, Serial);
						// Serial.println();

						isSubcribedToAtLeastOne = true;
					} else {
						LogE(F(">>> Not notifiable event - code: ") + String(eventCode) + F(" aid: ") + String(aid) + F(" iid: ") + String(iid) + F(" - value: "), false);
    					LogE(evParams[i].value, true);
					}
				}
			}

			// serializeJson(root, response);

			if (isSubcribedToAtLeastOne) {
#if HAP_DEBUG
				Serial.print(F("Event response: "));
				serializeJson(root, Serial);
				Serial.println();
#endif
				sendEvent(hapClient, root);
			}
		}

	}

};


bool HAPServer::sendEvent(HAPClient* hapClient, const JsonDocument& response){

#if defined( ARDUINO_ARCH_ESP32 )
	LogD(">>> Sending event to client [" + hapClient->client.remoteIP().toString() + "] ...", false);
#elif defined( CORE_TEENSY )
	// LogD(">>> Sending event to client [" + String(hapClient->client.remoteIP()) + "] ...", false);
	LogV(F("Sending event to client ["), false);
	Serial.print(hapClient->client.remoteIP());
	LogV(F("] ..."), false);

#endif

	if ( hapClient->client.connected() ){
		// sendEncrypt(hapClient, EVENT_200, response, true);
		send(hapClient, EVENT_200, response, HAP_ENCRYPTION_MODE_ENCRYPT_CHUNKED);
		LogV(F("OK"), true);
		return true;
	}

	LogW(F("WARNING: No client available to send the event to!"), true);
	return false;

}

bool HAPServer::isPaired(){
	return _accessorySet->isPaired();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::stopPlugins(bool value){
	if (value)
		_stopPlugins = false;
	else
		_stopPlugins = true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::__setFirmware(const char* name, const char* version, const char* rev) {

	if (strlen(name) + 1 - 10 > MAX_FIRMWARE_NAME_LENGTH || strlen(version) + 1 - 10 > MAX_FIRMWARE_VERSION_LENGTH) {
		LogE( F("[ERROR] Either the name or version string is too long"), true);
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
		LogE(F("[ERROR] The brand string is too long"), true);
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
	LogE(F("Updating configuration ..."), false);

	HAPLogger::setLogLevel(_configuration.getPlatformConfig()->logLevel());

	// ToDo: Update plugins and reinit
	// for (auto& plugin : _plugins) {
    // 	plugin->setConfig(_config.config()["plugins"][plugin->name()]);
	// }

	LogE(F("OK"), true);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPServer::handleEventUpdatedConfig(int eventCode, struct HAPEvent eventParam){

	// ToDo: find error
	LogI(F("Handle update config event"), true);
	const size_t capacity = HAP_ARDUINOJSON_BUFFER_SIZE;
    DynamicJsonDocument doc(capacity);

#if HAP_DEBUG_CONFIG
	Serial.println("before merging:");
	_configuration.toJson(Serial);
#endif

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
// 	Serial.println("after merging:");
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
	Serial.print("CALLBACK CLICK! - ");
	Serial.print("current wifi mode: ");
	Serial.println((uint8_t)_wifi.getNextMode());

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
	Serial.print("CALLBACK DOUBLE CLICK! - ");
	Serial.print("Set default wifi mode: ");
	Serial.println(HAP_WIFI_MODE_DEFAULT);

	_configuration.getWiFiConfig()->setWifiMode((uint8_t)HAP_WIFI_MODE_DEFAULT);
	_eventManager.queueEvent( EventManager::kEventUpdatedConfig, HAPEvent());

#if HAP_ENABLE_PIXEL_INDICATOR
	CRGB col = _wifi.getColorForMode((HAP_WIFI_MODE)HAP_WIFI_MODE_DEFAULT);
	_pixelIndicator.confirmWithColor(col);
#endif

}

void HAPServer::callbackHold(){
	Serial.print("CALLBACK HOLD! - ");
	Serial.println("Delete config");

	_eventManager.queueEvent( EventManager::kEventConfigReset, HAPEvent());
}

void HAPServer::callbackLongHold(){
	Serial.print("CALLBACK LONG HOLD! - ");
	Serial.println("Delete all pairings");

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
