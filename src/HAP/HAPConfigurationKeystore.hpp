//
// HAPConfigurationKeystore.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONKEYSTORE_HPP_
#define HAPCONFIGURATIONKEYSTORE_HPP_

#include <Arduino.h>
#include <functional>
#include "HAPGlobals.hpp"
#include "HAPLogging.hpp"
#include "HAPConfigurationItem.hpp"

#define HAP_KEYSTORE_LABEL_LENGTH	10
//
// Keystore
//
#if HAP_ENABLE_KEYSTORE

class HAPConfigurationKeystore : public HAPConfigurationItem {

public:

	char keystore[HAP_KEYSTORE_LABEL_LENGTH + 1] = {0,};

	HAPConfigurationKeystore(){
		memcpy(keystore, HAP_KEYSTORE_PARTITION_LABEL, HAP_KEYSTORE_LABEL_LENGTH);
	}

	~HAPConfigurationKeystore(){
	}

	HAPConfigurationKeystore(const char* keystore_){
			strncpy(keystore, keystore_, HAP_KEYSTORE_LABEL_LENGTH);
		}

	HAPConfigurationKeystore(const HAPConfigurationKeystore& rhs) {
		{
			uint8_t size = strlen(rhs.keystore);

			if (size > HAP_KEYSTORE_LABEL_LENGTH) {
				LOG_E("ERROR: KEYSTORE is too long!\n");
				return;
			}
			strncpy(keystore, rhs.keystore, HAP_KEYSTORE_LABEL_LENGTH);
		}
	}

	static size_t getDataSize(){
		return (sizeof(char) * HAP_KEYSTORE_LABEL_LENGTH);
	}

	void setKeystore(const char* keystore_){
		strncpy(keystore, keystore_, HAP_KEYSTORE_LABEL_LENGTH);
	}

	const char* getKeystore(){
		return (const char* )keystore;
	}

	HAPConfigurationKeystore& operator=(const HAPConfigurationKeystore& rhs) {
		if (this == &rhs) {
			return (*this);
		}

		{
			uint8_t size = strlen(rhs.keystore);

			if (size > HAP_KEYSTORE_LABEL_LENGTH) {
				LOG_E("ERROR: KEYSTORE is too long!\n");
				return (*this);
			}
			strncpy(keystore, rhs.keystore, HAP_KEYSTORE_LABEL_LENGTH);
		}
		return (*this);
	}

	bool operator== (const HAPConfigurationKeystore &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */
		return (
			( strncmp(this->keystore, rhs.keystore, strlen(this->keystore)) == 0 )
		) ? true : false;
	}


	void clear() override {
		memset(keystore, 0, HAP_KEYSTORE_LABEL_LENGTH);
	}

	void setDefaults() override {
		memcpy(keystore, HAP_KEYSTORE_PARTITION_LABEL, HAP_KEYSTORE_LABEL_LENGTH);
	}


	// {
	// 		"active": "keystore_0"
    // }
	void toJson(Print& prt) override {
		prt.print("{");
		prt.print("\"active\": ");
		prt.print(HAPHelper::wrap(keystore).c_str());
		prt.print("}");
	}
};

#endif /* HAP_ENABLE_KEYSTORE */


#endif /* HAPCONFIGURATIONKEYSTORE_HPP_ */