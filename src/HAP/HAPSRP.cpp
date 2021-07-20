//
// HAPSRP.cpp
// Homekit
//
//  Created on: 01.01.2021
//      Author: michael
//

#include "HAPSRP.hpp"
#include <cstring>

#define SRP_BITS_IN_PRIVKEY 256
#define SRP_BYTES_IN_PRIVKEY (SRP_BITS_IN_PRIVKEY/8)
#define SRP_DEFAULT_SALT_BYTES 32



bool                        HAPSRP::_isInitialized 	= false;
mbedtls_entropy_context     HAPSRP::_entropy_ctx;
mbedtls_ctr_drbg_context    HAPSRP::_ctr_drbg_ctx;
mbedtls_mpi*                HAPSRP::_RR				= nullptr;

const uint8_t hotBits[128] PROGMEM = {
	82, 42, 71, 87, 124, 241, 30, 1, 54, 239, 240, 121, 89, 9, 151, 11, 60,
	226, 142, 47, 115, 157, 100, 126, 242, 132, 46, 12, 56, 197, 194, 76,
	198, 122, 90, 241, 255, 43, 120, 209, 69, 21, 195, 212, 100, 251, 18,
	111, 30, 238, 24, 199, 238, 236, 138, 225, 45, 15, 42, 83, 114, 132,
	165, 141, 32, 185, 167, 100, 131, 23, 236, 9, 11, 51, 130, 136, 97, 161,
	36, 174, 129, 234, 2, 54, 119, 184, 70, 103, 118, 109, 122, 15, 24, 23,
	166, 203, 102, 160, 77, 100, 17, 4, 132, 138, 215, 204, 109, 245, 122,
	9, 184, 89, 70, 247, 125, 97, 213, 240, 85, 243, 91, 226, 127, 64, 136,
	37, 154, 232
};


#ifdef UNIT_TEST
#ifdef SRP_TEST
#undef SRP_TEST
#endif
#define SRP_TEST 1
#endif

#ifdef SRP_TEST

#define SRP_TEST_FIXED_SALT
//                               1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
#define SRP_TEST_FIXED_SALT_STR "BEB25379D1A8581EB5A727673A2441EE"

#define SRP_TEST_PRINT_SALT
#define SRP_TEST_PRINT_v

#define SRP_TEST_FIXED_b
#define SRP_TEST_FIXED_b_STR "E487CB59D31AC550471E81F00F6928E01DDA08E974A004F49E61F5D105284D20"
#define SRP_TEST_PRINT_B
#define SRP_TEST_PRINT_b

#define SRP_TEST_FIXED_a
#define SRP_TEST_FIXED_a_STR "60975527035CF2AD1989806F0407210BC81EDC04E2762A56AFD529DDDA2D4393"
#define SRP_TEST_PRINT_A
#define SRP_TEST_PRINT_a

#define SRP_TEST_DBG_VER
#define SRP_TEST_SHA512


void tutils_array_print(const char* tag, const unsigned char* buf, int len){
    Serial.printf("=== [%s] (len:%d) ===\n", tag, len);
	int need_lf=1;
    for (int i=0; i<len; i++) {
        if ((i & 0xf) == 0xf) {
	        Serial.printf("%02X\n", buf[i]);
			need_lf=0;
		} else {
	        Serial.printf("%02X ", buf[i]);
			need_lf=1;
		}
    }
    if (need_lf){
		Serial.printf("\n======\n");
	} else {
		Serial.printf("======\n");
	}
}

void tutils_mpi_print(const char* tag, const mbedtls_mpi* x){
    int len_x = mbedtls_mpi_size(x);
    uint8_t* num = (uint8_t*)malloc(len_x);
    mbedtls_mpi_write_binary(x, num, len_x);
	tutils_array_print(tag, num, len_x);
    free(num);
}

#endif


HAPSRP::HAPSRP(){
	_isInitialized = false;
	_username = nullptr;
	data = NULL;
}

HAPSRP::~HAPSRP(){
	clear();
}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::clear(){

	if (!_isInitialized) return;
	mbedtls_entropy_free(&_entropy_ctx);
	mbedtls_ctr_drbg_free(&_ctr_drbg_ctx);


	if (_RR != nullptr) {
		mbedtls_mpi_free(_RR);
		delete _RR;
		_RR = nullptr;
	}

	if ( data != NULL ) {
		delete data;
		data = NULL;
	}

	_isInitialized = false;
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::begin(const char* username){
	initRandom();

	if (data == NULL) {
		data = new HAPSRPData();
	} else {
		data->clear();
	}
	memcpy(data->username, username, 10);
}


/**
 * @brief Creates SRP Key Pair
 *
 * bytes_B must be freed !
 *
 * @param bytes_v
 * @param len_v
 * @param bytes_B
 * @param len_B
 * @return HAPSRP::KeyPair*
 */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
HAPSRP::SRPKeyPair* HAPSRP::createKeyPair(SRPSession *session, const uint8_t* bytes_v, int len_v, const uint8_t** bytes_B, int * len_B){

	mbedtls_mpi k;
	mbedtls_mpi tmp1;
	mbedtls_mpi tmp2;
	mbedtls_mpi v;

	SRPKeyPair* keys = nullptr;

	mbedtls_mpi_init(&tmp1);
	mbedtls_mpi_init(&tmp2);
	mbedtls_mpi_init(&k);
	mbedtls_mpi_init(&v);

	if (mbedtls_mpi_read_binary( &v, bytes_v, len_v )!=0) {
		mbedtls_mpi_free(&tmp1);
		mbedtls_mpi_free(&tmp2);
		mbedtls_mpi_free(&v);
		mbedtls_mpi_free(&k);

		return nullptr;
	}

	// ToDo: Can this be a member variable ?? -> don't forget to delete!
	keys = new SRPKeyPair();

#ifdef SRP_TEST_FIXED_b
	mbedtls_mpi_read_string(&(keys->b),16,SRP_TEST_FIXED_b_STR);
#else
	mbedtls_mpi_fill_random( &(keys->b), SRP_BYTES_IN_PRIVKEY,
					 &mbedtls_ctr_drbg_random,
					 &_ctr_drbg_ctx );
#endif

	H_nn(&k, session->algorithm, &(session->ng->N), &(session->ng->g), true);

	/* B = kv + g^b */
	mbedtls_mpi_mul_mpi( &tmp1, &k, &v);
	mbedtls_mpi_exp_mod( &tmp2, &(session->ng->g), &(keys->b), &(session->ng->N), _RR );
	mbedtls_mpi_add_mpi( &tmp1, &tmp1, &tmp2 );
	mbedtls_mpi_mod_mpi( &(keys->B), &tmp1, &(session->ng->N) );

	mbedtls_mpi_free(&tmp1);
	mbedtls_mpi_free(&tmp2);

#ifdef SRP_TEST_PRINT_b
	tutils_mpi_print ("server priv (b)",&(keys->b));
#endif
#ifdef SRP_TEST_PRINT_B
	tutils_mpi_print ("server pub (B)",&(keys->B));
#endif

	if (bytes_B) {
		*len_B   = mbedtls_mpi_size(&(keys->B));
		*bytes_B = (uint8_t*) malloc( *len_B );

		if( !*bytes_B ){
			keys->clear();
			mbedtls_mpi_free(&v);
			mbedtls_mpi_free(&k);
		}
		mbedtls_mpi_write_binary( &(keys->B), (uint8_t*)*bytes_B, *len_B );
	}

	mbedtls_mpi_free(&v);
	mbedtls_mpi_free(&k);


	return keys;
}

/**
 * @brief Hash 2 bignums
 *
 * @param bn 		Output bignum
 * @param alg 		Algorithm to use
 * @param n1 		bignum 1
 * @param n2 		bignum 2
 * @param do_pad 	Do padding
 */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::H_nn(mbedtls_mpi* bn, HAPHashAlgorithm::Type alg, const mbedtls_mpi* n1, const mbedtls_mpi* n2, bool do_pad)
{
    uint8_t   buff[ SHA512_DIGEST_LENGTH ];
    int       len_n1 = mbedtls_mpi_size(n1);
    int       len_n2 = mbedtls_mpi_size(n2);
	int       nbytes = len_n1 + len_n2;
	int kldiff;

	if (do_pad) {
		kldiff= len_n1 - len_n2;
		if (kldiff<0) return;
		nbytes +=kldiff;
	} else kldiff=0;


	uint8_t* bin    = (uint8_t *) malloc( nbytes );

    if (!bin)
       return;
	if (kldiff) memset(bin+len_n1,0,kldiff);
    mbedtls_mpi_write_binary( n1, bin, len_n1 );
    mbedtls_mpi_write_binary( n2, bin+len_n1+kldiff, len_n2 );
    HAPHash::hash( alg, bin, nbytes, buff );

    free(bin);

    // mbedtls_mpi * bn;
    // bn = (mbedtls_mpi *) malloc(sizeof(mbedtls_mpi));
    // mbedtls_mpi_init(bn);
    mbedtls_mpi_read_binary( bn, buff, HAPHash::digestLength(alg) );
    return;
}

/**
 * @brief Hash Bignum with salt
 *
 * @param bn 		Output bignum
 * @param alg 		Algorithm to use
 * @param n 		Salt
 * @param bytes 	add to hash
 * @param len_bytes length of bytes
 */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::H_ns(mbedtls_mpi* bn, HAPHashAlgorithm::Type alg, const mbedtls_mpi * n, const uint8_t* bytes, int len_bytes )
{
    uint8_t   		buff[ SHA512_DIGEST_LENGTH ];
    int             len_n  = mbedtls_mpi_size(n);
    int             nbytes = len_n + len_bytes;

	uint8_t* bin    = (uint8_t*) malloc( nbytes );

    if (!bin)
       return;
    mbedtls_mpi_write_binary( n, bin, len_n );
    memcpy( bin + len_n, bytes, len_bytes );
    HAPHash::hash( alg, bin, nbytes, buff );
    free(bin);

    // mbedtls_mpi * bn;
    // bn = (mbedtls_mpi *) malloc(sizeof(mbedtls_mpi));
    // mbedtls_mpi_init(bn);
    mbedtls_mpi_read_binary( bn, buff, HAPHash::digestLength(alg) );

}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::calculate_x( HAPHashAlgorithm::Type alg, mbedtls_mpi* x, const mbedtls_mpi* salt, const char* username, const uint8_t* password, int password_len )
{
	uint8_t ucp_hash[SHA512_DIGEST_LENGTH];

	HAPHash hash(alg);
    hash.init();
    hash.update( username, strlen(username) );
    hash.update( ":", 1 );
    hash.update( password, password_len );
    hash.final(ucp_hash);
	hash.clear();

#ifdef SRP_TEST_DBG_VER
	tutils_array_print("VER:username",(const unsigned char*)username, strlen(username));
	tutils_array_print("VAR:password",password, password_len);
	tutils_mpi_print("VAR:salt",salt);
	tutils_array_print("VAR:ucp_hash",ucp_hash, hash.digestLength());
#endif
    return H_ns( x, alg, salt, ucp_hash, hash.digestLength() );
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::calculate_M( HAPHashAlgorithm::Type alg, NGConstant *ng, uint8_t* dest, const char* I, const mbedtls_mpi * s, const mbedtls_mpi * A, const mbedtls_mpi * B, const uint8_t* K )
{
    uint8_t H_N[ SHA512_DIGEST_LENGTH ];
    uint8_t H_g[ SHA512_DIGEST_LENGTH ];
    uint8_t H_I[ SHA512_DIGEST_LENGTH ];
    uint8_t H_xor[ SHA512_DIGEST_LENGTH ];

	HAPHash hash(alg);

    int i = 0;
    uint8_t hash_len = hash.digestLength();

    hash.hash_num( &(ng->N), H_N );
    hash.hash_num( &(ng->g), H_g );

    hash.hash((const uint8_t*)I, strlen(I), H_I);


    for (i=0; i < hash_len; i++ )
        H_xor[i] = H_N[i] ^ H_g[i];

    hash.init();

    hash.update( H_xor, hash_len );
    hash.update( H_I,   hash_len );
    hash.update_hash_n( s );
   	hash.update_hash_n( A );
    hash.update_hash_n( B );
    hash.update( K, hash_len );

    hash.final( dest );

	hash.clear();
}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::calculate_H_AMK( HAPHashAlgorithm::Type alg, unsigned char *dest, const mbedtls_mpi * A, const uint8_t* M, const uint8_t* K )
{
	HAPHash hash(alg);
    hash.init();

    hash.update_hash_n( A );
    hash.update( M, hash.digestLength() );
    hash.update( K, hash.digestLength() );

    hash.final( dest );

	hash.clear();
}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::initRandom()
{
    if (_isInitialized) return;

    mbedtls_entropy_init( &_entropy_ctx );
    mbedtls_ctr_drbg_init( &_ctr_drbg_ctx );



    mbedtls_ctr_drbg_seed(
        &_ctr_drbg_ctx,
        mbedtls_entropy_func,
        &_entropy_ctx,
        hotBits,
        128
    );

	if (_RR == nullptr ) {
		_RR = new mbedtls_mpi();
	}
    mbedtls_mpi_init(_RR);

    _isInitialized = true;
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
HAPSRP::SRPSession* HAPSRP::newSession( HAPHashAlgorithm::Type alg, NGType_t ng_type, const char * N_hex, const char * g_hex){
	if ((unsigned)alg>=(unsigned)HAPHashAlgorithm::Last) return NULL;
	if ((unsigned)ng_type>=(unsigned)SRP_NG_LAST) return NULL;

    SRPSession* session = new SRPSession();

	if (!session) return NULL;

    // memset(session, 0, sizeof(SRPSession));

    session->algorithm = alg;
    session->ng   = new NGConstant( ng_type, N_hex, g_hex );

    initRandom(); /* Only happens once */

    return session;
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::randomSeed(const uint8_t* random_data, size_t data_length )
{
	_isInitialized = true;
	mbedtls_ctr_drbg_seed( &_ctr_drbg_ctx, mbedtls_entropy_func, &_entropy_ctx,
                           (const uint8_t*) random_data,
                           data_length );
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::createSaltedVerificationKey( SRPSession *session, const char * username,
                                         const uint8_t * password, int len_password,
                                         const uint8_t ** bytes_s, int * len_s,
                                         const uint8_t ** bytes_v, int * len_v)
{
	*len_s = SRP_DEFAULT_SALT_BYTES;
	createSaltedVerificationKey1(session, username,password,len_password,bytes_s,SRP_DEFAULT_SALT_BYTES,bytes_v,len_v);
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
void HAPSRP::createSaltedVerificationKey1( SRPSession *session, const char * username,
											const uint8_t * password, int len_password,
											const uint8_t ** bytes_s, int len_s,
											const uint8_t ** bytes_v, int* len_v)
{

	*bytes_s = NULL;
	*bytes_v = NULL;

    mbedtls_mpi s;
    mbedtls_mpi v;
    mbedtls_mpi x;


	mbedtls_mpi_init(&s);
	mbedtls_mpi_init(&v);
	mbedtls_mpi_init(&x);

#ifdef SRP_TEST_FIXED_SALT
	mbedtls_mpi_read_string(&s,16,SRP_TEST_FIXED_SALT_STR);
#else

    mbedtls_mpi_fill_random( &s, len_s,
                     &mbedtls_ctr_drbg_random,
                     &_ctr_drbg_ctx );
#endif

#ifdef SRP_TEST_PRINT_SALT
	tutils_mpi_print ("salt (s)", &s);
#endif


    calculate_x( session->algorithm, &x, &s, username, password, len_password );


    mbedtls_mpi_exp_mod(&v, &(session->ng->g), &x, &(session->ng->N), _RR);

#ifdef SRP_TEST_PRINT_v
	tutils_mpi_print ("verifier (v)", &v);
#endif


    *bytes_s = (const uint8_t *) malloc( len_s );
	if (*bytes_s==NULL) {
		mbedtls_mpi_free(&s);
		mbedtls_mpi_free(&v);
		mbedtls_mpi_free(&x);

		return;
	}

    *len_v   = mbedtls_mpi_size(&v);
    *bytes_v = (const uint8_t *) malloc( *len_v );
	if (*bytes_v == NULL) {
		free((void*)(*bytes_s));
		*bytes_s=NULL;
		mbedtls_mpi_free(&s);
		mbedtls_mpi_free(&v);
		mbedtls_mpi_free(&x);

		return;
	}

    mbedtls_mpi_write_binary( &s, (uint8_t*)*bytes_s, len_s );
    mbedtls_mpi_write_binary( &v, (uint8_t*)*bytes_v, *len_v );

	mbedtls_mpi_free(&s);
	mbedtls_mpi_free(&v);
	mbedtls_mpi_free(&x);

}


/* Out: bytes_B, len_B.
 *
 * On failure, bytes_B will be set to NULL and len_B will be set to 0
 */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
HAPSRP::SRPVerifier* HAPSRP::newVerifier( SRPSession *session,
                                        const char *username,
                                        const unsigned char * bytes_s, int len_s,
                                        const unsigned char * bytes_v, int len_v,
                                        const unsigned char * bytes_A, int len_A,
                                        const unsigned char ** bytes_B, int * len_B)
{
	return newVerifier1(
		session,username, true,bytes_s,len_s,bytes_v,len_v,
		bytes_A,len_A, bytes_B,len_B,
		NULL
	);
}

/* Out: bytes_B, len_B if not using SRPKeyPair keys
 *
 * On failure, bytes_B will be set to NULL and len_B will be set to 0, keys=NULL is OK!
 * bytes_B=NULL is OK
 */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
HAPSRP::SRPVerifier* HAPSRP::newVerifier1( SRPSession *session,
                                        const char *username, bool copy_username,
                                        const unsigned char * bytes_s, int len_s,
                                        const unsigned char * bytes_v, int len_v,
                                        const unsigned char * bytes_A, int len_A,
                                        const unsigned char ** bytes_B, int * len_B,
                                        SRPKeyPair *keys)
{
	if (bytes_B) {
		*bytes_B=NULL;
		*len_B=0;
	}

	if( session == NULL ) {
#if ARDUINO_ARCH_ESP32
        ESP_LOGE("SRP", "session is null\n");
#else
        Serial.printf("%s - session is null:\n", "SRP");
#endif
        return NULL;
    }

    mbedtls_mpi s;
    mbedtls_mpi_init(&s);
    mbedtls_mpi_read_binary(&s, bytes_s, len_s);

    mbedtls_mpi v;
    mbedtls_mpi_init(&v);
    mbedtls_mpi_read_binary(&v, bytes_v, len_v);

    mbedtls_mpi A;
    mbedtls_mpi_init(&A);
    mbedtls_mpi_read_binary(&A, bytes_A, len_A);

    mbedtls_mpi u;
	mbedtls_mpi_init(&u);

    mbedtls_mpi S;
    mbedtls_mpi_init(&S);


    //mbedtls_mpi             *k    = 0;

    mbedtls_mpi tmp1;
    mbedtls_mpi_init(&tmp1);

    mbedtls_mpi tmp2;
    mbedtls_mpi_init(&tmp2);


    SRPVerifier *ver = new SRPVerifier();

	// memset(ver,0,sizeof(SRPVerifier));
    ver->algorithm 	= session->algorithm;
    ver->ng       	= session->ng;

// 	if (copy_username){
// 		int ulen = strlen(username) + 1;
// 		ver->username = (char *) malloc( sizeof(char) * ulen ); // FIXME
// 		if (!ver->username) {
// 			delete ver;
// 			ver = 0;


// #if ARDUINO_ARCH_ESP32
//       		ESP_LOGE("SRP", "ver->username is NULL\n");
// #else
//       		Serial.printf("%s - ver->username is NULL\n", "SRP");
// #endif

// 			mbedtls_mpi_free(&s);
// 			mbedtls_mpi_free(&v);
// 			mbedtls_mpi_free(&A);
// 			mbedtls_mpi_free(&S);
// 			mbedtls_mpi_free(&tmp1);
// 			mbedtls_mpi_free(&tmp2);
// 			return nullptr;
// 		}
// 		memcpy( (char*)ver->username, username, ulen );
// 	}

    /* SRP-6a safety check */
    mbedtls_mpi_mod_mpi( &tmp1, &A, &(session->ng->N) );
    if ( mbedtls_mpi_cmp_int( &tmp1, 0 )  != 0)
    {
		int temp_keys;
		if (keys == NULL) {
			temp_keys = 1;

#if ARDUINO_ARCH_ESP32
        	ESP_LOGE("SRP", "keys is NULL\n");
#else
        	Serial.printf("%s - keys is NULL\n", "SRP");
#endif


			keys = createKeyPair(session, bytes_v, len_v, bytes_B, len_B);
			if (keys == NULL) {

				delete ver;

				mbedtls_mpi_free(&s);
				mbedtls_mpi_free(&v);
				mbedtls_mpi_free(&A);
				mbedtls_mpi_free(&S);
				mbedtls_mpi_free(&tmp1);
				mbedtls_mpi_free(&tmp2);
				return nullptr;
            }
		} else temp_keys = 0;


    	H_nn(&u, session->algorithm, &A, &(keys->B), true);

		/* S = (A *(v^u)) ^ b */
		mbedtls_mpi_exp_mod(&tmp1, &v, &u, &(session->ng->N), _RR);
		mbedtls_mpi_mul_mpi(&tmp2, &A, &tmp1);
		mbedtls_mpi_exp_mod(&S, &tmp2, &(keys->b), &(session->ng->N), _RR);

		HAPHash hash(session->algorithm);
		hash.hash_num(&S, ver->session_key);

		calculate_M( session->algorithm, session->ng, ver->M, username, &s, &A, &(keys->B), ver->session_key );
		calculate_H_AMK( session->algorithm, ver->H_AMK, &A, ver->M, ver->session_key );

		if (temp_keys) {
			keys->clear();
			delete keys;
		}
    }


	mbedtls_mpi_free(&s);
	mbedtls_mpi_free(&v);
	mbedtls_mpi_free(&A);
	mbedtls_mpi_free(&S);
	mbedtls_mpi_free(&tmp1);
	mbedtls_mpi_free(&tmp2);

    return ver;
}


//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
int HAPSRP::getVerifierIsAuthenticated( HAPSRP::SRPVerifier* ver )
{
    return ver->authenticated;
}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
// const char* HAPSRP::getVerifierUsername( HAPSRP::SRPVerifier* ver )
// {
//     return ver->username;
// }

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
const uint8_t* HAPSRP::getVerifierSessionKey( HAPSRP::SRPVerifier* ver, int* key_length )
{
    if (key_length)
        *key_length = HAPHash::digestLength( ver->algorithm );
    return ver->session_key;
}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
int HAPSRP::getSessionKeyLength( HAPSRP::SRPSession* ses ){
	return HAPHash::digestLength( ses->algorithm );
}

//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
int HAPSRP::getVerifierSessionKeyLength( HAPSRP::SRPVerifier* ver )
{
    return HAPHash::digestLength( ver->algorithm );
}


/* user_M,bytes_HAMK are digest generated with session selected hash */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
bool HAPSRP::verifySession( HAPSRP::SRPVerifier * ver, const uint8_t* user_M, const uint8_t** bytes_HAMK )
{

	// tutils_array_print("verifier proof", ver->M, 64);

    if ( memcmp( ver->M, user_M, HAPHash::digestLength(ver->algorithm) ) == 0 )
    {
        ver->authenticated = 1;
        if (bytes_HAMK) *bytes_HAMK = ver->H_AMK;
		return true;
    }
    else {
        if (bytes_HAMK) *bytes_HAMK = NULL;
		return false;
	}
}

/* return bytes_HAMK which is  digest generated with session selected hash */
//#if defined(ARDUINO_TEENSY41)
//FLASHMEM
//#endif
const uint8_t* HAPSRP::getVerifierHAMK( HAPSRP::SRPVerifier * ver) {
	return ver->H_AMK;
}