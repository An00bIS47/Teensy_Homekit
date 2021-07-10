//
// HAPAccessorySet.hpp
// Homekit
//
//  Created on: 18.08.2017
//      Author: michael
//

#ifndef HAPACCESSORYSET_HPP_
#define HAPACCESSORYSET_HPP_

#include <Arduino.h>
#include <vector>

#include "HAPAccessory.hpp"
#include "HAPAccessoryType.hpp"
// #include "HAPPairings.hpp"

#include "HAPConfiguration.hpp"

class HAPAccessorySet {
public:
	HAPAccessorySet();
	~HAPAccessorySet();

	static uint32_t configurationNumber;

	uint8_t aid(){ return _aid; };
	void begin();

	bool isPaired(){
		return (_configuration->pairings.size() > 0);
	}

	// HAPPairings* getPairings() {
	// 	return &_pairings;
	// }

	uint8_t numberOfPairings(){
		return _configuration->pairings.size();
	}

	void setLongTermKeyPair(const uint8_t* LTSK, const uint8_t* LTPK){
		_configuration->setLongTermKeyPair(LTSK, LTPK);
		_configuration->save();
	}

	uint8_t* LTPK() { return _configuration->LTPKPtr(); }
	uint8_t* LTSK() { return _configuration->LTSKPtr(); }

	uint8_t* getKeyForPairingWithId(const uint8_t* id){
		return _configuration->getKeyForPairingWithId(id);
	}

	HAPConfigurationPairingEntry* getPairingAtIndex(uint8_t index){
		return _configuration->getPairingAtIndex(index);
	}

	bool pairingIdIsAdmin(const uint8_t* id){
		return _configuration->isAdmin(id);
	}

	void addPairing(const uint8_t* id, const uint8_t* key, bool isAdminPairing){
		_configuration->addPairing(id, key, isAdminPairing);
		_configuration->save();
	}

	bool removePairing(const uint8_t* id){
		if (_configuration->removePairing(id)) {
			_configuration->save();
			return true;
		}
		return false;
	}

	void removeAllPairings(){
		_configuration->clearPairings();
		_configuration->save();
	}

	uint8_t accessoryTypeAsInt();
	HAPAccessoryType accessoryType();
	void setAccessoryType(HAPAccessoryType accessoryType);

	void addAccessoryInfo();

	const char* setupID();
	void generateSetupID();

	void setModelName(const char* name);
	const char* modelName();

	void setPinCode(const char* pinCode);
	const char* pinCode();

	const char* xhm();
	const char* setupHash();

	void printTo(Print& print);

	bool removeAccessory(HAPAccessory *acc);
	void addAccessory(HAPAccessory *acc);

	HAPAccessory* accessoryAtIndex(uint8_t index);
	HAPAccessory* accessoryWithAID(uint8_t aid);


	int32_t getValueForCharacteristics(uint8_t aid, uint8_t iid, char* out, size_t* outSize);

	HAPCharacteristicBase* getCharacteristic(uint8_t aid, uint32_t iid);

	HAPCharacteristicBase* getCharacteristicOfType(uint8_t aid, uint8_t type);
	HAPCharacteristicBase* getCharacteristicOfType(uint8_t aid, const char* type);

	void setIdentifyCharacteristic(bool value);

	uint8_t numberOfAccessories();

	void setConfiguration(HAPConfigurationAccessory* configuration){
		_configuration = configuration;
		configurationNumber += _configuration->initConfigNumber;
	}


protected:

	HAPAccessoryType _accessoryType; // enum HAP_ACCESSORY_TYPE

	// Setup ID can be provided, although, per spec, should be random
	// every time the instance is started. If not provided on init, will be random.
	// 4 digit string 0-9 A-Z
	// String 		_setupID;
	char		_setupHash[9] = {0,};
	char		_xhm[21] = {0,};

private:
	void computeSetupHash();
	char randomChar(char* letters);

	void generateXMI();

	HAPConfigurationAccessory* 	_configuration;

	// HAPPairings _pairings;
	std::vector<std::unique_ptr<HAPAccessory>> _accessories;

	uint8_t _aid = 0;
};

#endif /* HAPACCESSORYSET_HPP_ */
