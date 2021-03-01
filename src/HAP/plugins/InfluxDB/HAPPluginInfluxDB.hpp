//
// HAPPluginInfluxDB.hpp
// Homekit
//
//  Created on: 29.04.2018
//      Author: michael
//

#ifndef HAPPLUGININFLUXDB_HPP_
#define HAPPLUGININFLUXDB_HPP_

#include <Arduino.h>

#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"
#include <InfluxDb.h>


struct HAPPluginInfluxDBConfig {
	uint16_t port = 0;
	char username[32 + 1] 	= {0,};
	char password[32 + 1] 	= {0,};
	char database[32 + 1] 	= {0,};	
	char url[255 + 1] 		= {0,};

	size_t getDataSize(){
		return sizeof(username) + sizeof(password) + sizeof(url) + 	sizeof(database) + sizeof(uint16_t);
	}

	HAPPluginInfluxDBConfig(uint16_t port_, const char* username_, const char* password_, const char* database_, const char* url_){
		port = port_;

		if (strlen(username_) > 32) {
			username[0] = '\0';
		} else {
			memcpy(username, username_, strlen(username_));
			username[strlen(username_) > 32 ? 32 : strlen(username_)] = '\0';		
		}
		
		if (strlen(password_) > 32) {
			password[0] = '\0';
		} else {
			memcpy(password, password_, strlen(password_));
			password[strlen(password_) > 32 ? 32 : strlen(password_)] = '\0';
		}

		if (strlen(database_) > 32) {
			database[0] = '\0';
		} else {	
			memcpy(database, database_, strlen(database_));
			database[strlen(database_) > 32 ? 32 : strlen(database_)] = '\0';
		}

		if (strlen(url_) > 32) {
			url[0] = '\0';
		} else {	
			memcpy(url, url_, strlen(url_));
			url[strlen(url_) > 255 ? 255 : strlen(url_)] = '\0';		
		}
	}
	

};


class HAPPluginInfluxDB: public HAPPlugin {
public:

	HAPPluginInfluxDB();
	~HAPPluginInfluxDB() {
		delete _configInternal;
		delete _influxdb;
	}

	HAPAccessory* initAccessory() override;

	bool begin();

	
    void setValue(String oldValue, String newValue){};
    String getValue(int iid) { return ""; };

    void handleImpl(bool forced = false); 
	void handleEvents(int eventCode, struct HAPEvent eventParam);

#if HAP_ENABLE_WEBSERVER	
	HAPConfigurationValidationResult validateConfig(JsonObject object);
	
	JsonObject getConfigImpl();
	void setConfigImpl(JsonObject root);
#endif
	
	void addEventListener(EventManager* eventManager);

	HAPConfigurationPlugin* setDefaults() override;
	void internalConfigToJson(Print& prt); 	
	void setConfiguration(HAPConfigurationPlugin* cfg) override;

	
protected:

	Influxdb* _influxdb;

	HAPPluginInfluxDBConfig* _configInternal;

	uint16_t _usedSize;
	
	// bool shouldHandle();
};

REGISTER_PLUGIN(HAPPluginInfluxDB)

#endif /* HAPPLUGININFLUXDB_HPP_ */ 