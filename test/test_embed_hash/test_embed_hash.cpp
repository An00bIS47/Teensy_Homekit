//
// test_embed_hash.cpp
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
#include "HAP/HAPHash.hpp"

void setUp(void)
{
#ifdef NATIVE
    ArduinoFakeReset();
#endif
}


void test_hash_1(void){

    HAPHash hash(SRP_SHA1);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.hash(data, 5, digest);

    const uint8_t length = 20;
    uint8_t expected[length] = { 0x1C, 0xF2, 0x51, 0x47, 0x2D, 0x59, 0xF8, 0xFA, 0xDE, 0xB3, 0xAB, 0x25, 0x8E, 0x90, 0x99, 0x9D, 0x84, 0x91, 0xBE, 0x19 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA1_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_1_seq(void){

    HAPHash hash(SRP_SHA1);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.init();
    hash.update(data, 1);
    hash.update(data + 1, 1);
    hash.update(data + 2, 1);
    hash.update(data + 3, 1);
    hash.update(data + 4, 1);
    hash.final(digest);

    const uint8_t length = 20;
    uint8_t expected[length] = { 0x1C, 0xF2, 0x51, 0x47, 0x2D, 0x59, 0xF8, 0xFA, 0xDE, 0xB3, 0xAB, 0x25, 0x8E, 0x90, 0x99, 0x9D, 0x84, 0x91, 0xBE, 0x19 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA1_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_1_static(void){

    uint8_t digestLength = HAPHash::digestLength(SRP_SHA1);

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    HAPHash::hash(SRP_SHA1, data, 5, digest);

    const uint8_t length = 20;
    uint8_t expected[length] = { 0x1C, 0xF2, 0x51, 0x47, 0x2D, 0x59, 0xF8, 0xFA, 0xDE, 0xB3, 0xAB, 0x25, 0x8E, 0x90, 0x99, 0x9D, 0x84, 0x91, 0xBE, 0x19 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA1_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_224(void){

    HAPHash hash(SRP_SHA224);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.hash(data, 5, digest);

    const uint8_t length = 28;
    uint8_t expected[length] = { 0x98, 0x02, 0x9C, 0xB4, 0x58, 0xA3, 0x9A, 0x16, 0x35, 0x59, 0x63, 0x92, 0x2D, 0x32, 0xDA, 0xCD, 0x94, 0x39, 0xF9, 0x0E, 0x9F, 0xD1, 0x06, 0xD4, 0x2A, 0x0D, 0x12, 0x3C };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA224_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_224_seq(void){

    HAPHash hash(SRP_SHA224);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.init();
    hash.update(data, 1);
    hash.update(data + 1, 1);
    hash.update(data + 2, 1);
    hash.update(data + 3, 1);
    hash.update(data + 4, 1);
    hash.final(digest);

    const uint8_t length = 28;
    uint8_t expected[length] = { 0x98, 0x02, 0x9C, 0xB4, 0x58, 0xA3, 0x9A, 0x16, 0x35, 0x59, 0x63, 0x92, 0x2D, 0x32, 0xDA, 0xCD, 0x94, 0x39, 0xF9, 0x0E, 0x9F, 0xD1, 0x06, 0xD4, 0x2A, 0x0D, 0x12, 0x3C };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA224_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_256(void){

    HAPHash hash(SRP_SHA256);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.hash(data, 5, digest);

    const uint8_t length = 32;
    uint8_t expected[length] = { 0x08, 0xBB, 0x5E, 0x5D, 0x6E, 0xAA, 0xC1, 0x04, 0x9E, 0xDE, 0x08, 0x93, 0xD3, 0x0E, 0xD0, 0x22, 0xB1, 0xA4, 0xD9, 0xB5, 0xB4, 0x8D, 0xB4, 0x14, 0x87, 0x1F, 0x51, 0xC9, 0xCB, 0x35, 0x28, 0x3D };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA256_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_256_seq(void){

    HAPHash hash(SRP_SHA256);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.init();
    hash.update(data, 1);
    hash.update(data + 1, 1);
    hash.update(data + 2, 1);
    hash.update(data + 3, 1);
    hash.update(data + 4, 1);
    hash.final(digest);

    const uint8_t length = 32;
    uint8_t expected[length] = { 0x08, 0xBB, 0x5E, 0x5D, 0x6E, 0xAA, 0xC1, 0x04, 0x9E, 0xDE, 0x08, 0x93, 0xD3, 0x0E, 0xD0, 0x22, 0xB1, 0xA4, 0xD9, 0xB5, 0xB4, 0x8D, 0xB4, 0x14, 0x87, 0x1F, 0x51, 0xC9, 0xCB, 0x35, 0x28, 0x3D };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA256_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_256_static(void){

    uint8_t digestLength = HAPHash::digestLength(SRP_SHA256);
    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    HAPHash::hash(SRP_SHA256, data, 5, digest);

    const uint8_t length = 32;
    uint8_t expected[length] = { 0x08, 0xBB, 0x5E, 0x5D, 0x6E, 0xAA, 0xC1, 0x04, 0x9E, 0xDE, 0x08, 0x93, 0xD3, 0x0E, 0xD0, 0x22, 0xB1, 0xA4, 0xD9, 0xB5, 0xB4, 0x8D, 0xB4, 0x14, 0x87, 0x1F, 0x51, 0xC9, 0xCB, 0x35, 0x28, 0x3D };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA256_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_384(void){

    HAPHash hash(SRP_SHA384);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.hash(data, 5, digest);

    const uint8_t length = 48;
    uint8_t expected[length] = { 0x56, 0x1C, 0x16, 0x40, 0x4A, 0x1B, 0x59, 0x24, 0x06, 0x30, 0x17, 0x80, 0xC0, 0xC2, 0xDF, 0x6A, 0xA0, 0x55, 0x5F, 0x50, 0x4F, 0x35, 0xBF, 0xBE, 0xAC, 0x81, 0x0A, 0xE3, 0x6A, 0x34, 0x3B, 0x77, 0x68, 0x58, 0xC5, 0xE0, 0xDE, 0x56, 0xBB, 0x79, 0x60, 0x7A, 0x34, 0xD2, 0xF6, 0x71, 0x08, 0xF2 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA384_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_384_seq(void){

    HAPHash hash(SRP_SHA384);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.init();
    hash.update(data, 1);
    hash.update(data + 1, 1);
    hash.update(data + 2, 1);
    hash.update(data + 3, 1);
    hash.update(data + 4, 1);
    hash.final(digest);

    const uint8_t length = 48;
    uint8_t expected[length] = { 0x56, 0x1C, 0x16, 0x40, 0x4A, 0x1B, 0x59, 0x24, 0x06, 0x30, 0x17, 0x80, 0xC0, 0xC2, 0xDF, 0x6A, 0xA0, 0x55, 0x5F, 0x50, 0x4F, 0x35, 0xBF, 0xBE, 0xAC, 0x81, 0x0A, 0xE3, 0x6A, 0x34, 0x3B, 0x77, 0x68, 0x58, 0xC5, 0xE0, 0xDE, 0x56, 0xBB, 0x79, 0x60, 0x7A, 0x34, 0xD2, 0xF6, 0x71, 0x08, 0xF2 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA384_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

void test_hash_512(void){

    HAPHash hash(SRP_SHA512);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    hash.hash(data, 5, digest);

    const uint8_t length = 64;
    uint8_t expected[length] = { 0xB7, 0xB7, 0x0A, 0x0B, 0x14, 0xD7, 0xFA, 0x21, 0x3C, 0x6C, 0xCD, 0x3C, 0xBF, 0xFC, 0x8B, 0xB8, 0xF8, 0xE1, 0x1A, 0x85, 0xF1, 0x11, 0x3B, 0x0E, 0xB2, 0x6A, 0x00, 0x20, 0x8F, 0x2B, 0x9B, 0x3A, 0x1D, 0xD4, 0xAA, 0xF3, 0x99, 0x62, 0x86, 0x1E, 0x16, 0xAB, 0x06, 0x22, 0x74, 0x34, 0x2A, 0x1C, 0xE1, 0xF9, 0xDB, 0xA3, 0x65, 0x4F, 0x36, 0xFC, 0x33, 0x82, 0x45, 0x58, 0x9F, 0x29, 0x6C, 0x28 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA512_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);

    // uint8_t digestLength2 = HAPHASH::digestLength(SRP_SHA512);
    // TEST_ASSERT_EQUAL(SHA512_DIGEST_LENGTH, digestLength2);
}

void test_hash_512_seq(void){

    HAPHash hash(SRP_SHA512);

    uint8_t digestLength = hash.digestLength();

    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};

    hash.init();
    hash.update(data, 1);
    hash.update(data + 1, 1);
    hash.update(data + 2, 1);
    hash.update(data + 3, 1);
    hash.update(data + 4, 1);
    hash.final(digest);

    const uint8_t length = 64;
    uint8_t expected[length] = { 0xB7, 0xB7, 0x0A, 0x0B, 0x14, 0xD7, 0xFA, 0x21, 0x3C, 0x6C, 0xCD, 0x3C, 0xBF, 0xFC, 0x8B, 0xB8, 0xF8, 0xE1, 0x1A, 0x85, 0xF1, 0x11, 0x3B, 0x0E, 0xB2, 0x6A, 0x00, 0x20, 0x8F, 0x2B, 0x9B, 0x3A, 0x1D, 0xD4, 0xAA, 0xF3, 0x99, 0x62, 0x86, 0x1E, 0x16, 0xAB, 0x06, 0x22, 0x74, 0x34, 0x2A, 0x1C, 0xE1, 0xF9, 0xDB, 0xA3, 0x65, 0x4F, 0x36, 0xFC, 0x33, 0x82, 0x45, 0x58, 0x9F, 0x29, 0x6C, 0x28 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA512_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);

    // uint8_t digestLength2 = HAPHASH::digestLength(SRP_SHA512);
    // TEST_ASSERT_EQUAL(SHA512_DIGEST_LENGTH, digestLength2);
}


void test_hash_512_static(void){
    uint8_t digestLength = HAPHash::digestLength(SRP_SHA512);
    uint8_t digest[digestLength];

    uint8_t data[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
    HAPHash::hash(SRP_SHA512, data, 5, digest);

    const uint8_t length = 64;
    uint8_t expected[length] = { 0xB7, 0xB7, 0x0A, 0x0B, 0x14, 0xD7, 0xFA, 0x21, 0x3C, 0x6C, 0xCD, 0x3C, 0xBF, 0xFC, 0x8B, 0xB8, 0xF8, 0xE1, 0x1A, 0x85, 0xF1, 0x11, 0x3B, 0x0E, 0xB2, 0x6A, 0x00, 0x20, 0x8F, 0x2B, 0x9B, 0x3A, 0x1D, 0xD4, 0xAA, 0xF3, 0x99, 0x62, 0x86, 0x1E, 0x16, 0xAB, 0x06, 0x22, 0x74, 0x34, 0x2A, 0x1C, 0xE1, 0xF9, 0xDB, 0xA3, 0x65, 0x4F, 0x36, 0xFC, 0x33, 0x82, 0x45, 0x58, 0x9F, 0x29, 0x6C, 0x28 };

    TEST_ASSERT_EQUAL(length, sizeof(expected));
    TEST_ASSERT_EQUAL(SHA512_DIGEST_LENGTH, digestLength);
    TEST_ASSERT_EQUAL_MEMORY(expected, digest, length);
}

int runAllTests(){
    UNITY_BEGIN();

    for (int i=0; i < TEST_ITERATIONS; i++){
        RUN_TEST(test_hash_512);
        RUN_TEST(test_hash_384);
        RUN_TEST(test_hash_256);
        RUN_TEST(test_hash_224);
        RUN_TEST(test_hash_1);

        RUN_TEST(test_hash_512_seq);
        RUN_TEST(test_hash_384_seq);
        RUN_TEST(test_hash_256_seq);
        RUN_TEST(test_hash_224_seq);
        RUN_TEST(test_hash_1_seq);

        RUN_TEST(test_hash_512_static);
        RUN_TEST(test_hash_256_static);
        RUN_TEST(test_hash_1_static);
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

