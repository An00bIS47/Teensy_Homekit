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

class HAPService {
public:

    HAPService();
    HAPService(uint8_t _uuid);
    HAPService(const String& _uuid);

    uint16_t aid() { return _aid; }
    void setAID(uint16_t aid) { _aid = aid; }

    // String describe();
    void printTo(Print& print);

    // void toJson(JsonArray& array) __attribute__ ((deprecated));
    // void toJson(JsonObject& nested) __attribute__ ((deprecated));
    // std::vector<HAPCharacteristic*> _characteristics;
    
    bool isHidden() { return (_features && 0x01); }
    void setHidden(bool mode = true) { _features = mode << 0; }

    bool isPrimary() { return (_features && 0x02); }
    void setPrimary(bool mode = true){ _features = mode << 1; }

    void addLinkedServiceId(uint8_t serviceId_){
        _linkedServiceIds.push_back(serviceId_);
    }

#if defined(ARDUINO_TEENSY41)
    FLASHMEM 
#endif
    size_t numberOfCharacteristics() { return _characteristics.size(); }

    template <class T>
#if defined(ARDUINO_TEENSY41)
    FLASHMEM 
#endif
	std::shared_ptr<HAPCharacteristicT<T>*> characteristicAtIndex(size_t index) {
        _characteristics[index];
    }

	void* operator new(size_t size);
    void operator delete(void* ptr);


protected:
    uint16_t    _aid;
    uint8_t     _uuid;
    String      _uuidString;
    

    uint8_t     _features; // hidden == 0x01, primary == 0x02
    // bool        _hidden;
    // bool        _primary;

    std::vector<std::shared_ptr<HAPCharacteristicBase*>> _characteristics;
    std::vector<uint8_t> _linkedServiceIds;

};


#endif /* HAPSERVICE_HPP_ */