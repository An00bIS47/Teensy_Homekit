//
// HAPEncryption.cpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//

#include "HAPEncryption.hpp"
#include "HAPLogger.hpp"
#include "HAPHelper.hpp"
#include "HAPGlobals.hpp"



#define TAG "CHACHA20_POLY1305"



int HAPEncryption::decrypt(const uint8_t tag[CHACHA20_POLY1305_TAG_BYTES],
        uint8_t* m,
        const uint8_t* c,
        size_t c_len,
        const uint8_t* a,
        size_t a_len,
        const uint8_t* n,
        size_t n_len,
        const uint8_t k[CHACHA20_POLY1305_KEY_BYTES]){

    mbedtls_chachapoly_context ctx;

    int ret;

    mbedtls_chachapoly_init(&ctx);
    ret = mbedtls_chachapoly_setkey(&ctx, k);
    if (ret != 0){
        mbedtls_chachapoly_free(&ctx);
        return -1;
    }

    if (a_len > 0) {
        ret = chacha20_poly1305_update_aad(&ctx, MBEDTLS_CHACHAPOLY_DECRYPT, a, a_len, n, n_len, k);
        if (ret != 0){
            mbedtls_chachapoly_free(&ctx);
            return -1;
        }

    }
    ret = chacha20_poly1305_update(&ctx, MBEDTLS_CHACHAPOLY_DECRYPT, m, c, c_len, n, n_len, k);

    if (ret != 0) {
        mbedtls_chachapoly_free(&ctx);
        return -1;

    }

    uint8_t tag2[CHACHA20_POLY1305_TAG_BYTES];
    ret = mbedtls_chachapoly_finish(&ctx, tag2);
    mbedtls_chachapoly_free(&ctx);

    if (ret != 0) {
        return -1;
    }

    return memcmp(tag, tag2, CHACHA20_POLY1305_TAG_BYTES) == 0;
}


int HAPEncryption::chacha20_poly1305_update_aad(
        mbedtls_chachapoly_context* ctx,
        mbedtls_chachapoly_mode_t mode,
        const uint8_t* a,
        size_t a_len,
        const uint8_t* n,
        size_t n_len,
        const uint8_t k[CHACHA20_POLY1305_KEY_BYTES]) {
    if (chacha20_poly1305_update(ctx, mode, NULL, NULL, 0, n, n_len, k) != 0){
        return -1;
    }

    return mbedtls_chachapoly_update_aad(ctx, a, a_len);
}


int HAPEncryption::chacha20_poly1305_update( mbedtls_chachapoly_context* ctx, mbedtls_chachapoly_mode_t mode, uint8_t* output, const uint8_t* input, size_t input_len, const uint8_t* n, size_t n_len, const uint8_t k[CHACHA20_POLY1305_KEY_BYTES]){

    int ret;


    if (n_len >= CHACHA20_POLY1305_NONCE_BYTES_MAX) {
        n_len = CHACHA20_POLY1305_NONCE_BYTES_MAX;
    }
    // // pad nonce
    // uint8_t nonce[CHACHA20_POLY1305_NONCE_BYTES_MAX];
    // memset(nonce, 0, sizeof nonce);
    // memcpy(nonce + sizeof nonce - n_len, n, n_len);

    ret = mbedtls_chachapoly_starts(ctx, n, mode);
    if (ret != 0){
        return -1;
    }

    if (input_len > 0) {
        ret = mbedtls_chachapoly_update(ctx, input_len, input, output);
        if (ret != 0){
            return -1;
        }
    }
    return 0;
}



// ToDo: Move to hapClient ?
/**
 * @brief
 *
 * @param message
 * @param length
 * @param encrypted_len
 * @param key
 * @param encryptCount
 * @return char*
 */
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
uint8_t* HAPEncryption::encrypt(uint8_t *message, size_t length, int* encrypted_len, uint8_t* key, uint16_t encryptCount) {

	// ToDo: Take care of bigger than hap encryp buffer
    //uint32_t tmp = length + (length / HAP_ENCRYPTION_BUFFER_SIZE + 1) * (HAP_ENCRYPTION_AAD_SIZE + CHACHA20_POLY1305_AUTH_TAG_LENGTH) + 1;
    // Serial.printf(">>>>>>>> tmp:                                %d\n", tmp);
    // Serial.printf(">>>>>>>> length:                             %d\n", length);
    // Serial.printf(">>>>>>>> HAP_ENCRYPTION_BUFFER_SIZE:         %d\n", HAP_ENCRYPTION_BUFFER_SIZE);
    // Serial.printf(">>>>>>>> HAP_ENCRYPTION_AAD_SIZE:            %d\n", HAP_ENCRYPTION_AAD_SIZE);
    // Serial.printf(">>>>>>>> CHACHA20_POLY1305_AUTH_TAG_LENGTH:  %d\n", CHACHA20_POLY1305_AUTH_TAG_LENGTH);

    uint32_t tmp = (length + HAP_ENCRYPTION_AAD_SIZE + CHACHA20_POLY1305_AUTH_TAG_LENGTH) < HAP_ENCRYPTION_BUFFER_SIZE ? length + HAP_ENCRYPTION_AAD_SIZE + CHACHA20_POLY1305_AUTH_TAG_LENGTH : HAP_ENCRYPTION_BUFFER_SIZE;
    // Serial.printf(">>>>>>>> tmp:                                %d\n", tmp);

    uint8_t* encrypted = (uint8_t*) calloc(1, tmp);

	uint8_t nonce[12] = {0,};
	uint8_t* decrypted_ptr = (uint8_t*)message;
	uint8_t* encrypted_ptr = (uint8_t*)encrypted;

	while (length > 0) {
        int err_code = 0;

		int chunk_len = (length < HAP_ENCRYPTION_BUFFER_SIZE) ? length : HAP_ENCRYPTION_BUFFER_SIZE;
		length -= chunk_len;

		uint8_t aad[HAP_ENCRYPTION_AAD_SIZE];
		aad[0] = chunk_len % 256;
		aad[1] = chunk_len / 256;

		memcpy(encrypted_ptr, aad, HAP_ENCRYPTION_AAD_SIZE);
		encrypted_ptr += HAP_ENCRYPTION_AAD_SIZE;
		*encrypted_len += HAP_ENCRYPTION_AAD_SIZE;

		nonce[4] = encryptCount % 256;
		nonce[5] = encryptCount++ / 256;


#if HAP_DEBUG_ENCRYPTION
        Serial.println("=========================================================");
        HAPHelper::array_print("nonce", nonce, 12);
        HAPHelper::array_print("key", key, 32);
        HAPHelper::array_print("aad", aad, HAP_ENCRYPTION_AAD_SIZE);
        HAPHelper::array_print("decrypted_ptr", decrypted_ptr, strlen((const char*)decrypted_ptr));
        printf("chunk_len %d\n", chunk_len);
        printf("length %zu\n", length);
#endif
		err_code = chacha20_poly1305_encrypt_with_nonce(nonce, key, aad, HAP_ENCRYPTION_AAD_SIZE, decrypted_ptr, chunk_len, encrypted_ptr);

#if HAP_DEBUG_ENCRYPTION
        HAPHelper::array_print("encrypted_ptr", encrypted_ptr, chunk_len + 16);
        Serial.println("=========================================================");
#endif

		if (err_code != 0 ) {
			LogE("[ERROR] Encrypting failed!", true);
		}

		decrypted_ptr += chunk_len;
		encrypted_ptr += chunk_len + CHACHA20_POLY1305_AUTH_TAG_LENGTH;
		*encrypted_len += (chunk_len + CHACHA20_POLY1305_AUTH_TAG_LENGTH);
	}

	//_pairSetup->encryptCount = 0;

	return encrypted;
}