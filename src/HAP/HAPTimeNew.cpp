//
// HAPTimeNew.cpp
// Homekit
//
//  Created on: 08.07.2021
//      Author: michael
//

#include "HAPTimeNew.hpp"
#include "HAPLogging.hpp"



char HAPTimeNew::_timestring[30] = {'\0',};
std::function<time_t(void)> HAPTimeNew::_callbackSync = nullptr;

HAPTimezone HAPTimeNew::_tz;
time_t HAPTimeNew::_refTime = 0;

#if HAP_TIME_NTP_ENABLED
uint8_t HAPTimeNew::_packetBuffer[HAP_TIME_NTP_PACKET_SIZE] = {0,};
EthernetUDP HAPTimeNew::_udp;
#endif



void HAPTimeNew::begin(HAPTimezone tz){
    _tz = tz;
    setTime(_tz.toUTC(compileTime()));

#if HAP_TIME_NTP_ENABLED
    _udp.begin(8888);
    setSyncCallback(HAPTimeNew::getNTPTime);

    setSyncInterval(HAP_TIME_SYNC_INTERVAL);
#endif
}


// given a Timezone object, UTC and a string description, convert and print local time with time zone
void HAPTimeNew::printDateTime(Print& prt, HAPTimezone tz, time_t utc, const char *descr)
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
time_t HAPTimeNew::compileTime()
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


void HAPTimeNew::setSyncCallback(std::function<time_t(void)> callback){
    _callbackSync = callback;
    setSyncProvider(_callbackSync);
}

time_t HAPTimeNew::utcTimestamp(){
    return now();
}

time_t HAPTimeNew::timestamp(){
	if (timeStatus() != timeNotSet) {
        return _tz.toLocal(now());
	}
	return millis();
}

const char* HAPTimeNew::timestring(){
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

time_t HAPTimeNew::getTimeFromCallback(){
    time_t utc = now();

    if (_callbackSync){
        utc = _callbackSync();
    }

    return utc;
}


#if HAP_TIME_NTP_ENABLED
void HAPTimeNew::sendNTPpacket(const char* address){
	// set all bytes in the buffer to 0
	memset(_packetBuffer, 0, NTP_PACKET_SIZE);
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
	_udp.write(_packetBuffer, NTP_PACKET_SIZE);
	_udp.endPacket();
}

time_t HAPTimeNew::getNTPTime(){
    LOG_V("Get NTP time\n");

	while (_udp.parsePacket() > 0) ; // discard any previously received packets

    for (uint8_t i=0; i < HAP_NTP_SERVER_URLS_SIZE; i++){

        LOG_D("Getting time from %s\n", HAP_NTP_SERVER_URLS[i]);

        sendNTPpacket(HAP_NTP_SERVER_URLS[i]);
        uint32_t beginWait = millis();

        while (millis() - beginWait < HAP_NTP_TIMEOUT) {
            int size = _udp.parsePacket();
            if (size >= NTP_PACKET_SIZE) {
                // Serial.println("Receive NTP Response");
                _udp.read(_packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
                unsigned long secsSince1900;
                // convert four bytes starting at location 40 to a long integer
                secsSince1900 =  (unsigned long)_packetBuffer[40] << 24;
                secsSince1900 |= (unsigned long)_packetBuffer[41] << 16;
                secsSince1900 |= (unsigned long)_packetBuffer[42] << 8;
                secsSince1900 |= (unsigned long)_packetBuffer[43];

                LOGRAW_D("OK\n");
                return secsSince1900 - UNIX_OFFSET + (HAP_TIMEZONE * SECS_PER_HOUR);
            }
        }
        LOGRAW_W("WARNING: Failed to get time from server: %s\n", HAP_NTP_SERVER_URLS[i]);
    }
	LOG_E("ERROR: Failed get time from NTP!\n");
	return 0; // return 0 if unable to get the time
}
#endif

#if HAP_TIME_FAKEGATO_ENABLED
uint32_t HAPTimeNew::refTime() {
    return _refTime;
}

uint32_t HAPTimeNew::fakegatoTimestamp(){
    if (timeStatus() != timeNotSet) {
        return (uint32_t)_tz.toLocal(now()) - HAP_FAKEGATO_EPOCH;
    }
    return millis();
}

uint16_t HAPTimeNew::daysToDST(){
    time_t localTime = _tz.locDSTStart(year(timestamp()));
    double daysToChange = (localTime - HAP_FAKEGATO_EPOCH) / SECS_PER_DAY;
    return (uint16_t) ceil(daysToChange);
}

uint16_t HAPTimeNew::daysToSTD(){
    time_t localTime = _tz.locSTDStart(year(timestamp()));
    double daysToChange = (localTime - HAP_FAKEGATO_EPOCH) / SECS_PER_DAY;
    return (uint16_t) ceil(daysToChange);
}

int HAPTimeNew::offset(){
    return _tz.offset(_tz.toLocal(now()));
}
#endif