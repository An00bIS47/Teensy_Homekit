// 
// HAPFakeGato2.hpp
// Teensy_Homekit
//
//  Created on: 31.03.2021
//      Author: michael
//

#ifndef HAPFAKEGATO2_HPP_
#define HAPFAKEGATO2_HPP_

#include <Arduino.h>
#include <CircularBuffer.h>
#include <cstdarg>
#include "HAPAccessory.hpp"

#define HAP_FAKEGATO_BUFFER_SIZE 100
#define HAP_FAKEGATO_BATCH_SIZE  16
#define HAP_FAKEGATO_EPOCH       978307200

enum HAP_FAKEGATO_TYPE {
    HAP_FAKEGATO_TYPE_NONE      = 0x00,
    HAP_FAKEGATO_TYPE_WEATHER,

    HAP_FAKEGATO_TYPE_CUSTOM   
};

class HAPFakeGato2 {
public:
    HAPFakeGato2();
    ~HAPFakeGato2();



    virtual void getData(uint8_t *data, size_t* length);

    // 
    // t == temperature - float     >> calculate value * 100
    // h == humidity    - float
    // p == airpressure - uint16_t
    // ...
    virtual void addEntry(uint8_t bitmask, const char* fmt, ...);

    uint32_t timestampLastEntry(){
        return _entries[size() - 1].timestamp;
    }

    size_t memoryUsed(){
        return _entries.size();
    }

    size_t capacity(){
        return _entries.capacity;
    }

    void setAsTimeSource(bool mode = true){
        _isTimeSource = mode;
    }

    void setType(enum HAP_FAKEGATO_TYPE fakegatoType){
        _fakegatoType = fakegatoType;
    }

    void setCallbackAddEntry(std::function<bool()> callback){
        _callbackAddEntry = callback;
    }

    void handle(bool forced);

protected:

    struct HAPFakegatoDataEntry {
        uint8_t bitmask = 0;
        uint32_t timestamp = 0;
        uint8_t* data = nullptr;
        uint8_t length = 0;

        HAPFakegatoDataEntry(uint8_t bitmask_, uint32_t timestamp_, uint8_t* data_, uint8_t length_)
        : bitmask(bitmask_)
        , timestamp(timestamp_)
        , length(length_)
        {
            data = (uint8_t*) malloc(sizeof(uint8_t) * length_);
            memcpy(data, data_, length);
        }

        ~HAPFakegatoDataEntry(){
            if (data) free(data);
        }
    };

    union ui32_to_ui8 {
        uint32_t ui32;
        uint8_t ui8[4];
    };

    union ui16_to_ui8 {
        uint16_t ui16;
        uint8_t ui8[2];
    };


    bool shouldHandle();

    virtual void addDataToBuffer(uint8_t bitmask, uint8_t* data, uint8_t length);

    virtual void callbackHistorySetTime(String oldValue, String newValue);
    virtual void callbackHistoryRequest(String oldValue, String newValue);

    virtual void updateHistoryInfo();


    // FIXME: Change to uint8_t* data characteristic with base64 encoding inside of the charactersitic 
    //        Add this to a new file specific for data chars ? 
    HAPCharacteristicT<String>* _historyInfo;    // 116 // _s2r1Characteristics; 
    HAPCharacteristicT<String>* _historyEntries; // 117 // _s2r2Characteristics;
    HAPCharacteristicT<String>* _historyRequest; // 11C // _s2w1Characteristics;
    HAPCharacteristicT<String>* _historySetTime; // 121 // _s2w2Characteristics;

    // Optional?? 
    // ToDo: Move to Energy ??
    HAPCharacteristicT<String>* _configRead;
    HAPCharacteristicT<String>* _configWrite;

    std::function<bool()> _callbackAddEntry = nullptr;

    uint8_t _signature[12] = {0, };
    uint8_t _fakegatoType = 0;
    size_t  _requestedIndex = 0;    
    
    uint32_t _previousMillis = 0;
    uint32_t _interval = HAP_FAKEGATO_INTERVAL;

    bool    _isTimeSource = false;
    bool    _rolledOver = false;
    bool    _isEnabled = true;
    bool    _periodicUpdates;

    CircularBuffer<HAPFakegatoDataEntry, HAP_FAKEGATO_BUFFER_SIZE> _entries;
};


#endif /* HAPFAKEGATO2_HPP_ */