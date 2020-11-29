

#include <stdio.h>
#include "m_X25519.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"

#if defined (ARDUINO_ARCH_ESP32)    
#include <esp_system.h>
#elif defined ( CORE_TEENSY)
#include "mbedtls/entropy_poll.h"
#endif


static int blinding_rng(void* context, uint8_t* buffer, size_t n) {
    
#if defined ( CORE_TEENSY)
    size_t olen;
    mbedtls_hardware_poll(NULL, buffer, n, &olen);
#elif defined (ARDUINO_ARCH_ESP32)    
    esp_fill_random(buffer, n);
#endif    
    return 0;
}

int X25519_scalarmult_base(uint8_t r[X25519_BYTES], const uint8_t n[X25519_SCALAR_BYTES]) {
    
    mbedtls_ecp_keypair key;
    mbedtls_ecp_keypair_init(&key);    
        
    int ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_CURVE25519, &key, n, X25519_SCALAR_BYTES);
    if (ret != 0) {
        // printf("ERROR at mbedtls_ecp_read_key\n");
        mbedtls_ecp_keypair_free(&key);
        return ret;
    }
    
    ret = mbedtls_ecp_mul(&key.grp, &key.Q, &key.d, &key.grp.G, blinding_rng, NULL);
    if (ret != 0) {
        // printf("ERROR at mbedtls_ecp_mul\n");
        mbedtls_ecp_keypair_free(&key);
        return ret;
    }
    
    size_t out_len;        
    ret = mbedtls_ecp_point_write_binary(&key.grp, &key.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &out_len, r, X25519_BYTES);        
    if (ret != 0) {
        // printf("ERROR at mbedtls_ecp_point_write_binary\n");
        mbedtls_ecp_keypair_free(&key);
        return ret;
    }

    if (out_len != X25519_BYTES) {
        ret = -1;
    }
    
    mbedtls_ecp_keypair_free(&key);
    return ret;    
    
}

int X25519_scalarmult( uint8_t r[X25519_BYTES], const uint8_t n[X25519_SCALAR_BYTES], const uint8_t p[X25519_BYTES]) {
    
    mbedtls_ecp_keypair our_key;

    mbedtls_ecp_keypair_init(&our_key);    

    int ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_CURVE25519, &our_key, n, X25519_SCALAR_BYTES);
    if (ret != 0) {
        mbedtls_ecp_keypair_free(&our_key);
        return ret;
    }

    mbedtls_ecp_keypair their_key;
    mbedtls_ecp_keypair_init(&their_key);   
    ret = mbedtls_ecp_group_load(&their_key.grp, MBEDTLS_ECP_DP_CURVE25519);        
    if (ret != 0) {
        mbedtls_ecp_keypair_free(&our_key);
        mbedtls_ecp_keypair_free(&their_key);
        return ret;
    }  
        
            
            
    ret = mbedtls_ecp_point_read_binary(&their_key.grp, &their_key.Q, p, X25519_BYTES);
    if (ret != 0) {
        mbedtls_ecp_keypair_free(&our_key);
        mbedtls_ecp_keypair_free(&their_key);
        return ret;
    }


    mbedtls_ecdh_context ecdh;
    mbedtls_ecdh_init(&ecdh);


            
    ret = mbedtls_ecdh_get_params(&ecdh, &their_key, MBEDTLS_ECDH_THEIRS);
    if (ret != 0) {
        mbedtls_ecdh_free(&ecdh);
        mbedtls_ecp_keypair_free(&our_key);
        mbedtls_ecp_keypair_free(&their_key);
        return ret;
    }
    
    ret = mbedtls_ecdh_get_params(&ecdh, &our_key, MBEDTLS_ECDH_OURS);
    if (ret != 0) {
        mbedtls_ecdh_free(&ecdh);
        mbedtls_ecp_keypair_free(&our_key);
        mbedtls_ecp_keypair_free(&their_key);
        return ret;
    }
    
    size_t out_len;            
    ret = mbedtls_ecdh_calc_secret(&ecdh, &out_len, r, X25519_BYTES, blinding_rng, NULL);
    if (ret != 0) {
        mbedtls_ecdh_free(&ecdh);
        mbedtls_ecp_keypair_free(&our_key);
        mbedtls_ecp_keypair_free(&their_key);
        return ret;
    }                        

    if (out_len != X25519_BYTES) {
        ret = -1;
    }        
    
    mbedtls_ecdh_free(&ecdh);
    mbedtls_ecp_keypair_free(&our_key);
    mbedtls_ecp_keypair_free(&their_key);
    return ret;
}
