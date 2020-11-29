
#include <stdio.h>
#include "m_ed25519.h"
#include "Ed25519/curve25519_utils.c"
#include "Ed25519/curve25519_order.c"
#include "Ed25519/ed25519_sign.c"
#include "Ed25519/ed25519_verify.c"
#include "mbedtls/entropy_poll.h"

void ed25519_key_generate(uint8_t pk[ED25519_PUBLIC_KEY_BYTES], const uint8_t sk[ED25519_SECRET_KEY_BYTES]){
    uint8_t seed[ED25519_SECRET_KEY_BYTES];

    size_t olen;    
    mbedtls_hardware_poll(NULL, seed, sizeof seed, &olen);
    
    uint8_t privKey[ED25519_PRIVATE_KEY_BYTES];
    
    EDP_BLINDING_CTX ctx; 

    ed25519_Blinding_Init(&ctx, seed, sizeof seed); 
    ed25519_CreateKeyPair(pk, privKey, &ctx, sk);
    ed25519_Blinding_Finish(&ctx);

    memset(privKey, 0, ED25519_PRIVATE_KEY_BYTES);
}


int ed25519_verify(const uint8_t sig[ED25519_BYTES], const uint8_t* m, size_t m_len, const uint8_t pk[ED25519_PUBLIC_KEY_BYTES]) {
    int ret = ed25519_VerifySignature(sig, pk, m, m_len);
    return (ret == 1) ? 0 : -1;
}

void ed25519_sign(uint8_t sig[ED25519_BYTES], const uint8_t* m, size_t m_len, const uint8_t sk[ED25519_SECRET_KEY_BYTES], const uint8_t pk[ED25519_PUBLIC_KEY_BYTES]){
    uint8_t seed[ED25519_SECRET_KEY_BYTES];
    size_t olen;
    mbedtls_hardware_poll(NULL, seed, sizeof seed, &olen);
    
    uint8_t privKey[ED25519_PRIVATE_KEY_BYTES];
    
    EDP_BLINDING_CTX ctx; 

    memcpy(privKey, sk, ED25519_SECRET_KEY_BYTES);
    memcpy(privKey + ED25519_SECRET_KEY_BYTES, pk, ED25519_PUBLIC_KEY_BYTES);

    ed25519_Blinding_Init(&ctx, seed, sizeof seed); 
    ed25519_SignMessage(sig, privKey, &ctx, m, m_len);
    ed25519_Blinding_Finish(&ctx);
    memset(privKey, 0, ED25519_PRIVATE_KEY_BYTES);
}
