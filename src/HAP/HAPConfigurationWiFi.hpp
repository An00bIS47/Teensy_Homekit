//
// HAPConfigurationWiFi.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONWIFI_HPP_
#define HAPCONFIGURATIONWIFI_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include "HAPLogger.hpp"
#include "HAPHelper.hpp"

#if HAP_ENABLE_WIFI

#define HAP_WIFI_CREDENTIALS_SIZE	5
#define HAP_WIFI_SSID_LENGTH		32
#define HAP_WIFI_PASSWORD_LENGTH	64

#include <functional>
#include <vector>
#include "HAPConfigurationItem.hpp"


struct HAPConfigurationWiFiCredentials {
	char ssid[HAP_WIFI_SSID_LENGTH + 1] = {0,};
	char password[HAP_WIFI_PASSWORD_LENGTH + 1] = {0,};

	static size_t getDataSize() {
		return ( (sizeof(char) * HAP_WIFI_SSID_LENGTH) +  (sizeof(char) * HAP_WIFI_PASSWORD_LENGTH) );
	}

	HAPConfigurationWiFiCredentials(){
		memset(ssid, 0, HAP_WIFI_SSID_LENGTH + 1);
		memset(password, 0, HAP_WIFI_PASSWORD_LENGTH + 1);
	}
	
	HAPConfigurationWiFiCredentials(const char* ssid_, const char* password_){
		strncpy(ssid, ssid_, HAP_WIFI_SSID_LENGTH);		
		strncpy(password, password_, HAP_WIFI_PASSWORD_LENGTH);
	}

	HAPConfigurationWiFiCredentials(const HAPConfigurationWiFiCredentials& rhs) {		
		
		{
			uint8_t size = strlen(rhs.ssid);

			if (size > HAP_WIFI_SSID_LENGTH) {
				LogE(F("ERROR: SSID is too long!"), true);
				return;
			}
			strncpy(ssid, rhs.ssid, HAP_WIFI_SSID_LENGTH);
		}

		{
			uint8_t size = strlen(rhs.password);

			if (size > HAP_WIFI_PASSWORD_LENGTH) {
				LogE(F("ERROR: PASSWORD is too long!"), true);
				return;
			}
			strncpy(password, rhs.password, HAP_WIFI_PASSWORD_LENGTH);
		}

	}

	HAPConfigurationWiFiCredentials operator=(const HAPConfigurationWiFiCredentials& rhs) {
		if (this == &rhs) {
			return (*this);
		}


		{
			uint8_t size = strlen(rhs.ssid);

			if (size > HAP_WIFI_SSID_LENGTH) {
				LogE(F("ERROR: SSID is too long!"), true);
				return (*this);
			}
			strncpy(ssid, rhs.ssid, HAP_WIFI_SSID_LENGTH);
		}

		{
			uint8_t size = strlen(rhs.password);

			if (size > HAP_WIFI_PASSWORD_LENGTH) {
				LogE(F("ERROR: PASSWORD is too long!"), true);
				return (*this);
			}
			strncpy(password, rhs.password, HAP_WIFI_PASSWORD_LENGTH);
		}
	
		
	}

	bool operator== (const HAPConfigurationWiFiCredentials &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */ 
		return ( 
			( strncmp(this->ssid, rhs.ssid, strlen(this->ssid)) == 0 ) && 		
			( strncmp(this->password, rhs.password, strlen(this->password)) == 0 ) 
		) ? true : false;
	}

};


class HAPConfigurationWiFi : public HAPConfigurationItem {
public:

	bool  enabled;
	uint8_t mode;	
	std::vector<HAPConfigurationWiFiCredentials*> credentials;

	HAPConfigurationWiFi(){
		enabled = true;
		mode = HAP_WIFI_MODE_DEFAULT;
	}

	static size_t getDataSize(){
		return (sizeof(bool) + sizeof(uint8_t) + (HAP_WIFI_CREDENTIALS_SIZE * HAPConfigurationWiFiCredentials::getDataSize()));
	}

	~HAPConfigurationWiFi(){
		credentials.clear();
	}

	bool isEnabled(){ return enabled; }

	void setWifiMode(uint8_t mode_) { mode = mode_; }
	uint8_t wifiMode() { return mode; }
	
	void addNetwork(const String& ssid, const String& password){
		addNetwork(ssid.c_str(), password.c_str());
	}

	void addNetwork(const char* ssid, const char* password){
		int index = getIndexOfSSID(ssid);

		if (index < 0) {			
			credentials.push_back(new HAPConfigurationWiFiCredentials(ssid, password));
		} else {	
			HAPConfigurationWiFiCredentials* newCred = new HAPConfigurationWiFiCredentials(ssid, password);

			if (newCred == credentials[index]){
			} else {
				credentials[index] = newCred;
			}
			delete newCred;				
		}
	}

	int getIndexOfSSID(const char* ssid){
		for (uint8_t i = 0; i < credentials.size(); i++){
			if (strncmp(ssid, credentials[i]->ssid, strlen(credentials[i]->ssid)) == 0){
				return i;
			}
		}
		return -1;
	}
	
	void clear() override {
		credentials.clear();
	}

	void setDefaults() override {

		credentials.clear();

		enabled = true;
		mode = HAP_WIFI_MODE_DEFAULT;

#if defined(WIFI_SSID)
		HAPConfigurationWiFiCredentials* creds = new HAPConfigurationWiFiCredentials(WIFI_SSID, WIFI_PASSWORD); 
		credentials.push_back(creds);
#endif
	}

	 /*
        {
			"enabled": true,
            "mode": 0,
            "networks" [
                {
                    "ssid": "XXX",
                    "password": "XXX"
                }
            ]
        }
     */
	void toJson(Print& prt) override {
		prt.print("{");
		prt.print("\"enabled\": ");
		prt.print(enabled);
		prt.print(",");

		prt.print("\"mode\": ");
		prt.print(mode);
		prt.print(",");

		prt.print("\"networks\": [");				
		for (uint8_t i = 0; i < credentials.size(); i++){
			prt.print("{");
			prt.print("\"ssid\":");
			prt.print(HAPHelper::wrap(credentials[i]->ssid));
			prt.print(",");
			prt.print("\"passwort\":");
			prt.print(HAPHelper::wrap(credentials[i]->password));
			prt.print("}");
			if (i + 1 < credentials.size()){
				prt.print(",");
			}
		}
				
		prt.print("]");
		prt.print("}");
	}
};
#endif /* HAP_ENABLE_WIFI */


#endif /* HAPCONFIGURATIONWIFI_HPP_ */