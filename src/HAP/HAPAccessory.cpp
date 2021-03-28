//
// HAPAccessory.cpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//
#include "HAPAccessory.hpp"
#include "HAPHelper.hpp"
#include "HAPServer.hpp"
#include "EventManager.h"


#include "HAPCharacteristics.hpp"
#include "HAPServices.hpp"


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPAccessory::HAPAccessory() {
	aid = 0;
	
	_infoService    = nullptr;
	_accessoryName  = nullptr;
	_manufacturer   = nullptr;
	_modelName      = nullptr; 
	_serialNumber   = nullptr;
	_identify       = nullptr;
	_firmware       = nullptr;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPAccessory::~HAPAccessory(){
	if (_infoService) delete _infoService;

	if (_accessoryName) delete _accessoryName;
	if (_firmware) delete _firmware;
	if (_manufacturer) delete _manufacturer;
	if (_modelName) delete _modelName;
	if (_serialNumber) delete _serialNumber;
	if (_identify) delete _identify;

	for (std::vector<HAPService *>::iterator it = _services.begin(); it != _services.end(); it++) {
		for (std::vector<HAPCharacteristic *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			
			struct HAPEvent event = HAPEvent(NULL, aid, (*jt)->iid, "");
			HAPServer::_eventManager.queueEvent( EventManager::kEventIncrementConfigNumber, event);					
			(*it)->_characteristics.erase(jt);			                        
		}
		_services.erase(it);
	}

}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::addService(HAPService *ser) {
	ser->serviceID = ++numberOfInstance;
	_services.push_back(ser);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::addCharacteristics(HAPService *ser, HAPCharacteristic *cha) {
	cha->iid = ++numberOfInstance;
	ser->_characteristics.push_back(cha);

	// ToDo: Add event for update mdns config
	
	struct HAPEvent event = HAPEvent(NULL, aid, cha->iid, "");					
	HAPServer::_eventManager.queueEvent( EventManager::kEventIncrementConfigNumber, event);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPAccessory::removeService(HAPService *ser) {
	bool exist = false;
	for (std::vector<HAPService *>::iterator it = _services.begin(); it != _services.end(); it++) {
		if (*it == ser) {
			_services.erase(it);
			exist = true;
		}
	}
	return exist;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
bool HAPAccessory::removeCharacteristics(HAPCharacteristic *cha) {
	bool exist = false;
	for (std::vector<HAPService *>::iterator it = _services.begin(); it != _services.end(); it++) {
		for (std::vector<HAPCharacteristic *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if (*jt == cha) {
				(*it)->_characteristics.erase(jt);
				exist = true;

				// ToDo: Add event for update mdns config                                                     
				struct HAPEvent event = HAPEvent(NULL, aid, (*jt)->iid, "");					
				HAPServer::_eventManager.queueEvent( EventManager::kEventIncrementConfigNumber, event);
			}
		}
	}
	return exist;
}



uint8_t HAPAccessory::numberOfService() const { 
	return _services.size(); 
}


HAPService *HAPAccessory::serviceAtIndex(uint8_t index) {
	return _services[index];
}

HAPCharacteristic *HAPAccessory::characteristicsAtIndex(uint8_t index) {
	for (std::vector<HAPService *>::iterator it = _services.begin(); it != _services.end(); it++) {
		for (std::vector<HAPCharacteristic *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if ((*jt)->iid == index) {
				return *jt;
			}
		}
	}
	return NULL;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPCharacteristic *HAPAccessory::characteristicsOfType(int type) {
	for (std::vector<HAPService *>::iterator it = _services.begin(); it != _services.end(); it++) {
		for (std::vector<HAPCharacteristic *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if ((*jt)->type == type) {
				return *jt;
			}
		}
	}
	return NULL;
}


// void HAPAccessory::toJson(JsonArray& array){

// 	JsonObject nested = array.createNestedObject();
// 	nested["aid"] = aid;
	
// 	JsonArray servicesArray = nested.createNestedArray("services");
// 	for (int i = 0; i < numberOfService(); i++) {
// 		// services[i] = _services[i]->describe();
// 		_services[i]->toJson(servicesArray);
// 	}        
// }

void HAPAccessory::printTo(Print& print){
	print.print("{");
	
	// aid
	print.print("\"aid\":");
	print.print(aid);    
	print.print(",");

	// services
	print.print("\"services\":[");    
	for (int i = 0; i < numberOfService(); i++) {
		_services[i]->printTo(print);

		if (i+1 < numberOfService()){
			print.print(",");
		}
	}
	print.print("]}");
}


// String HAPAccessory::describe() const {

//     String keys[2];
//     String values[2];
	
//     {
//         keys[0] = "aid";
//         char temp[8];
//         sprintf(temp, "%d", aid);
//         values[0] = temp;
//     }
	
//     {
//         //Form services list
//         int noOfService = numberOfService();
//         String *services = new String[noOfService];
//         for (int i = 0; i < noOfService; i++) {
//             services[i] = _services[i]->describe();
//         }
//         keys[1] = "services";
//         values[1] = HAPHelper::arrayWrap(services, noOfService);
//         delete [] services;
//     }
	
	
//     return HAPHelper::dictionaryWrap(keys, values, 2);
// }



#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPService* HAPAccessory::addInfoService(const String& accessoryName, const String& manufactuerName, const String& modelName, const String& serialNumber, identifyFunctionCallback callback, const String& firmwareRev){
	
	setName(accessoryName);
	
	setIdentifyCallback(callback);

	setManufacturer(manufactuerName);

	setModelName(modelName);

	setSerialNumber(serialNumber);
	
	if ( firmwareRev != "" ) {
		setFirmware(firmwareRev);
	}
	
	return _infoService;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initInfoService(){
	if (_infoService == nullptr) {
		_infoService = new HAPService(HAP_SERVICE_ACCESSORY_INFORMATION);
		addService(_infoService);    
	}    
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initAccessoryName(){
	initInfoService();
	if (_accessoryName == nullptr) {
		_accessoryName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);            
		addCharacteristics(_infoService, _accessoryName);
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initFirmware(){
	initInfoService();
	if (_firmware == nullptr) {
		_firmware = new HAPCharacteristicString(HAP_CHARACTERISTIC_FIRMWARE_REVISION, permission_read);            
		addCharacteristics(_infoService, _firmware);
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initManufacturer(){
	initInfoService();
	if (_manufacturer == nullptr) {
		_manufacturer = new HAPCharacteristicString(HAP_CHARACTERISTIC_MANUFACTURER, permission_read);            
		addCharacteristics(_infoService, _manufacturer);
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initModelName(){
	initInfoService();
	if (_modelName == nullptr) {
		_modelName = new HAPCharacteristicString(HAP_CHARACTERISTIC_MODEL, permission_read);            
		addCharacteristics(_infoService, _modelName);
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initSerialNumber(){
	initInfoService();
	if (_serialNumber == nullptr) {
		_serialNumber = new HAPCharacteristicString(HAP_CHARACTERISTIC_SERIAL_NUMBER, permission_read);            
		addCharacteristics(_infoService, _serialNumber);
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::initIdentify(){
	initInfoService();
	if (_identify == nullptr) {
		_identify = new HAPCharacteristicBool(HAP_CHARACTERISTIC_IDENTIFY, permission_write);            
		addCharacteristics(_infoService, _identify);
	}
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::setName(const String& name){
	initAccessoryName();
	_accessoryName->setValueString(name);
}

String HAPAccessory::name(){
	if (_accessoryName == nullptr) {
		return "";    
	}
	return _accessoryName->valueString();
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::setFirmware(const String& firmware){
	initFirmware();
	_firmware->setValueString(firmware);
}


String HAPAccessory::firmware(){
	if (_firmware == nullptr) {
		return "";    
	}
	return _firmware->valueString();
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::setManufacturer(const String& manufacturer){
	initManufacturer();
	_manufacturer->setValueString(manufacturer);
}


String HAPAccessory::manufacturer(){
	if (_manufacturer == nullptr) {
		return "";    
	}
	return _manufacturer->valueString();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::setModelName(const String& modelName){
	initModelName();
	_modelName->setValueString(modelName);
}

String HAPAccessory::modelName(){
	if (_modelName == nullptr) {
		return "";    
	}
	return _modelName->valueString();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::setSerialNumber(const String& serialNumber){
	initSerialNumber();
	_serialNumber->setValueString(serialNumber);
}

String HAPAccessory::serialNumber(){
	if (_serialNumber == nullptr) {
		return "";    
	}
	return _serialNumber->valueString();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPAccessory::setIdentifyCallback(identifyFunctionCallback callback){
	initIdentify();
	_identify->valueChangeFunctionCall = callback;
}