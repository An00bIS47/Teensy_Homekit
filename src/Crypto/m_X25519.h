
#ifndef _M_X25519_H_
#define _M_X25519_H_

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#define X25519_SCALAR_BYTES 32
#define X25519_BYTES        32

int X25519_scalarmult_base(uint8_t r[X25519_BYTES], const uint8_t n[X25519_SCALAR_BYTES]);
int X25519_scalarmult( uint8_t r[X25519_BYTES], const uint8_t n[X25519_SCALAR_BYTES], const uint8_t p[X25519_BYTES]);

#ifdef __cplusplus
}
#endif

#endif //#ifndef _M_X25519_H_
