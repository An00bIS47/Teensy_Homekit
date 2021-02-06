// HAPCharacteristicsUInt32.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICUINT32_HPP_
#define HAPCHARACTERISTICUINT32_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicUInt32 : public HAPCharacteristic {
public:
    uint32_t _value;
    const uint32_t _minVal, _maxVal, _step;
    const unit _unit;
    
    // void (*valueChangeFunctionCall)(int oldValue, int newValue) = NULL;
    std::function<void(uint32_t, uint32_t)> valueChangeFunctionCall = NULL;

    HAPCharacteristicUInt32(uint16_t _type, uint8_t _permission, uint32_t minVal, uint32_t maxVal, uint32_t step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }

    HAPCharacteristicUInt32(const char* _type, uint8_t _permission, uint32_t minVal, uint32_t maxVal, uint32_t step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }


    inline String value() override{
        if (valueGetFunctionCall)
            valueGetFunctionCall();
        char temp[32];
        snprintf(temp, 32, "%" PRIu32, _value);
        return String(temp);
    }

    inline void setValue(const String& str) override {
        uint32_t temp = atoi(str.c_str());
        
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
            root["format"] = "uint32"; 
        }
    }

};

#endif /* HAPCHARACTERISTICUINT32_HPP_ */