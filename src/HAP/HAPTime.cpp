//
// HAPTime.cpp
// Homekit
//
//  Created on: 08.07.2021
//      Author: michael
//

#include "HAPTime.hpp"
#include "HAPLogging.hpp"


TimeChangeRule HAPTime::CEST = {"CEST", Last, Sun, Mar, 2, 120};      // Central European Summer Time
TimeChangeRule HAPTime::CET  = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time

char HAPTime::_timestring[30] = {'\0',};
std::function<time_t(void)> HAPTime::_callbackSync = nullptr;

Timezone* HAPTime::_tz = nullptr;
time_t HAPTime::_refTime = 0;

#if HAP_TIME_NTP_ENABLED
uint8_t HAPTime::_packetBuffer[HAP_TIME_NTP_PACKET_SIZE] = {0,};
EthernetUDP HAPTime::_udp;
#endif



void HAPTime::begin(TimeChangeRule dstStart, TimeChangeRule stdStart){

    LOG_V("Starting HAPTime ...");
    if (_tz == nullptr) {
        _tz = new Timezone(dstStart, stdStart);
    } else {
        _tz->setRules(dstStart, stdStart);
    }
    LOGRAW_V("OK\n");

    setTime(_tz->toUTC(compileTime()));

#if HAP_TIME_NTP_ENABLED
    LOG_V("Setup NTP ...");
    _udp.begin(8888);
    //_callbackSync = getNTPTime;
    setSyncCallback(HAPTime::getNTPTime);
    LOGRAW_V("OK\n");
#endif

    //setSyncCallback(getNTPTime);
    LOG_V("Setup Sync interval to %d ...", HAP_TIME_SYNC_INTERVAL);
    setSyncInterval(HAP_TIME_SYNC_INTERVAL);
    LOGRAW_V("OK\n");
}


// given a Timezone object, UTC and a string description, convert and print local time with time zone
void HAPTime::printDateTime(Print& prt, Timezone tz, time_t utc, const char *descr)
{
    char buf[40];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev

    time_t t = tz.toLocal(utc, &tcr);
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tcr -> abbrev);
    prt.print(buf);
    prt.print(' ');
    prt.println(descr);
    // LOG_I("%s %s\n", buf, descr);
}


// Function to return the compile date and time as a time_t value
time_t HAPTime::compileTime()
{
    const time_t FUDGE(10);     // fudge factor to allow for compile time (seconds, YMMV)

    const char *compDate = __DATE__;
    const char *compTime = __TIME__;
    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";

    char chMon[4], *m;
    tmElements_t tm;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);

    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    time_t t = makeTime(tm);
    return t + FUDGE;           // add fudge factor to allow for compile time
}


void HAPTime::setSyncCallback(std::function<time_t(void)> callback){
    _callbackSync = callback;
    setSyncProvider(getTimeFromCallback);
}

time_t HAPTime::utcTimestamp(){
    if (timeStatus() != timeNotSet) {
        return now();
    }
    return millis();
}

time_t HAPTime::timestamp(){
	if (timeStatus() != timeNotSet) {
        return _tz->toLocal(now());
	}
	return millis();
}

const char* HAPTime::timestring(){
    memset((uint8_t*)_timestring, 0, 30);

	if (timeStatus() != timeNotSet) {
		struct tm curTtime;
		convertToTimeH(timestamp(), curTtime);
		strftime(_timestring, 30, HAP_TIME_STR_FORMAT, &curTtime);

        return (const char*)_timestring;
	}

    snprintf(_timestring, 30, "%" PRIu32, millis());
    return (const char*)_timestring;
}

time_t HAPTime::getTimeFromCallback(){
    // LOG_V("Get time from callback\n");
    //Serial.println("GET TIME FROM CALLBACK");

#if HAP_DEBUG_TIME
    LOGDEVICE->println("GET TIME FROM CALLBACK");
#endif

    time_t utc = 0;

    if (_callbackSync){
        utc = _callbackSync();
    } else {
        utc = getTeensy3Time();
    }

    return utc;
}

void HAPTime::setTimeFromTimestamp(time_t date) {
    setTime(date);
}


time_t HAPTime::getTeensy3Time() {
	return Teensy3Clock.get();
}


#if HAP_TIME_NTP_ENABLED
void HAPTime::sendNTPpacket(const char* address){
	// set all bytes in the buffer to 0
	memset(_packetBuffer, 0, HAP_TIME_NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	_packetBuffer[1] = 0;     // Stratum, or type of clock
	_packetBuffer[2] = 6;     // Polling Interval
	_packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	_packetBuffer[12]  = 49;
	_packetBuffer[13]  = 0x4E;
	_packetBuffer[14]  = 49;
	_packetBuffer[15]  = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	_udp.beginPacket(address, 123); //NTP requests are to port 123
	_udp.write(_packetBuffer, HAP_TIME_NTP_PACKET_SIZE);
	_udp.endPacket();
}

time_t HAPTime::getNTPTime(){
    // LOG_V("Get NTP time\n");
	while (_udp.parsePacket() > 0) ; // discard any previously received packets

    for (uint8_t i=0; i < HAP_TIME_NTP_SERVER_URLS_SIZE; i++){

        // LOG_D("Getting time from %s\n", HAP_TIME_NTP_SERVER_URLS[i]);
#if HAP_DEBUG_TIME
        LOGDEVICE->printf("Transmit NTP Request to %s\n", HAP_TIME_NTP_SERVER_URLS[i]);
#endif

        sendNTPpacket(HAP_TIME_NTP_SERVER_URLS[i]);
        uint32_t beginWait = millis();

        while (millis() - beginWait < HAP_NTP_TIMEOUT) {
            int size = _udp.parsePacket();
            if (size >= HAP_TIME_NTP_PACKET_SIZE) {
#if HAP_DEBUG_TIME
                Serial.printf("Receive NTP Response: %d\n", size);
#endif

                _udp.read(_packetBuffer, HAP_TIME_NTP_PACKET_SIZE);  // read packet into the buffer
                unsigned long secsSince1900;
                // convert four bytes starting at location 40 to a long integer
                secsSince1900 =  (unsigned long)_packetBuffer[40] << 24;
                secsSince1900 |= (unsigned long)_packetBuffer[41] << 16;
                secsSince1900 |= (unsigned long)_packetBuffer[42] << 8;
                secsSince1900 |= (unsigned long)_packetBuffer[43];

#if HAP_DEBUG_TIME
                // LOGRAW_D("OK\n");
                LOGDEVICE->printf("result: %d\n", secsSince1900 - HAP_TIME_UNIX_OFFSET);
#endif
                return (secsSince1900 - HAP_TIME_UNIX_OFFSET);
            }
        }
        // LOGRAW_W("WARNING: Failed to get time from server: %s\n", HAP_TIME_NTP_SERVER_URLS[i]);
    }
#if HAP_DEBUG_TIME
	// LOG_E("ERROR: Failed get time from NTP!\n");
    LOGDEVICE->println("Received no NTP Response :(");
#endif
	return 0; // return 0 if unable to get the time
}
#endif

#if HAP_TIME_FAKEGATO_ENABLED

void HAPTime::setRefTime(time_t refTime){
    _refTime = refTime;
}

time_t HAPTime::refTime() {
    return _refTime;
}

time_t HAPTime::fakegatoTimestamp(){
    if (timeStatus() != timeNotSet) {
        return (uint32_t)_tz->toLocal(now()) - HAP_TIME_FAKEGATO_EPOCH;
    }
    return millis();
}

uint16_t HAPTime::daysToDST(const unsigned int year, const uint8_t month){
    tmElements_t dstDate;

    dstDate.Year = CalendarYrToTm(year);
    dstDate.Month = month;
    if (month == 3) {
        dstDate.Day = getDstStartDay(year);
        dstDate.Hour = 1;           // 1:00 UTC     == 2:00 MEZ
    } else if ( month == 10 ) {
        dstDate.Day = getDstEndDay(year);
        dstDate.Hour = 2;           // 2:00 UTC     == 3:00 MESZ
    }

    time_t dstTime = makeTime(dstDate);
    double daysToDST = (dstTime - HAP_TIME_FAKEGATO_EPOCH) / SECS_PER_DAY;

    return (uint16_t) ceil(daysToDST);
}


uint8_t HAPTime::getDstStartDay(unsigned int yearValue){
    // Day in March that DST starts on, at 1 am
    uint8_t dstOn = (31 - (5 * yearValue / 4 + 4) % 7);
    return dstOn;
}

uint8_t HAPTime::getDstEndDay(unsigned int yearValue){
    // Day in October that DST ends  on, at 2 am
    uint8_t dstOff = (31 - (5 * yearValue / 4 + 1) % 7);
    return dstOff;
}

int HAPTime::offset(){
    return 0;
}
#endif