// HAPCharacteristicInt.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICINT_HPP_
#define HAPCHARACTERISTICINT_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"


class HAPCharacteristicInt : public HAPCharacteristic {
public:
    int32_t _value;
    const int32_t _minVal, _maxVal, _step;
    const unit _unit;
    
    // void (*valueChangeFunctionCall)(int oldValue, int newValue) = NULL;
    std::function<void(int32_t, int32_t)> valueChangeFunctionCall = NULL;

    HAPCharacteristicInt(uint16_t _type, uint8_t _permission, int minVal, int maxVal, int step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }

    HAPCharacteristicInt(const char* _type, uint8_t _permission, int minVal, int maxVal, int step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }


    virtual String value() override {
        if (valueGetFunctionCall)
            valueGetFunctionCall();
            
        char temp[16];
        snprintf(temp, 16, "%" PRId32, _value);
        return String(temp);
    }

    virtual void setValue(const String& str) override {
        int32_t temp = atoi(str.c_str());
        
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, temp);

        _value = temp;
        
    }


    void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) override {
        
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
            root["format"] = "int";
        }
    }
};

#endif /* HAPCHARACTERISTICINT_HPP_ */