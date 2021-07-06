//
// HAPTime.cpp
// Homekit
//
//  Created on: 07.03.2021
//      Author: michael
//

#include "HAPTime.hpp"
// #include "HAPLogger.hpp"
// #include "HAPHelper.hpp"


const char *monthName[12] PROGMEM = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const float toRad = M_PI/180.0;
const float toDeg = 180.0/M_PI;
const float twoPi = 2 * M_PI;
const float zenith = 90.83 * toRad;

int HAPTime::_utcOffset = 0;
float HAPTime::_longitude = 0;
float HAPTime::_latitude = 0;
uint32_t HAPTime::_refTime = 0;
uint32_t HAPTime::_t_offset = 0;

char HAPTime::_timestring[30] = {'\0',};


callbackGetTime_t HAPTime::_callbackGetTime = nullptr;

#if HAP_ENABLE_NTP
uint8_t HAPTime::_packetBuffer[NTP_PACKET_SIZE] = {0,};
EthernetUDP HAPTime::_udp;
#endif

#if HAP_ENABLE_KNX_TIME
uint16_t HAPTime::_koReadTime = 0;
uint16_t HAPTime::_koWriteTime = 0;
#endif

HAPTime::HAPTime()
{

}

HAPTime::~HAPTime(){

}


bool HAPTime::begin(){

    setSyncProvider(getDstCorrectedTime);
    setSyncInterval(HAP_TIME_SYNC_INTERVAL);
    return true;

}

#if HAP_ENABLE_NTP
bool HAPTime::beginNTP(){

#if defined( ARDUINO_ARCH_ESP32 )
    for (uint8_t i=0; i < HAP_NTP_SERVER_URLS_SIZE; i++){
		configTzTime(HAP_NTP_TZ_INFO, HAP_NTP_SERVER_URLS[i]);
		if (getLocalTime(&_timeinfo, 10000)) {  // wait up to 10sec to sync
			//Serial.println(&_timeinfo, "Time set: %B %d %Y %H:%M:%S (%A)");
			LogI( " OK", true);
			LogI("Set time to: " + timeString(), true);

			_configuration.getPlatformConfig()->setRefTime(timestamp());
			LogI("Current refTime is: " + String(_configuration.getPlatformConfig()->refTime()), true);
			return true;
            break;
		}
	}
    return false;
#elif defined (CORE_TEENSY)
    _udp.begin(8888);
    return true;
#else
    return false
#endif
}
#endif

bool HAPTime::setLongitudeLatitude(float longitude, float latitude){
    if ((longitude < -180) || (longitude > 180)) return false;
    if ((latitude < -90) || (latitude > 90))     return false;

    _longitude = longitude;
    _latitude = latitude;

    return true;
}


//const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)
bool HAPTime::setTimeZone(int utcOffset){
    if ((utcOffset < - 720) || (utcOffset > 720))  return false;
    _utcOffset = utcOffset;

    return true;
}

time_t HAPTime::sunRise(time_t date) {
    return sunTime(true, date);
}

time_t HAPTime::sunSet(time_t date) {
    return sunTime(false, date);
}

time_t HAPTime::sunTime(bool sunRise, time_t date) {

    if (_latitude == 0 && _longitude == 0) return 0;

    if (date == 0) date = now();

    // Calculate the sunrise and sunset times for date and 'noon time'
    tmElements_t tm;
    breakTime(date, tm);
    tm.Hour   = 12;
    tm.Minute = 0;
    tm.Second = 0;
    date = makeTime(tm);
    date -= _utcOffset * SECS_PER_HOUR;

    // first calculate the day of the year
    int N1 = 275 * (month(date)) / 9;
    int N2 = (month(date)+9)/12;
    int N3 = 1 + (year(date) - 4 * (year(date) / 4) + 2) / 3;
    int N = N1 - (N2 * N3) + day(date) - 30;

    // convert the longitude to hour value and calculate an approximate time
    float lngHour = _longitude / 15.0;
    float t = 0;
    if (sunRise)
        t = N + ((6 - lngHour) / 24);
    else
        t = N + ((18 - lngHour) / 24);

    // Sun's mean anomaly
    float M = (0.9856 * t) - 3.289;
    M *= toRad;

    // the Sun's true longitude
    float L = M + (1.916*toRad * sin(M)) + (0.020*toRad * sin(2 * M)) + 282.634*toRad;
    if (L < 0)      L += twoPi;
    if (L > twoPi) L -= twoPi;

    // the Sun's right ascension
    float RA = toDeg*atan(0.91764 * tan(L));
    if (RA < 0)   RA += 360;
    if (RA > 360) RA -= 360;

    // right ascension value needs to be in the same quadrant as L
    int Lquadrant  = (floor( L/M_PI_2)) * 90;
    int RAquadrant = (floor(RA/90)) * 90;
    RA += Lquadrant - RAquadrant;
    RA /= 15;         // right ascension value needs to be converted into hours

    // calculate the Sun's declination
    float sinDec = 0.39782 * sin(L);
    float cosDec = cos(asin(sinDec));

    // float decl = toDeg*asin(sinDec);

    // calculate the Sun's local hour angle
    float cosH = (cos(zenith) - (sinDec * sin(_latitude*toRad))) / (cosDec * cos(_latitude*toRad));
    if (cosH >  1) return 0;            // the Sun never rises on this location on the specified date
    if (cosH < -1) return 0;            // the Sun never sets on this location on the specified date

    // finish calculating H and convert into hours
    float H = 0;
    if (sunRise)
        H = 360 - toDeg*acos(cosH);
    else
        H = toDeg*acos(cosH);
    H /= 15;

    // calculate local mean time of rising/setting
    float T = H + RA - (0.06571 * t) - 6.622;
    if (T < 0)  T += 24;
    if (T > 24) T -= 24;
    float UT = T - lngHour;
    float localT = UT + (float)_utcOffset / 60.0;

    tm.Hour = (uint8_t)localT;
    localT -= tm.Hour;
    localT *= 60;
    tm.Minute = (uint8_t)localT;
    localT -= tm.Minute;
    localT *= 60;
    tm.Second = (uint8_t)localT;

    time_t ret = makeTime(tm);
    return ret;
}

/* This function returns the DST offset for the current UTC time.
 * This is valid for the EU, for other places see
 * http://www.webexhibits.org/daylightsaving/i.html
 *
 * Results have been checked for 2012-2030 (but should work since
 * 1996 to 2099) against the following references:
 * - http://www.uniquevisitor.it/magazine/ora-legale-italia.php
 * - http://www.calendario-365.it/ora-legale-orario-invernale.html
 */
uint8_t HAPTime::dstOffset(uint8_t dayValue, uint8_t monthValue, unsigned int yearValue, uint8_t hourValue) {
    // Day in March that DST starts on, at 1 am
    uint8_t dstOn = (31 - (5 * yearValue / 4 + 4) % 7);

    // Day in October that DST ends  on, at 2 am
    uint8_t dstOff = (31 - (5 * yearValue / 4 + 1) % 7);

    if ((monthValue > 3 && monthValue < 10) ||
        (monthValue == 3 && (dayValue > dstOn || (dayValue == dstOn && hourValue >= 1))) ||
        (monthValue == 10 && (dayValue < dstOff || (dayValue == dstOff && hourValue <= 1))))
        return 1;
    else
        return 0;
}


uint16_t HAPTime::getDaysToDST(const unsigned int year, const uint8_t month){
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
    double daysToDST = (dstTime - HAP_FAKEGATO_EPOCH) / SECS_PER_DAY;

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

time_t HAPTime::getTimeFromCompiling(){
    time_t t = 0;
    tmElements_t tm;
    if (getDateFromString(__DATE__, tm) && getTimeFromString(__TIME__, tm)) {
        t = makeTime(tm);
        //t += (_utcOffset + dstOffset(tm.Day, tm.Month, tm.Year + 1970, tm.Hour)) * SECS_PER_HOUR;
    }
    // Teensy3Clock.get();
    return t;
}

/* Alternative SyncProvider that automatically handles Daylight Saving Time (DST) periods,
 * at least in Europe, see above.
 */
time_t HAPTime::getDstCorrectedTime(){
    time_t t = 0;

    if (_callbackGetTime == nullptr){
        t = getTimeFromCompiling();
    } else {
        t = _callbackGetTime();
    }

    if (t > 0) {
        TimeElements tm;
        breakTime (t, tm);
        _t_offset = (_utcOffset * dstOffset(tm.Day, tm.Month, tm.Year + 1970, tm.Hour)) * SECS_PER_HOUR;
        t += _t_offset;
    }

    return t;
}

void HAPTime::setCallbackGetTime(callbackGetTime_t callback){
    _callbackGetTime = callback;
    setSyncProvider(getDstCorrectedTime);
    setSyncInterval(HAP_TIME_SYNC_INTERVAL);
}



bool HAPTime::getTimeFromString(const char *str, TimeElements& tm){
    int Hour, Min, Sec;

    if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
    tm.Hour = Hour;
    tm.Minute = Min;
    tm.Second = Sec;
    return true;
}

bool HAPTime::getDateFromString(const char *str, TimeElements& tm){
    char Month[12];
    int Day, Year;
    uint8_t monthIndex;

    if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(Month, monthName[monthIndex]) == 0) break;
    }
    if (monthIndex >= 12) return false;
    tm.Day = Day;
    tm.Month = monthIndex + 1;
    tm.Year = CalendarYrToTm(Year);
    return true;
}


#if HAP_ENABLE_NTP
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
time_t HAPTime::getNTPTime(){

	while (_udp.parsePacket() > 0) ; // discard any previously received packets
	// LogD(F("Transmit NTP Request to ..."), false);

    for (uint8_t i=0; i < HAP_NTP_SERVER_URLS_SIZE; i++){
        // LogD(F("   * "), false);
        // LogD(HAP_NTP_SERVER_URLS[i], false);
        // LogD(F(" ... "), false);

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

                // LogD(F("OK"), true);
                return secsSince1900 - UNIX_OFFSET + (HAP_TIMEZONE * SECS_PER_HOUR);
            }
        }
        // LogD(F("FAILED\n"), true);
    }
	// LogE(F("ERROR - No NTP Response :-("), true);
	return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPTime::sendNTPpacket(const char* address){
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
#endif

/**
 * @brief Returns current timestamp in seconds
 *
 * @return uint32_t timestamp (in seconds)
 */
uint32_t HAPTime::timestamp(){
#if defined( ARDUINO_ARCH_ESP32 )
	timeval now;
	gettimeofday(&now, NULL);
	return now.tv_sec;
#elif defined( CORE_TEENSY )
	if (timeStatus() != timeNotSet) {
		return (uint32_t) (now() - (2 * _t_offset));	// - UNIX_OFFSET;
	}
	return millis();
#else
	return millis();
#endif
}

/**
 * @brief Return current time as string
 * 		  Format: YYYY-MM-DD HH:MM:SS.sss
 * 		  example 2019-09-22 21:30:57.239
 *
 * @return String current time
 */
const char* HAPTime::timeString(){

    memset((uint8_t*)_timestring, '\0', 30);

#if defined( ARDUINO_ARCH_ESP32 )
    timeval curTime;
    gettimeofday(&curTime, NULL);

    if (String(HAP_NTP_TIME_FORMAT).endsWith(".%f")){
        char tmp[30];
        const char* timeformat = String(HAP_NTP_TIME_FORMAT).substring(0, String(HAP_NTP_TIME_FORMAT).length() - 3).c_str();
        strftime(tmp, 30, timeformat, localtime(&curTime.tv_sec));

        int milli = curTime.tv_usec / 1000;

        snprintf(_timestring, 30, "%s.%03d", tmp, milli);

        return _timestring;
    }

    strftime(_timestring, 30, HAP_NTP_TIME_FORMAT, localtime(&curTime.tv_sec));
    return _timestring;

#elif defined( CORE_TEENSY )
	if (timeStatus() != timeNotSet) {

		struct tm curTtime;
		convertToTimeH(now(), curTtime);
		strftime(_timestring, 30, HAP_NTP_TIME_FORMAT, &curTtime);

        return (const char*)_timestring;
	} else {
        snprintf(_timestring, 30, "%" PRIu32, millis());
        return (const char*)_timestring;
    }
#else
    snprintf(_timestring, 30, "%" PRIu32, millis());
    return (const char*)_timestring;
#endif
}

// ToDo: Is this neccessay? Can it be optianed from a global define?
// !!! First implement the KNX configuration for this and then add it to the time lib !!!
// !!! KNX is required to be at HAPServer level for this to get the infos before the plugins are laoded !!!
#if HAP_ENABLE_KNX_TIME
void HAPTime::setKNXComObjects(uint16_t koReadTime, uint16_t koWriteTime){
    _koReadTime = koReadTime;
    if (koWriteTime > 0){
        _koWriteTime = koWriteTime;
    }
}
#endif