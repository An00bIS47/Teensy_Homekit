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


// typedef void (*identifyFunction)(bool oldValue, bool newValue);
typedef std::function<void(bool, bool)> identifyFunctionCallback;

class HAPAccessory {
public:

	HAPAccessory();
	~HAPAccessory();

	void addService(HAPService *service);

	void addCharacteristic(HAPService *service, HAPCharacteristicBase* characteristic);

	bool removeService(HAPService *service);

	bool removeCharacteristic(HAPCharacteristicBase* characteristic);

	size_t numberOfServices() const;
	std::shared_ptr<HAPService*> serviceAtIndex(size_t index);

	
	std::shared_ptr<HAPCharacteristicBase*> characteristicAtIndex(size_t index);
	// HAPCharacteristic *characteristicsOfType(int type);

	void printTo(Print& print);

	HAPService* addInfoService(const String& accessoryName, const String& manufactuerName, const String& modelName, const String& serialNumber, identifyFunctionCallback callback, const String& firmwareRev = "");
	

	void setIdentifyCallback(identifyFunctionCallback callback);
	
	void setFirmware(const String& firmwareRev);


protected:

	void initInfoService();

	uint8_t _aid;    
	uint32_t _numberOfInstances = 0;

	std::vector<std::shared_ptr<HAPService*>> _services;
	
	HAPService*	_infoService;
	HAPCharacteristicT<bool>* 	_identify;
	HAPCharacteristicT<String>* _accessoryName;
	HAPCharacteristicT<String>* _firmware;
	HAPCharacteristicT<String>* _manufacturer;
	HAPCharacteristicT<String>* _modelName;
	HAPCharacteristicT<String>* _serialNumber;
};


#endif /* HAPACCESSORY_HPP_ */