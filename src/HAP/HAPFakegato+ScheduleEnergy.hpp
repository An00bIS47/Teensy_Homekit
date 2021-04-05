// 
// HAPFakegato+ScheduleEnergy.hpp
// Teensy_Homekit
//
//  Created on: 05.04.2021
//      Author: michael
//

#ifndef HAPFAKEGATOSCHEDULEENERGY_HPP_
#define HAPFAKEGATOSCHEDULEENERGY_HPP_

#include <Arduino.h>
#include "HAPFakegato+Schedule.hpp"
#include <functional>

class HAPFakegatoScheduleEnergy : public HAPFakegatoSchedule {
public:
    HAPFakegatoScheduleEnergy() : HAPFakegatoSchedule() {

    }

    static uint32_t encodeTimerCount(uint8_t timerCount);
    static uint8_t encodeProgramCount(uint8_t programCount);    

    bool decodeToggleOnOff(uint8_t* data);
    void decodeDays(uint8_t *data);
    void decodePrograms(uint8_t* data);
    void encodePrograms(uint8_t* data, size_t *dataSize);
    
    String buildScheduleString();

    void setCallbackGetTimestampLastActivity(std::function<uint32_t(void)> callback){
        _callbackGetTimestampLastActivity = callback;
    }

protected:
    std::function<uint32_t(void)> _callbackGetTimestampLastActivity;
};

#endif /* HAPFAKEGATOSCHEDULEENERGY_HPP_ */