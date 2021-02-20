// HAPCharacteristicsUInt8.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICUINT8_HPP_
#define HAPCHARACTERISTICUINT8_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicUInt8 : public HAPCharacteristic {
public:
    uint8_t _value;
    const uint8_t _minVal, _maxVal, _step;
    const unit _unit;
    uint8_t* _validValues;
    uint8_t _validValuesSize;
    
    // void (*valueChangeFunctionCall)(int oldValue, int newValue) = NULL;
    std::function<void(uint8_t, uint8_t)> valueChangeFunctionCall = NULL;

    HAPCharacteristicUInt8(uint16_t _type, uint8_t _permission, uint8_t minVal, uint8_t maxVal, uint8_t step, unit charUnit, uint8_t validValuesSize, uint8_t validValues[]): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;

        if (validValuesSize > 0){
            _validValues = (uint8_t*) malloc (validValuesSize * sizeof(uint8_t));
            memcpy(_validValues, validValues, validValuesSize);
            _validValuesSize = validValuesSize;
        } else {
            _validValues = NULL;
            _validValuesSize = 0;
        }           
    }

    HAPCharacteristicUInt8(const char* _type, uint8_t _permission, uint16_t minVal, uint16_t maxVal, uint16_t step, unit charUnit, uint8_t validValuesSize, uint8_t validValues[]): HAPCharacteristic(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;

        if (validValuesSize > 0){
            _validValues = (uint8_t*) malloc (validValuesSize * sizeof(uint8_t));
            memcpy(_validValues, validValues, validValuesSize);
            _validValuesSize = validValuesSize;
        } else {
            _validValues = NULL;
            _validValuesSize = 0;
        } 
    }

    ~HAPCharacteristicUInt8(){
        free(_validValues);
    }

    String valueString() override {
        if (valueGetFunctionCall)
            valueGetFunctionCall();
        char temp[16];
        snprintf(temp, 16, "%" PRIu8, _value);
        return String(temp);
    }

    void setValueString(const String& str) override {
        uint8_t temp = atoi(str.c_str());
        
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
            root["format"] = "uint8"; 

            if (_validValues != NULL){
                JsonArray validValuesJson = root.createNestedArray("valid-values");
                for (int i = 0; i < _validValuesSize; i++){
                    validValuesJson.add(_validValues[i]);
                }
            }
        }
    }
};

#endif /* HAPCHARACTERISTICUINT8_HPP_ */