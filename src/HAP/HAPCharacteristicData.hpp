// HAPCharacteristicData.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICDATA_HPP_
#define HAPCHARACTERISTICDATA_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

class HAPCharacteristicData: public HAPCharacteristic {
public:    
    const size_t maxDataLen;
    uint8_t* _value = nullptr;
    size_t _valueLen = 0;
    
    // void (*valueChangeFunctionCall)(String oldValue, String newValue) = NULL;
    std::function<void(String, String)> valueChangeFunctionCall = NULL;    

    HAPCharacteristicData(uint16_t _type, uint8_t _permission, size_t _maxDataLen, const char* _desc = ""): HAPCharacteristic(_type, _permission), maxDataLen(_maxDataLen) {
        setDescription(_desc);
        _value = nullptr;
    }

    HAPCharacteristicData(const char* _type, uint8_t _permission, size_t _maxDataLen, const char* _desc = ""): HAPCharacteristic(_type, _permission), maxDataLen(_maxDataLen) {
        setDescription(_desc);
        _value = nullptr;     
    }

    ~HAPCharacteristicData(){
        delete[] _value;
    }

    inline String value() override {  
        if (valueGetFunctionCall)
            valueGetFunctionCall(); 

        if (_value) {
            return String((char*)_value);
        } 
        
        return "";        
    }

    inline size_t setValueWithoutCallback(uint8_t* data, size_t dataLen){
        if (dataLen > maxDataLen) return 0;        
        
        if (_value != nullptr) {
            delete[] _value;
        }

        _value = new uint8_t[dataLen + 1];
        _valueLen = dataLen;
        memcpy(_value, data, dataLen);
        _value[_valueLen] = '\0'; 

        return dataLen;
    }

    inline void setValue(const String& str) override {        
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(String((char*)_value), str);
        
        setValueWithoutCallback((uint8_t*)str.c_str(), str.length());
        
        // // _value = str;
        // memcpy(_value, str.c_str(), str.length() > maxDataLen ? maxDataLen : str.length());
        // _value[str.length() > maxDataLen ? maxDataLen : str.length()] = '\0';
    }

    inline void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) override {
        
        if (readable()) {
            if (_value[0] == '\0') {
                root["value"] = (char*)NULL;
            } else {
                root["value"] = (const char*)_value;
            } 
        }
        
        if (meta){
            root["maxDataLen"] = maxDataLen;
            root["format"] = "data";  
        }
    }
};

#endif /* HAPCHARACTERISTICDATA_HPP_ */
