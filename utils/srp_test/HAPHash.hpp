//
// HAPHash.hpp
// Homekit
//
//  Created on: 01.01.2021
//      Author: michael
//

#ifndef HAPHASH_HPP_
#define HAPHASH_HPP_

//#include <Arduino.h>

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/bignum.h"

#define SHA1_DIGEST_LENGTH      20
#define SHA224_DIGEST_LENGTH    28
#define SHA256_DIGEST_LENGTH    32
#define SHA384_DIGEST_LENGTH    48
#define SHA512_DIGEST_LENGTH    64

typedef enum {
    SRP_SHA1,
    SRP_SHA224,
    SRP_SHA256,
    SRP_SHA384,
    SRP_SHA512,
    SRP_SHA_LAST
} HAPHashAlgorithm;

typedef union {
    mbedtls_sha1_context   sha;
    mbedtls_sha256_context sha256;
    mbedtls_sha512_context sha512;
} HAPHashContext;




class HAPHash {
public:
    explicit HAPHash(HAPHashAlgorithm algorithm);
    ~HAPHash();

    void init();
    void clear();

    void update(const uint8_t* data, size_t len){
        update(_hashAlgorithm, _context, data, len);
    }

    void update(const char* data, size_t len){
        update(_hashAlgorithm, _context, (const uint8_t*) data, len);
    }


    void final(uint8_t* md){
        final(_hashAlgorithm, _context, md);
    }

    void hash( const uint8_t* d, size_t n, uint8_t* md ){
        hash(_hashAlgorithm, d, n, md);
    }

     void hash( const char* d, size_t n, uint8_t* md ){
        hash(_hashAlgorithm, (const uint8_t*)d, n, md);
    }

    void hash_num( const mbedtls_mpi * n, uint8_t* dest );
    void update_hash_n( const mbedtls_mpi * n );

    uint8_t length(){
        return hashLength(_hashAlgorithm);
    }

    HAPHashAlgorithm algorithm() { return _hashAlgorithm; }

    static uint8_t hashLength( HAPHashAlgorithm alg );
    static void init( HAPHashAlgorithm alg, HAPHashContext* context);
    static void update( HAPHashAlgorithm alg, HAPHashContext *context, const uint8_t *data, size_t len );
    static void final( HAPHashAlgorithm alg, HAPHashContext *context, uint8_t* md );
    static void clear( HAPHashAlgorithm alg, HAPHashContext* context);
    static void hash( HAPHashAlgorithm alg, const uint8_t* d, size_t n, uint8_t* md );

protected:
    HAPHashAlgorithm _hashAlgorithm;
    HAPHashContext*  _context;

};


#endif /* HAPHASH_HPP_ */