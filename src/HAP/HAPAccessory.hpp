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
#include <ArduinoJson.h>

#include "HAPService.hpp"
#include "HAPCharacteristics.hpp"


// typedef void (*identifyFunction)(bool oldValue, bool newValue);
typedef std::function<void(bool, bool)> identifyFunctionCallback;

class HAPAccessory {
public:

	HAPAccessory();
	~HAPAccessory();

	uint8_t aid;    
	uint8_t numberOfInstance = 0;
	
	std::vector<HAPService *>_services;

	void addService(HAPService *ser);   
	void addCharacteristics(HAPService *ser, HAPCharacteristic *cha);
	bool removeService(HAPService *ser);
	bool removeCharacteristics(HAPCharacteristic *cha);

	

	uint8_t numberOfService() const;
	HAPService *serviceAtIndex(uint8_t index);

	HAPCharacteristic *characteristicsAtIndex(uint8_t index);
	HAPCharacteristic *characteristicsOfType(int type);

	void printTo(Print& print);
	void toJson(JsonArray& array);
	// String describe() const;

	HAPService* addInfoService(const String& accessoryName, const String& manufactuerName, const String& modelName, const String& serialNumber, identifyFunctionCallback callback, const String& firmwareRev = "");
	
	void setName(const String& name);
	String name();
	
	void setFirmware(const String& firmware);
	String firmware();

	void setIdentifyCallback(identifyFunctionCallback callback);


	String serialNumber();
	void setSerialNumber(const String& serialNumber);


	String modelName();
	void setModelName(const String& serialNumber);


	String manufacturer();
	void setManufacturer(const String& serialNumber);

private:

	void initInfoService();
	void initAccessoryName();
	void initFirmware();
	void initSerialNumber();
	void initIdentify();
	void initModelName();
	void initManufacturer();

	// String _accessoryName;

	HAPService*               _infoService;

	HAPCharacteristicString*  _accessoryName;
	HAPCharacteristicString*  _firmware;
	HAPCharacteristicString*  _manufacturer;
	HAPCharacteristicString*  _modelName;
	HAPCharacteristicString*  _serialNumber;
	HAPCharacteristicBool*    _identify;
};


#endif /* HAPACCESSORY_HPP_ */