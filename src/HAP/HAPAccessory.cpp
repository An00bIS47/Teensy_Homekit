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
#include "HAPCharacteristicType.hpp"
#include "HAPCharacteristic.hpp"
#include "HAPServiceType.hpp"


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory::HAPAccessory() {
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

	// for (std::vector<HAPService *>::iterator it = _services.begin(); it != _services.end(); it++) {
	// 	for (std::vector<HAPCharacteristic *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {

	// 		struct HAPEvent event = HAPEvent(NULL, aid, (*jt)->iid, "");
	// 		HAPServer::_eventManager.queueEvent( EventManager::kEventIncrementConfigNumber, event);
	// 		(*it->_characteristics->erase(jt);
	// 	}
	// 	_services.erase(it);
	// }
	_services.clear();

}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessory::addService(HAPService *service) {
	service->setAID(++_numberOfInstances);
	_services.emplace_back(std::move(service));
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessory::addCharacteristicToService(HAPService *service, HAPCharacteristicBase* characteristic) {

	characteristic->setIID(++_numberOfInstances);
	service->_characteristics.emplace_back(std::move(characteristic));

	// ToDo: Refactor Eventmanager
	struct HAPEvent event = HAPEvent(NULL, _aid, characteristic->iid());
	HAPServer::_eventManager.queueEvent( EventManager::kEventIncrementConfigNumber, event);
}

bool HAPAccessory::removeService(HAPService *service) {
	for (size_t i=0; i < _services.size(); i++){
		if (_services[i]->aid() == service->aid()){
			_services.erase(_services.begin() + i);
			return true;
		}
	}
	return false;
}



bool HAPAccessory::removeCharacteristicFromService(HAPService *service, HAPCharacteristicBase* characteristic) {
	for (size_t i=0; i < service->_characteristics.size(); i++){
		if (service->_characteristics[i]->iid() == characteristic->iid()){
			service->_characteristics.erase(service->_characteristics.begin() + i);
			return true;
		}
	}
	return false;
}



size_t HAPAccessory::numberOfServices() const {
	return _services.size();
}


HAPService* HAPAccessory::serviceAtIndex(size_t index) {
	return _services.at(index).get();
}


HAPCharacteristicBase* HAPAccessory::characteristicAtIndex(HAPService *service, size_t index) {
	return service->_characteristics.at(index).get();
}


HAPCharacteristicBase* HAPAccessory::characteristicWithIID(uint32_t iid) {
	for (size_t i=0; i < _services.size(); i++){
		HAPCharacteristicBase* chr = _services[i]->characteristicWithIID(iid);
		if (chr != nullptr){
			return chr;
		}
	}
	return nullptr;
}


HAPCharacteristicBase* HAPAccessory::characteristicsOfType(HAPCharacteristicType::Type type) {
	for (auto it = _services.begin(); it != _services.end(); it++) {
		for (auto jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if ((*jt)->type() == type) {
				return &(*jt->get());
			}
		}
	}
	return nullptr;
}

HAPCharacteristicBase* HAPAccessory::characteristicsOfType(const char* typeString){
	for (auto it = _services.begin(); it != _services.end(); it++) {
		for (auto jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if (strcmp((*jt)->typeString(), typeString) == 0) {
				return &(*jt->get());
			}
		}
	}
	return nullptr;
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
	print.print(_aid);
	print.print(",");

	// services
	print.print("\"services\":[");
	for (size_t i = 0; i < numberOfServices(); i++) {
		_services[i]->printTo(print);

		if (i+1 < numberOfServices()){
			print.print(",");
		}
	}
	print.print("]}");
}



#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPService* HAPAccessory::addInfoService(const char* accessoryName, const char* manufacturerName, const char* modelName, const char* serialNumber, identifyFunctionCallback callback, const char* firmwareRev){


	initInfoService();

	LOG_V("Set accessory name to %s\n", accessoryName);
	if (_accessoryName == nullptr) {
		_accessoryName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		addCharacteristicToService(_infoService, _accessoryName);
	}
	_accessoryName->setValue(accessoryName, false);

	LOG_V("Set manufacturer to %s\n", manufacturerName);
	if (_manufacturer == nullptr) {
		_manufacturer = new HAPCharacteristic<std::string>(HAPCharacteristicType::Manufacturer, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		addCharacteristicToService(_infoService, _manufacturer);
	}
	_manufacturer->setValue(manufacturerName, false);

	LOG_V("Set model to %s\n", modelName);
	if (_modelName == nullptr) {
		_modelName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Model, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		addCharacteristicToService(_infoService, _modelName);
	}
	_modelName->setValue(modelName, false);


	LOG_V("Set serialnumber to %s\n", serialNumber);
	if (_serialNumber == nullptr) {
		_serialNumber = new HAPCharacteristic<std::string>(HAPCharacteristicType::SerialNumber, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		addCharacteristicToService(_infoService, _serialNumber);
	}
	_serialNumber->setValue(serialNumber, false);

	setFirmware(firmwareRev);

	setIdentifyCallback(callback);

	return _infoService;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessory::initInfoService(){
	if (_infoService == nullptr) {
		_infoService = new HAPService(HAPServiceType::AccessoryInformation);
		addService(_infoService);
	}
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessory::setFirmware(const char* firmwareRev){
	initInfoService();

	LOG_V("Set firmware revision to %s\n", firmwareRev);
	if (_firmware == nullptr) {
		_firmware = new HAPCharacteristic<std::string>(HAPCharacteristicType::FirmwareRevision, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		addCharacteristicToService(_infoService, _firmware);
	}
	_firmware->setValue(firmwareRev, false);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessory::setIdentifyCallback(identifyFunctionCallback callback){
	initInfoService();
	if (_identify == nullptr) {
		_identify = new HAPCharacteristic<bool>(HAPCharacteristicType::Identify, HAPPermission::Write);
		addCharacteristicToService(_infoService, _identify);
	}
	_identify->setValueChangeCallback(callback);
}