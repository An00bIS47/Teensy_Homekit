// HAPCharacteristicsUInt64.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICUINT64_HPP_
#define HAPCHARACTERISTICUINT64_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicUInt64 : public HAPCharacteristic {
public:
    uint64_t _value;
    const uint64_t _minVal, _maxVal, _step;
    const unit _unit;
    
    // void (*valueChangeFunctionCall)(int oldValue, int newValue) = NULL;
    std::function<void(uint64_t, uint64_t)> valueChangeFunctionCall = NULL;

    HAPCharacteristicUInt64(uint16_t _type, uint8_t _permission, uint64_t minVal, uint64_t maxVal, uint64_t step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }

    HAPCharacteristicUInt64(const char* _type, uint8_t _permission, uint64_t minVal, uint64_t maxVal, uint64_t step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }

    inline String value() override {
        if (valueGetFunctionCall)
            valueGetFunctionCall();
            
        char temp[64];
        snprintf(temp, 64, "%" PRIu64, _value);
        return String(temp);
    }

    inline void setValue(const String& str) override {
        uint64_t temp = atoi(str.c_str());
        
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, temp);
        
        _value = temp;        
    }

    inline void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) override {
        if (readable()) {
            root["value"] = _value; 
        }
        
        if (meta){
            root["minValue"] = _minVal;
            root["maxValue"] = _maxVal;
            root["minStep"] = _step;
            if (_unit != unit_none) {
                root["unit"] = unitJson(_unit);                    
            }
            root["format"] = "uint64"; 
        }
    }

};

#endif /* HAPCHARACTERISTICUINT64_HPP_ */