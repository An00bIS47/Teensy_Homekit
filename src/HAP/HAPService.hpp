//
// HAPService.hpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//

#ifndef HAPSERVICE_HPP_
#define HAPSERVICE_HPP_

#include <Arduino.h>
#include <vector>
#include <memory>
#include "HAPCharacteristic.hpp"
#include "HAPServiceType.hpp"

class HAPService {
public:

    HAPService();
    HAPService(HAPServiceType uuid);
    HAPService(const char* uuid);


    uint16_t uuidAsInt(){ return static_cast<uint16_t>(_uuid); }

    uint32_t aid() { return _aid; }
    void setAID(uint32_t aid) { _aid = aid; }

    // String describe();
    void printTo(Print& print);

    // void toJson(JsonArray& array) __attribute__ ((deprecated));
    // void toJson(JsonObject& nested) __attribute__ ((deprecated));
    // std::vector<HAPCharacteristic*> _characteristics;

    bool isHidden() { return (_features && 0x01); }
    void setHidden(bool mode = true) { _features = mode << 0; }

    bool isPrimary() { return (_features && 0x02); }
    void setPrimary(bool mode = true){ _features = mode << 1; }

    void addLinkedServiceId(uint32_t serviceId_){
        _linkedServiceIds.push_back(serviceId_);
    }

    // std::vector< std::unique_ptr< HAPCharacteristicBase > > characteristics(){
    //     return _characteristics;
    // }

#if defined(ARDUINO_TEENSY41)
    FLASHMEM
#endif
    size_t numberOfCharacteristics() { return _characteristics.size(); }


#if defined(ARDUINO_TEENSY41)
    FLASHMEM
#endif
	HAPCharacteristicBase* characteristicAtIndex(size_t index) {
        return _characteristics.at(index).get();
    }


#if defined(ARDUINO_TEENSY41)
    FLASHMEM
#endif
	HAPCharacteristicBase* characteristicWithIID(uint32_t iid) {
        for (size_t i=0; i < _characteristics.size(); i++){
            if (_characteristics[i]->iid() == iid){
                return characteristicAtIndex(i);
            }
        }
        return nullptr;
    }

	// void* operator new(size_t size);
    // void operator delete(void* ptr);

    std::vector<std::unique_ptr<HAPCharacteristicBase>> _characteristics;
protected:
    uint32_t        _aid;
    HAPServiceType  _uuid;
    std::string     _uuidString;


    uint8_t     _features; // hidden == 0x01, primary == 0x02
    // bool        _hidden;
    // bool        _primary;


    std::vector<uint32_t> _linkedServiceIds;

};


#endif /* HAPSERVICE_HPP_ */