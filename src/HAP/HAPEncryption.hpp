//
// HAPEncryption.hpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//

#ifndef HAPENCRYPTION_HPP_
#define HAPENCRYPTION_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"




#include "m_chacha20_poly1305.h"
#include "mbedtls/chachapoly.h"



#ifndef HAP_ENCRYPTION_AAD_SIZE
#define HAP_ENCRYPTION_AAD_SIZE 			2
#endif

#ifndef HAP_ENCRYPTION_NONCE_SIZE
#define HAP_ENCRYPTION_NONCE_SIZE 			12		// Don't change!
#endif

#ifndef HAP_ENCRYPTION_HMAC_SIZE
#define HAP_ENCRYPTION_HMAC_SIZE			16		// Don't change!
#endif

#ifndef HAP_ENCRYPTION_KEY_SIZE
#define HAP_ENCRYPTION_KEY_SIZE				32		// Don't change!
#endif

#ifndef CHACHA20_POLY1305_AUTH_TAG_LENGTH
#define CHACHA20_POLY1305_AUTH_TAG_LENGTH	16
#endif

#ifndef CHACHA20_POLY1305_NONCE_LENGTH
#define CHACHA20_POLY1305_NONCE_LENGTH      12
#endif

#define CHACHA20_POLY1305_KEY_BYTES       32
#define CHACHA20_POLY1305_NONCE_BYTES_MAX 12
#define CHACHA20_POLY1305_TAG_BYTES       16

// #define HAP_ENCRYPTION_AAD_SIZE 			2		// Don't change!


#define HAP_ENCRYPTION_DEBUG				0		// Be careful, it will print all keys on console
#define HAP_ENCRYPTION_EXIT_ON_FAILURE 		0		// 0 = ignore
													// 1 = Don't ignore and return failure

#define HAP_ENCRYPTION_SUPPRESS_WARNINGS 	1		// Surpress crypto_verify_16 warning

class HAPEncryption {

public:



	static int decrypt(const uint8_t tag[CHACHA20_POLY1305_TAG_BYTES],
			uint8_t* m,
			const uint8_t* c,
			size_t c_len,
			const uint8_t* a,
			size_t a_len,
			const uint8_t* n,
			size_t n_len,
			const uint8_t k[CHACHA20_POLY1305_KEY_BYTES]);

	// not needed
	// static size_t encrypt(uint8_t *message, size_t length, uint8_t* buffer, uint8_t* key, uint16_t encryptCount);

	static uint8_t* encrypt(uint8_t *message, size_t length, int* encrypted_len, uint8_t* key, uint16_t encryptCount);

	static int chacha20_poly1305_update_aad(
        mbedtls_chachapoly_context* ctx,
        mbedtls_chachapoly_mode_t mode,
        const uint8_t* a,
        size_t a_len,
        const uint8_t* n,
        size_t n_len,
        const uint8_t k[CHACHA20_POLY1305_KEY_BYTES]);

	static int chacha20_poly1305_update(
        mbedtls_chachapoly_context* ctx,
        mbedtls_chachapoly_mode_t mode,
        uint8_t* output,
        const uint8_t* input,
        size_t input_len,
        const uint8_t* n,
        size_t n_len,
        const uint8_t k[CHACHA20_POLY1305_KEY_BYTES]);
};




#endif /* HAPENCRYPTION_HPP_ */