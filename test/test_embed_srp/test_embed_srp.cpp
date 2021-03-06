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
#include "HAP/HAPSRP.hpp"

#define USERNAME "alice"
#define PASSWORD "password123"

void setUp(void)
{
#ifdef NATIVE
    ArduinoFakeReset();
#endif
}


void test_srp(void){

    HAPSRP* _hapsrp = new HAPSRP();
    TEST_ASSERT_NOT_NULL(_hapsrp);
    _hapsrp->begin(USERNAME);

    _hapsrp->data->len_s = 16;

    _hapsrp->data->session = _hapsrp->newSession(SRP_SHA512, HAPSRP::SRP_NG_3072, NULL,NULL);
    TEST_ASSERT_NOT_NULL(_hapsrp->data->session);

    _hapsrp->createSaltedVerificationKey1(_hapsrp->data->session,
									USERNAME,
									(const uint8_t*)PASSWORD, strlen(PASSWORD),
									&_hapsrp->data->bytes_s, _hapsrp->data->len_s,
									&_hapsrp->data->bytes_v, &_hapsrp->data->len_v
									);

    TEST_ASSERT_NOT_NULL(_hapsrp->data->bytes_s);
    TEST_ASSERT_NOT_NULL(_hapsrp->data->bytes_v);


    const uint8_t saltlength = 16;
    uint8_t salt[saltlength] = { 0xBE, 0xB2, 0x53, 0x79, 0xD1, 0xA8, 0x58, 0x1E, 0xB5, 0xA7, 0x27, 0x67, 0x3A, 0x24, 0x41, 0xEE };

    TEST_ASSERT_EQUAL_MEMORY(salt, _hapsrp->data->bytes_s, saltlength);


    _hapsrp->data->keys = _hapsrp->createKeyPair(_hapsrp->data->session, _hapsrp->data->bytes_v, _hapsrp->data->len_v, &_hapsrp->data->bytes_B, &_hapsrp->data->len_B);
    TEST_ASSERT_NOT_NULL(_hapsrp->data->keys);

    const uint16_t decodedLen = 384;
    uint8_t device_public_key[decodedLen] = { 0xFA, 0xB6, 0xF5, 0xD2, 0x61, 0x5D, 0x1E, 0x32, 0x35, 0x12, 0xE7, 0x99, 0x1C, 0xC3, 0x74, 0x43, 0xF4, 0x87, 0xDA, 0x60, 0x4C, 0xA8, 0xC9, 0x23, 0x0F, 0xCB, 0x04, 0xE5, 0x41, 0xDC, 0xE6, 0x28, 0x0B, 0x27, 0xCA, 0x46, 0x80, 0xB0, 0x37, 0x4F, 0x17, 0x9D, 0xC3, 0xBD, 0xC7, 0x55, 0x3F, 0xE6, 0x24, 0x59, 0x79, 0x8C, 0x70, 0x1A, 0xD8, 0x64, 0xA9, 0x13, 0x90, 0xA2, 0x8C, 0x93, 0xB6, 0x44, 0xAD, 0xBF, 0x9C, 0x00, 0x74, 0x5B, 0x94, 0x2B, 0x79, 0xF9, 0x01, 0x2A, 0x21, 0xB9, 0xB7, 0x87, 0x82, 0x31, 0x9D, 0x83, 0xA1, 0xF8, 0x36, 0x28, 0x66, 0xFB, 0xD6, 0xF4, 0x6B, 0xFC, 0x0D, 0xDB, 0x2E, 0x1A, 0xB6, 0xE4, 0xB4, 0x5A, 0x99, 0x06, 0xB8, 0x2E, 0x37, 0xF0, 0x5D, 0x6F, 0x97, 0xF6, 0xA3, 0xEB, 0x6E, 0x18, 0x20, 0x79, 0x75, 0x9C, 0x4F, 0x68, 0x47, 0x83, 0x7B, 0x62, 0x32, 0x1A, 0xC1, 0xB4, 0xFA, 0x68, 0x64, 0x1F, 0xCB, 0x4B, 0xB9, 0x8D, 0xD6, 0x97, 0xA0, 0xC7, 0x36, 0x41, 0x38, 0x5F, 0x4B, 0xAB, 0x25, 0xB7, 0x93, 0x58, 0x4C, 0xC3, 0x9F, 0xC8, 0xD4, 0x8D, 0x4B, 0xD8, 0x67, 0xA9, 0xA3, 0xC1, 0x0F, 0x8E, 0xA1, 0x21, 0x70, 0x26, 0x8E, 0x34, 0xFE, 0x3B, 0xBE, 0x6F, 0xF8, 0x99, 0x98, 0xD6, 0x0D, 0xA2, 0xF3, 0xE4, 0x28, 0x3C, 0xBE, 0xC1, 0x39, 0x3D, 0x52, 0xAF, 0x72, 0x4A, 0x57, 0x23, 0x0C, 0x60, 0x4E, 0x9F, 0xBC, 0xE5, 0x83, 0xD7, 0x61, 0x3E, 0x6B, 0xFF, 0xD6, 0x75, 0x96, 0xAD, 0x12, 0x1A, 0x87, 0x07, 0xEE, 0xC4, 0x69, 0x44, 0x95, 0x70, 0x33, 0x68, 0x6A, 0x15, 0x5F, 0x64, 0x4D, 0x5C, 0x58, 0x63, 0xB4, 0x8F, 0x61, 0xBD, 0xBF, 0x19, 0xA5, 0x3E, 0xAB, 0x6D, 0xAD, 0x0A, 0x18, 0x6B, 0x8C, 0x15, 0x2E, 0x5F, 0x5D, 0x8C, 0xAD, 0x4B, 0x0E, 0xF8, 0xAA, 0x4E, 0xA5, 0x00, 0x88, 0x34, 0xC3, 0xCD, 0x34, 0x2E, 0x5E, 0x0F, 0x16, 0x7A, 0xD0, 0x45, 0x92, 0xCD, 0x8B, 0xD2, 0x79, 0x63, 0x93, 0x98, 0xEF, 0x9E, 0x11, 0x4D, 0xFA, 0xAA, 0xB9, 0x19, 0xE1, 0x4E, 0x85, 0x09, 0x89, 0x22, 0x4D, 0xDD, 0x98, 0x57, 0x6D, 0x79, 0x38, 0x5D, 0x22, 0x10, 0x90, 0x2E, 0x9F, 0x9B, 0x1F, 0x2D, 0x86, 0xCF, 0xA4, 0x7E, 0xE2, 0x44, 0x63, 0x54, 0x65, 0xF7, 0x10, 0x58, 0x42, 0x1A, 0x01, 0x84, 0xBE, 0x51, 0xDD, 0x10, 0xCC, 0x9D, 0x07, 0x9E, 0x6F, 0x16, 0x04, 0xE7, 0xAA, 0x9B, 0x7C, 0xF7, 0x88, 0x3C, 0x7D, 0x4C, 0xE1, 0x2B, 0x06, 0xEB, 0xE1, 0x60, 0x81, 0xE2, 0x3F, 0x27, 0xA2, 0x31, 0xD1, 0x84, 0x32, 0xD7, 0xD1, 0xBB, 0x55, 0xC2, 0x8A, 0xE2, 0x1F, 0xFC, 0xF0, 0x05, 0xF5, 0x75, 0x28, 0xD1, 0x5A, 0x88, 0x88, 0x1B, 0xB3, 0xBB, 0xB7, 0xFE };

    _hapsrp->data->verifier = _hapsrp->newVerifier1(_hapsrp->data->session,
										USERNAME,
										0,
										_hapsrp->data->bytes_s,
										_hapsrp->data->len_s,
										_hapsrp->data->bytes_v,
										_hapsrp->data->len_v,
										device_public_key, decodedLen,
										NULL, NULL, _hapsrp->data->keys
									);

    TEST_ASSERT_NOT_NULL(_hapsrp->data->verifier);

    const uint8_t proofLen = 64;
    uint8_t proof[proofLen] = { 0x5F, 0x7C, 0x14, 0xAB, 0x57, 0xED, 0x0E, 0x94, 0xFD, 0x1D, 0x78, 0xC6, 0xB4, 0xDD, 0x09, 0xED, 0x7E, 0x34, 0x0B, 0x7E, 0x05, 0xD4, 0x19, 0xA9, 0xFD, 0x76, 0x0F, 0x6B, 0x35, 0xE5, 0x23, 0xD1, 0x31, 0x07, 0x77, 0xA1, 0xAE, 0x1D, 0x28, 0x26, 0xF5, 0x96, 0xF3, 0xA8, 0x51, 0x16, 0xCC, 0x45, 0x7C, 0x7C, 0x96, 0x4D, 0x4F, 0x44, 0xDE, 0xD5, 0x55, 0x9D, 0xA8, 0x18, 0xC8, 0x8B, 0x61, 0x7F };

    TEST_ASSERT_EQUAL_MEMORY(proof, _hapsrp->data->verifier->M, proofLen);

    bool result = _hapsrp->verifySession(_hapsrp->data->verifier, proof, NULL);
    TEST_ASSERT_TRUE(result);

    delete _hapsrp;
}

void test_srp_copyUsername(void){

    HAPSRP* _hapsrp = new HAPSRP();
    TEST_ASSERT_NOT_NULL(_hapsrp);
    _hapsrp->begin(USERNAME);

    _hapsrp->data->len_s = 16;

    _hapsrp->data->session = _hapsrp->newSession(SRP_SHA512, HAPSRP::SRP_NG_3072, NULL,NULL);
    TEST_ASSERT_NOT_NULL(_hapsrp->data->session);

    _hapsrp->createSaltedVerificationKey1(_hapsrp->data->session,
									USERNAME,
									(const uint8_t*)PASSWORD, strlen(PASSWORD),
									&_hapsrp->data->bytes_s, _hapsrp->data->len_s,
									&_hapsrp->data->bytes_v, &_hapsrp->data->len_v
									);

    TEST_ASSERT_NOT_NULL(_hapsrp->data->bytes_s);
    TEST_ASSERT_NOT_NULL(_hapsrp->data->bytes_v);


    const uint8_t saltlength = 16;
    uint8_t salt[saltlength] = { 0xBE, 0xB2, 0x53, 0x79, 0xD1, 0xA8, 0x58, 0x1E, 0xB5, 0xA7, 0x27, 0x67, 0x3A, 0x24, 0x41, 0xEE };

    TEST_ASSERT_EQUAL_MEMORY(salt, _hapsrp->data->bytes_s, saltlength);


    _hapsrp->data->keys = _hapsrp->createKeyPair(_hapsrp->data->session, _hapsrp->data->bytes_v, _hapsrp->data->len_v, &_hapsrp->data->bytes_B, &_hapsrp->data->len_B);
    TEST_ASSERT_NOT_NULL(_hapsrp->data->keys);

    const uint16_t decodedLen = 384;
    uint8_t device_public_key[decodedLen] = { 0xFA, 0xB6, 0xF5, 0xD2, 0x61, 0x5D, 0x1E, 0x32, 0x35, 0x12, 0xE7, 0x99, 0x1C, 0xC3, 0x74, 0x43, 0xF4, 0x87, 0xDA, 0x60, 0x4C, 0xA8, 0xC9, 0x23, 0x0F, 0xCB, 0x04, 0xE5, 0x41, 0xDC, 0xE6, 0x28, 0x0B, 0x27, 0xCA, 0x46, 0x80, 0xB0, 0x37, 0x4F, 0x17, 0x9D, 0xC3, 0xBD, 0xC7, 0x55, 0x3F, 0xE6, 0x24, 0x59, 0x79, 0x8C, 0x70, 0x1A, 0xD8, 0x64, 0xA9, 0x13, 0x90, 0xA2, 0x8C, 0x93, 0xB6, 0x44, 0xAD, 0xBF, 0x9C, 0x00, 0x74, 0x5B, 0x94, 0x2B, 0x79, 0xF9, 0x01, 0x2A, 0x21, 0xB9, 0xB7, 0x87, 0x82, 0x31, 0x9D, 0x83, 0xA1, 0xF8, 0x36, 0x28, 0x66, 0xFB, 0xD6, 0xF4, 0x6B, 0xFC, 0x0D, 0xDB, 0x2E, 0x1A, 0xB6, 0xE4, 0xB4, 0x5A, 0x99, 0x06, 0xB8, 0x2E, 0x37, 0xF0, 0x5D, 0x6F, 0x97, 0xF6, 0xA3, 0xEB, 0x6E, 0x18, 0x20, 0x79, 0x75, 0x9C, 0x4F, 0x68, 0x47, 0x83, 0x7B, 0x62, 0x32, 0x1A, 0xC1, 0xB4, 0xFA, 0x68, 0x64, 0x1F, 0xCB, 0x4B, 0xB9, 0x8D, 0xD6, 0x97, 0xA0, 0xC7, 0x36, 0x41, 0x38, 0x5F, 0x4B, 0xAB, 0x25, 0xB7, 0x93, 0x58, 0x4C, 0xC3, 0x9F, 0xC8, 0xD4, 0x8D, 0x4B, 0xD8, 0x67, 0xA9, 0xA3, 0xC1, 0x0F, 0x8E, 0xA1, 0x21, 0x70, 0x26, 0x8E, 0x34, 0xFE, 0x3B, 0xBE, 0x6F, 0xF8, 0x99, 0x98, 0xD6, 0x0D, 0xA2, 0xF3, 0xE4, 0x28, 0x3C, 0xBE, 0xC1, 0x39, 0x3D, 0x52, 0xAF, 0x72, 0x4A, 0x57, 0x23, 0x0C, 0x60, 0x4E, 0x9F, 0xBC, 0xE5, 0x83, 0xD7, 0x61, 0x3E, 0x6B, 0xFF, 0xD6, 0x75, 0x96, 0xAD, 0x12, 0x1A, 0x87, 0x07, 0xEE, 0xC4, 0x69, 0x44, 0x95, 0x70, 0x33, 0x68, 0x6A, 0x15, 0x5F, 0x64, 0x4D, 0x5C, 0x58, 0x63, 0xB4, 0x8F, 0x61, 0xBD, 0xBF, 0x19, 0xA5, 0x3E, 0xAB, 0x6D, 0xAD, 0x0A, 0x18, 0x6B, 0x8C, 0x15, 0x2E, 0x5F, 0x5D, 0x8C, 0xAD, 0x4B, 0x0E, 0xF8, 0xAA, 0x4E, 0xA5, 0x00, 0x88, 0x34, 0xC3, 0xCD, 0x34, 0x2E, 0x5E, 0x0F, 0x16, 0x7A, 0xD0, 0x45, 0x92, 0xCD, 0x8B, 0xD2, 0x79, 0x63, 0x93, 0x98, 0xEF, 0x9E, 0x11, 0x4D, 0xFA, 0xAA, 0xB9, 0x19, 0xE1, 0x4E, 0x85, 0x09, 0x89, 0x22, 0x4D, 0xDD, 0x98, 0x57, 0x6D, 0x79, 0x38, 0x5D, 0x22, 0x10, 0x90, 0x2E, 0x9F, 0x9B, 0x1F, 0x2D, 0x86, 0xCF, 0xA4, 0x7E, 0xE2, 0x44, 0x63, 0x54, 0x65, 0xF7, 0x10, 0x58, 0x42, 0x1A, 0x01, 0x84, 0xBE, 0x51, 0xDD, 0x10, 0xCC, 0x9D, 0x07, 0x9E, 0x6F, 0x16, 0x04, 0xE7, 0xAA, 0x9B, 0x7C, 0xF7, 0x88, 0x3C, 0x7D, 0x4C, 0xE1, 0x2B, 0x06, 0xEB, 0xE1, 0x60, 0x81, 0xE2, 0x3F, 0x27, 0xA2, 0x31, 0xD1, 0x84, 0x32, 0xD7, 0xD1, 0xBB, 0x55, 0xC2, 0x8A, 0xE2, 0x1F, 0xFC, 0xF0, 0x05, 0xF5, 0x75, 0x28, 0xD1, 0x5A, 0x88, 0x88, 0x1B, 0xB3, 0xBB, 0xB7, 0xFE };

    _hapsrp->data->verifier = _hapsrp->newVerifier1(_hapsrp->data->session,
										USERNAME,
										1,
										_hapsrp->data->bytes_s,
										_hapsrp->data->len_s,
										_hapsrp->data->bytes_v,
										_hapsrp->data->len_v,
										device_public_key, decodedLen,
										NULL, NULL, _hapsrp->data->keys
									);

    TEST_ASSERT_NOT_NULL(_hapsrp->data->verifier);


    // const uint16_t vlength = 384;
    // uint8_t verifier[vlength] = { 0x9B, 0x5E, 0x06, 0x17, 0x01, 0xEA, 0x7A, 0xEB, 0x39, 0xCF, 0x6E, 0x35, 0x19, 0x65, 0x5A, 0x85, 0x3C, 0xF9, 0x4C, 0x75, 0xCA, 0xF2, 0x55, 0x5E, 0xF1, 0xFA, 0xF7, 0x59, 0xBB, 0x79, 0xCB, 0x47, 0x70, 0x14, 0xE0, 0x4A, 0x88, 0xD6, 0x8F, 0xFC, 0x05, 0x32, 0x38, 0x91, 0xD4, 0xC2, 0x05, 0xB8, 0xDE, 0x81, 0xC2, 0xF2, 0x03, 0xD8, 0xFA, 0xD1, 0xB2, 0x4D, 0x2C, 0x10, 0x97, 0x37, 0xF1, 0xBE, 0xBB, 0xD7, 0x1F, 0x91, 0x24, 0x47, 0xC4, 0xA0, 0x3C, 0x26, 0xB9, 0xFA, 0xD8, 0xED, 0xB3, 0xE7, 0x80, 0x77, 0x8E, 0x30, 0x25, 0x29, 0xED, 0x1E, 0xE1, 0x38, 0xCC, 0xFC, 0x36, 0xD4, 0xBA, 0x31, 0x3C, 0xC4, 0x8B, 0x14, 0xEA, 0x8C, 0x22, 0xA0, 0x18, 0x6B, 0x22, 0x2E, 0x65, 0x5F, 0x2D, 0xF5, 0x60, 0x3F, 0xD7, 0x5D, 0xF7, 0x6B, 0x3B, 0x08, 0xFF, 0x89, 0x50, 0x06, 0x9A, 0xDD, 0x03, 0xA7, 0x54, 0xEE, 0x4A, 0xE8, 0x85, 0x87, 0xCC, 0xE1, 0xBF, 0xDE, 0x36, 0x79, 0x4D, 0xBA, 0xE4, 0x59, 0x2B, 0x7B, 0x90, 0x4F, 0x44, 0x2B, 0x04, 0x1C, 0xB1, 0x7A, 0xEB, 0xAD, 0x1E, 0x3A, 0xEB, 0xE3, 0xCB, 0xE9, 0x9D, 0xE6, 0x5F, 0x4B, 0xB1, 0xFA, 0x00, 0xB0, 0xE7, 0xAF, 0x06, 0x86, 0x3D, 0xB5, 0x3B, 0x02, 0x25, 0x4E, 0xC6, 0x6E, 0x78, 0x1E, 0x3B, 0x62, 0xA8, 0x21, 0x2C, 0x86, 0xBE, 0xB0, 0xD5, 0x0B, 0x5B, 0xA6, 0xD0, 0xB4, 0x78, 0xD8, 0xC4, 0xE9, 0xBB, 0xCE, 0xC2, 0x17, 0x65, 0x32, 0x6F, 0xBD, 0x14, 0x05, 0x8D, 0x2B, 0xBD, 0xE2, 0xC3, 0x30, 0x45, 0xF0, 0x38, 0x73, 0xE5, 0x39, 0x48, 0xD7, 0x8B, 0x79, 0x4F, 0x07, 0x90, 0xE4, 0x8C, 0x36, 0xAE, 0xD6, 0xE8, 0x80, 0xF5, 0x57, 0x42, 0x7B, 0x2F, 0xC0, 0x6D, 0xB5, 0xE1, 0xE2, 0xE1, 0xD7, 0xE6, 0x61, 0xAC, 0x48, 0x2D, 0x18, 0xE5, 0x28, 0xD7, 0x29, 0x5E, 0xF7, 0x43, 0x72, 0x95, 0xFF, 0x1A, 0x72, 0xD4, 0x02, 0x77, 0x17, 0x13, 0xF1, 0x68, 0x76, 0xDD, 0x05, 0x0A, 0xE5, 0xB7, 0xAD, 0x53, 0xCC, 0xB9, 0x08, 0x55, 0xC9, 0x39, 0x56, 0x64, 0x83, 0x58, 0xAD, 0xFD, 0x96, 0x64, 0x22, 0xF5, 0x24, 0x98, 0x73, 0x2D, 0x68, 0xD1, 0xD7, 0xFB, 0xEF, 0x10, 0xD7, 0x80, 0x34, 0xAB, 0x8D, 0xCB, 0x6F, 0x0F, 0xCF, 0x88, 0x5C, 0xC2, 0xB2, 0xEA, 0x2C, 0x3E, 0x6A, 0xC8, 0x66, 0x09, 0xEA, 0x05, 0x8A, 0x9D, 0xA8, 0xCC, 0x63, 0x53, 0x1D, 0xC9, 0x15, 0x41, 0x4D, 0xF5, 0x68, 0xB0, 0x94, 0x82, 0xDD, 0xAC, 0x19, 0x54, 0xDE, 0xC7, 0xEB, 0x71, 0x4F, 0x6F, 0xF7, 0xD4, 0x4C, 0xD5, 0xB8, 0x6F, 0x6B, 0xD1, 0x15, 0x81, 0x09, 0x30, 0x63, 0x7C, 0x01, 0xD0, 0xF6, 0x01, 0x3B, 0xC9, 0x74, 0x0F, 0xA2, 0xC6, 0x33, 0xBA, 0x89 };
    // TEST_ASSERT_EQUAL_MEMORY(verifier, _hapsrp->data->verifier, vlength);

    const uint8_t proofLen = 64;
    uint8_t proof[proofLen] = { 0x5F, 0x7C, 0x14, 0xAB, 0x57, 0xED, 0x0E, 0x94, 0xFD, 0x1D, 0x78, 0xC6, 0xB4, 0xDD, 0x09, 0xED, 0x7E, 0x34, 0x0B, 0x7E, 0x05, 0xD4, 0x19, 0xA9, 0xFD, 0x76, 0x0F, 0x6B, 0x35, 0xE5, 0x23, 0xD1, 0x31, 0x07, 0x77, 0xA1, 0xAE, 0x1D, 0x28, 0x26, 0xF5, 0x96, 0xF3, 0xA8, 0x51, 0x16, 0xCC, 0x45, 0x7C, 0x7C, 0x96, 0x4D, 0x4F, 0x44, 0xDE, 0xD5, 0x55, 0x9D, 0xA8, 0x18, 0xC8, 0x8B, 0x61, 0x7F };

    TEST_ASSERT_EQUAL_MEMORY(proof, _hapsrp->data->verifier->M, proofLen);

    bool result = _hapsrp->verifySession(_hapsrp->data->verifier, proof, NULL);
    TEST_ASSERT_TRUE(result);

    delete _hapsrp;
}

int runAllTests(){
    UNITY_BEGIN();

    for (int i=0; i < TEST_ITERATIONS; i++){
        RUN_TEST(test_srp);
        RUN_TEST(test_srp_copyUsername);
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

