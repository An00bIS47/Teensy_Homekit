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

	template <class T>
	void addCharacteristic(HAPService *service, HAPCharacteristicT<T>* characteristic);

	bool removeService(HAPService *service);

	template <class T>
	bool removeCharacteristic(HAPCharacteristicT<T>* characteristic);

	size_t numberOfServices() const;
	std::shared_ptr<HAPService*> serviceAtIndex(size_t index);

	template <class T>
	std::shared_ptr<HAPCharacteristicT<T>*> characteristicsAtIndex(size_t index);
	// HAPCharacteristic *characteristicsOfType(int type);

	void printTo(Print& print);
	// void toJson(JsonArray& array);
	// String describe() const;

	HAPService* addInfoService(const String& accessoryName, const String& manufactuerName, const String& modelName, const String& serialNumber, identifyFunctionCallback callback, const String& firmwareRev = "");
	
	// void setName(const String& name);
	// String name();
	
	// void setFirmware(const String& firmware);
	// String firmware();

	void setIdentifyCallback(identifyFunctionCallback callback);


	// String serialNumber();
	// void setSerialNumber(const String& serialNumber);


	// String modelName();
	// void setModelName(const String& serialNumber);


	// String manufacturer();
	// void setManufacturer(const String& serialNumber);

protected:

	// void initInfoService();
	// void initAccessoryName();
	// void initFirmware();
	// void initSerialNumber();
	// void initIdentify();
	// void initModelName();
	// void initManufacturer();
	// String _accessoryName;

	uint8_t _aid;    
	uint32_t _numberOfInstances = 0;

	std::vector<std::shared_ptr<HAPService*>> _services;
	

	// HAPCharacteristicString*  _accessoryName;
	// HAPCharacteristicString*  _firmware;
	// HAPCharacteristicString*  _manufacturer;
	// HAPCharacteristicString*  _modelName;
	// HAPCharacteristicString*  _serialNumber;	
	// HAPCharacteristicBool*    _identify;

	HAPService*	_infoService;
	HAPCharacteristicT<bool>* 	_identify;
	HAPCharacteristicT<String>* _accessoryName;
	HAPCharacteristicT<String>* _firmware;
	HAPCharacteristicT<String>* _manufacturer;
	HAPCharacteristicT<String>* _modelName;
	HAPCharacteristicT<String>* _serialNumber;
};


#endif /* HAPACCESSORY_HPP_ */