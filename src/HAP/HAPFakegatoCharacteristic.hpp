// 
// HAPFakegatoCharacteristic.hpp
// Teensy_Homekit
//
//  Created on: 03.04.2021
//      Author: michael
//

#ifndef HAPFakegatoCharacteristic_HPP_
#define HAPFakegatoCharacteristic_HPP_

#include <Arduino.h>
#include <functional>
#include "HAPHelper.hpp"


union float_to_ui8 {
    float flt;
    uint8_t ui8[sizeof(float)];
};

union ui32_to_ui8 {
    uint32_t ui32;
    uint8_t ui8[4];
};

union ui16_to_ui8 {
    uint16_t ui16;
    uint8_t ui8[2];
};

const uint8_t type_size_map[] = {
    0,
    2,  // HAPFakegatoSignature_Temperature
    2,  // HAPFakegatoSignature_Humidity
    2,  // HAPFakegatoSignature_AirPressure
    2,  // HAPFakegatoSignature_AirQuality
    0,  // HAPFakegatoSignature_PowerApparent       // unknown
    1
};

enum HAPFakegatoSignature {
    HAPFakegatoSignature_Temperature                = 0x01,     // Length: 2  = temperature   x 100
    HAPFakegatoSignature_Humidity                   = 0x02,     // Length: 2  = humidity      x 100
    HAPFakegatoSignature_AirPressure                = 0x03,     // Length: 2  = air pressure  x 10
    HAPFakegatoSignature_AirQuality                 = 0x04,     // Length: 2  == PPM
    HAPFakegatoSignature_PowerApparent              = 0x05,
    HAPFakegatoSignature_Door                       = 0x06,     // Length: 1
    HAPFakegatoSignature_Power10thWh                = 0x07,     // Length: 2  = W             x 10
    HAPFakegatoSignature_WaterFlow                  = 0x08,
    HAPFakegatoSignature_WaterTemperature           = 0x09,
    HAPFakegatoSignature_WaterEnergy                = 0x0A, 
    HAPFakegatoSignature_PowerWatt                  = 0x0B,     // Length: 2
    HAPFakegatoSignature_PowerVoltage               = 0x0C,     // Length: 2  = volt          x 10
    HAPFakegatoSignature_PowerCurrent               = 0x0D,     // Length: 2
    HAPFakegatoSignature_PowerOnOff                 = 0x0E,     // Length: 1
    HAPFakegatoSignature_VOCHeatSense               = 0x0F,     // Length: 3
    HAPFakegatoSignature_ValvePercent               = 0x10,     // Length: 1
    HAPFakegatoSignature_TargetTemperature          = 0x11,     // Length: 2
    HAPFakegatoSignature_ThermoTarget               = 0x12,     // Length: 1 or current heating mode
    HAPFakegatoSignature_Motion                     = 0x13,
    HAPFakegatoSignature_Switch                     = 0x14,
    HAPFakegatoSignature_PowerOnOff2                = 0x15,
    HAPFakegatoSignature_SmokeDetected              = 0x16,
    HAPFakegatoSignature_CurrentPosition            = 0x17,
    HAPFakegatoSignature_TargetPosition             = 0x18,
    HAPFakegatoSignature_PositionState              = 0x19,
    HAPFakegatoSignature_ObstructionDetected        = 0x1A,
    HAPFakegatoSignature_SmokeDetectorStatus        = 0x1B,
    HAPFakegatoSignature_MotionActive               = 0x1C,     // Length: 1
    HAPFakegatoSignature_OpenWindow                 = 0x1D,     // Length: 1 or target heating mode
    // 1E unknown                                   = 0x1E,
    HAPFakegatoSignature_InUse                      = 0x1F,     // Length: 3 ??
    HAPFakegatoSignature_WindowState                = 0x20,
    HAPFakegatoSignature_PotState                   = 0x21,
    HAPFakegatoSignature_VOCDensity                 = 0x22,
    HAPFakegatoSignature_BatteryLevelMillivolts     = 0x23,     // Length: 2
    HAPFakegatoSignature_StatelessSwitchEvent       = 0x24,
    HAPFakegatoSignature_BatteryLevelPercent        = 0x25,
    HAPFakegatoSignature_Lock                       = 0x26,
    HAPFakegatoSignature_AirPressureChange          = 0x27,
    // unknown                                      = 0x28,     // Length: 8
};


/**
 * @brief 
 * 
 * 
 */
class HAPFakegatoCharacteristic {
public:
    HAPFakegatoCharacteristic(HAPFakegatoSignature sigType) {
        _type = sigType;
        _size = type_size_map[sigType];
    }

    ~HAPFakegatoCharacteristic() {

    }

    virtual uint8_t valueLength(){
        return _size;
    }

    virtual void getSignatureBytes(uint8_t* bytes, uint8_t* length) {
        bytes[0] = _type;
        bytes[1] = _size;
        *length = _size;
    }
    
    virtual void getBytes(uint8_t* bytes, uint8_t* length);
    
protected:    
    uint8_t _type;
    uint8_t _size;
};



/**
 * @brief HAPFakegatoCharacteristicBase
 * 
 */
template <class T>
class HAPFakegatoCharacteristicBase : public HAPFakegatoCharacteristic {
public:
    HAPFakegatoCharacteristicBase(std::function<T(void)> callback) : HAPFakegatoCharacteristic(HAPFakegatoSignature_Temperature) {
        _valueGetFunctionCall = callback;
    }

    HAPFakegatoCharacteristicBase(HAPFakegatoSignature type, std::function<T(void)> callback) : HAPFakegatoCharacteristic(type) {
        _valueGetFunctionCall = callback;
    }

    virtual T valueFromBytes(uint8_t* bytes, uint8_t length) = 0;
    virtual void bytesFromValue(T value, uint8_t* bytes, uint8_t* length) = 0;

    void setValueGetCallback(std::function<T(void)> callback){
        _valueGetFunctionCall = callback;
    }

    void getBytes(uint8_t* bytes, uint8_t* length) override {
        if (_valueGetFunctionCall != nullptr) {
            bytesFromValue(_valueGetFunctionCall(), bytes, length);
            return;
        }
        *length = 0;
        bytes = nullptr;
    }
    
protected:    
    
    std::function<T(void)> _valueGetFunctionCall = nullptr;
};

/**
 * @brief HAPFakegatoCharacteristicTemperature
 *  valueType: float
 *  size: 2
 *  calculation to fg: value * 100 
 *  calculation from fg: value / 100 
 * 
 */
class HAPFakegatoCharacteristicTemperature : public HAPFakegatoCharacteristicBase<float> {
public:
    HAPFakegatoCharacteristicTemperature(std::function<float(void)> callback) : HAPFakegatoCharacteristicBase<float>(HAPFakegatoSignature_Temperature, callback) {        
    }

    HAPFakegatoCharacteristicTemperature(HAPFakegatoSignature type, std::function<float(void)> callback) : HAPFakegatoCharacteristicBase<float>(type, callback) {        
    }

    float valueFromBytes(uint8_t* bytes, uint8_t length){
        ui16_to_ui8 converter;
        memcpy(converter.ui8, bytes, length);         
        return (float)HAPHelper::round2(converter.ui16 / 100);
    }


    void bytesFromValue(float value, uint8_t* bytes, uint8_t* length){
        // value = 23.4
        ui16_to_ui8 converter;
        converter.ui16 = HAPHelper::round2(value) * 100;
        memcpy(bytes, converter.ui8, 2);
        *length = 2;
    }
};


/**
 * @brief HAPFakegatoCharacteristicHumidity
 *  valueType: float
 *  size: 2
 *  calculation to fg: value * 100 
 *  calculation from fg: value / 100 
 * 
 */
class HAPFakegatoCharacteristicHumidity : public HAPFakegatoCharacteristicTemperature {
public:
    HAPFakegatoCharacteristicHumidity(std::function<float(void)> callback) : HAPFakegatoCharacteristicTemperature(HAPFakegatoSignature_Humidity, callback) {}
};

/**
 * @brief HAPFakegatoCharacteristicAirPressure
 *  valueType: float
 *  size: 2
 *  calculation to fg: value * 100 
 *  calculation from fg: value / 100 
 * 
 */
class HAPFakegatoCharacteristicAirPressure : public HAPFakegatoCharacteristicBase<uint16_t> {
public:
    HAPFakegatoCharacteristicAirPressure(std::function<uint16_t(void)> callback) : HAPFakegatoCharacteristicBase<uint16_t>(HAPFakegatoSignature_AirPressure, callback) {
    }

    HAPFakegatoCharacteristicAirPressure(HAPFakegatoSignature type, std::function<uint16_t(void)> callback) : HAPFakegatoCharacteristicBase<uint16_t>(type, callback) {
    }

    uint16_t valueFromBytes(uint8_t* bytes, uint8_t length){
        ui16_to_ui8 converter;
        memcpy(converter.ui8, bytes, length);         
        return (uint16_t)converter.ui16 / 10;
    }


    void bytesFromValue(uint16_t value, uint8_t* bytes, uint8_t* length){
        // value = 665
        ui16_to_ui8 converter;
        converter.ui16 = value * 10;
        memcpy(bytes, converter.ui8, 2);
        *length = 2;
    }
};


#endif /* HAPFakegatoCharacteristic_HPP_ */