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
struct HAPFakegatoScheduleDeviceType {
    enum Type : uint8_t {
        Weather         = 0x01,
        Room            = 0x02,
        DoorWindow      = 0x03,
        EnergyEU        = 0x04,
        LightSwitch     = 0x0B,
        Thermo          = 0x0C,
        EnergyUS        = 0x0D,
        Motion          = 0x11,
        EnergyUK        = 0x12,
        EnergyEU2       = 0x14,
        EnergyAU        = 0x1A,
        LightSwitch2    = 0x22,
        Aqua            = 0x23,
        EnergyEU3       = 0x24,
        Motion2         = 0x25,
        DoorWindow2     = 0x26,
        Room2           = 0x27,
        EnergyUK2       = 0x28,
        EnergyAU2       = 0x29,
        Thermo2         = 0x2C,
        Button          = 0x31,
        Flare           = 0x33,
        LightStrip      = 0x36,
        EnergyStripEU   = 0x37,
        EnergyStripUS   = 0x38,
        LightSwitch3    = 0x39,
        Extend          = 0x3B,
        Thermo3         = 0x47
    };
};

struct HAPFakegatoScheduleTLVType {
    enum Type : uint8_t {
        DeviceType              = 0x00,
        SerialNumber            = 0x04,
        UsedMemory              = 0x06,
        RolledOverIndex         = 0x07,
        CommandToggleSchedule   = 0x44,
        CommandStatusLed        = 0x20,
        Programs                = 0x45,
        Days                    = 0x46,
        DST                     = 0x47,
        StatusLed               = 0x60,
        LastActivity            = 0xD0,
        EveTime                 = 0x9B,

        EndMark                 = 0xD2        
    };
};

// enum class HAPFakegatoScheduleTLVType : uint8_t {
//     DeviceType              = 0x00,
//     SerialNumber            = 0x04,
//     UsedMemory              = 0x06,
//     RolledOverIndex         = 0x07,
//     CommandToggleSchedule   = 0x44,
//     CommandStatusLed        = 0x20,
//     Programs                = 0x45,
//     Days                    = 0x46,
//     DST                     = 0x47,
//     StatusLed               = 0x60,
//     LastActivity            = 0xD0,
//     EveTime                 = 0x9B,

//     EndMark                 = 0xD2
// };

// ToDo: define or enum ?
#define HAP_FAKEGATO_SCHEDULE_TYPE_DEVICE_TYPE              0x00
#define HAP_FAKEGATO_SCHEDULE_TYPE_SERIALNUMBER             0x04

#define HAP_FAKEGATO_SCHEDULE_TYPE_USED_MEMORY              0x06
#define HAP_FAKEGATO_SCHEDULE_TYPE_ROLLED_OVER_INDEX        0x07

#define HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_TOGGLE_SCHEDULE  0x44
#define HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_STATUS_LED       0x20
#define HAP_FAKEGATO_SCHEDULE_TYPE_PROGRAMS                 0x45
#define HAP_FAKEGATO_SCHEDULE_TYPE_DAYS                     0x46
#define HAP_FAKEGATO_SCHEDULE_TYPE_DST                      0x47
#define HAP_FAKEGATO_SCHEDULE_TYPE_STATUS_LED               0x60
#define HAP_FAKEGATO_SCHEDULE_TYPE_LAST_ACTIVITY            0xD0

#define HAP_FAKEGATO_SCHEDULE_TYPE_EVE_TIME                 0x9B

#define HAP_FAKEGATO_SCHEDULE_TYPE_END_MARK                 0xD2

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
        if (_scheduleRead) delete _scheduleRead;
        if (_scheduleWrite) delete _scheduleWrite;

        _programEvents.clear();
        _timers.enable(false);
        _timers.clear();
    }

    HAPService* registerFakeGatoService(HAPFakegatoScheduleDeviceType::Type deviceType, HAPAccessory* accessory, const char* name);

    bool isEnabled(){
        return _timers.isEnabled();
    }

    void enable(bool on){
        _timers.enable(on);
    }

    void setSerialNumber(const char* serialNumber){
        _serialNumber = serialNumber;
    }


    void setStatusLED(uint8_t mode){
	    _statusLED = mode;
    }

    // void scheduleFromJson(JsonObject &root);
    // JsonObject scheduleToJson();

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


    void programTimers();

    void setCallbackSaveConfig(std::function<void(void)> callback){
        _callbackSaveConfig = callback;
    }

    void setDeviceType(HAPFakegatoScheduleDeviceType::Type deviceType){
        _deviceType = deviceType;
    }



protected:

    // Schedules
    HAPCharacteristicData* _scheduleRead     = nullptr;
    HAPCharacteristicData* _scheduleWrite    = nullptr;

    // Get data
    virtual void callbackGetSchedule(uint8_t* output, size_t* len) = 0;

    // Set data
    virtual void callbackSetSchedule(const uint8_t* decoded, const size_t len) = 0;


    void callbackTimerStart(uint16_t state);
    void callbackTimerEnd(uint16_t state);

    virtual bool decodeToggleOnOff(uint8_t* data) = 0;
    virtual void decodeDays(uint8_t *data) = 0;
    virtual void decodePrograms(uint8_t* data) = 0;
    virtual void encodePrograms(uint8_t* data, size_t *dataSize) = 0;


    void saveConfig(){
        if (_shouldSave){
            if (_callbackSaveConfig != nullptr) _callbackSaveConfig();
        }
    }

    std::string  _serialNumber;
    uint8_t _statusLED;

    bool    _shouldSave = false;

    std::vector<HAPFakeGatoScheduleProgramEvent> _programEvents; // 7
    HAPFakeGatoScheduleDays _days;
    HAPDailyTimerFactory _timers;

    std::function<void(uint16_t)> _callbackTimerStart = nullptr;
    std::function<void(uint16_t)> _callbackTimerEnd = nullptr;

    std::function<void(void)> _callbackSaveConfig = nullptr;

    HAPFakegatoScheduleDeviceType::Type _deviceType;
};

#endif /* HAPFAKEGATOSCHEDULE_HPP_ */