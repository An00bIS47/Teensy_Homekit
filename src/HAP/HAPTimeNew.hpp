//
// HAPTimeNew.hpp
// Teensy_Homekit
//
//  Created on: 08.07.2021
//      Author: michael
//

#ifndef HAPTIMENEW_HPP_
#define HAPTIMENEW_HPP_


#include <TimeLib.h>
#include <time.h>
#include <functional>
#include "HAPTimezone.hpp"

#ifndef HAP_TIME_NTP_ENABLED
#define HAP_TIME_NTP_ENABLED        1
#endif

#ifndef HAP_TIME_FAKEGATO_ENABLED
#define HAP_TIME_FAKEGATO_ENABLED   1
#endif

#ifndef HAP_TIME_SYNC_INTERVAL
#define HAP_TIME_SYNC_INTERVAL      600000
#endif

#define HAP_TIME_STR_FORMAT         "%Y-%m-%d %H:%M:%S"	    // strftime format


#ifndef HAP_FAKEGATO_EPOCH
#define HAP_FAKEGATO_EPOCH          978307200
#endif

#if HAP_TIME_NTP_ENABLED
#include <NativeEthernetUdp.h>
#define HAP_NTP_TIMEOUT	            3000
#define HAP_TIME_NTP_PACKET_SIZE    48
#endif


// Central European Time (Frankfurt, Paris)
HAPTimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};      // Central European Summer Time
HAPTimeChangeRule CET  = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
HAPTimezone CE(CEST, CET);

class HAPTimeNew {
public:
    HAPTimeNew();
    ~HAPTimeNew();

    static void begin(HAPTimezone tz = CE);
    static void setSyncCallback(std::function<time_t(void)> callback);

    static time_t utcTimestamp();       // utc timestamp
    static time_t timestamp();          // localized timestamp
    static const char* timestring();    // localized time string

    static void printDateTime(Print& prt, HAPTimezone tz, time_t utc, const char *descr);


#if HAP_TIME_FAKEGATO_ENABLED
    static void setRefTime(time_t refTime);   // ToDo: change to time_t in HAPFakegato
    static time_t refTime();                  // ToDo: change to time_t in HAPFakegato
    static time_t fakegatoTimestamp();        // ToDo: change to time_t in HAPFakegato

    static time_t daysToDST();        // FAKEGATO EPOCH TIME is already substracted
    static time_t daysToSDT();        // FAKEGATO EPOCH TIME is already substracted

    static int offset();                        // ToDo: change to proper format!! currently in minutes
#endif

protected:
    static char _timestring[30];
    static std::function<time_t(void)> _callbackSync;
    static HAPTimezone _tz;

    static time_t compileTime();
    static time_t getTimeFromCallback();

    


#if HAP_TIME_FAKEGATO_ENABLED
    static uint32_t _refTime;
#endif

    

#if HAP_TIME_NTP_ENABLED
    static uint8_t _packetBuffer[HAP_TIME_NTP_PACKET_SIZE];
    static EthernetUDP _udp;
    static void sendNTPpacket(const char* address);
    
    static time_t getNTPTime();         // method to be used for begin and/or setCallbackGetTime
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

#endif /* HAPTIMENEW_HPP_ */