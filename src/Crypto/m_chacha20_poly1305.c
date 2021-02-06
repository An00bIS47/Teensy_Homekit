#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if ARDUINO_ARCH_ESP32  
#include <esp_log.h>
#endif

#include "m_chacha20_poly1305.h"

#include "mbedtls/chachapoly.h"

#define TAG "CHACHA20_POLY1305"

static uint8_t nonce[][CHACHA20_POLY1305_NONCE_LENGTH] = {
    {0, 0, 0, 0, 'P', 'S', '-', 'M', 's', 'g', '0', '5'},
    {0, 0, 0, 0, 'P', 'S', '-', 'M', 's', 'g', '0', '6'},
    {0, 0, 0, 0, 'P', 'V', '-', 'M', 's', 'g', '0', '2'},
    {0, 0, 0, 0, 'P', 'V', '-', 'M', 's', 'g', '0', '3'},
};

static uint8_t* _type_to_nonce(enum chacha20_poly1305_type type) {
    return nonce[type];
}

int chacha20_poly1305_decrypt_with_nonce(uint8_t* curNonce, uint8_t* key, uint8_t* aad, int aad_len, uint8_t* encrypted, int encrypted_len, uint8_t* decrypted)
{
    // uint8_t* cipher_text = encrypted;
    int cipher_text_len = encrypted_len - CHACHA20_POLY1305_AUTH_TAG_LENGTH;
    uint8_t* auth_tag = encrypted + cipher_text_len;

	mbedtls_chachapoly_context chachapoly_ctx;
	mbedtls_chachapoly_init(&chachapoly_ctx);
	int err = mbedtls_chachapoly_setkey(&chachapoly_ctx, key);
    if (err < 0){
        mbedtls_chachapoly_free(&chachapoly_ctx);
        return err;
    }


    if (aad_len == 0) {
        err = mbedtls_chachapoly_auth_decrypt(&chachapoly_ctx,cipher_text_len,curNonce,aad,aad_len,auth_tag,encrypted,decrypted);
    } else {

        err = mbedtls_chachapoly_starts(&chachapoly_ctx, curNonce, MBEDTLS_CHACHAPOLY_DECRYPT);
        if (err < 0){
            mbedtls_chachapoly_free(&chachapoly_ctx);
            return err;
        }


        // if (aad_len > 0) {
        //     err = mbedtls_chachapoly_update_aad(&chachapoly_ctx, aad, aad_len);
        //     if (err < 0){
        //         mbedtls_chachapoly_free(&chachapoly_ctx);
        //         return err;
        //     }
        // }

        uint8_t tag[CHACHA20_POLY1305_AUTH_TAG_LENGTH];
        err = mbedtls_chachapoly_update(&chachapoly_ctx, encrypted_len, encrypted, decrypted);
        if (err < 0){
            mbedtls_chachapoly_free(&chachapoly_ctx);
            return err;
        }
        
        err = mbedtls_chachapoly_finish(&chachapoly_ctx, tag);
        if (err < 0) {
#if ARDUINO_ARCH_ESP32          
            ESP_LOGE(TAG, "mbedtls_chachapoly_auth_decrypt failed. err:%d\n", err);    
#else
            // printf("%s - mbedtls_chachapoly_auth_decrypt failed. err:%d\n", TAG, err);
#endif        
            mbedtls_chachapoly_free(&chachapoly_ctx);
            return err;
        }

        err = memcmp(auth_tag, tag, CHACHA20_POLY1305_AUTH_TAG_LENGTH);
    }

    mbedtls_chachapoly_free(&chachapoly_ctx);

    return err;
}

int chacha20_poly1305_decrypt(enum chacha20_poly1305_type type, uint8_t* key, 
        uint8_t* aad, int aad_len,
        uint8_t* encrypted, int encrypted_len, uint8_t* decrypted)
{
    uint8_t* usedNonce = _type_to_nonce(type);
    return chacha20_poly1305_decrypt_with_nonce(usedNonce, key, aad, aad_len, encrypted, encrypted_len, decrypted);
}

int chacha20_poly1305_encrypt_with_nonce(uint8_t curNonce[CHACHA20_POLY1305_NONCE_LENGTH], 
        uint8_t* key, uint8_t* aad, int aad_len,
        uint8_t* plain_text, int plain_text_length, 
        uint8_t* encrypted)
{
    uint8_t* auth_tag = encrypted + plain_text_length;

	mbedtls_chachapoly_context chachapoly_ctx;
	mbedtls_chachapoly_init(&chachapoly_ctx);
	mbedtls_chachapoly_setkey(&chachapoly_ctx,key);
	
	int err = mbedtls_chachapoly_encrypt_and_tag(&chachapoly_ctx,plain_text_length,curNonce,aad,aad_len,plain_text,encrypted,auth_tag);
    if (err < 0) {
        
#if ARDUINO_ARCH_ESP32         
        ESP_LOGE(TAG, "mbedtls_chachapoly_encrypt_and_tag failed. err:%d\n", err);
#else
        // printf("%s - mbedtls_chachapoly_encrypt_and_tag failed. err:%d\n", TAG, err);
#endif                
    }

    mbedtls_chachapoly_free(&chachapoly_ctx);
    return err;
}


int chacha20_poly1305_encrypt(enum chacha20_poly1305_type type, uint8_t* key, 
        uint8_t* aad, int aad_len,
        uint8_t* plain_text, int plain_text_length, 
        uint8_t* encrypted)
{

	mbedtls_chachapoly_context chachapoly_ctx;
	mbedtls_chachapoly_init(&chachapoly_ctx);
	mbedtls_chachapoly_setkey(&chachapoly_ctx,key);
	unsigned char *tag = encrypted + plain_text_length;
	
    uint8_t* usedNonce = _type_to_nonce(type);
    int err = mbedtls_chachapoly_encrypt_and_tag(&chachapoly_ctx,plain_text_length,usedNonce,aad,aad_len,plain_text,encrypted,tag);

    mbedtls_chachapoly_free(&chachapoly_ctx);
    return err;
}
