//
// HAPServer.hpp
// Homekit
//
//  Created on: 08.08.2017
//      Author: michael
//

#ifndef HAPSERVER_HPP_
#define HAPSERVER_HPP_

#include <Arduino.h>
#include <vector>
#include <map>

#include "HAPGlobals.hpp"

#include <ArduinoJson.h>
#include <StreamUtils.h>

#include "HAPClient.hpp"
#include "HAPAccessorySet.hpp"
#include "HAPVerifyContext.hpp"
#include "HAPVersion.hpp"
#include "HAPSRP.hpp"
#include "HAPTLV8Types.hpp"
#include "HAPFakegatoFactory.hpp"
#include "HAPTime.hpp"
#include "HAPPrintEncrypted.hpp"
#include "EventManager.h"

#include "HAPPlugins.hpp"
#include "plugins/Plugins.hpp"

// #include "HAPEventManager.hpp"

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFiClient.h>
#include <WiFiServer.h>

#if HAP_USE_PREFERENCES
#include "HAPConfigurationPreferences.hpp"
#endif

#if HAP_USE_EEPROM
#include "HAPConfigurationEEPROM.hpp"
#endif

#include "HAPWiFiHelper.hpp"

#if HAP_ENABLE_WEBSERVER
#include "HAPWebServer.hpp"
#endif

#if HAP_ENABLE_KEYSTORE
#include "HAPKeystore.hpp"
#endif


#if HAP_WEBSERVER_USE_SPIFFS
#include <FS.h>
#include <SPIFFS.h>
#endif

#elif defined(CORE_TEENSY)

#if HAP_USE_EEPROM
#include "HAPConfigurationEEPROM.hpp"
#endif

#if HAP_USE_SPIFFS_CONFIGURATION
#include "HAPConfigurationT41SPIFFSExt.hpp"
#endif


#ifdef HAP_ENABLE_WEBSERVER
#undef HAP_ENABLE_WEBSERVER
#endif

#include <NativeEthernet.h>
#endif



#if HAP_ENABLE_PIXEL_INDICATOR
#include "HAPIndicatorPixel.hpp"
#endif

#if HAP_ENABLE_UPDATE_OTA || HAP_ENABLE_UPDATE_WEB
#include "HAPUpdate.hpp"
#endif



#define Homekit_setFirmware(name, version, rev) \
const char* __FLAGGED_FW_NAME 		= "\xbf\x84\xe4\x13\x54" name "\x93\x44\x6b\xa7\x75"; \
const char* __FLAGGED_FW_VERSION 	= "\x6a\x3f\x3e\x0e\xe1" version "\xb0\x30\x48\xd4\x1a"; \
const char* __FLAGGED_FW_REV 		= "\x6a\x3f\x3e\x0e\xe2" rev "\xb0\x30\x48\xd4\x1b"; \
hap.__setFirmware(__FLAGGED_FW_NAME, __FLAGGED_FW_VERSION, __FLAGGED_FW_REV);

#define Homekit_setBrand(brand) \
const char* __FLAGGED_BRAND = "\xfb\x2a\xf5\x68\xc0" brand "\x6e\x2f\x0f\xeb\x2d"; \
hap.__setBrand(__FLAGGED_BRAND);

#if HAP_PRINT_QRCODE
#if HAP_GENERATE_XHM
#else
#error HAP_GENERATE_XHM must be enabled!
#endif
#endif


#define SRP_SALT_LENGTH         16
#define SRP_PUBLIC_KEY_LENGTH   384
#define SRP_PROOF_LENGTH        64
#define SRP_SESSION_KEY_LENGTH  64



// ToDo: Remove?
static const char HTTP_200[] PROGMEM 					= "HTTP/1.1 200 OK\r\n";
static const char HTTP_204[] PROGMEM 					= "HTTP/1.1 204 No Content\r\n\r\n";
static const char HTTP_207[] PROGMEM 					= "HTTP/1.1 207 Multi-Status\r\n";
static const char HTTP_400[] PROGMEM 					= "HTTP/1.1 400 Bad Request\r\n";

static const char HTTP_CONTENT_TYPE_HAPJSON[] PROGMEM 	= "Content-Type: application/hap+json\r\n";
static const char HTTP_CONTENT_TYPE_TLV8[] PROGMEM 		= "Content-Type: application/pairing+tlv8\r\n";

static const char HTTP_KEEP_ALIVE[] PROGMEM		 		= "Connection: keep-alive\r\n";
static const char HTTP_TRANSFER_ENCODING[] PROGMEM		= "Transfer-Encoding: chunked\r\n";

static const char HTTP_CRLF[] PROGMEM 					= "\r\n";

static const char EVENT_200[] PROGMEM 					= "EVENT/1.0 200 OK\r\n";


#if defined( CORE_TEENSY )
struct HAP_MDNS_TXT {
	char md[32 + 1];
	char id[20 + 1];
	char cN[20 + 1];
	char ff[2  + 1];
	char pv[4  + 1];
	char sN[2  + 1];
	char sf[2  + 1];
	char ci[2  + 1];
	char sh[12 + 1];

	void setModellDescription(const char* md_){
		strncpy(md, md_, 32);
		md[32] ='\0';
	}

	void setConfigNumber(const uint32_t c_){
		sprintf(cN, "%" PRIu32, c_);
		cN[20] ='\0';
	}

	void setId(const char* id_){
		strncpy(id, id_, 20);
		id[20] ='\0';
	}

	void setFf(bool value){
		sprintf(ff, "%d", value);
		ff[2] ='\0';
	}

	void setPv(const char* value){
		strncpy(pv, value, 4);
		pv[4] ='\0';
	}

	void setStateNumber(uint8_t value){
		sprintf(sN, "%d", value);
		sN[2] ='\0';
	}

	void setSf(bool value){
		sprintf(sf, "%d", value);
		sf[2] ='\0';
	}

	void setCi(uint8_t value){
		sprintf(ci, "%d", value);
		sf[2] ='\0';
	}

	void setSh(const char* value){
		strncpy(sh, value, 12);
		sh[12] ='\0';
	}
};
#endif


class HAPServer {
public:
	HAPServer(uint16_t port = 51628, uint8_t maxClients = 8);
	~HAPServer();

	bool begin(bool resume = false);
	void handle();

	static const char* versionString(){
		return HAPVersion(HOMEKIT_VERSION).toString().c_str();
	}


	static HAPVersion version() {
		return HAPVersion(HOMEKIT_VERSION);
	}


	void __setFirmware(const char* name, const char* version, const char* rev);
	void __setBrand(const char* brand);

	HAPAccessorySet* getAccessorySet();

	bool isPaired();

	static EventManager _eventManager;

	// HAPEventManager _evtMgr;
protected:



	HAPAccessorySet* _accessorySet;
	std::vector<HAPClient*> _clients;

	HAPTime _time;

#if defined(ARDUINO_ARCH_ESP32)
	WiFiServer _server;
	HAPWiFiHelper _wifi;

#if HAP_USE_PREFERENCES
	HAPConfigurationPreferences _configuration;
#else
	HAPConfigurationEEPROM _configuration;
#endif


#if HAP_ENABLE_KEYSTORE
	HAPKeystore	  _keystore;
#endif

#if HAP_ENABLE_WEBSERVER
	HAPWebServer* _webserver;
#endif



	// Preferences _preferences;
#elif defined(CORE_TEENSY)
	EthernetServer _server;

#if HAP_USE_SPIFFS_CONFIGURATION
	HAPConfigurationT41SPIFFSExt _configuration;
#elif HAP_USE_EEPROM
	HAPConfigurationEEPROM _configuration;
#endif


	static const fnet_mdns_txt_key_t* HomekitTXTRecord();
	static HAP_MDNS_TXT _hapMdnsTxt;

#endif

	std::vector<std::unique_ptr<HAPPlugin>> _plugins;
	HAPFakegatoFactory _fakeGatoFactory;

	bool _homekitStarted;
	bool _isInPairingMode;

	uint8_t _homekitFailedLoginAttempts;

	// String _curLine;
	uint16_t _port;

#if HAP_DEBUG
	unsigned long _previousMillisHeap;
#endif


	HAPSRP* _hapsrp;

	char _brand[MAX_BRAND_LENGTH];



	void updateConfig();

	//
	// Event handler
	//
	void processEvents();

	void handleEventUpdateConfigNumber( int eventCode, struct HAPEvent eventParam );
	void handleEventUpdatedConfig(int eventCode, struct HAPEvent eventParam);
	void handleEventRebootNow(int eventCode, struct HAPEvent eventParam);

	void handleEventConfigReset(int eventCode, struct HAPEvent eventParam);
	void handleEventDeleteAllPairings(int eventCode, struct HAPEvent eventParam);



	// HAPEventManager	_evtMgr;

	//
	// Event Member callbacks
	//  ToDo: Rename
    MemberFunctionCallable<HAPServer> listenerUpdateConfigNumber;
	MemberFunctionCallable<HAPServer> listenerConfigUpdated;
	MemberFunctionCallable<HAPServer> listenerNotificaton;
	MemberFunctionCallable<HAPServer> listenerRebootNow;

	MemberFunctionCallable<HAPServer> listenerConfigReset;
	MemberFunctionCallable<HAPServer> listenerDeleteAllPairings;

#if HAP_ENABLE_UPDATE_OTA || HAP_ENABLE_UPDATE_WEB
	HAPUpdate _updater;
#endif



	//
	// Homekit HTTP paths
	//

	//
	// Pairing
	//

	// Pair-Setup states
	bool handlePairSetupM1(HAPClient* hapClient);
	bool handlePairSetupM3(HAPClient* hapClient);
	bool handlePairSetupM5(HAPClient* hapClient);


	// Pair-Verify states
	bool handlePairVerifyM1(HAPClient* hapClient);
	bool handlePairVerifyM3(HAPClient* hapClient);


	// /accessories
	void handleAccessories(HAPClient* hapClient);

	// /characteristics
	void handleCharacteristicsGet(HAPClient* hapClient);
	void handleCharacteristicsPut(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen);

	// pairings
	void handlePairingsPost(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen);
	void handlePairingsRemove(HAPClient* hapClient, const uint8_t* identifier);
	void handlePairingsList(HAPClient* hapClient);
	void handlePairingsAdd(HAPClient* hapClient, const uint8_t* identifier, const uint8_t* publicKey, bool isAdmin);

	// Identify
	void handleIdentify(HAPClient* hapClient);



	// Callbacks
	void handleAllPairingsRemoved();

private:

#if HAP_DEBUG && HAP_WEBSERVER_USE_SPIFFS
	static void listDir(FS &fs, const char * dirname, uint8_t levels);
#endif

	//
	// Bonjour
	//
	bool updateServiceTxt();


	//
	// HTTP
	//

	// parsing
	void processIncomingRequest(HAPClient* hapClient, ReadBufferingClient* bufferedClient);
	void processIncomingEncryptedRequest(HAPClient* hapClient, ReadBufferingClient* bufferedClient);

	void processIncomingLine(HAPClient* hapClient, const char* line, size_t lineLength);
	void processPathParameters(HAPClient* hapClient, const char* line, size_t lineLength, int curPos);

	void parseRequest(HAPClient* hapClient, const char* msg, size_t msg_len, uint8_t** out, int* outLen);
	bool handlePath(HAPClient* hapClient, uint8_t* bodyData, size_t bodyDataLen);

	// Connection states
	void handleClientState(HAPClient* hapClient);
	void handleClientAvailable(HAPClient* hapClient);
	void handleClientDisconnect(HAPClient* hapClient);


	//
	// Sending responses
	//
	void sendResponse(HAPClient* hapClient, TLV8* response);

	bool send(HAPClient* hapClient, const String httpStatus, const JsonDocument& doc, const enum HAP_ENCRYPTION_MODE mode, const char* contentType = "application/hap+json");
	bool send(HAPClient* hapClient, const String httpStatus, const uint8_t* data, const size_t length, const enum HAP_ENCRYPTION_MODE mode, const char* contentType = "application/hap+json");
	bool send(HAPClient* hapClient, const String httpStatus, const char* data, const size_t length, const enum HAP_ENCRYPTION_MODE mode, const char* contentType = "application/hap+json"){
		return send(hapClient, httpStatus, (const uint8_t*)data, length, mode, contentType);
	}

	void sendErrorTLV(HAPClient* hapClient, uint8_t state, uint8_t error);
	bool sendEvent(HAPClient* hapClient, const JsonDocument& response);
	bool send204(HAPClient* hapClient);

#if HAP_ENABLE_PIXEL_INDICATOR
	// ToDo: Pixel Indicator
	HAPIndicatorPixel _pixelIndicator;
#endif

#if HAP_ENABLE_WIFI_BUTTON
	//
	// Button
	//
	static void taskButtonRead(void* pvParameters);
	TaskHandle_t _taskButtonHandle;
	void callbackClick();
	void callbackDoubleClick();
	void callbackHold();
	void callbackLongHold();
#endif

	//
	// TLV8 Encoding
	//
	static bool encode(HAPClient* hapClient, ReadBufferingClient* bufferedClient);

	static bool _isConnected;

	const char* __HOMEKIT_SIGNATURE;
};

extern HAPServer hap;

#endif /* HAPSERVER_HPP_ */
