// 
// HAPTime.hpp
// Teensy_Homekit
//
//  Created on: 07.03.2021
//      Author: michael
//

#ifndef HAPTIME_HPP_
#define HAPTIME_HPP_

#include <Arduino.h> 
#include <TimeLib.h>
#include <math.h>
#include <functional>
#include "HAPGlobals.hpp"


#if HAP_ENABLE_NTP
#include <NativeEthernetUdp.h>
#endif

#define callbackGetTime_t std::function<time_t(void)>

class HAPTime {
public:
    HAPTime();
    ~HAPTime();

    static bool begin();

    static String timeString();
    static uint32_t timestamp();

    static time_t sunRise(time_t date = 0);
    static time_t sunSet(time_t date = 0);


    static bool setLongitudeLatitude(float longitude, float latitude);
    static bool setTimeZone(int timezone);
    
    static uint8_t dstOffset(uint8_t dayValue, uint8_t monthValue, unsigned int yearValue, uint8_t hourValue);
    
    static time_t getDstCorrectedTime();

#if HAP_ENABLE_NTP
    static time_t getNTPTime();
#endif

    static void setCallbackGetTime(callbackGetTime_t callback);

protected:
    static int _utcOffset;      // GMP? offset in minutes
    static float _longitude;
    static float _latitude;

    static callbackGetTime_t _callbackGetTime;      
    
    static time_t sunTime(bool sunRise, time_t date = 0);     
    
    static bool getTimeFromString(const char *str, TimeElements& tm);
    static bool getDateFromString(const char *str, TimeElements& tm);

#if HAP_ENABLE_NTP
    static uint8_t _packetBuffer[NTP_PACKET_SIZE];
    static EthernetUDP _udp;
    static void sendNTPpacket(const char* address);
#endif    
    
    
    // zenith:      Sun's zenith for sunrise/sunset
    // offical      = 90 degrees 50'
    // civil        = 96 degrees
    // nautical     = 102 degrees
    // astronomical = 108 degrees

    // int utcOffset;     // Central European Time
    //const int utcOffset = -5;  // Eastern Standard Time (USA)
    //const int utcOffset = -4;  // Eastern Daylight Time (USA)
    //const int utcOffset = -8;  // Pacific Standard Time (USA)
    //const int utcOffset = -7;  // Pacific Daylight Time (USA)

};

#endif /* HAPTIME_HPP_ */
