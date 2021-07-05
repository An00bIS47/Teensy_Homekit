//
// HAPConfigurationOTA.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONOTA_HPP_
#define HAPCONFIGURATIONOTA_HPP_

#include <Arduino.h>
#include <functional>
#include "HAPGlobals.hpp"
#include "HAPLogging.hpp"
#include "HAPHelper.hpp"
#include "HAPConfigurationItem.hpp"

//
// Arduino OTA
//
#if HAP_ENABLE_UPDATE_OTA

#define HAP_OTA_PASSWORD_LENGTH	32

class HAPConfigurationArduinoOTA : public HAPConfigurationItem {
public:

	bool enabled = false;
	uint16_t port = 0;
	char password[HAP_OTA_PASSWORD_LENGTH + 1] = {0,};



	HAPConfigurationArduinoOTA(){
		enabled = true;
		port = 3232;
		memset(password, 0, HAP_OTA_PASSWORD_LENGTH + 1);
	}

	~HAPConfigurationArduinoOTA(){

	}


	static size_t getDataSize(){
		return sizeof(bool) + sizeof(uint16_t) + (sizeof(char) * HAP_OTA_PASSWORD_LENGTH);
	}


	HAPConfigurationArduinoOTA(bool enabled_, uint16_t port_, const char* password_)
		: enabled(enabled_)
		, port(port_)
		{
			strncpy(password, password_, 32);
		}

	bool isEnabled(){ return enabled; }

	HAPConfigurationArduinoOTA(const HAPConfigurationArduinoOTA& rhs) {

		enabled = rhs.enabled;
		port = rhs.port;

		{
			uint8_t size = strlen(rhs.password);

			if (size > 32) {
				LOG_E("ERROR: PASSWORD is too long!\n");
				return;
			}
			strncpy(password, rhs.password, 32);
		}
	}

	void setPassword(const char* password_){
		strncpy(password, password_, 32);
	}

	const char* getPassword(){
		return (const char* )password;
	}

	HAPConfigurationArduinoOTA& operator=(const HAPConfigurationArduinoOTA& rhs) {
		if (this == &rhs) {
			return (*this);
		}

		enabled = rhs.enabled;
		port = rhs.port;

		{
			uint8_t size = strlen(rhs.password);

			if (size > 32) {
				LOG_E("ERROR: PASSWORD is too long!\n");
				return (*this);
			}
			strncpy(password, rhs.password, 32);
		}
		return (*this);
	}

	bool operator== (const HAPConfigurationArduinoOTA &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */
		return (
			( this->enabled == rhs.enabled ) &&
			( strncmp(this->password, rhs.password, strlen(this->password)) == 0 ) &&
			( this->port == rhs.port )
		) ? true : false;
	}


	void clear() override {
		memset(password, 0, 32);
	}

	void setDefaults() override {
		enabled = true;
		port = 3232;
		const char* passw = "secret";
		strcpy(password, passw);
	}

	// {
	// 		"enabled": true,
	// 		"port": 3232,
	// 		"password": "admin"
    // }
	void toJson(Print& prt) override {
		prt.print("{");
		prt.print("\"enabled\": ");
		prt.print(enabled);
		prt.print(",");

		prt.print("\"port\": ");
		prt.print(port);
		prt.print(",");

		prt.print("\"password\": ");
		prt.print(HAPHelper::wrap(password));

		prt.print("}");
	}
};

#endif /* HAP_ENABLE_UPDATE_OTA */


#endif /* HAPCONFIGURATIONOTA_HPP_ */