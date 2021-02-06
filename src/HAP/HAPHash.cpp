// 
// HAPHash.cpp
// Homekit
//
//  Created on: 01.01.2021
//      Author: michael
//

#include "HAPHash.hpp"


HAPHash::HAPHash(HAPHashAlgorithm algorithm){
    _hashAlgorithm = algorithm;  
    _context = nullptr;
}

HAPHash::~HAPHash(){
    clear(_hashAlgorithm, _context);     
    if (_context) delete _context;
}

void HAPHash::init(){
    if (_context == nullptr) _context = new HAPHashContext();   
    init(_hashAlgorithm, _context);
}

void HAPHash::clear(){
    clear(_hashAlgorithm, _context);
}

void HAPHash::clear(HAPHashAlgorithm alg, HAPHashContext* context){
    if (context == nullptr) return;

	switch (alg) {
		case SRP_SHA1  : {
			mbedtls_sha1_free( &context->sha );
			break;
		}
		case SRP_SHA224:
		case SRP_SHA256: {
			mbedtls_sha256_free( &context->sha256 );
			break;
		}
		case SRP_SHA384:
		case SRP_SHA512:{
			mbedtls_sha512_free( &context->sha512 );
			break;
		}
    	default:
			return;
	};
}

void HAPHash::update_hash_n( const mbedtls_mpi * n )
{
    uint32_t len = mbedtls_mpi_size(n);
    uint8_t* n_bytes = (uint8_t*) malloc( len );
    
	if (!n_bytes)
       return;
    
	mbedtls_mpi_write_binary( n, n_bytes, len );

    update(n_bytes, len);
    free(n_bytes);

}

void HAPHash::hash_num( const mbedtls_mpi * n, uint8_t* dest )
{
    int             nbytes = mbedtls_mpi_size(n);
    uint8_t * bin    = (uint8_t*) malloc( nbytes );
    if(!bin)
       return;
    mbedtls_mpi_write_binary( n, bin, nbytes );
	// ToDo: Hash
    hash( bin, nbytes, dest );
    free(bin);
}

// 
// Static Functions
// ==========================================================================================================

void HAPHash::init(HAPHashAlgorithm alg, HAPHashContext* context){
	switch (alg) {
		case SRP_SHA1  : {
			mbedtls_sha1_init( &context->sha );
			mbedtls_sha1_starts( &context->sha );            
			break;
		}
		case SRP_SHA224:
		case SRP_SHA256: {
			mbedtls_sha256_init( &context->sha256 );
			if (alg==SRP_SHA224) {
				mbedtls_sha256_starts( &context->sha256, 1 );
			} else {
				mbedtls_sha256_starts( &context->sha256, 0 );
			}
			break;
		}
		case SRP_SHA384:
		case SRP_SHA512:{
			mbedtls_sha512_init( &context->sha512 );
			if (alg==SRP_SHA384) {
				mbedtls_sha512_starts( &context->sha512, 1 );
			} else {
				mbedtls_sha512_starts( &context->sha512, 0 );
			}
			break;
		}
    	default:
			return;
	};
}




void HAPHash::update( HAPHashAlgorithm alg, HAPHashContext *context, const uint8_t *data, size_t len )
{
    switch (alg)
    {
      case SRP_SHA1  : mbedtls_sha1_update( &context->sha, data, len ); break;
      case SRP_SHA224: mbedtls_sha256_update( &context->sha256, data, len ); break;
      case SRP_SHA256: mbedtls_sha256_update( &context->sha256, data, len ); break;
      case SRP_SHA384: mbedtls_sha512_update( &context->sha512, data, len ); break;
      case SRP_SHA512: mbedtls_sha512_update( &context->sha512, data, len ); break;
      default:
        return;
    };
}

void HAPHash::final( HAPHashAlgorithm alg, HAPHashContext *context, uint8_t *md )
{
    switch (alg)
    {
      case SRP_SHA1  : mbedtls_sha1_finish( &context->sha, md ); break;
      case SRP_SHA224: mbedtls_sha256_finish( &context->sha256, md ); break;
      case SRP_SHA256: mbedtls_sha256_finish( &context->sha256, md ); break;
      case SRP_SHA384: mbedtls_sha512_finish( &context->sha512, md ); break;
      case SRP_SHA512: mbedtls_sha512_finish( &context->sha512, md ); break;
      default:
        return;
    };
}

void HAPHash::hash( HAPHashAlgorithm alg, const uint8_t* d, size_t n, uint8_t* md )
{
    switch (alg)
    {
      case SRP_SHA1  : mbedtls_sha1( d, n, md ); break;
      case SRP_SHA224: mbedtls_sha256( d, n, md, 1); break;
      case SRP_SHA256: mbedtls_sha256( d, n, md, 0); break;
      case SRP_SHA384: mbedtls_sha512( d, n, md, 1 ); break;
      case SRP_SHA512: mbedtls_sha512( d, n, md, 0 ); break;
      default:
        return;
    };
}

uint8_t HAPHash::hashLength( HAPHashAlgorithm alg )
{
    switch (alg)
    {
      case SRP_SHA1  : return SHA1_DIGEST_LENGTH;
      case SRP_SHA224: return SHA224_DIGEST_LENGTH;
      case SRP_SHA256: return SHA256_DIGEST_LENGTH;
      case SRP_SHA384: return SHA384_DIGEST_LENGTH;
      case SRP_SHA512: return SHA512_DIGEST_LENGTH;
      default:
        return -1;
    };
}


