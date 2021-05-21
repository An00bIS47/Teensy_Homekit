//
// test_embed_srp.cpp
// Homekit
//
//  Created on: 10.05.2021
//      Author: michael
//

#include <Arduino.h>
#include <unity.h>

#ifdef NATIVE
using namespace fakeit;
#endif

#include "../test_config.h"
#include "HAP/HAPTime.hpp"


void setUp(void)
{
#ifdef NATIVE
    ArduinoFakeReset();
#endif
}


void test_dst_2021(void){

    uint16_t year = 2021;
    uint8_t startDst = HAPTime::getDstStartDay(year);
    uint8_t endDst = HAPTime::getDstEndDay(year);

    printf("start: %d.03.%d end: %d.10.%d", startDst, year, endDst, year);

    TEST_ASSERT_EQUAL(28, startDst);
    TEST_ASSERT_EQUAL(31, endDst);
}

void test_dst_2022(void){

    uint16_t year = 2022;
    uint8_t startDst = HAPTime::getDstStartDay(year);
    uint8_t endDst = HAPTime::getDstEndDay(year);

    printf("start: %d.03.%d end: %d.10.%d", startDst, year, endDst, year);

    TEST_ASSERT_EQUAL(27, startDst);
    TEST_ASSERT_EQUAL(30, endDst);
}

void test_dst_2025(void){

    uint16_t year = 2025;
    uint8_t startDst = HAPTime::getDstStartDay(year);
    uint8_t endDst = HAPTime::getDstEndDay(year);

    printf("start: %d.03.%d end: %d.10.%d", startDst, year, endDst, year);

    TEST_ASSERT_EQUAL(30, startDst);
    TEST_ASSERT_EQUAL(26, endDst);
}


void test_dst_2021_days_steps(void){

    int year = 2021;
    uint8_t startDayDst = HAPTime::getDstStartDay(year);
    uint8_t endDayDst = HAPTime::getDstEndDay(year);

    printf("start: %d.03.%d end: %d.10.%d", startDayDst, year, endDayDst, year);

    tmElements_t startDST;
    startDST.Year = CalendarYrToTm(year);
    startDST.Month = 3;
    startDST.Hour = 1;
    startDST.Day = startDayDst;

    tmElements_t endDST;
    endDST.Year = CalendarYrToTm(year);;
    endDST.Month = 10;
    endDST.Hour = 2;
    endDST.Day = endDayDst;

    time_t tStartDst = makeTime(startDST);
    time_t tEndDst = makeTime(endDST);

    TEST_ASSERT_EQUAL_UINT32(1616886000, tStartDst);
    TEST_ASSERT_EQUAL_UINT32(1635631200, tEndDst);

    // (1616886000 - 978307200) / 86400 = 7.390,9583333333    ==> 7391
    // (1635631200 - 978307200) / 86400 = 7.607,9166666667    ==> 7608
    
    double daysToDSTStart = (tStartDst - HAP_FAKEGATO_EPOCH) / SECS_PER_DAY;
    double daysToDSTEnd = (tEndDst - HAP_FAKEGATO_EPOCH) / SECS_PER_DAY;
    
    size_t roundedDSTStart = ceil(daysToDSTStart);
    size_t roundedDSTEnd = ceil(daysToDSTEnd);

    TEST_ASSERT_EQUAL(7391, roundedDSTStart);
    TEST_ASSERT_EQUAL(7608, roundedDSTEnd);
}

void test_dst_2021_days(void){
    int year = 2021;

    uint16_t roundedDSTStart = HAPTime::getDaysToDST(year, 3);
    uint16_t roundedDSTEnd = HAPTime::getDaysToDST(year, 10);

    TEST_ASSERT_EQUAL(7391, roundedDSTStart);
    TEST_ASSERT_EQUAL(7608, roundedDSTEnd);
}


int runAllTests(){
    UNITY_BEGIN();

    for (int i=0; i < TEST_ITERATIONS; i++){
        RUN_TEST(test_dst_2021);
        RUN_TEST(test_dst_2022);
        RUN_TEST(test_dst_2025);

        RUN_TEST(test_dst_2021_days);
        RUN_TEST(test_dst_2021_days_steps);
    }


    return UNITY_END();
}

#ifdef ARDUINO
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(3000);

    runAllTests();
}

void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv)
{
    return runAllTests();
}

#endif

