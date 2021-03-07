// HAPCharacteristicsUInt16.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICUINT16_HPP_
#define HAPCHARACTERISTICUINT16_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicUInt16 : public HAPCharacteristic {
public:
    uint16_t _value;
    const uint16_t _minVal, _maxVal, _step;
    const unit _unit;
    
    // void (*valueChangeFunctionCall)(int oldValue, int newValue) = NULL;
    std::function<void(uint16_t, uint16_t)> valueChangeFunctionCall = NULL;

    HAPCharacteristicUInt16(uint16_t _type, uint8_t _permission, uint16_t minVal, uint16_t maxVal, uint16_t step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }

    HAPCharacteristicUInt16(const char* _type, uint8_t _permission, uint16_t minVal, uint16_t maxVal, uint16_t step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }

    uint16_t value(bool withCallback = true){
        if (valueGetFunctionCall && withCallback)
            valueGetFunctionCall();

        return _value;
    }

    void setValue(const uint16_t value, bool withCallback = true){
        uint16_t temp = _value;
        if (valueChangeFunctionCall && withCallback) {
            valueChangeFunctionCall(_value, value);
        }

        if ( withCallback ) {
            if (temp == _value) {
                _value = value;
            } else {
                _value = temp;
            }            
        } else  {
            _value = value;
        }
    }


    String valueString() override{
        if (valueGetFunctionCall)
            valueGetFunctionCall();
        char temp[16];
        snprintf(temp, 16, "%" PRIu16, _value);
        return String(temp);
    }

    void setValueString(const String& str) override {
        uint16_t temp = atoi(str.c_str());
        
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
            root["format"] = "uint16"; 
        }
    }

};

#endif /* HAPCHARACTERISTICUINT16_HPP_ */