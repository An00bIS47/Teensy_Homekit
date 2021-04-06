// 
// HAPFakegato.hpp
// Teensy_Homekit
//
//  Created on: 31.03.2021
//      Author: michael
//

#ifndef HAPFakegato_HPP_
#define HAPFakegato_HPP_

#include <Arduino.h>
#include <functional>
#include <vector>
#include <cstdarg>
#include <CircularBuffer.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include "HAPFakegatoCharacteristic.hpp"

#ifndef HAP_FAKEGATO_BUFFER_SIZE
#define HAP_FAKEGATO_BUFFER_SIZE 100
#endif

#ifndef HAP_FAKEGATO_BATCH_SIZE
#define HAP_FAKEGATO_BATCH_SIZE  16
#endif

#define HAP_FAKEGATO_EPOCH       978307200

#ifndef HAP_FAKEGATO_CHUNK_BUFFER_SIZE
#define HAP_FAKEGATO_CHUNK_BUFFER_SIZE      512     // base64 256 bits = 344
#endif

#define HAP_FAKEGATO_TYPE_REFTIME       0x81

#define HAP_SERVICE_FAKEGATO_ENDING                             "-079E-48FF-8F27-9C2605A29F52"

// 
// Services
// 
#define HAP_SERVICE_FAKEGATO_WEATHER                            "E863F001-079E-48FF-8F27-9C2605A29F52"  // 001
#define HAP_SERVICE_FAKEGATO_HISTORY                            "E863F007-079E-48FF-8F27-9C2605A29F52"  // 007
#define HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR                "E863F00A-079E-48FF-8F27-9C2605A29F52"  // 00A

// 
// Characteristics
// 
#define HAP_CHARACTERISTIC_FAKEGATO_VOLTAGE                     "E863F10A-079E-48FF-8F27-9C2605A29F52"  // 10A
#define HAP_CHARACTERISTIC_FAKEGATO_AIR_PARTICULATE_DENSITY     "E863F10B-079E-48FF-8F27-9C2605A29F52"  // 10B
#define HAP_CHARACTERISTIC_FAKEGATO_TOTAL_CONSUMPTION           "E863F10C-079E-48FF-8F27-9C2605A29F52"  // 10C
#define HAP_CHARACTERISTIC_FAKEGATO_CURRENT_CONSUMPTION         "E863F10D-079E-48FF-8F27-9C2605A29F52"  // 10D
#define HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE                "E863F10F-079E-48FF-8F27-9C2605A29F52"  // 10F
#define HAP_CHARACTERISTIC_FAKEGATO_OPEN_DURATION               "E863F118-079E-48FF-8F27-9C2605A29F52"  // 118
#define HAP_CHARACTERISTIC_FAKEGATO_CLOSED_DURATION             "E863F119-079E-48FF-8F27-9C2605A29F52"  // 119
#define HAP_CHARACTERISTIC_FAKEGATO_LAST_ACTIVATION             "E863F11A-079E-48FF-8F27-9C2605A29F52"  // 11A
#define HAP_CHARACTERISTIC_FAKEGATO_SENSITIVITY                 "E863F120-079E-48FF-8F27-9C2605A29F52"  // 120
#define HAP_CHARACTERISTIC_FAKEGATO_ELECTRIC_CURRENT            "E863F126-079E-48FF-8F27-9C2605A29F52"  // 126
#define HAP_CHARACTERISTIC_FAKEGATO_kVAh                        "E863F127-079E-48FF-8F27-9C2605A29F52"  // 127
#define HAP_CHARACTERISTIC_FAKEGATO_TIMES_OPENED                "E863F129-079E-48FF-8F27-9C2605A29F52"  // 129
#define HAP_CHARACTERISTIC_FAKEGATO_PROGRAM_COMMAND             "E863F12C-079E-48FF-8F27-9C2605A29F52"  // 12C
#define HAP_CHARACTERISTIC_FAKEGATO_DURATION                    "E863F12D-079E-48FF-8F27-9C2605A29F52"  // 12D
#define HAP_CHARACTERISTIC_FAKEGATO_VALVE_POSITION              "E863F12E-079E-48FF-8F27-9C2605A29F52"  // 12E
#define HAP_CHARACTERISTIC_FAKEGATO_PROGRAM_DATA                "E863F12E-079E-48FF-8F27-9C2605A29F52"  // 12E
#define HAP_CHARACTERISTIC_FAKEGATO_ELEVATION                   "E863F130-079E-48FF-8F27-9C2605A29F52"  // 130

// Additional Characteristics
#define HAP_CHARACTERISTIC_FAKEGATO_CLOUDS                      "64392FED-1401-4F7A-9ADB-1710DD6E3897"  // unit: percentage     uint8   pr ev       minVal: 0       maxVal: 100     step 1
#define HAP_CHARACTERISTIC_FAKEGATO_COLOR_TEMPERATURE           "E887EF67-509A-552D-A138-3DA215050F46"  // unit: mired          int     pr pw ev
#define HAP_CHARACTERISTIC_FAKEGATO_COLOR_TEMPERATURE_K         "A18E5901-CFA1-4D37-A10F-0071CEEEEEBD"  // unit: K              int     pr pw ev    minVal: 2000    maxVal: 6536    step 1
#define HAP_CHARACTERISTIC_FAKEGATO_CONDITION                   "CD65A9AB-85AD-494A-B2BD-2F380084134D"  // unit:                string  pr ev
#define HAP_CHARACTERISTIC_FAKEGATO_CONDITION_CATEGORY          "CD65A9AB-85AD-494A-B2BD-2F380084134C"  // unit:                uint16  pr pw ev    minVal: 0       maxVal: 1000    step 1
#define HAP_CHARACTERISTIC_FAKEGATO_DAY                         "57F1D4B2-0E7E-4307-95B5-808750E2C1C7"  // unit:                string  pr ev
#define HAP_CHARACTERISTIC_FAKEGATO_DEW_POINT                   "095C46E2-278E-4E3C-B9E7-364622A0F501"  // unit: C              float   pr ev       minVal: -40     maxVal: 100     step 0.1
#define HAP_CHARACTERISTIC_FAKEGATO_MAX_WIND_SPEED              "6B8861E5-D6F3-425C-83B6-069945FFD1F1"  // unit: km/h           float   pr ev       minVal: 0       maxVal: 100     step 0.1
#define HAP_CHARACTERISTIC_FAKEGATO_MIN_TEMPERATURE             "707B78CA-51AB-4DC9-8630-80A58F07E419"  // unit: C              float   pr ev       minVal: -40     maxVal: 100     step 0.1
#define HAP_CHARACTERISTIC_FAKEGATO_OBSERVATION_TIME            "234FD9F1-1D33-4128-B622-D052F0C402AF"  // unit:                string  pr ev
#define HAP_CHARACTERISTIC_FAKEGATO_OZONE                       "BBEFFDDD-1BCD-4D75-B7CD-B57A90A04D13"  // unit: DU             uint16  pr ev       minVal: 0       maxVal: 500     step 1
#define HAP_CHARACTERISTIC_FAKEGATO_RAIN_LAST_HOUR              "10C88F40-7EC4-478C-8D5A-BD0C3CCE14B7"  // unit: mm             uint16  pr ev       minVal: 0       maxVal: 1000    step 1
#define HAP_CHARACTERISTIC_FAKEGATO_RAIN_TOTAL                  "CCC04890-565B-4376-B39A-3113341D9E0F"  // unit: mm             uint16  pr ev       minVal: 0       maxVal: 1000    step 1
#define HAP_CHARACTERISTIC_FAKEGATO_RAIN_PROBABILITY            "FC01B24F-CF7E-4A74-90DB-1B427AF1FFA3"  // unit: percentage     uint8   pr ev       minVal: 0       maxVal: 100     step 1
#define HAP_CHARACTERISTIC_FAKEGATO_SNOW                        "F14EB1AD-E000-4CE6-BD0E-384F9EC4D5DD"  // unit:                bool    pr ev
#define HAP_CHARACTERISTIC_FAKEGATO_UV_INDEX                    "05BA0FE0-B848-4226-906D-5B64272E05CE"  // unit:                uint8   pr ev       minVal: 0       maxVal: 10      step 1
#define HAP_CHARACTERISTIC_FAKEGATO_VISIBILITY                  "D24ECC1E-6FAD-4FB5-8137-5AF88BD5E857"  // unit: km             uint8   pr ev       minVal: 0       maxVal: 100     step 1
#define HAP_CHARACTERISTIC_FAKEGATO_WIND_DIRECTION              "46F1284C-1912-421B-82F5-EB75008B167E"  // unit:                string  pr ev
#define HAP_CHARACTERISTIC_FAKEGATO_WIND_SPEED                  "49C8AE5A-A3A5-41AB-BF1F-12D5654F9F41"  // unit: km/h           float   pr ev       minVal: 0       maxVal: 100     step 0.1

// 
// Characteristics Common
// 
#define HAP_CHARACTERISTIC_FAKEGATO_RESET_TOTAL                 "E863F112-079E-48FF-8F27-9C2605A29F52"  // 112  => Reset Total
#define HAP_CHARACTERISTIC_FAKEGATO_FIRMWARE_UPDATE             "E863F11E-079E-48FF-8F27-9C2605A29F52"  // 11E  => Firmware Update ??

// 
// Characteristics History
// 
#define HAP_CHARACTERISTIC_FAKEGATO_SET_TIME                    "E863F121-079E-48FF-8F27-9C2605A29F52"  // 121  => S2W2
#define HAP_CHARACTERISTIC_FAKEGATO_HISTORY_REQUEST             "E863F11C-079E-48FF-8F27-9C2605A29F52"  // 11C  => S2W1
#define HAP_CHARACTERISTIC_FAKEGATO_HISTORY_STATUS              "E863F116-079E-48FF-8F27-9C2605A29F52"  // 116  => S2R1
#define HAP_CHARACTERISTIC_FAKEGATO_HISTORY_ENTRIES             "E863F117-079E-48FF-8F27-9C2605A29F52"  // 117  => S2R2

// 
// Characteristics Schedule
// 
#define HAP_CHARACTERISTIC_FAKEGATO_CONFIG_READ                 "E863F131-079E-48FF-8F27-9C2605A29F52"  // 131  => Config Read
#define HAP_CHARACTERISTIC_FAKEGATO_CONFIG_WRITE                "E863F11D-079E-48FF-8F27-9C2605A29F52"  // 11D  => Config Write



enum HAP_FAKEGATO_TYPE {
    HAP_FAKEGATO_TYPE_NONE      = 0x00,
    HAP_FAKEGATO_TYPE_WEATHER,
    HAP_FAKEGATO_TYPE_CUSTOM   
};

class HAPFakegato {
public:
    HAPFakegato();
    virtual ~HAPFakegato();

    virtual void begin() {};

    void registerFakeGatoService(HAPAccessory* accessory, const String& name, bool withSchedule = false);

    void addEntry(uint8_t bitmask);
    
    String name() { return _name; }

    uint32_t timestampLastEntry(){
        return _entries[_entries.size() - 1]->timestamp;
    }

    void setInterval(uint32_t interval){
		_interval = interval;
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


    void registerCallbackAddEntry(std::function<bool()> callback){
        _callbackAddEntry = callback;
    }

    bool isEnabled(){
		return _isEnabled;
	}

	void enable(bool mode){
		_isEnabled = mode;
	}

    void handle(bool forced = false);


    void getSignature(uint8_t* bytes){
        
        uint8_t offset = 0;
        for (auto &chr : _signatures){            
            uint8_t len = 0;
            chr->getSignatureBytes(bytes + offset, &len);
            offset += len;
        }
    }


    uint8_t getMaxEntryValueLength(){
        uint8_t bitmask = 0;
        for (uint8_t i=0; i < _signatures.size(); i++){
            bitmask += (1 << i);
        }        
        return getEntryValueLength(bitmask);
    }

    uint8_t getEntryValueLength(uint8_t bitmask){
        uint8_t length = 0;
        for (uint8_t i=0; i < _signatures.size(); i++){
            if (bitmask && (1 << i)){                        
                length += _signatures[i]->valueLength();
            }
        }
        return length;    
    }

    uint8_t valueLength(){
        uint8_t length = 0;
        for (auto &chr : _signatures){
            length += chr->valueLength();
        }
        return length;
    }

    uint8_t signatureLength(){
        return (_signatures.size() * 2);
    }    

    void enablePeriodicUpdates(bool mode = true){
        _periodicUpdates = mode;
    }


    void addCharacteristic(HAPFakegatoCharacteristic* characteristic){
        _signatures.emplace_back(std::move(characteristic));
    }

protected:

    struct HAPFakegatoDataEntry {
        uint8_t bitmask = 0;
        uint32_t timestamp = 0;
        uint8_t* data = nullptr;
        uint8_t length = 0;

        HAPFakegatoDataEntry(){

        }

        HAPFakegatoDataEntry(uint8_t bitmask_, uint32_t timestamp_, uint8_t* data_, uint8_t length_)
        : bitmask(bitmask_)
        , timestamp(timestamp_)
        , length(length_)
        {
            data = (uint8_t*) malloc(sizeof(uint8_t) * length_);
            memcpy(data, data_, length_);
        }

        ~HAPFakegatoDataEntry(){
            if (data) free(data);
        }

#if HAP_DEBUG
        void printTo(Print& prt){
            prt.print(F("Entry:"));
            prt.print(F(" bitmask:")); prt.print(bitmask);
            prt.print(F(" timestamp:")); prt.print(timestamp);
            prt.println("");
            HAPHelper::array_print("data", data, length);
        }

#endif
    };

    bool shouldHandle();

    virtual void addDataToBuffer(uint8_t bitmask, uint8_t* data, uint8_t length);

    virtual void callbackHistorySetTime(String oldValue, String newValue);
    virtual void callbackHistoryRequest(String oldValue, String newValue);

    virtual String callbackGetHistoryInfo();
    virtual String callbackGetHistoryEntries();

    void getRefTime(uint8_t* data, uint16_t* length);



    // FIXME: Change to uint8_t* data characteristic with base64 encoding inside of the charactersitic 
    //        Add this to a new file specific for data chars ? 
    HAPCharacteristicT<String>* _historyInfo    = nullptr;  // 116 // _s2r1Characteristics; 
    HAPCharacteristicT<String>* _historyEntries = nullptr;  // 117 // _s2r2Characteristics;
    HAPCharacteristicT<String>* _historyRequest = nullptr;  // 11C // _s2w1Characteristics;
    HAPCharacteristicT<String>* _historySetTime = nullptr;  // 121 // _s2w2Characteristics;



    // Schedules
    HAPCharacteristicT<String>* _configRead     = nullptr;
    HAPCharacteristicT<String>* _configWrite    = nullptr;
    
    virtual void scheduleRead(String oldValue, String newValue)     {}
    virtual void scheduleWrite(String oldValue, String newValue)    {}
    virtual String buildScheduleString() { return ""; }

    std::function<bool()> _callbackAddEntry = nullptr;

    String  _name;
    // uint8_t _signature[12] = {0, };
    // uint8_t _sigLength;
    // uint8_t _fakegatoType = 0;
    size_t  _requestedIndex = 0;    
    
    uint32_t _previousMillis = 0;
    uint32_t _interval = HAP_FAKEGATO_INTERVAL;

    bool    _isTimeSource = false;
    bool    _rolledOver = false;
    bool    _isEnabled = true;
    bool    _periodicUpdates = true;

    bool    _transfer = false;    

    std::vector< std::unique_ptr<HAPFakegatoCharacteristic> > _signatures;
    CircularBuffer<HAPFakegatoDataEntry*, HAP_FAKEGATO_BUFFER_SIZE> _entries;
};


#endif /* HAPFakegato_HPP_ */