//
// test_common_tlv8.cpp
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

#include "HAP/HAPTLV8.hpp"

void setUp(void)
{
#ifdef NATIVE
    ArduinoFakeReset();
#endif
}

void test_tlv8_simple_encode(void){
    TLV8 tlv;
    const int length = 9;
    uint8_t data[length] = {0x01, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    tlv.encode(data, length);

    uint8_t result[tlv.size()];
    size_t s = 0;
    tlv.decode(result, &s);

    TEST_ASSERT_EQUAL(s, tlv.size());
    TEST_ASSERT_EQUAL_MEMORY(data, result, length);
    TEST_ASSERT_EQUAL(s, sizeof(data));

    tlv.clear();
}

void test_tlv_duo(void) {

    TLV8 tlv;
    const int length = 13;
    uint8_t data[length] = {0x01, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x02, 0x02, 0x01, 0x02};
    tlv.encode(data, length);

    uint8_t result[tlv.size()];
    size_t s = 0;
    tlv.decode(result, &s);

    TEST_ASSERT_EQUAL_MEMORY(data, result, length);
    TEST_ASSERT_EQUAL(s, sizeof(data));
    TEST_ASSERT_EQUAL(s, length);
    TEST_ASSERT_EQUAL(s, tlv.size());

    tlv.clear();
}

void test_tlv_duo_get_single(void) {

    uint8_t expected[2] = {0xA1, 0xA2};

    TLV8 tlv;
    const int length = 13;
    uint8_t data[length] = {0x01, 0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x02, 0x02, 0xA1, 0xA2};
    tlv.encode(data, length);

    uint8_t result[tlv.size(0x02)];
    size_t s = 0;

    tlv.decode(0x02, result, &s);

    TEST_ASSERT_EQUAL_MEMORY(expected, result, 2);
    TEST_ASSERT_EQUAL(s, 2);
    TEST_ASSERT_EQUAL(tlv.size(0x02), 2);
    TEST_ASSERT_EQUAL(tlv.size(), 13);
    TEST_ASSERT_EQUAL(tlv.hasType(0x02), true);

    tlv.clear();
}

void test_tlv_long(void) {

    uint8_t expected[58] = { 0x84, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0xA5, 0x2F, 0x50, 0x0A, 0x4D, 0x61, 0x73, 0x74, 0x65, 0x72, 0x43, 0x61, 0x72, 0x64, 0x9F, 0x38, 0x06, 0x9F, 0x5C, 0x08, 0x9F, 0x40, 0x05, 0xBF, 0x0C, 0x17, 0x9F, 0x5E, 0x09, 0x54, 0x13, 0x33, 0x90, 0x00, 0x00, 0x15, 0x13, 0x01, 0x9F, 0x5D, 0x03, 0x01, 0x01, 0x00, 0x9F, 0x4D, 0x02, 0x0B, 0x0A };

    TLV8 tlv;

    const uint8_t length = 60;
    uint8_t data[length] = { 0x6F, 0x3A, 0x84, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0xA5, 0x2F, 0x50, 0x0A, 0x4D, 0x61, 0x73, 0x74, 0x65, 0x72, 0x43, 0x61, 0x72, 0x64, 0x9F, 0x38, 0x06, 0x9F, 0x5C, 0x08, 0x9F, 0x40, 0x05, 0xBF, 0x0C, 0x17, 0x9F, 0x5E, 0x09, 0x54, 0x13, 0x33, 0x90, 0x00, 0x00, 0x15, 0x13, 0x01, 0x9F, 0x5D, 0x03, 0x01, 0x01, 0x00, 0x9F, 0x4D, 0x02, 0x0B, 0x0A };

    tlv.encode(data, length);

    uint8_t result[tlv.size(0x6F)];
    size_t s = 0;

    tlv.decode(0x6F, result, &s);

    TEST_ASSERT_EQUAL_MEMORY(expected, result, 58);
    TEST_ASSERT_EQUAL(s, 58);
    TEST_ASSERT_EQUAL(tlv.size(0x6F), 58);
    TEST_ASSERT_EQUAL(tlv.size(), length);
    TEST_ASSERT_EQUAL(tlv.hasType(0x6F), true);

    tlv.clear();
}


void test_tlv_long_sub(void) {

    uint8_t expected[14] = { 0x82, 0x02, 0x58, 0x00, 0x94, 0x08, 0x08, 0x01, 0x01, 0x00, 0x10, 0x01, 0x03, 0x01 };

    TLV8 tlv;

    const uint8_t length = 16;
    uint8_t data[length] = { 0x77, 0x0E, 0x82, 0x02, 0x58, 0x00, 0x94, 0x08, 0x08, 0x01, 0x01, 0x00, 0x10, 0x01, 0x03, 0x01 };

    tlv.encode(data, length);

    uint8_t result[tlv.size(0x77)];
    size_t s = 0;

    tlv.decode(0x77, result, &s);

    TEST_ASSERT_EQUAL_MEMORY(expected, result, 14);
    TEST_ASSERT_EQUAL(s, 14);
    TEST_ASSERT_EQUAL(tlv.size(0x77), 14);
    TEST_ASSERT_EQUAL(tlv.size(), length);
    TEST_ASSERT_EQUAL(tlv.hasType(0x77), true);

    tlv.clear();
}




void test_tlv_separator(void) {

    TLV8 tlv;
    const int length = 6;
    uint8_t data[length] = {0xFF, 0x00, 0x01, 0x02, 0x03, 0x04};
    tlv.encode(data, length);

    uint8_t result[128];
    size_t s = 0;
    tlv.decode(result, &s);

    TEST_ASSERT_EQUAL(s, tlv.size());
    TEST_ASSERT_EQUAL_MEMORY(data, result, length);
    TEST_ASSERT_EQUAL(s, sizeof(data));
    TEST_ASSERT_EQUAL(s, length);
    TEST_ASSERT_EQUAL(tlv.hasType(0x01), true);
    TEST_ASSERT_EQUAL(tlv.size(0x01), 2);

    uint8_t expected[2] = {0x03, 0x04};
    uint8_t result2[tlv.size(0x01)];
    size_t s2 = 0;

    tlv.decode(0x01, result2, &s2);
    TEST_ASSERT_EQUAL(s2, tlv.size(0x01));
    TEST_ASSERT_EQUAL_MEMORY(expected, result2, tlv.size(0x01));

    tlv.clear();
}

int runAllTests(){
    UNITY_BEGIN();

    RUN_TEST(test_tlv8_simple_encode);
    RUN_TEST(test_tlv_duo);
    RUN_TEST(test_tlv_duo_get_single);
    RUN_TEST(test_tlv_separator);
    RUN_TEST(test_tlv_long);
    RUN_TEST(test_tlv_long_sub);


    return UNITY_END();
}

#ifdef ARDUINO
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

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

