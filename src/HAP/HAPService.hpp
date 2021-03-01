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

#include "HAPCharacteristic.hpp"

class HAPService {
public:

    HAPService();
    HAPService(uint8_t _uuid);
    HAPService(const String& _uuid);

    // String describe();
    void printTo(Print& print);

    void toJson(JsonArray& array) __attribute__ ((deprecated));
    void toJson(JsonObject& nested) __attribute__ ((deprecated));

    uint8_t serviceID;
    uint8_t uuid;
    String uuidString;
    
    bool hidden;
    bool primary;

    std::vector<HAPCharacteristic *> _characteristics;
    std::vector<uint8_t> _linkedServiceIds;

    void setHiddenService(bool mode = true){
        hidden = mode;
    }

    void setPrimaryService(bool mode = true){
        primary = mode;
    }

    void addLinkedServiceId(uint8_t serviceId_){
        _linkedServiceIds.push_back(serviceId_);
    }

    uint8_t numberOfCharacteristics() { return _characteristics.size(); }
    HAPCharacteristic *characteristicsAtIndex(uint8_t index) { return _characteristics[index]; }
    
};


#endif /* HAPSERVICE_HPP_ */