//
// HAPConfiguration.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATION_HPP_
#define HAPCONFIGURATION_HPP_

#include <Arduino.h>
#include <functional>
#include <vector>
#include <ArduinoJson.h>

#include "HAPGlobals.hpp"


#include "HAPConfigurationAccessory.hpp"
#include "HAPConfigurationPlatform.hpp"
#include "HAPConfigurationPluginData.hpp"
// #include "HAPConfigurationPairing.hpp"

#if HAP_ENABLE_WEBSERVER
#include "HAPConfigurationWebServer.hpp"
#endif

#if HAP_ENABLE_WIFI
#include "HAPConfigurationWiFi.hpp"
#endif

#if HAP_ENABLE_UPDATE_OTA
#include "HAPConfigurationOTA.hpp"
#endif

#if HAP_ENABLE_KEYSTORE
#include "HAPConfigurationKeystore.hpp"
#endif

#define HAP_CONFIGURATION_STRING_LENGTH				32

#define HAP_CONFIGURATION_WEBSERVER_MAX_CREDENTIALS	5

#define HAP_CONFIGURATION_WIFI_MAX_SAVED_NETWORKS	5
#define HAP_CONFIGURATION_WIFI_MAX_SSID_LENGTH		32
#define HAP_CONFIGURATION_WIFI_MAX_PASSWORD_LENGTH	64

struct HAPConfigurationValidationResult {
    bool valid;
    String reason;

	HAPConfigurationValidationResult() : valid(true), reason("") {}
};


class HAPConfiguration {
public:
	HAPConfiguration();
	~HAPConfiguration();

	virtual void setDefaults();
	virtual void clear();

	virtual bool save();
	virtual bool load();

	virtual bool begin();
	virtual void end();
	virtual void reset();

	virtual void validateConfig() {};
	virtual bool validConfig() {return true;};

	virtual bool getDataForPlugin(const char* name, HAPConfigurationPlugin* data, size_t dataSize);

	virtual bool getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize) = 0;
	virtual size_t getDataLengthForPlugin(const char* name) = 0;

	void update();	// calls _callbackUpdate

	virtual void toJson(Print& prt);

	void registerCallbackUpdate(std::function<void(void)> callback){
        _callbackUpdate = callback;
    }

	static HAPConfigurationValidationResult validateConfig(const JsonObject object);

	//
	// Accessory
	//
	virtual HAPConfigurationAccessory* getAccessoryConfig() { return _accessoryConfig; }
	virtual void setAccessoryConfig(HAPConfigurationAccessory* data) { _accessoryConfig = data; }

	//
	// ESP32 / Plattform
	//
	virtual HAPConfigurationPlatform* getPlatformConfig() { return _platformConfig; }
	virtual void setPlatformConfig(HAPConfigurationPlatform* data) { _platformConfig = data; }

	//
	// Pairing
	//
	// virtual HAPConfigurationPairing* getPairingConfig() { return _pairingConfig; }
	// virtual void setPairingConfig(HAPConfigurationPairing* data) { _pairingConfig = data; }

	//
	// Keystore
	//
#if HAP_ENABLE_KEYSTORE
	virtual HAPConfigurationKeystore* getKeystoreConfig() { return _keystoreConfig; }
	virtual void setKeystoreConfig(HAPConfigurationKeystore* data) { _keystoreConfig = data; }
#endif


	//
	// OTA
	//
#if HAP_ENABLE_UPDATE_OTA
	virtual HAPConfigurationArduinoOTA* getOTAConfig() { return _otaConfig; }
	virtual void setOTAConfig(HAPConfigurationArduinoOTA* data) { _otaConfig = data; }
#endif

	//
	// Webserver
	//
#if HAP_ENABLE_WEBSERVER
	virtual HAPConfigurationWebServer* getWebServerConfig() { return _webserverConfig; }
	virtual void setWebServerConfig(HAPConfigurationWebServer* data) { _webserverConfig = data; }
#endif

	//
	// WiFi
	//
#if HAP_ENABLE_WIFI
	virtual HAPConfigurationWiFi* getWiFiConfig() { return _wifiConfig; }
	virtual void setWiFiConfig(HAPConfigurationWiFi* data) { _wifiConfig = data; }
#endif

protected:
	std::function<void(void)> 	_callbackUpdate;
	// std::vector<HAPConfigurationPlugin*> 		_pluginConfigs;


	virtual size_t writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen) = 0;
	virtual size_t readBytes(const char* label, uint8_t* output, const size_t expectedDataLen) = 0;
	virtual size_t getBytesLength(const char* label) = 0;

	//
	// ESP32 / Plattform
	//
	struct HAPConfigurationPlatform* 	_platformConfig;
	bool savePlatformConfig();
	bool loadPlatformConfig();

	//
	// Accessory
	//
	struct HAPConfigurationAccessory* 	_accessoryConfig;
	bool saveAccessoryConfig();
	bool loadAccessoryConfig();

	//
	// Pairings
	//
	// struct HAPConfigurationPairing* 	_pairingConfig;
	// bool savePairingConfig();
	// bool loadPairingConfig();


	//
	// Plugins
	//
	bool savePluginConfig();
	bool getDataForPlugin(const char* name, uint8_t* data, size_t dataSize);

	//
	// WiFi
	//
#if HAP_ENABLE_WIFI
	struct HAPConfigurationWiFi*		_wifiConfig;
	bool saveWiFiConfig();
	bool loadWiFiConfig();
#endif

	//
	// WebServer
	//
#if HAP_ENABLE_WEBSERVER
	struct HAPConfigurationWebServer* 	_webserverConfig;
	bool saveWebServerConfig();
	bool loadWebServerConfig();
#endif


	//
	// Keystore
	//
#if HAP_ENABLE_KEYSTORE
	struct HAPConfigurationKeystore* 	_keystoreConfig;
	bool saveKeystoreConfig();
	bool loadKeystoreConfig();
#endif

	//
	// OTA
	//
#if HAP_ENABLE_UPDATE_OTA
	struct HAPConfigurationArduinoOTA* 	_otaConfig;
	bool saveOTAConfig();
	bool loadOTAConfig();
#endif


#if 0
    static HAPConfigurationValidationResult validateConfigHomekit(const JsonObject object) __attribute__ ((deprecated));
    static HAPConfigurationValidationResult validateConfigAccessory(const JsonObject object) __attribute__ ((deprecated));
    static HAPConfigurationValidationResult validateConfigWifi(const JsonObject object) __attribute__ ((deprecated));
    static HAPConfigurationValidationResult validateConfigWebserver(const JsonObject object) __attribute__ ((deprecated));
    static HAPConfigurationValidationResult validateConfigUpdate(const JsonObject object) __attribute__ ((deprecated));
    static HAPConfigurationValidationResult validateConfigPlugins(const JsonObject object) __attribute__ ((deprecated));
#endif
	// virtual size_t writeBytes(uint16_t address, const uint8_t* input, const size_t expectedDataLen) = 0;
	// virtual size_t readBytes(uint16_t address, uint8_t* output, const size_t expectedDataLen) = 0;
	// virtual size_t getBytesLength(uint16_t address) = 0;


	static bool isOnlyZeros(const uint8_t* bytearray, int length);
};




#endif /* HAPCONFIGURATION_HPP_ */