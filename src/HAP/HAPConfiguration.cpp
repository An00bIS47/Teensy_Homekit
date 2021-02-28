//
// HAPConfiguration.cpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#include "HAPConfiguration.hpp"
#include "HAPLogger.hpp"
#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPConfiguration::HAPConfiguration(){
	_platformConfig = new HAPConfigurationPlatform();
	_platformConfig->setSaveCallback(std::bind(&HAPConfiguration::savePlatformConfig, this));
	
	_accessoryConfig = new HAPConfigurationAccessory();
	_accessoryConfig->setSaveCallback(std::bind(&HAPConfiguration::saveAccessoryConfig, this));

#if HAP_ENABLE_WIFI
	_wifiConfig = new HAPConfigurationWiFi();
	_wifiConfig->setSaveCallback(std::bind(&HAPConfiguration::saveWiFiConfig, this));
#endif
	
#if HAP_ENABLE_WEBSERVER	
	_webserverConfig = new HAPConfigurationWebServer();
	_webserverConfig->setSaveCallback(std::bind(&HAPConfiguration::saveWebServerConfig, this));
#endif	
	
#if HAP_ENABLE_KEYSTORE	
	_keystoreConfig = new HAPConfigurationKeystore();
	_keystoreConfig->setSaveCallback(std::bind(&HAPConfiguration::saveKeystoreConfig, this));
#endif

#if HAP_ENABLE_UPDATE_OTA
	_otaConfig = new HAPConfigurationArduinoOTA();
	_otaConfig->setSaveCallback(std::bind(&HAPConfiguration::saveOTAConfig, this));
#endif

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPConfiguration::~HAPConfiguration(){
	clear();

	if (_platformConfig) delete _platformConfig;
	if (_accessoryConfig) delete _accessoryConfig;
    // if (_pairingConfig) delete _pairingConfig;

#if HAP_ENABLE_KEYSTORE		
	if (_keystoreConfig) delete _keystoreConfig;
#endif

#if HAP_ENABLE_UPDATE_OTA		
	if (_otaConfig) delete _otaConfig;
#endif

#if HAP_ENABLE_WEBSERVER		
	if (_webserverConfig) delete _webserverConfig;
#endif

#if HAP_ENABLE_WIFI			
	if (_wifiConfig) delete _wifiConfig;
#endif

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::begin(){
    return validConfig();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPConfiguration::end(){

}

void HAPConfiguration::reset(){
    setDefaults();
    save();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPConfiguration::clear(){	
    _accessoryConfig->clear();	
#if HAP_ENABLE_WIFI		
	_wifiConfig->clear();
#endif

#if HAP_ENABLE_WEBSERVER		
	_webserverConfig->clear();
#endif
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPConfiguration::setDefaults(){

	_accessoryConfig->setDefaults();
	_platformConfig->setDefaults();

#if HAP_ENABLE_WIFI
	_wifiConfig->setDefaults();
#endif

#if HAP_ENABLE_UPDATE_OTA
	_otaConfig->setDefaults();
#endif

#if HAP_ENABLE_KEYSTORE
	_keystoreConfig->setDefaults();
#endif

#if HAP_ENABLE_WEBSERVER
	_webserverConfig->setDefaults();
#endif

}


	// {
	// 		"platfrom": { ... } 
    //
	//		,
	// 		"accessory": { ... } 
    //		
	//		,
	// 		"ota": { ... } 
	//
	//		,
	// 		"wifi": { ... } 
	//
	//		,
	// 		"webserver": { ... } 
	//
	//		,
	// 		"keystore": { ... } 
    // }
#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif    	
void HAPConfiguration::toJson(Print& prt){
	prt.print("{");

	prt.print("\"platform\": ");
	_platformConfig->toJson(prt);
	
	prt.print(",");
	prt.print("\"accessory\": ");
	_accessoryConfig->toJson(prt);	

#if HAP_ENABLE_WIFI
	prt.print(",");
	prt.print("\"wifi\": ");
	_wifiConfig->toJson(prt);
#endif

#if HAP_ENABLE_WEBSERVER
	prt.print(",");
	prt.print("\"webserver\": ");
	_webserverConfig->toJson(prt);
#endif

#if HAP_ENABLE_UPDATE_OTA
	prt.print(",");
	prt.print("\"ota\": ");
	_otaConfig->toJson(prt);	
#endif


#if HAP_ENABLE_KEYSTORE
	prt.print(",");
	prt.print("\"keystore\": ");
	_keystoreConfig->toJson(prt);	
#endif	

	prt.print(",");
	prt.print("\"plugins\": [");

	auto &factory = HAPPluginFactory::Instance();        
    std::vector<String> names = factory.names();    

	uint8_t counter = 0;
    for (std::vector<String>::iterator it = names.begin(); it != names.end(); ++it) {
    	
    	auto plugin = factory.getPlugin(*it);
		plugin->configToJson(prt);

		if (counter + 1 < names.size()){
			prt.print(",");
		}
		counter++;
    }
	prt.print("]");


	prt.print("}");
}

void HAPConfiguration::update(){    
    _callbackUpdate();
}


#if 0   
HAPConfigurationValidationResult HAPConfiguration::validateConfig(const JsonObject object){
    
    HAPConfigurationValidationResult result;
    result.valid = false;

    // homekit
    result = validateConfigHomekit(object);
    if (!result.valid) return result;
    
    // accessory
    result = validateConfigAccessory(object);
    if (!result.valid) return result;

    // wifi
    result = validateConfigWifi(object);
    if (!result.valid) return result;

    // webserver
    result = validateConfigWebserver(object);
    if (!result.valid) return result;

    // update
    result = validateConfigUpdate(object);
    if (!result.valid) return result;

    // plugins
    result = validateConfigPlugins(object);
    if (!result.valid) return result;


    result.valid = true;
    return result;
}

HAPConfigurationValidationResult HAPConfiguration::validateConfigHomekit(const JsonObject object){
    HAPConfigurationValidationResult result;
    result.valid = false;

    // homekit.loglevel - int
    if (!object["homekit"].as<JsonObject>().containsKey("loglevel") || !object["homekit"]["loglevel"].is<uint8_t>()) {
        result.reason = "homekit.loglevel is not an integer";        
        return result;
    }

    // homekit.keystore - string
    if (!object["homekit"].as<JsonObject>().containsKey("keystore") || !object["homekit"]["keystore"].is<const char*>()) {
        result.reason = "homekit.keystore is not a string";        
        return result;
    }

    // accessory.keystore - length
    if (strlen(object["accessory"]["hostname"]) > 15) {
        result.reason = "homekit.keystore is too long";
        return result;
    }

    result.valid = true;
    return result;
}


HAPConfigurationValidationResult HAPConfiguration::validateConfigAccessory(const JsonObject object){
    HAPConfigurationValidationResult result;
    result.valid = false;
    
    /*
        "accessory": {
            "pin-code": "031-45-712",
            "hostname": "esp32-"
        }
     */

    // accessory
    if (object.containsKey("accessory") && !object["accessory"].is<JsonObject>()) {
        result.reason = "accessory is not an object";
        return result;
    }

    // accessory.hostname - string
    if (!object["accessory"].as<JsonObject>().containsKey("hostname") || !object["accessory"]["hostname"].is<const char*>()) {
        result.reason = "accessory.hostname is not a string";
        return result;
    }
    // accessory.hostname - length
    if (strlen(object["accessory"]["hostname"]) + 1 > HAP_STRING_LENGTH_MAX) {
        result.reason = "accessory.hostname is too long";
        return result;
    }

    // accessory.pincode - string
    if (!object["accessory"].as<JsonObject>().containsKey("pincode") || !object["accessory"]["pincode"].is<const char*>()) {
        result.reason = "accessory.pincode is not a string";
        return result;
    }

    // accessory.pincode - length max 10
    if (strlen(object["accessory"]["pincode"]) != 10) {
        result.reason = "accessory.pincode is not 10 characters long";
        return result;
    }

    result.valid = true;
    return result;
}

HAPConfigurationValidationResult HAPConfiguration::validateConfigWifi(const JsonObject object){
    HAPConfigurationValidationResult result;
    result.valid = false;
    
    /*
        "wifi": {
            "mode": 0,
            "networks" [
                {
                    "ssid": "XXX",
                    "password": "XXX"
                }
            ]
        }
     */

    // wifi
    if (object.containsKey("wifi") && !object["wifi"].is<JsonObject>()) {
        result.reason = "wifi is not an object";
        return result;
    }

    // wifi.mode - uint8_t
    if (!object["wifi"].as<JsonObject>().containsKey("mode") || !object["wifi"]["mode"].is<uint8_t>()) {
        if (object["wifi"]["mode"] >= 0 && object["wifi"]["mode"] <= 3)
        result.reason = "wifi.mode is not a valid value";
        return result;
    }

    // if ( object["wifi"]["mode"].as<uint8_t>() == HAPWiFiModeMulti) {
        // wifi.networks - array
        if (object["wifi"].as<JsonObject>().containsKey("networks") && !object["wifi"]["networks"].is<JsonArray>()) {
            result.reason = "wifi.networks is not an array";
            return result;
        }


        uint8_t count = 0;
        for( const auto& value : object["wifi"]["networks"].as<JsonArray>() ) {
                
            
            // wifi.networks.count.username
            if (!value.containsKey("ssid") || !value["ssid"].is<const char*>()) {
                result.reason = "wifi.networks." + String(count) + ".ssid is not a string";
                return result;
            }   

            // wifi.networks.count.username - length -> MAX 31    
            if (strlen(value["ssid"]) > 31) {
                result.reason = "wifi.networks." + String(count) + ".ssid is too long";
                return result;
            } 


            // wifi.networks.count.password
            if (!value.containsKey("password") || !value["password"].is<const char*>()) {
                result.reason = "wifi.networks." + String(count) + ".password is not a string";
                return result;
            }   

            // wifi.networks.count.password - length -> MAX 63      
            if (strlen(value["password"]) > 63) {
                result.reason = "wifi.networks." + String(count) + ".password is too long";
                return result;
            } 
                
            count++;
        }
    // }

    result.valid = true;
    return result;
}


HAPConfigurationValidationResult HAPConfiguration::validateConfigWebserver(const JsonObject object){
    HAPConfigurationValidationResult result;
    result.valid = false;
    
    /*
        "webserver": {
            "enabled": true,
            "admins": [
                {
                    "username": "admin",
                    "password": "secret"
                }
            ],
            "apis": [
                {
                    "username": "api",
                    "password": "secret"
                }
            ]
        }
     */

    // webserver
    if (object.containsKey("webserver") && !object["webserver"].is<JsonObject>()) {
        result.reason = "webserver is not an object";
        return result;
    }

    // webserver.enabled - bool
    if (!object["webserver"].as<JsonObject>().containsKey("enabled") || !object["webserver"]["enabled"].is<bool>()) {
        result.reason = "webserver.enabled is not a bool";
        return result;
}

    // webserver.admins
    if (!object["webserver"].as<JsonObject>().containsKey("admins") || !object["webserver"]["admins"].is<JsonArray>()) {
        result.reason = "webserver.admins is not an array";
        return result;
    }

    // webserver.admins array
    uint8_t count = 0;
    for( const auto& value : object["webserver"]["admins"].as<JsonArray>() ) {
            
        
        // webserver.admins.count.username
        if (!value.containsKey("username") || !value["username"].is<const char*>()) {
            result.reason = "webserver.admins." + String(count) + ".username is not a string";
            return result;
        }   

        // webserver.admins.count.username - length        
        if (strlen(value["username"]) + 1 > HAP_STRING_LENGTH_MAX) {
            result.reason = "webserver.admins." + String(count) + ".username is too long";
            return result;
        } 


        // webserver.admins.count.password
        if (!value.containsKey("password") || !value["password"].is<const char*>()) {
            result.reason = "webserver.admins." + String(count) + ".password is not a string";
            return result;
        }   

        // webserver.admins.count.password - length        
        if (strlen(value["password"]) + 1 > HAP_STRING_LENGTH_MAX) {
            result.reason = "webserver.admins." + String(count) + ".password is too long";
            return result;
        } 
              
        count++;
    }

    // webserver.apis
    if (!object["webserver"].as<JsonObject>().containsKey("apis") || !object["webserver"]["apis"].is<JsonArray>()) {
        result.reason = "webserver.apis is not an array";
        return result;
    }

    // webserver.apis array
    count = 0;
    for( const auto& value : object["webserver"]["apis"].as<JsonArray>() ) {
            
        
        // webserver.apis.count.username
        if (!value.containsKey("username") || !value["username"].is<const char*>()) {
            result.reason = "webserver.apis." + String(count) + ".username is not a string";
            return result;
        }   

        // webserver.apis.count.username - length        
        if (strlen(value["username"]) + 1 > HAP_STRING_LENGTH_MAX) {
            result.reason = "webserver.apis." + String(count) + ".username is too long";
            return result;
        } 


        // webserver.apis.count.password
        if (!value.containsKey("password") || !value["password"].is<const char*>()) {
            result.reason = "webserver.apis." + String(count) + ".password is not a string";
            return result;
        }   

        // webserver.apis.count.password - length        
        if (strlen(value["password"]) + 1 > HAP_STRING_LENGTH_MAX) {
            result.reason = "webserver.apis." + String(count) + ".password is too long";
            return result;
        }
              
        count++;
    }

    result.valid = true;
    return result;
}


HAPConfigurationValidationResult HAPConfiguration::validateConfigUpdate(const JsonObject object){
    HAPConfigurationValidationResult result;
    result.valid = false;
    
    /*
        "update": {
            "web": {
                "enabled": true    	
            },
            "ota": {
                "enabled": true,
                "port": 3232,
                "password": "admin"
            }
        }
     */

    // update
    if (object.containsKey("update") && !object["update"].is<JsonObject>()) {
        result.reason = "update is not an object";
        return result;
    }

    // update.web - JsonObject
    if (!object["update"].as<JsonObject>().containsKey("web") || !object["update"]["web"].is<JsonObject>()) {
        result.reason = "update.web is not a object";
        return result;
    }

    // update.web.enabled - bool
    if (!object["update"]["web"].as<JsonObject>().containsKey("enabled") || !object["update"]["web"]["enabled"].is<bool>()) {
        result.reason = "update.web.enabled is not a bool";
        return result;
    }


    // update.ota - JsonObject
    if (!object["update"].as<JsonObject>().containsKey("ota") || !object["update"]["ota"].is<JsonObject>()) {
        result.reason = "update.ota is not a object";
        return result;
    }

    // update.ota.enabled - bool
    if (!object["update"]["ota"].as<JsonObject>().containsKey("enabled") || !object["update"]["ota"]["enabled"].is<bool>()) {
        result.reason = "update.ota.enabled is not a bool";
        return result;
    }

    // update.ota.password
    if (object["update"]["ota"].as<JsonObject>().containsKey("password") && !object["update"]["ota"]["password"].is<const char*>()) {
        result.reason = "update.ota.password is not a string";
        return result;
    }   

    // update.ota.password - length        
    if ((object["update"]["ota"].as<JsonObject>().containsKey("password")) && (strlen(object["update"]["ota"]["password"].as<const char*>()) + 1 > HAP_STRING_LENGTH_MAX)) {
        result.reason = "update.ota.password is too long";
        return result;
    }

    // update.ota.port
    if (object["update"]["ota"].as<JsonObject>().containsKey("port") && !object["update"]["ota"]["port"].is<uint16_t>()) {
        result.reason = "update.ota.port is not a integer";
        return result;
    } 

    result.valid = true;
    return result;
}


HAPConfigurationValidationResult HAPConfiguration::validateConfigPlugins(const JsonObject object){
    HAPConfigurationValidationResult result;
    result.valid = false;

    auto &factory = HAPPluginFactory::Instance();        
    std::vector<String> names = factory.names();    

    if (!object.containsKey("plugins")) {
        result.reason = "plugins is required";
        return result;
    }

    if (object.containsKey("plugins") && !object["plugins"].is<JsonObject>()) {
        result.reason = "plugins is not an object";
        return result;
    }

    for (std::vector<String>::iterator it = names.begin(); it != names.end(); ++it) {
    	//Serial.println(*it);
    	auto plugin = factory.getPlugin(*it);

                
        if (object["plugins"].containsKey(plugin->name()) && !object["plugins"][plugin->name()].is<JsonObject>()) {
            result.reason = "plugins." + plugin->name() + " is not an object";
            return result;
        }

        result = plugin->validateConfig(object["plugins"][plugin->name()]);
        if (!result.valid) return result;
        

    }

    result.valid = true;
    return result;
}
#endif


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::getDataForPlugin(const char* name, HAPConfigurationPlugin* data, size_t dataSize) { 
    uint8_t buffer[dataSize];
    bool result = getBytesForPlugin(name, buffer, dataSize);

    if (result == true) {
        memcpy(data->name, buffer, 20);
        data->name[20] = '\0';
        data->enabled = buffer[21];
        data->interval = HAPHelper::u8_to_u32(buffer+22);
        data->dataSize = HAPHelper::u8_to_u32(buffer+26);
       
        data->dataPtr = new uint8_t[data->dataSize];			
        memcpy(data->dataPtr, buffer+30, data->dataSize);

        return true;
    }
    return false;
};

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::load() {

    LogD(F("   >>> Validation ..."), false);
	bool result = validConfig();
	if (result == false) {		
		LogE(F("ERROR: Invalid configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);

    LogD(F("   >>> Platform ..."), false);
	result = loadPlatformConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to load platform configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);

    LogD(F("   >>> Accessory ..."), false);
	result = loadAccessoryConfig();
	if (result == false) {		
		LogE(F("ERROR: Failed to load accessory configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);


#if HAP_ENABLE_WIFI
    LogD(F("   >>> WiFi ..."), false);
	result = loadWiFiConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to load wifi configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif


#if HAP_ENABLE_KEYSTORE
    LogD(F("   >>> Keystore ..."), false);
	result = loadKeystoreConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to load keystore configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif
    

#if HAP_ENABLE_WEBSERVER
    LogD(F("   >>> WebServer ..."), false);
	result = loadWebServerConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to load webserver configuration!"), true);
		return false;	
	}
    LogD(F(" OK"), true);
#endif	


#if HAP_ENABLE_UPDATE_OTA
    LogD(F("   >>> OTA ..."), false);
	result = loadOTAConfig();	
	if (result == false) {
		LogE(F("ERROR: Failed to load ota configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif

	return result;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::save() {

	bool result = true;

    LogD(F("   <<< Platform ..."), false);
	result = savePlatformConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save platform configuration!"), true);	
		return false;
	}   
    LogD(F(" OK"), true);


    LogD(F("   <<< Accessory ..."), false);
	result = saveAccessoryConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save accessory configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);

#if HAP_ENABLE_WIFI
    LogD(F("   <<< WiFi ..."), false);
	result = saveWiFiConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save wifi configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif

#if HAP_ENABLE_KEYSTORE
    LogD(F("   <<< Keystore ..."), false);
	result = saveKeystoreConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save keystore configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif

#if HAP_ENABLE_WEBSERVER
    LogD(F("   <<< WebServer ..."), false);
	result = saveWebServerConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save webserver configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif

#if HAP_ENABLE_UPDATE_OTA
    LogD(F("   <<< OTA ..."), false);
	result = saveOTAConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save ota configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);
#endif

    LogD(F("   <<< Plugins ..."), false);
    result = savePluginConfig();
	if (result == false) {
		LogE(F("ERROR: Failed to save plugin configuration!"), true);
		return false;
	}
    LogD(F(" OK"), true);



    LogD(F("   <<< Validation ..."), false);
    validateConfig();
    LogD(F(" OK"), true);
	
	return result;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::savePluginConfig(){
	auto &factory = HAPPluginFactory::Instance();        
    std::vector<String> names = factory.names();    

	
    for (std::vector<String>::iterator it = names.begin(); it != names.end(); ++it) {
    	
    	auto plugin = factory.getPlugin(*it);
		char label[20];	
		sprintf(label, "p%s", plugin->name().c_str());
		HAPConfigurationPlugin* pluginConfig = plugin->getConfiguration();

		// Serial.println("Saving data to EEPROM: ");
		// Serial.print("name: ");
		// Serial.println(plugin->name());
		// Serial.print("label: ");
		// Serial.println(label);
		// Serial.print("commonSize: ");
		// Serial.println(pluginConfig->getCommonSize());
		// Serial.print("dataSize: ");
		// Serial.println(pluginConfig->getDataSize());

		uint8_t buffer[pluginConfig->getCommonSize() + pluginConfig->getDataSize()];

		memcpy(buffer, pluginConfig->name, 20);
		buffer[20] = '\0';
		buffer[21] = pluginConfig->enabled;
		HAPHelper::u32_to_u8(pluginConfig->interval, buffer+22);
		HAPHelper::u32_to_u8(pluginConfig->dataSize, buffer+26);
		memcpy(buffer+30, pluginConfig->dataPtr, pluginConfig->getDataSize());

#if HAP_DEBUG_CONFIGURATION    	
        HAPHelper::array_print("buffer", (uint8_t*)buffer, sizeof(buffer));
#endif
        size_t written = writeBytes(label, buffer, sizeof(buffer));
		if (written < sizeof(buffer)) {
			return false;
		}
    }

	return true;
}


#if HAP_ENABLE_UPDATE_OTA
bool HAPConfiguration::saveOTAConfig(){

    size_t bufferSize = HAPConfigurationArduinoOTA::getDataSize();
	uint8_t buffer[bufferSize] = {0,};	

	size_t offset = 0;
	buffer[offset++] = _otaConfig->enabled;
	buffer[offset++] =  _otaConfig->port 	& 0xFF;
	buffer[offset++] = (_otaConfig->port 	>> 8);	

    memcpy(buffer + offset, _otaConfig->password, HAP_OTA_PASSWORD_LENGTH);
    offset += HAP_OTA_PASSWORD_LENGTH;

    if (offset < bufferSize){
        LogE("ERROR: OTA offset < buffersize!", true);
    }


#if HAP_DEBUG_CONFIGURATION
    HAPHelper::array_print("SAVE OTA", (uint8_t*)buffer, bufferSize);
#endif

	size_t written = writeBytes("cOTA", (uint8_t*)buffer, bufferSize);
	if (written == bufferSize) {
		return true;
	}
	
	return false;
}

bool HAPConfiguration::loadOTAConfig(){

    size_t bufferSize = HAPConfigurationArduinoOTA::getDataSize();
	uint8_t buffer[bufferSize];	
	size_t offset = 0;

	readBytes("cOTA", buffer, bufferSize);

#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("LOAD OTA", buffer, bufferSize);
#endif

	_otaConfig->enabled = buffer[0];
	offset++;
	_otaConfig->port = ((uint16_t)buffer[2] << 8) | buffer[1];	
	offset++;
    offset++;

    memcpy(_otaConfig->password, buffer + offset, HAP_OTA_PASSWORD_LENGTH);
    _otaConfig->password[HAP_OTA_PASSWORD_LENGTH] = '\0';

    return true;
}
#endif

#if HAP_ENABLE_KEYSTORE
bool HAPConfiguration::saveKeystoreConfig(){

    size_t bufferSize = HAPConfigurationKeystore::getDataSize();
	uint8_t buffer[bufferSize] = {0,};	

	size_t offset = 0;

    memcpy(buffer + offset, _keystoreConfig->keystore, HAP_KEYSTORE_LABEL_LENGTH);
    offset += HAP_KEYSTORE_LABEL_LENGTH;

#if HAP_DEBUG_CONFIGURATION
    HAPHelper::array_print("SAVE KEYSTORE", (uint8_t*)buffer, offset);
#endif

	size_t written = writeBytes("cKeySt", (uint8_t*)buffer, bufferSize);
	if (written == bufferSize) {
		return true;
	}
	
	return false;
}

bool HAPConfiguration::loadKeystoreConfig(){

    size_t bufferSize = HAPConfigurationKeystore::getDataSize();
	uint8_t buffer[bufferSize];	
	size_t offset = 0;

	readBytes("cKeySt", buffer, bufferSize);
    
#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("LOAD KEYSTORE", buffer, bufferSize);
#endif

    memcpy(_keystoreConfig->keystore, buffer + offset, HAP_KEYSTORE_LABEL_LENGTH);
    offset += HAP_KEYSTORE_LABEL_LENGTH;
    _keystoreConfig->keystore[HAP_KEYSTORE_LABEL_LENGTH] = '\0';

    return true;
}

#endif



#if HAP_ENABLE_WEBSERVER

bool HAPConfiguration::saveWebServerConfig(){
	
	size_t bufferSize = HAPConfigurationWebServer::getDataSize();
	uint8_t buffer[bufferSize] = {0,};
	size_t offset = 0;
	
	buffer[offset++] = _webserverConfig->enabled;
	buffer[offset++] =  _webserverConfig->port 	& 0xFF;
	buffer[offset++] = (_webserverConfig->port 	>> 8);	

	for (int i=0; i < _webserverConfig->credentials.size(); i++){
		buffer[offset++] = _webserverConfig->credentials[i]->permission;

		memcpy(buffer + offset, &_webserverConfig->credentials[i]->username, 32);		
		offset += 32;
		// buffer[offset++] = '\0';

		memcpy(buffer + offset, &_webserverConfig->credentials[i]->password, 32);		
		offset += 32;
		// buffer[offset++] = '\0';
	}

#if HAP_DEBUG_CONFIGURATION
    HAPHelper::array_print("SAVE WEBSERVER", (uint8_t*)buffer, offset);
#endif

	size_t written = writeBytes("cWebSvr", buffer, bufferSize);
	if (written == bufferSize) {
		return true;
	}


	return false;
}

bool HAPConfiguration::loadWebServerConfig(){

	size_t bufferSize = HAPConfigurationWebServer::getDataSize();
	uint8_t buffer[bufferSize];	
	size_t offset = 0;

	readBytes("cWebSvr", buffer, bufferSize);

#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("LOAD WEBSERVER", buffer, bufferSize);
#endif


	_webserverConfig->enabled = buffer[0];
	offset++;
	_webserverConfig->port = ((uint16_t)buffer[2] << 8) | buffer[1];	
	offset += 2;

	for (uint8_t i = 0; i < (bufferSize - offset) / HAPConfigurationWebServerCredentials::getDataSize(); i++){

        if (!isOnlyZeros(buffer + offset, HAPConfigurationWebServerCredentials::getDataSize())) {

            uint8_t permission = 0;
            char username[32] = {0,};
            char password[32] = {0,};

            permission = buffer[offset++];

            memcpy((uint8_t*)username, (uint8_t*)buffer + offset, 32);	
            offset += 32;
            memcpy((uint8_t*)password, (uint8_t*)buffer + offset, 32);	
            offset += 32;
                     
            _webserverConfig->credentials.push_back(new HAPConfigurationWebServerCredentials(permission, username, password));

        }
	}
	return true;
}

#endif


#if HAP_ENABLE_WIFI

bool HAPConfiguration::saveWiFiConfig(){
	size_t bufferSize = HAPConfigurationWiFi::getDataSize();
	uint8_t buffer[bufferSize] = {0,};	

	size_t offset = 0;
	buffer[offset++] = _wifiConfig->enabled;
	buffer[offset++] = _wifiConfig->mode;	
	
	for (int i=0; i < _wifiConfig->credentials.size(); i++){
		memcpy(buffer + offset, &_wifiConfig->credentials[i]->ssid, HAP_WIFI_SSID_LENGTH);		
		offset += HAP_WIFI_SSID_LENGTH;
		//buffer[offset++] = '\0';

		memcpy(buffer + offset, &_wifiConfig->credentials[i]->password, HAP_WIFI_PASSWORD_LENGTH);		
		offset += HAP_WIFI_PASSWORD_LENGTH;
		//buffer[offset++] = '\0';
	}

#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("SAVE WIFI", buffer, bufferSize);
#endif	
	

	size_t written = writeBytes("cWiFi", buffer, bufferSize);
	if (written == bufferSize) {
		return true;
	}
	
	return false;
}

bool HAPConfiguration::loadWiFiConfig(){

	size_t bufferSize = HAPConfigurationWiFi::getDataSize();
	uint8_t buffer[bufferSize];	
	size_t offset = 0;

	size_t read = readBytes("cWiFi", buffer, bufferSize);
    if (read < bufferSize) {
		return false;
    }
    
#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("LOAD WIFI", buffer, bufferSize);
#endif

	_wifiConfig->enabled = buffer[offset++];	
	_wifiConfig->mode 	 = buffer[offset++];	
	
	for (uint8_t i = 0; i < (bufferSize - offset) / HAPConfigurationWiFiCredentials::getDataSize(); i++){

        if (!isOnlyZeros(buffer + offset, HAPConfigurationWiFiCredentials::getDataSize())) {
            
            char ssid[32] = {0,};
            char password[64] = {0,};

            memcpy((uint8_t*)ssid, (uint8_t*)buffer + offset, 32);	
            offset += 32;
            memcpy((uint8_t*)password, (uint8_t*)buffer + offset, 64);	
            offset += 64;
                     
            _wifiConfig->credentials.push_back(new HAPConfigurationWiFiCredentials(ssid, password));
        }		
	}
	return true;
}

#endif

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::savePlatformConfig(){

    size_t bufferSize = HAPConfigurationPlatform::getDataSize();
	uint8_t buffer[bufferSize];	
	size_t offset = 0;

    buffer[offset++] = (uint8_t)_platformConfig->_logLevel;    
    HAPHelper::u32_to_u8(_platformConfig->_refTime, buffer + offset);
    offset += 4;


#if HAP_DEBUG_CONFIGURATION
    HAPHelper::array_print("SAVE PLATFORM", (uint8_t*)buffer, offset);
#endif

    size_t written = writeBytes("cPltfrm", (uint8_t*)buffer, bufferSize);

    if (written == bufferSize) {
        return true;
    }

    return false;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::loadPlatformConfig(){
	
    size_t bufferSize = HAPConfigurationPlatform::getDataSize();
	uint8_t buffer[bufferSize];	
	size_t offset = 0;

	readBytes("cPltfrm", buffer, bufferSize);

#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("LOAD PLATFORM", buffer, bufferSize);
#endif

	_platformConfig->_logLevel = (enum LogLevel)buffer[0];
	offset++;
	_platformConfig->_refTime = HAPHelper::u8_to_u32(buffer + offset);

    return true;
}



#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::saveAccessoryConfig(){


    size_t bufferSize = HAPConfigurationAccessory::getDataSize();
    
	uint8_t buffer[bufferSize] = {0,};	

	size_t offset = 0;
    // port
    buffer[offset++] =  _accessoryConfig->port 	& 0xFF;
	buffer[offset++] = (_accessoryConfig->port 	>> 8);	
    // HAPHelper::u16_to_u8(_accessoryConfig->port, buffer + offset);
    // offset += 2;

    // modelname
    memcpy(buffer + offset, _accessoryConfig->modelname, 32);
    offset += 32;
    // pincode
    memcpy(buffer + offset, _accessoryConfig->pincode, 10);
    offset += 10;
    // setupId
    memcpy(buffer + offset, _accessoryConfig->setupId, 4);
    offset += 4;
    // configNumber    
    HAPHelper::u32_to_u8(_accessoryConfig->initConfigNumber, buffer + offset);
    offset += 4;

    // LTPK + LTSK
    memcpy(buffer + offset, _accessoryConfig->LTKP.LTPK, HAP_PAIRINGS_LTPK_LENGTH);
    offset += HAP_PAIRINGS_LTPK_LENGTH;
    memcpy(buffer + offset, _accessoryConfig->LTKP.LTSK, HAP_PAIRINGS_LTSK_LENGTH);
	offset += HAP_PAIRINGS_LTSK_LENGTH;

    // Pairings
	for (int i=0; i < _accessoryConfig->pairings.size(); i++){
		memcpy(buffer + offset, &_accessoryConfig->pairings[i]->id, HAP_PAIRINGS_ID_LENGTH);		
		offset += HAP_PAIRINGS_ID_LENGTH;
		
		memcpy(buffer + offset, &_accessoryConfig->pairings[i]->key, CURVE25519_SECRET_LENGTH);		
		offset += CURVE25519_SECRET_LENGTH;
		buffer[offset++] = _accessoryConfig->pairings[i]->isAdmin;
	}

#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("SAVE ACCESSORY", buffer, bufferSize);
#endif


	size_t written = writeBytes("cAcc", buffer, bufferSize);
	if (written == bufferSize) {
		return true;
	}
	
	return false;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::loadAccessoryConfig(){


    size_t bufferSize = HAPConfigurationAccessory::getDataSize();
    if (bufferSize == 0) return false;


	uint8_t buffer[bufferSize];	
	size_t read = readBytes("cAcc", buffer, bufferSize);
    if (read < bufferSize) {
		return false;
    }

#if HAP_DEBUG_CONFIGURATION
	HAPHelper::array_print("LOAD ACCESSORY", buffer, read);
#endif

	size_t offset = 0;
    // port 
    // _accessoryConfig->port = HAPHelper::u8_to_u16(buffer + offset);
    // offset += 2;
    _accessoryConfig->port = ((uint16_t)buffer[1] << 8) | buffer[0];	
	offset += 2;

    // modelname
    memset(_accessoryConfig->modelname, 0, 32 + 1);
    memcpy(_accessoryConfig->modelname, buffer + offset, 32);
    offset += 32;

    // pincode
    memset(_accessoryConfig->pincode, 0, 10 + 1);
    memcpy(_accessoryConfig->pincode, buffer + offset, 10);
    offset += 10;

    // setupId
    memset(_accessoryConfig->setupId, 0, 4 + 1);
    memcpy(_accessoryConfig->setupId, buffer + offset, 4);
    offset += 4;

    // configNumber    
    _accessoryConfig->initConfigNumber = HAPHelper::u8_to_u32(buffer + offset);
    offset += 4;


    // LTPK + LTSK
    memcpy(_accessoryConfig->LTKP.LTPK, buffer + offset, HAP_PAIRINGS_LTPK_LENGTH);
    offset += HAP_PAIRINGS_LTPK_LENGTH;
    memcpy(_accessoryConfig->LTKP.LTSK, buffer + offset, HAP_PAIRINGS_LTSK_LENGTH);
	offset += HAP_PAIRINGS_LTSK_LENGTH;



	for (uint8_t i = 0; i < (bufferSize - offset) / sizeof(HAPConfigurationPairingEntry); i++){
		
        if (!isOnlyZeros(buffer + offset, sizeof(HAPConfigurationPairingEntry))) {
            HAPConfigurationPairingEntry* newPairing = new HAPConfigurationPairingEntry();
            memcpy((uint8_t*)newPairing, (uint8_t*)buffer + offset, sizeof(HAPConfigurationPairingEntry));		
            offset += sizeof(HAPConfigurationPairingEntry);
            
            _accessoryConfig->pairings.push_back(newPairing);	
        }			
	}
	return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPConfiguration::isOnlyZeros(const uint8_t* bytearray, int length) {

    for (size_t i = 0; i < length; i++){
        if (bytearray[i] > 0) return false;
    }

    return true;
}