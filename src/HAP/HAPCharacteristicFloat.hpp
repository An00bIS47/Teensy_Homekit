// HAPCharacteristicsFloat.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICFLOAT_HPP_
#define HAPCHARACTERISTICFLOAT_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicFloat: public HAPCharacteristic {
public:
    float _value;
    const float _minVal, _maxVal, _step;
    const unit _unit;
    
    // void (*valueChangeFunctionCall)(float oldValue, float newValue) = NULL;
    std::function<void(float, float)> valueChangeFunctionCall = NULL;

    HAPCharacteristicFloat(uint16_t _type, uint8_t _permission, float minVal, float maxVal, float step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) { _value = 0; }
    HAPCharacteristicFloat(const char* _type, uint8_t _permission, float minVal, float maxVal, float step, unit charUnit): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) { _value = 0; }
    

    float value(bool withCallback = true){
        if (valueGetFunctionCall && withCallback)
            valueGetFunctionCall();

        return _value;
    }
    
    virtual String valueString() override {
        if (valueGetFunctionCall)
            valueGetFunctionCall();
        
#if defined(ARDUINO_TEENSY41)
        char temp[64] = {0,};  
        snprintf(temp, 64, "%12.9lf", _value);
#else
        char temp[16] = {0,};  
        snprintf(temp, 16, "%.2f", _value);
#endif

        return String(temp);
    }
    
    virtual void setValueString(const String& str) override {
        float temp = atof(str.c_str());            
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, temp);


        _value = temp;
    
    }

    void setValue(const float value, bool withCallback = true){
        float temp = _value;
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


    void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) override {

        if (readable()) {
            root["value"] = HAPHelper::round2(_value);
        }
        
        if (meta){
            root["minValue"] = HAPHelper::round2(_minVal);
            root["maxValue"] = HAPHelper::round2(_maxVal);
            root["minStep"] = HAPHelper::round2(_step);
            if (_unit != unit_none) {
                root["unit"] = unitJson(_unit);                    
            }            
            root["format"] = "float";  
        }
    } 
};

#endif /* HAPCHARACTERISTICFLOAT_HPP_ */