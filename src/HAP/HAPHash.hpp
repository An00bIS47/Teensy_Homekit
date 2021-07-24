//
// HAPHash.hpp
// Homekit
//
//  Created on: 01.01.2021
//      Author: michael
//

#ifndef HAPHASH_HPP_
#define HAPHASH_HPP_

#include <Arduino.h>

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/bignum.h"

#define SHA1_DIGEST_LENGTH      20
#define SHA224_DIGEST_LENGTH    28
#define SHA256_DIGEST_LENGTH    32
#define SHA384_DIGEST_LENGTH    48
#define SHA512_DIGEST_LENGTH    64


struct HAPHashAlgorithm {
    enum Type : uint8_t {
        Sha1, 
        Sha224,
        Sha256,
        Sha384,
        Sha512,
        Last
    };
};

typedef union {
    mbedtls_sha1_context   sha;
    mbedtls_sha256_context sha256;
    mbedtls_sha512_context sha512;
} HAPHashContext;




class HAPHash {
public:


    explicit HAPHash(HAPHashAlgorithm::Type algorithm);
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

    size_t digestLength(){
        return digestLength(_hashAlgorithm);
    }

    HAPHashAlgorithm::Type algorithm() { return _hashAlgorithm; }

    static size_t digestLength( HAPHashAlgorithm::Type alg );
    static void init( HAPHashAlgorithm::Type alg, HAPHashContext* context);
    static void update( HAPHashAlgorithm::Type alg, HAPHashContext *context, const uint8_t *data, size_t len );
    static void final( HAPHashAlgorithm::Type alg, HAPHashContext *context, uint8_t* md );
    static void clear( HAPHashAlgorithm::Type alg, HAPHashContext* context);
    static void hash( HAPHashAlgorithm::Type alg, const uint8_t* d, size_t n, uint8_t* md );

protected:
    HAPHashAlgorithm::Type _hashAlgorithm;
    HAPHashContext*  _context;

};


#endif /* HAPHASH_HPP_ */