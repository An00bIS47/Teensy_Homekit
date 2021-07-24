//
// HAPConfigurationAccessory.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONACCESSORY_HPP_
#define HAPCONFIGURATIONACCESSORY_HPP_

#include <Arduino.h>
#include <vector>
#include <functional>
#include "HAPGlobals.hpp"
#include "HAPLogging.hpp"
#include "HAPHelper.hpp"
#include "HAPConfigurationItem.hpp"

// HAPConfigurationPairingEntry
//	id 	=  36
//	key =  32
// 	bool = 1
//	================
//         69 Bytes
struct HAPConfigurationPairingEntry {
	uint8_t id[HAP_PAIRINGS_ID_LENGTH];
	uint8_t key[HAP_PAIRINGS_LTPK_LENGTH];
	bool isAdmin;
};

// HAPConfigurationLTKP
// Long Termn Key Pair
//	LTPK 	=  32		Long Term Public Key
//	LTSK	=  64		Long Term Secret Key
//	================
//         96 Bytes
struct HAPConfigurationLTKP {
	uint8_t LTPK[HAP_PAIRINGS_LTPK_LENGTH] = {0,};
	uint8_t LTSK[HAP_PAIRINGS_LTSK_LENGTH] = {0,};
};


//
// Accessory
//
class HAPConfigurationAccessory : public HAPConfigurationItem {
public:
	uint16_t port 			= 51628;
	char pincode[10+1] 		= {0,};			// 031-45-712 = 8 digits + 2 "-"
	char setupId[4+1] 		= {0,};			// setupId	4 char
	char modelname[32+1] 	= {0,};

	uint32_t initConfigNumber = HOMEKIT_CONFIGURATION_NUMBER;		// unused ...

	struct HAPConfigurationLTKP LTKP;

	std::vector<HAPConfigurationPairingEntry*> pairings;



	static size_t getDataSize(){
		return sizeof(uint16_t) + (sizeof(uint8_t) * 10) + (sizeof(uint8_t) * 4) + (sizeof(uint8_t) * 32) + sizeof(uint32_t) + ((sizeof(uint8_t) * HAP_PAIRINGS_LTPK_LENGTH) + (sizeof(uint8_t) * HAP_PAIRINGS_LTSK_LENGTH)) + (HAP_PAIRINGS_MAX * ((sizeof(uint8_t) * HAP_PAIRINGS_ID_LENGTH) + (sizeof(uint8_t) * HAP_PAIRINGS_LTPK_LENGTH) + sizeof(bool)) );
	}


	HAPConfigurationAccessory(){

	}

	~HAPConfigurationAccessory(){

	}

	HAPConfigurationAccessory(const HAPConfigurationAccessory& rhs) {

		port = rhs.port;
		initConfigNumber = rhs.initConfigNumber;

		setPincode(rhs.pincode);
		setSetupId(rhs.setupId);
		setModelName(rhs.modelname);

		pairings = rhs.pairings;
		LTKP = rhs.LTKP;
	}

	void increaseConfigNumber(){
		initConfigNumber++;
	}

	void setPincode(const char* pin){
		if (strlen(pin) > 11) {
			LOG_E("ERROR: PIN CODE is too long!\n");
			return;
		}
		strncpy(pincode, pin, 11);
		pincode[10] = '\0';
	}

	void setSetupId(const char* setupId_){
		if (strlen(setupId_) > 5) {
			LOG_E("ERROR: SETUP ID is too long!\n");
			return;
		}
		strncpy(setupId, setupId_, 5);
		setupId[4] = '\0';
	}

	void setModelName(const char* modelName_){
		if (strlen(modelName_) > 33) {
			LOG_E("ERROR: MODEL NAME is too long!\n");
			return;
		}
		strncpy(modelname, modelName_, 33);
		modelname[32] = '\0';
	}


	uint8_t size(){
        return pairings.size();
    }

    int getIndex(const uint8_t* id){
        for(size_t i = 0; i < pairings.size(); i++) {
            struct HAPConfigurationPairingEntry* item = pairings[i];
            if ( memcmp(item->id, id, HAP_PAIRINGS_ID_LENGTH) == 0) {
                return i;
            }
        }
        return -1;
    }

	HAPConfigurationPairingEntry* getPairingAtIndex(uint8_t index){
		return pairings[index];
	}


    uint8_t* getKeyForPairingWithId(const uint8_t* id){
        int index = getIndex(id);
        if (index != -1) {
            // memcpy(outkey, pairings[index]->key, HAP_PAIRINGS_LTPK_LENGTH);
			return pairings[index]->key;
        }
        return nullptr;
    }

	bool removePairing(const uint8_t* id){
		int index = getIndex(id);
		if (index != -1) {
			pairings.erase(pairings.begin() + index);
			return true;
		} else {
			LOG_E("ERROR: Pairing id not found!\n");
			LOGARRAY_D("id", id, HAP_PAIRINGS_ID_LENGTH);
			return false;
		}
	}

    bool addPairing(const uint8_t* id, const uint8_t* key, bool isAdminPairing){
        bool result = false;
        int index = getIndex(id);
        if (index == -1) {
            struct HAPConfigurationPairingEntry* item = new HAPConfigurationPairingEntry();
            memcpy(item->id, id, HAP_PAIRINGS_ID_LENGTH);
            memcpy(item->key, key, HAP_PAIRINGS_LTPK_LENGTH);
            item->isAdmin = isAdminPairing;

#if HAP_DEBUG_PAIRINGS
            LOGARRAY_D("ID", item->id, HAP_PAIRINGS_ID_LENGTH);
            LOGARRAY_D("Key", item->key, HAP_PAIRINGS_LTPK_LENGTH);
			LOG_D("isAdmin: %d\n", item->isAdmin);
#endif

            pairings.push_back(item);
            result = true;
        } else {
            if (memcmp(pairings[index]->key, key, HAP_PAIRINGS_LTPK_LENGTH) == 0) {
                result = true;
            }
            pairings[index]->isAdmin = isAdminPairing;
        }

        return result;
    }

    bool isAdmin(const uint8_t *id){

        int index = getIndex(id);

        if (index != -1) {
            return pairings[index]->isAdmin;
        }
        return false;
    }

	void setLongTermKeyPair(const uint8_t *secretKey, const uint8_t *publicKey){
		memcpy(LTKP.LTPK, publicKey, HAP_PAIRINGS_LTPK_LENGTH);
		memcpy(LTKP.LTSK, secretKey, HAP_PAIRINGS_LTSK_LENGTH);
	}

	uint8_t* LTPKPtr(){
		return (uint8_t*)&LTKP.LTPK;
	}

	uint8_t* LTSKPtr(){
		return (uint8_t*)&LTKP.LTSK;
	}


	void clearPairings(){
		pairings.clear();
	}

	void clear() override {
		pairings.clear();
		memset(pincode, 0, 10 + 1);
		memset(setupId, 0, 4 + 1);
		memset(modelname, 0, 32 + 1);
		initConfigNumber = 0;

		memset(LTKP.LTSK, 0, HAP_PAIRINGS_LTPK_LENGTH);
		memset(LTKP.LTPK, 0, HAP_PAIRINGS_LTSK_LENGTH);
	}

	void setDefaults() override {
		clear();
		{
			memcpy(pincode, HAP_PIN_CODE, 10);
		}

		{
			memcpy(setupId, HAP_SETUP_ID, 4);
		}

		{
			memcpy(modelname, HAP_MODELL_NAME, strlen(HAP_MODELL_NAME));
		}
	}


	// "homekit": {
	// 		"port": 51628,
	// 		"initConfigNumber": 2,
	// 		"setupId": "UPDF",
	// 		"pinCode": "XXX-XX-XXX",
	// 		"modelname_": "abcdse",
	//		"pairings": [
	//				{
	//					"id": "x",
	//					"key": "x",
	//					"isAdmin": 0,
	//				}, ...
	//			]
    // }
	void toJson(Print& prt) override {
		prt.print("{");

		prt.print("\"port\": ");
		prt.print(port);

		prt.print(",");
		prt.print("\"initConfigNumber\": ");
		prt.print(initConfigNumber);

		prt.print(",");
		prt.print("\"pincode\": ");
		prt.print(HAPHelper::wrap(pincode).c_str());

		prt.print(",");
		prt.print("\"setupId\": ");
		prt.print(HAPHelper::wrap(setupId).c_str());

		prt.print(",");
		prt.print("\"modelname\": ");
		prt.print(HAPHelper::wrap(modelname).c_str());

		prt.print(",");
		prt.print("\"pairings\": [");
        for (size_t i=0; i < pairings.size(); i++){
            prt.print("{");
            prt.print("\"id\": ");
            // ToDo: Print id properly
            prt.print(HAPHelper::wrap(String(pairings[i]->id[0], HEX).c_str()).c_str());

            prt.print(",");
            prt.print("\"key\": ");
            // ToDo: Print key properly
            prt.print(HAPHelper::wrap(String(pairings[i]->key[0], HEX).c_str()).c_str());

			prt.print(",");
            prt.print("\"isAdmin\": ");
            prt.print(pairings[i]->isAdmin);

            prt.print("}");

            if (i+1 < pairings.size()){
                prt.print(",");
            }
        }
        prt.print("]");


		prt.print("}");
	}
};


#endif /* HAPCONFIGURATIONACCESSORY_HPP_ */