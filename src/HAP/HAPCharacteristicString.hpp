// HAPCharacteristicString.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICSTRING_HPP_
#define HAPCHARACTERISTICSTRING_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicString : public HAPCharacteristic {
public:
    char* _value;
    const uint8_t maxLen;    
        
    // void (*valueChangeFunctionCall)(String oldValue, String newValue) = NULL;
    std::function<void(String, String)> valueChangeFunctionCall = NULL;

    HAPCharacteristicString(uint16_t _type, uint8_t _permission, uint8_t _maxLen = HAP_HOMEKIT_DEFAULT_STRING_LENGTH, const char* _desc = ""): HAPCharacteristic(_type, _permission), maxLen(_maxLen) {
        setDescription(_desc);
        _value = nullptr;    
    }

    HAPCharacteristicString(const char* _type, uint8_t _permission, uint8_t _maxLen = HAP_HOMEKIT_DEFAULT_STRING_LENGTH, const char* _desc = ""): HAPCharacteristic(_type, _permission), maxLen(_maxLen) {
        setDescription(_desc);
        _value = nullptr;
    }

    ~HAPCharacteristicString(){
        if (_value != nullptr) delete[] _value;
    }

    void clear() {
        if (_value != nullptr) delete[] _value;
        _value = nullptr;
    }

    String value() override {
        if (valueGetFunctionCall)
            valueGetFunctionCall();

        if (_value) {
            return String(_value);
        } 
        
        return "";         
    }

    void setValue(const String& str) override {
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(String(_value), str);
        
        uint16_t dataLen = str.length();

        if (dataLen > maxLen) return;        
        
        if (_value != nullptr) {
            delete[] _value;
        }

        _value = new char[dataLen + 1];
        // _valueLen = dataLen;
        strncpy(_value, str.c_str(), dataLen);
        _value[dataLen] = '\0';        
    }


    void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) override {
        
        if (readable()) {
            if (_value != nullptr){
                if (_value[0] == '\0') {
                    root["value"] = "";
                } else {
                    root["value"] = (const char*)_value;
                } 
            } else {
                root["value"] = "";
            }
        }
        
        if (meta){    
            if (maxLen > 0) {
                root["maxLen"] = maxLen;   
            }                
            root["format"] = "string";                 
        }
    }

};

#endif /* HAPCHARACTERISTICSTRING_HPP_ */
