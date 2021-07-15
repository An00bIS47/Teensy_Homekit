//
// HAPTime.hpp
// Teensy_Homekit
//
//  Created on: 08.07.2021
//      Author: michael
//

#ifndef HAPTIME_HPP_
#define HAPTIME_HPP_


#include <TimeLib.h>
#include <Timezone.h>
#include <time.h>
#include <functional>



#ifndef HAP_DEBUG_TIME
#define HAP_DEBUG_TIME 		0
#endif

#ifndef HAP_TIME_NTP_ENABLED
#define HAP_TIME_NTP_ENABLED        0
#endif

#ifndef HAP_TIME_FAKEGATO_ENABLED
#define HAP_TIME_FAKEGATO_ENABLED   1
#endif

#ifndef HAP_TIME_SYNC_INTERVAL
#define HAP_TIME_SYNC_INTERVAL      600
#endif

#define HAP_TIME_STR_FORMAT         "%Y-%m-%d %H:%M:%S"	    // strftime format


#ifndef HAP_TIME_FAKEGATO_EPOCH
#define HAP_TIME_FAKEGATO_EPOCH		978307200
#endif

#define HAP_TIME_UNIX_OFFSET		2208988800UL

#if HAP_TIME_NTP_ENABLED
#include <NativeEthernetUdp.h>
#define HAP_NTP_TIMEOUT	            3000
#define HAP_TIME_NTP_PACKET_SIZE    48


#define HAP_TIME_NTP_SERVER_URLS_SIZE 	3

#ifndef HAP_TIME_NTP_SERVER_URL_1
#define HAP_TIME_NTP_SERVER_URL_1	"fritz.box"						// NTP server url
#endif

#ifndef HAP_TIME_NTP_SERVER_URL_2
#define HAP_TIME_NTP_SERVER_URL_2	"time.euro.apple.com"			// NTP server url
#endif

#ifndef HAP_TIME_NTP_SERVER_URL_3
#define HAP_TIME_NTP_SERVER_URL_3	"pool.ntp.org"
#endif /* HAP_TIME_NTP_ENABLED */

const char* const HAP_TIME_NTP_SERVER_URLS[] = {HAP_TIME_NTP_SERVER_URL_1, HAP_TIME_NTP_SERVER_URL_2, HAP_TIME_NTP_SERVER_URL_3};
#endif

class HAPTime {
public:
    HAPTime() {}
    ~HAPTime() {}

	// Central European Time (Frankfurt, Paris)
    static TimeChangeRule CEST; // = {"CEST", Last, Sun, Mar, 2, 120};      // Central European Summer Time
    static TimeChangeRule CET;  // = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time

    static void begin(TimeChangeRule dstStart = CEST, TimeChangeRule stdStart = CET);
    static void setSyncCallback(std::function<time_t(void)> callback);

    static time_t utcTimestamp();       // utc timestamp
    static time_t timestamp();          // localized timestamp
    static const char* timestring();    // localized time string

    static void printDateTime(Print& prt, Timezone tz, time_t utc, const char *descr);

	static void setTimeFromTimestamp(time_t date = 0);

	static time_t getTeensy3Time();

#if HAP_TIME_NTP_ENABLED
    static time_t getNTPTime();         // method to be used for begin and/or setCallbackGetTime
#endif

#if HAP_TIME_FAKEGATO_ENABLED
    static void setRefTime(time_t refTime);   // ToDo: change to time_t in HAPFakegato
    static time_t refTime();                  // ToDo: change to time_t in HAPFakegato
    static time_t fakegatoTimestamp();        // ToDo: change to time_t in HAPFakegato

	static uint16_t daysToDST(const unsigned int year, const uint8_t month);

    static int offset();                        // ToDo: change to proper format!! currently in minutes
#endif

protected:
    static char _timestring[30];
    static std::function<time_t(void)> _callbackSync;
    static Timezone* _tz;

    static time_t compileTime();
    static time_t getTimeFromCallback();




#if HAP_TIME_FAKEGATO_ENABLED
    static time_t _refTime;

	static uint8_t getDstEndDay(unsigned int yearValue);
	static uint8_t getDstStartDay(unsigned int yearValue);
#endif

    

#if HAP_TIME_NTP_ENABLED
    static uint8_t _packetBuffer[HAP_TIME_NTP_PACKET_SIZE];
    static EthernetUDP _udp;
    static void sendNTPpacket(const char* address);
#endif




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
};

#endif /* HAPTIME_HPP_ */