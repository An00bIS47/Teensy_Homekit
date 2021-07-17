//
// HAPConfigurationWebServer.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONWEBSERVER_HPP_
#define HAPCONFIGURATIONWEBSERVER_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"

#if HAP_ENABLE_WEBSERVER

#define HAP_WEBSERVER_MAX_CREDENTIALS	5


#include <functional>
#include <vector>
#include "HAPConfigurationItem.hpp"

enum HAPConfigurationWebServerPermission {
	HAPConfigurationWebserverPermission_API		= 1,
	HAPConfigurationWebserverPermission_Admin	= 1 << 1,
};

struct HAPConfigurationWebServerCredentials {
	uint8_t permission   = 0; // enum HAPConfigurationWebServerPermission permission;
	char 	username[32 + 1] = {0,};
	char 	password[32 + 1] = {0,};


	static size_t getDataSize(){
		return (sizeof(uint8_t) + (sizeof(char) * 32) + (sizeof(char) * 32));
	}


	HAPConfigurationWebServerCredentials(){

	}

	HAPConfigurationWebServerCredentials(const uint8_t permission_, const char* username_, const char* password_)
		: permission(permission_) {

		strncpy(username, username_, 32);
		strncpy(password, password_, 32);
	}

	HAPConfigurationWebServerCredentials(const HAPConfigurationWebServerCredentials& rhs) {

		permission = rhs.permission;

		{
			uint8_t size = strlen(rhs.username);

			if (size > 32) {
				LogE(F("ERROR: USERNAME is too long!"), true);
				return;
			}
			strncpy(username, rhs.username, 32);
		}

		{
			uint8_t size = strlen(rhs.password);

			if (size > 32) {
				LogE(F("ERROR: PASSWORD is too long!"), true);
				return;
			}
			strncpy(password, rhs.password, 32);
		}

	}

	HAPConfigurationWebServerCredentials operator=(const HAPConfigurationWebServerCredentials& rhs) {
		if (this == &rhs) {
			return (*this);
		}

		permission = rhs.permission;

		{
			uint8_t size = strlen(rhs.username);

			if (size > 32) {
				LogE(F("ERROR: USERNAME is too long!"), true);
				return (*this);
			}
			strncpy(username, rhs.username, 32);
		}

		{
			uint8_t size = strlen(rhs.password);

			if (size > 32) {
				LogE(F("ERROR: PASSWORD is too long!"), true);
				return (*this);
			}
			strncpy(password, rhs.password, 32);
		}
		return (*this);
	}

	bool operator== (const HAPConfigurationWebServerCredentials &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */
		return (
			( strncmp(this->username, rhs.username, strlen(this->username)) == 0 ) &&
			( strncmp(this->password, rhs.password, strlen(this->password)) == 0 ) &&
			( this->permission == rhs.permission )
		) ? true : false;
	}

};


class HAPConfigurationWebServer : public HAPConfigurationItem{
public:
	bool  enabled;
	uint16_t port;
	std::vector<HAPConfigurationWebServerCredentials*> credentials;

	bool isEnabled(){ return enabled; }



	static size_t getDataSize(){
		return (sizeof(bool) + sizeof(uint16_t) + (HAP_WEBSERVER_MAX_CREDENTIALS * HAPConfigurationWebServerCredentials::getDataSize()));
	}

	~HAPConfigurationWebServer(){
		credentials.clear();
	}


	void addCredentials(const uint8_t permission, const String& username, const String& password){
		addCredentials(permission, username.c_str(), password.c_str());
	}

	void addCredentials(const uint8_t permission, const char* username, const char* password){
		int index = getIndexOfUsername(username);

		if (index < 0) {
			credentials.push_back(new HAPConfigurationWebServerCredentials(permission, username, password));
		} else {
			HAPConfigurationWebServerCredentials* newCred = new HAPConfigurationWebServerCredentials(permission, username, password);
			if (newCred == credentials[index]){

			} else {
				credentials[index] = newCred;
			}

			delete newCred;
		}
	}

	int getIndexOfUsername(const char* username){
		for (uint8_t i = 0; i < credentials.size(); i++){
			if (strncmp(username, credentials[i]->username, strlen(credentials[i]->username)) == 0){
				return i;
			}
		}
		return -1;
	}

	bool getPasswordForUser(const char* username, char* password_){
		int index = getIndexOfUsername(username);
		if (index >= 0) {
			strncpy(password_, credentials[index]->password, 32);
			return true;
		}
		return false;
	}

	bool validatePassword(const char* username, const char* password){
		int index = getIndexOfUsername(username);
		if (index >= 0) {
			if (strncmp(password, credentials[index]->password, strlen(credentials[index]->password)) == 0) {
				return true;
			}
		}
		return false;
	}

	bool isAdminUser(const char* username, uint8_t permission){
		int index = getIndexOfUsername(username);
		if (index >= 0) {
			return (permission & HAPConfigurationWebserverPermission_Admin);
		}
		return false;
	}

	bool isAPIUser(const char* username, uint8_t permission){
		int index = getIndexOfUsername(username);
		if (index >= 0) {
			return ((permission & HAPConfigurationWebserverPermission_API) || (permission & HAPConfigurationWebserverPermission_Admin));
		}
		return false;
	}

	void clear() override {
		credentials.clear();
	}

	void setDefaults() override {

		credentials.clear();

		enabled = true;
		port = 443;

#if defined(HAP_WEBSERVER_ADMIN_USERNAME)
		{
			credentials.push_back(new HAPConfigurationWebServerCredentials(HAPConfigurationWebserverPermission_Admin, HAP_WEBSERVER_ADMIN_USERNAME, HAP_WEBSERVER_ADMIN_PASSWORD));
		}
#endif

#if defined(HAP_WEBSERVER_API_USERNAME)
		{
			credentials.push_back(new HAPConfigurationWebServerCredentials(HAPConfigurationWebserverPermission_API, HAP_WEBSERVER_API_USERNAME, HAP_WEBSERVER_API_PASSWORD));
		}
#endif

	}


    /*
        {
            "enabled": true,
			"port": 443,
            "credentials": [
                {
                    "permission": 1 << 1,
					"username": "admin",
                    "password": "secret"
                },
				{
                    "permission": 1,
					"username": "api",
                    "password": "secret"
                },
            ]
        }
     */
	void toJson(Print& prt) override {
		prt.print("{");
		prt.print("\"enabled\": ");
		prt.print(enabled);
		prt.print(",");

		prt.print("\"port\": ");
		prt.print(port);
		prt.print(",");

		prt.print("\"credentials\": [");
		for (uint8_t i = 0; i < credentials.size(); i++){
			prt.print("{");

			prt.print("\"permission\":");
			prt.print(credentials[i]->permission);
			prt.print(",");
			prt.print("\"username\":");
			prt.print(HAPHelper::wrap(credentials[i]->username).c_str());
			prt.print(",");
			prt.print("\"passwort\":");
			prt.print(HAPHelper::wrap(credentials[i]->password).c_str());
			prt.print("}");
			if (i + 1 < credentials.size()){
				prt.print(",");
			}
		}

		prt.print("]");
		prt.print("}");
	}

};
#endif /* HAP_ENABLE_WEBSERVER */


#endif /* HAPCONFIGURATIONWEBSERVER_HPP_ */