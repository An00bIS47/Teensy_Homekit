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
#include "HAPCharacteristics.hpp"
#include "HAPCharacteristic.hpp"


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
	HAPCharacteristicBase* characteristicsOfType(int type);
	HAPCharacteristicBase* characteristicsOfType(const char* typeString);

	void printTo(Print& print);
	// void toJson(JsonArray& array);
	// String describe() const;

	HAPService* addInfoService(const String& accessoryName, const String& manufactuerName, const String& modelName, const String& serialNumber, identifyFunctionCallback callback, const String& firmwareRev = "");


	void setIdentifyCallback(identifyFunctionCallback callback);

	void setFirmware(const String& firmwareRev);

	uint8_t aid() { return _aid; }
    void setAID(uint8_t aid) { _aid = aid; }

protected:

	void initInfoService();

	uint8_t _aid;	// only 150 accessories are allowed!
	uint32_t _numberOfInstances = 0;

	std::vector<std::unique_ptr<HAPService>> _services;

	HAPService*	_infoService;
	HAPCharacteristic<bool>* 	_identify;
	HAPCharacteristic<String>* _accessoryName;
	HAPCharacteristic<String>* _firmware;
	HAPCharacteristic<String>* _manufacturer;
	HAPCharacteristic<String>* _modelName;
	HAPCharacteristic<String>* _serialNumber;
};


#endif /* HAPACCESSORY_HPP_ */