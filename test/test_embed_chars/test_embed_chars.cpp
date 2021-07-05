//
// test_embed_chars.cpp
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
#include "HAP/HAPCharacteristic.hpp"


void setUp(void)
{
#ifdef NATIVE
    ArduinoFakeReset();
#endif
}


void test_string(void){

    HAPCharacteristic<String>* testChar = new HAPCharacteristic<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE);
    testChar->setDescription("Test");
    testChar->setValue("Test", false);


    TEST_ASSERT_EQUAL_STRING("Test", testChar->value(false).c_str());
    delete testChar;
}



void test_int(void){

    HAPCharacteristic<int>* testChar = new HAPCharacteristic<int>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE, 0, 5, 1, HAP_UNIT_NONE);
    testChar->setDescription("Test");
    testChar->setValue(1, false);

    TEST_ASSERT_EQUAL(1, testChar->value(false));
    TEST_ASSERT_EQUAL_STRING("1", testChar->valueString().c_str());

    testChar->valueFromString("22");
    TEST_ASSERT_EQUAL(22, testChar->value(false));
    TEST_ASSERT_EQUAL_STRING("22", testChar->valueString().c_str());

    delete testChar;
}

void test_float(void){

    HAPCharacteristic<float>* testChar = new HAPCharacteristic<float>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE, 0, 123456, 0.1, HAP_UNIT_NONE);
    testChar->setDescription("Test");
    testChar->setValue(1.1, false);

    TEST_ASSERT_EQUAL(1.1, testChar->value(false));
    TEST_ASSERT_EQUAL_STRING("1.10", testChar->valueString().c_str());

    testChar->valueFromString("22.234");
    TEST_ASSERT_EQUAL(22.23, testChar->value(false));
    TEST_ASSERT_EQUAL_STRING("22.23", testChar->valueString().c_str());

    delete testChar;
}

void getDataCallback(uint8_t* data, size_t* len){
    uint8_t d[6] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A};
    *len = 6;
    if (data) memcpy(data, d, 6);
}

void test_data(void){

    HAPCharacteristicData* testChar = new HAPCharacteristicData(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE, 512);
    testChar->setDescription("Test");

    testChar->setDataGetCallback(getDataCallback);

    size_t dataLen = 3;
    uint8_t data[dataLen];
    data[0] = 0x0E;
    data[1] = 0x0A;
    data[2] = 0x0F;
    testChar->setValue(data, 3, false);

    uint8_t output[dataLen] = {0,};
    testChar->value(output, &dataLen, false);

    TEST_ASSERT_EQUAL(3, dataLen);
    TEST_ASSERT_EQUAL_MEMORY(data, output, dataLen);

    size_t len = 0;
    testChar->value(nullptr, &len, false);
    TEST_ASSERT_EQUAL(3, len);

    uint8_t output2[len] = {0,};
    testChar->value(output2, &len, false);
    TEST_ASSERT_EQUAL_MEMORY(data, output2, dataLen);

    size_t len3 = 6;
    uint8_t d[len3] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A};

    uint8_t output3[len3] = {0,};
    testChar->value(output3, &len3, true);

    TEST_ASSERT_EQUAL(6, len3);
    TEST_ASSERT_EQUAL_MEMORY(d, output3, len3);
    TEST_ASSERT_EQUAL_STRING("Dw4NDAsK", testChar->valueString().c_str());

    delete testChar;
}


int runAllTests(){
    UNITY_BEGIN();

    for (int i=0; i < TEST_ITERATIONS; i++){
        RUN_TEST(test_string);
        RUN_TEST(test_int);
        RUN_TEST(test_float);
        RUN_TEST(test_data);
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

