//
// HAPTime.hpp
// Homekit
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
#include <time.h>

#include "HAPGlobals.hpp"

#if HAP_ENABLE_NTP
#include <NativeEthernetUdp.h>
#endif

#ifndef HAP_ENABLE_KNX_TIME
#define HAP_ENABLE_KNX_TIME 0
#endif

#ifndef HAP_ENABLE_NTP
#define HAP_ENABLE_NTP 0
#endif

#ifndef NTP_PACKET_SIZE
#define NTP_PACKET_SIZE 48
#endif

#ifndef HAP_NTP_TIME_FORMAT
#define HAP_NTP_TIME_FORMAT	"%Y-%m-%d %H:%M:%S"	    // strftime format
#endif

#ifndef HAP_NTP_TIMEOUT
#define HAP_NTP_TIMEOUT	3000
#endif

#ifndef HAP_TIME_SYNC_INTERVAL
#define HAP_TIME_SYNC_INTERVAL 600000
#endif

#ifndef HAP_FAKEGATO_EPOCH
#define HAP_FAKEGATO_EPOCH       978307200
#endif

#if HAP_ENABLE_KNX_TIME
#include <knx.h>
#endif




#define callbackGetTime_t std::function<time_t(void)>

class HAPTime {
public:
    HAPTime();
    ~HAPTime();

    static bool begin();

    static const char* timeString();
    static uint32_t timestamp();

    static time_t sunRise(time_t date = 0);
    static time_t sunSet(time_t date = 0);


    static bool setLongitudeLatitude(float longitude, float latitude);
    static bool setTimeZone(int timezone);

    static uint8_t dstOffset(uint8_t dayValue, uint8_t monthValue, unsigned int yearValue, uint8_t hourValue);

    static time_t getDstCorrectedTime();

#if HAP_ENABLE_NTP
    static time_t getNTPTime();
    bool beginNTP();
#endif

    static void setTimeFromTimestamp(time_t date = 0) {
        setTime(date);
    }

    static void setCallbackGetTime(callbackGetTime_t callback);

    static void setReftime(const uint32_t reftime){
        _refTime = reftime;
    }

    static uint32_t refTime() {
        return _refTime;
    }

    static uint32_t getTOffset(){
        return _t_offset;
    }

    static int getUTCOffset(){
        return _utcOffset;
    }


    static uint16_t getDaysToDST(const unsigned int year, const uint8_t month);

    static uint8_t getDstStartDay(unsigned int yearValue);
    static uint8_t getDstEndDay(unsigned int yearValue);

#if defined( CORE_TEENSY )
	//converts TimeLib's time representations to time.h's representations
	static void convertToTimeH(tmElements_t &timelib, tm &timeh){
		time_t t = makeTime(timelib); // - UNIX_OFFSET;
		localtime_r(&t, &timeh);
	}
	static void convertToTimeH(time_t timelib, tm &timeh){
		time_t t = timelib; // - UNIX_OFFSET;
		localtime_r(&t, &timeh);
	}

	static time_t convertToTimeH(tmElements_t &timelib){
		return makeTime(timelib); // - UNIX_OFFSET;
	}

	static time_t convertToTimeH(time_t timelib){
		return timelib; // - UNIX_OFFSET;
	}

	//converts time.h's time representations to TimeLib's representations
	static void convertToTimeLib(tm &timeh, tmElements_t &timelib){
		// breakTime(mktime(&timeh) + UNIX_OFFSET, timelib);
		breakTime(mktime(&timeh), timelib);
	}

	static void convertToTimeLib(time_t timeh, tmElements_t &timelib){
		// breakTime(timeh + UNIX_OFFSET, timelib);
		breakTime(timeh, timelib);
	}
	static time_t convertToTimeLib(tm &timeh){
		// return mktime(&timeh) + UNIX_OFFSET;
		return mktime(&timeh); // + UNIX_OFFSET;
	}

	static time_t convertToTimeLib(time_t timeh){
		// return timeh + UNIX_OFFSET;
		return timeh; // + UNIX_OFFSET;
	}
#endif

protected:
    static int _utcOffset;      // GMP? offset in minutes
    static float _longitude;
    static float _latitude;
    static uint32_t _refTime;
    static uint32_t _t_offset;


    static char _timestring[30];

    static callbackGetTime_t _callbackGetTime;

    static time_t sunTime(bool sunRise, time_t date = 0);

    static bool getTimeFromString(const char *str, TimeElements& tm);
    static bool getDateFromString(const char *str, TimeElements& tm);

#if HAP_ENABLE_NTP
    static uint8_t _packetBuffer[NTP_PACKET_SIZE];
    static EthernetUDP _udp;
    static void sendNTPpacket(const char* address);
#endif

#if HAP_ENABLE_KNX_TIME
    static void setKNXComObjects(uint16_t koReadTime, uint16_t koWriteTime = 0);
    static uint16_t _koReadTime;
    static uint16_t _koWriteTime;
#endif

    static time_t getTimeFromCompiling();

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
