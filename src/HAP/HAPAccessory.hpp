//
// HAPAccessory.hpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//

#ifndef HAPACCESSORY_HPP_
#define HAPACCESSORY_HPP_

#include <Arduino.h>
#include <vector>
#include <memory>
#include <ArduinoJson.h>

#include "HAPService.hpp"

#include "HAPCharacteristic.hpp"
#include "HAPCharacteristicType.hpp"

// typedef void (*identifyFunction)(bool oldValue, bool newValue);
typedef std::function<void(bool, bool)> identifyFunctionCallback;

class HAPAccessory {
public:

	HAPAccessory();
	~HAPAccessory();

	void addService(HAPService *service);

	void addCharacteristicToService(HAPService *service, HAPCharacteristicBase* characteristic);

	bool removeService(HAPService *service);
	bool removeCharacteristicFromService(HAPService *service, HAPCharacteristicBase* characteristic);

	size_t numberOfServices() const;
	HAPService* serviceAtIndex(size_t index);

	HAPCharacteristicBase* characteristicWithIID(uint32_t iid);
	HAPCharacteristicBase* characteristicAtIndex(HAPService *service, size_t index);
	HAPCharacteristicBase* characteristicsOfType(HAPCharacteristicType::Type type);
	HAPCharacteristicBase* characteristicsOfType(const char* typeString);

	void printTo(Print& print);

	HAPService* addInfoService(const char* accessoryName, const char* manufactuerName, const char* modelName, const char* serialNumber, identifyFunctionCallback callback, const char* firmwareRev = "");


	void setIdentifyCallback(identifyFunctionCallback callback);

	void setFirmware(const char* firmwareRev);

	uint8_t aid() { return _aid; }
    void setAID(uint8_t aid) { _aid = aid; }

protected:

	void initInfoService();

	uint8_t _aid;	// only 150 accessories are allowed!
	uint32_t _numberOfInstances = 0;

	std::vector<std::unique_ptr<HAPService>> _services;

	HAPService*	_infoService;
	HAPCharacteristic<bool>*   _identify;
	HAPCharacteristic<std::string>* _accessoryName;
	HAPCharacteristic<std::string>* _firmware;
	HAPCharacteristic<std::string>* _manufacturer;
	HAPCharacteristic<std::string>* _modelName;
	HAPCharacteristic<std::string>* _serialNumber;
};


#endif /* HAPACCESSORY_HPP_ */