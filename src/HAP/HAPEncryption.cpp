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



#if HAP_USE_LIBSODIUM

int HAPEncryption::begin(){

    int result = sodium_init();

    if (result < 0) {
        /* panic! the library couldn't be initialized, it is not safe to use */
        LogE("[ERROR] Sodium couldn't be initialized!", true);
    }

    return result;
}


int HAPEncryption::pad(size_t *padded_buflen_p, uint8_t *msg, 
        const uint8_t *buf, size_t unpadded_buflen, size_t blocksize, 
        size_t max_msglen, bool zeroPadded )
{
    unsigned char          *tail;
    size_t                  i;
    size_t                  xpadlen;
    size_t                  xpadded_len;
    volatile unsigned char  mask;
    unsigned char           barrier_mask;

    if (blocksize <= 0U) {
        return -1;
    }
    xpadlen = blocksize - 1U;

    if ((blocksize & (blocksize - 1U)) == 0U) {
        xpadlen -= unpadded_buflen & (blocksize - 1U);
    } else {
        xpadlen -= unpadded_buflen % blocksize;
    }
    if ((size_t) SIZE_MAX - unpadded_buflen <= xpadlen) {
        //ESP_LOGE("Missuse");
        return -1;
    }
    xpadded_len = unpadded_buflen + xpadlen;
    

    if (max_msglen != 0) {
        if (xpadded_len >= max_msglen) {
            return -1;
        } 
    }
   
    
    if (padded_buflen_p != NULL) {
        *padded_buflen_p = xpadded_len + 1U;
    }
    
    if (msg == NULL) {
        return -2;
    }


    memcpy(msg, buf, unpadded_buflen);
    tail = &msg[xpadded_len];

    mask = 0U;
    for (i = 0; i < blocksize; i++) {
        barrier_mask = (unsigned char) (((i ^ xpadlen) - 1U) >> 8);

        if (zeroPadded) {
            tail[-i] = (tail[-i] & mask) | (0x00 & barrier_mask);
        } else {
            tail[-i] = (tail[-i] & mask) | (0x80 & barrier_mask);    
        }
        
        mask |= barrier_mask;
    }
    return 0;
}




int HAPEncryption::unpad(size_t *unpadded_buflen_p, const unsigned char *buf,
             size_t padded_buflen, size_t blocksize)
{
    const unsigned char *tail;
    unsigned char        acc = 0U;
    
    unsigned char        valid = 0U;
    volatile size_t      pad_len = 0U;
    size_t               i;
    

    if (padded_buflen < blocksize || blocksize <= 0U) {
        return -1;
    }
    tail = &buf[padded_buflen - 1U];

    for (i = 0U; i < blocksize; i++) {

        unsigned char        c;
        size_t               is_barrier;

        c = tail[-i];
        is_barrier =
            (( (acc - 1U) & (pad_len - 1U) & ((c ^ 0x80) - 1U) ) >> 8) & 1U;
        acc |= c;
        pad_len |= i & (1U + ~is_barrier);
        valid |= (unsigned char) is_barrier;
    }
    *unpadded_buflen_p = padded_buflen - 1U - pad_len;

    return (int) (valid - 1U);
}




int HAPEncryption::computePoly1305(uint8_t* hmac, uint8_t* cipherText, 
            size_t cipherTextLength, uint8_t* AAD, uint8_t *nonce, 
            uint8_t *key) {

    begin();

    if (AAD == nullptr) {
        //AAD = Buffer.alloc(0);
    }

    size_t block_size = 16;

    int paddedCipherLength  = paddedLength(cipherTextLength, block_size);
    int paddedAADLength     = paddedLength(HAP_ENCRYPTION_AAD_SIZE, block_size);

    int paddedAADLengthNum       = paddedLength(HAP_ENCRYPTION_AAD_SIZE, 8);
    int paddedCipherLengthNum    = paddedLength( HAPHelper::numDigits(paddedCipherLength), 8);

    int paddedLength        = paddedAADLength 
                            + paddedCipherLength 
                            + paddedAADLengthNum 
                            + paddedCipherLengthNum;


    uint8_t msg[paddedLength];
    
#if HAP_DEBUG_ENCRYPTION    
    Serial.printf("paddedLength: %d\n", paddedLength);
#endif
    
    int aad_len = HAP_ENCRYPTION_AAD_SIZE;

    memcpy(msg, AAD, aad_len);
    memcpy(msg + paddedAADLength, cipherText, cipherTextLength);
    memcpy(msg + paddedAADLength + paddedCipherLength, &aad_len, 1);
    memcpy(msg + paddedAADLength + paddedCipherLength + paddedAADLengthNum, &cipherTextLength, HAPHelper::numDigits(cipherTextLength) );


#if HAP_DEBUG_ENCRYPTION
    Serial.printf("msg: %d = %d\n", paddedLength, sizeof msg);
    HAPHelper::arrayPrint(msg, paddedLength);
#endif

#if HAP_USE_LIBSODIUM_1
    uint8_t polyKey[HAP_ENCRYPTION_KEY_SIZE] = { 0, };
    if ( crypto_stream_chacha20_ietf(polyKey, HAP_ENCRYPTION_KEY_SIZE, nonce, key) != 0 ) {
        LogE("[ERROR] Generating polyKey failed!", true);
        return -1;
    }
#else
    // ToDo: 
#endif


#if HAP_DEBUG_ENCRYPTION    
    Serial.println("polyKey: ");
    HAPHelper::arrayPrint(polyKey, HAP_ENCRYPTION_KEY_SIZE);
#endif

    // uint8_t hmac[crypto_onetimeauth_BYTES];

#if HAP_USE_LIBSODIUM_1
    if ( crypto_onetimeauth(hmac, msg, paddedLength, polyKey) != 0 ) {
        LogE("[ERROR] Generating crypto_onetimeauth!", true);
        return -1;
    }
#else
    // ToDo: 
#endif    
    
#if HAP_DEBUG_ENCRYPTION    
    Serial.println("generated hmac:");
    HAPHelper::arrayPrint(hmac, crypto_onetimeauth_BYTES);
    //Serial.printf("msg: %d\n", sizeof msg);
    //HAPHelper::arrayPrint(msg, sizeof msg);
#endif


    return 0;
}


// i'd really prefer for this to be a direct call to
// Sodium.crypto_aead_chacha20poly1305_decrypt()
// but unfortunately the way it constructs the message to
// calculate the HMAC is not compatible with homekit
// (long story short, it uses [ AAD, AAD.length, CipherText, CipherText.length ]
// whereas homekit expects [ AAD, CipherText, AAD.length, CipherText.length ]
// 
// function verifyAndDecrypt(cipherText, mac, AAD, nonce, key) {
//     const matches =
//         Sodium.crypto_verify_16(
//             mac,
//             computePoly1305(cipherText, AAD, nonce, key)
//         );
//
//     if (matches === 0) {
//         return Sodium
//             .crypto_stream_chacha20_xor_ic(cipherText, nonce, 1, key);
//     }
//
//     return null;
// }
int HAPEncryption::verifyAndDecrypt(uint8_t *plainText, uint8_t cipherText[], 
            uint16_t length, uint8_t mac[], uint8_t aad[], 
            int decryptCount, uint8_t key[]){


    begin();

    if ( length > 1024 + HAP_ENCRYPTION_AAD_SIZE + HAP_ENCRYPTION_HMAC_SIZE ){
        LogE("NOWNOW!!", true);
    }
    
    // nonce
    // the nonce is 12 byte long
    // 
    // the first 4 bytes are always 
    //      
    //      00 00 00 00 
    // 
    // the nonce is incremented each time a decryption with the same
    // encryption key is performed
    // 
    // !!! the counter starts at the last bits (10, 11) (!started by 0!)
    // 
    // thus the remaining 8 bytes will look as follows
    // 
    //      | 00 00 00 00 | 00 00 00 00 00 00 00 01 
    //          first 4       remaining 8 bytes   ^
    //                                            | -> this will be incremented each time this function is called
    //  
    //                                               
    uint8_t nonce[HAP_ENCRYPTION_NONCE_SIZE] = { 0, };            
    nonce[4] = decryptCount % 256;
    nonce[5] = decryptCount++ / 256;
    

#if HAP_DEBUG_ENCRYPTION    
    LogD("decryptCount: " + String(decryptCount), true);

    Serial.println("nonce:");
    HAPHelper::arrayPrint(nonce, HAP_ENCRYPTION_NONCE_SIZE);

    Serial.printf("cipherText: %d\n", length);
    HAPHelper::arrayPrint(cipherText, length);

    Serial.println("AAD:");
    HAPHelper::arrayPrint(aad, HAP_ENCRYPTION_AAD_SIZE);

    Serial.println("mac:");
    HAPHelper::arrayPrint(mac, HAP_ENCRYPTION_HMAC_SIZE);

    Serial.println("key:");
    HAPHelper::arrayPrint(key, HAP_ENCRYPTION_KEY_SIZE);

#endif

    // 16 bytes long
    uint8_t hmac[HAP_ENCRYPTION_HMAC_SIZE] = {0,};


    if ( computePoly1305(hmac, cipherText, length, aad, nonce, key) != 0 ) {
        LogE("[ERROR] computePoly1305 failed!", true);
        return -1;
    }

#if HAP_DEBUG_ENCRYPTION  
    Serial.println("computed hmac:");
    HAPHelper::arrayPrint(hmac, HAP_ENCRYPTION_HMAC_SIZE);
#endif



    if ( crypto_verify_16(mac, hmac) != 0 ) {
#if !HAP_ENCRYPTION_SUPPRESS_WARNINGS      
        LogW("[WARNING] crypto_verify_16 failed! - Trying to decrypt it anyway ...", true);
#endif

#if HAP_ENCRYPTION_EXIT_ON_FAILURE      
        return -1;
#endif
    }

    
    // 
    // The output from the AEAD is twofold:
    //   -  A ciphertext of the same length as the plaintext.
    //   -  A 128-bit tag, which is the output of the Poly1305 function.
    // 

    if ( crypto_stream_chacha20_ietf_xor_ic(plainText, cipherText, length, nonce, 1, key) != 0 ) {
        LogE("[ERROR] crypto_stream_chacha20_xor_ic failed!", true);
        return -1;
    }

#if HAP_DEBUG_ENCRYPTION  
    Serial.printf("plainText: %d\n", length );
    HAPHelper::arrayPrint(plainText, length);
    Serial.println((char*) plainText);
#endif
    
    return 0;
}
#endif


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