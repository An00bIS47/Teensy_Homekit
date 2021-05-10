// 
// m_ed25519.h
// Teensy_Homekit
//
//  Created on: 10.05.2021
//      Author: michael
//
#ifndef _M_ED25519_H_
#define _M_ED25519_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Ed25519/curve25519_mehdi.h"
// #include "Ed25519/curve25519_utils.c"
// #include "Ed25519/curve25519_order.c"
// #include "Ed25519/ed25519_sign.c"
// #include "Ed25519/ed25519_verify.c"
#include <stdint.h>

#define ED25519_PUBLIC_KEY_BYTES    32
#define ED25519_PRIVATE_KEY_BYTES   64
#define ED25519_SECRET_KEY_BYTES    32
#define ED25519_BYTES               64

void ed25519_key_generate(uint8_t pk[ED25519_PUBLIC_KEY_BYTES], const uint8_t sk[ED25519_SECRET_KEY_BYTES]);
int ed25519_verify(const uint8_t sig[ED25519_BYTES], const uint8_t* m, size_t m_len, const uint8_t pk[ED25519_PUBLIC_KEY_BYTES]);
void ed25519_sign(uint8_t sig[ED25519_BYTES], const uint8_t* m, size_t m_len, const uint8_t sk[ED25519_SECRET_KEY_BYTES], const uint8_t pk[ED25519_PUBLIC_KEY_BYTES]);

#ifdef __cplusplus
}
#endif

#endif //#ifndef _M_ED25519_H_
