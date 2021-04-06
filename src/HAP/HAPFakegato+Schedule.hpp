// 
// HAPFakegato+Schedule.hpp
// Teensy_Homekit
//
//  Created on: 05.04.2021
//      Author: michael
//

#ifndef HAPFAKEGATOSCHEDULE_HPP_
#define HAPFAKEGATOSCHEDULE_HPP_

#include <functional>
#include "HAPFakegato.hpp"
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include <vector>
#include "HAPDailyTimerFactory.hpp"

// ToDo: define or enum ?
#define HAP_FAKEGATO_SCHEDULE_TYPE_SERIALNUMBER             0x04

#define HAP_FAKEGATO_SCHEDULE_TYPE_USED_MEMORY              0x06
#define HAP_FAKEGATO_SCHEDULE_TYPE_ROLLED_OVER_INDEX        0x07

#define HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_TOGGLE_SCHEDULE  0x44
#define HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_STATUS_LED       0x20
#define HAP_FAKEGATO_SCHEDULE_TYPE_PROGRAMS                 0x45
#define HAP_FAKEGATO_SCHEDULE_TYPE_DAYS                     0x46
#define HAP_FAKEGATO_SCHEDULE_TYPE_STATUS_LED               0x60


typedef enum {
    TIME = 0,
    SUN  = 1
} HAPFakeGatoScheduleTimerType;


typedef enum {  
    SUNSET  = 0,
    SUNRISE = 1
} HAPFakeGatoScheduleSunriseType;


struct HAPFakeGatoScheduleTimerEvent {
    HAPFakeGatoScheduleTimerType   type;

    uint8_t     hour;
    uint8_t     minute;
    int32_t     offset;

    bool        state;
    HAPFakeGatoScheduleSunriseType sunrise;    
};


struct HAPFakeGatoScheduleProgramEvent {
    uint8_t id;
    std::vector<HAPFakeGatoScheduleTimerEvent> timerEvents; // 15
};


struct HAPFakeGatoScheduleDays {

    uint8_t mon;
    uint8_t tue;
    uint8_t wed;
    uint8_t thu;
    uint8_t fri;
    uint8_t sat;
    uint8_t sun;

    HAPFakeGatoScheduleDays(){
        mon = 0;
        tue = 0;
        wed = 0;
        thu = 0;
        fri = 0;
        sat = 0;
        sun = 0;
    }

    // decode
    HAPFakeGatoScheduleDays(uint32_t daysnumber) {
        mon = daysnumber & 0x07;
        tue = ( daysnumber >>  3 ) & 0x07;
        wed = ( daysnumber >>  6 ) & 0x07;
        thu = ( daysnumber >>  9 ) & 0x07;
        fri = ( daysnumber >> 12 ) & 0x07;
        sat = ( daysnumber >> 15 ) & 0x07;
        sun = ( daysnumber >> 18 ) & 0x07;  
    }

    // encode
    uint32_t daysnumber(){
        uint32_t d = sun << 18 | sat << 15 | fri << 12 | thu << 9 | wed << 6 | tue << 3 | mon;
        d = d << 4;
		d = d + 0x0F;   // 15
		return d;
    }
};

class HAPFakegatoSchedule : public HAPFakegato {
public:    

    HAPFakegatoSchedule() : HAPFakegato() {

    }
    

    virtual ~HAPFakegatoSchedule() {
        if (_configRead) delete _configRead;
        if (_configWrite) delete _configWrite;

        _programEvents.clear();
        _timers.enable(false);
        _timers.clear();
    }
    
    virtual void begin() = 0;
    virtual bool decodeToggleOnOff(uint8_t* data) = 0;
    virtual void decodeDays(uint8_t *data) = 0;
    virtual void decodePrograms(uint8_t* data) = 0;
    virtual void encodePrograms(uint8_t* data, size_t *dataSize) = 0;
    
    virtual String buildScheduleString() = 0;


    bool isEnabled(){
        return _timers.isEnabled();
    }

    void enable(bool on){
        _timers.enable(on);
    }

    void setSerialNumber(String serialNumber){
        _serialNumber = serialNumber;
    }

    
    void setStatusLED(uint8_t mode){	
	    _statusLED = mode;
    }

    void scheduleFromJson(JsonObject &root);
    JsonObject scheduleToJson();
    
    void clear();

    void setCallbackTimerStart(std::function<void(uint16_t)> callback){
        _callbackTimerStart = callback;
    }

    void setCallbackTimerEnd(std::function<void(uint16_t)> callback){
        _callbackTimerEnd = callback;
    }

    void handle() {        
        // _alarms.delay(0);
        _timers.handle();
    }

    void callbackTimerStart(uint16_t state);
    void callbackTimerEnd(uint16_t state);
    
    void programTimers();

    void setCallbackSaveConfig(std::function<void(void)> callback){
        _callbackSaveConfig = callback;
    }

protected:
    String  _serialNumber;
    uint8_t _statusLED;

    std::vector<HAPFakeGatoScheduleProgramEvent> _programEvents; // 7
    HAPFakeGatoScheduleDays _days;  
    HAPDailyTimerFactory _timers;

    std::function<void(uint16_t)> _callbackTimerStart = nullptr;
    std::function<void(uint16_t)> _callbackTimerEnd = nullptr;

    std::function<void(void)> _callbackSaveConfig = nullptr;   
};

#endif /* HAPFAKEGATOSCHEDULE_HPP_ */