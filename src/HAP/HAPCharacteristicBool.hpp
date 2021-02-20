// HAPCharacteristicBool.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTICBOOL_HPP_
#define HAPCHARACTERISTICBOOL_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"
#include "HAPCharacteristic.hpp"

// Boolean value expressed as one of the following: true, false, 0 (false), and 1 (true).
class HAPCharacteristicBool: public HAPCharacteristic {
public:
    bool _value;
    
    //void (*valueChangeFunctionCall)(bool oldValue, bool newValue) = NULL;
    std::function<void(bool, bool)> valueChangeFunctionCall = NULL;

    HAPCharacteristicBool(uint16_t _type, uint8_t _permission): HAPCharacteristic(_type, _permission) { _value = false; }
    HAPCharacteristicBool(const char* _typeString, uint8_t _permission): HAPCharacteristic(_typeString, _permission) { _value = false; }    

    String valueString() override {
        if (valueGetFunctionCall)
            valueGetFunctionCall();
        if (_value)
            return "1";
        return "0";
    }

    void setValueString(const String& str) override {
        bool newValue = false;
        if ( str == "1") {
            newValue = true;
        }
        
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, newValue);
        _value = newValue;
    }
    
    void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) override {
        if (readable()) {
            root["value"] = (uint8_t)_value;     
        }

        if (meta){
            root["format"] = "bool";
        }
    }
};


#endif /* HAPCHARACTERISTICBOOL_HPP_ */