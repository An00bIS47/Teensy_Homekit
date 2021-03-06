//
// HAPSRP.hpp
// Homekit
//
//  Created on: 01.01.2021
//      Author: michael
//

#ifndef HAPSRP_HPP_
#define HAPSRP_HPP_

#define HAP_SRP_SUPPORT_ALL_NG_SIZES 0

#include "HAPHash.hpp"
#include "mbedtls/bignum.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

typedef struct NGHex {
    const char* N_hex;
    const char* g_hex;
} NGHex;

/* All constants here were pulled from Appendix A of RFC 5054 */
const NGHex global_Ng_constants[] = {

 #if HAP_SRP_SUPPORT_ALL_NG_SIZES
	{ /* 512 */
	"D66AAFE8E245F9AC245A199F62CE61AB8FA90A4D80C71CD2ADFD0B9DA163B29F2A34AFBDB3B"
	"1B5D0102559CE63D8B6E86B0AA59C14E79D4AA62D1748E4249DF3",
	"2"
	},
	{ /* 768 */
	"B344C7C4F8C495031BB4E04FF8F84EE95008163940B9558276744D91F7CC9F402653BE7147F"
	"00F576B93754BCDDF71B636F2099E6FFF90E79575F3D0DE694AFF737D9BE9713CEF8D837ADA"
	"6380B1093E94B6A529A8C6C2BE33E0867C60C3262B",
	"2"
	},
	{ /* 1024 */
	"EEAF0AB9ADB38DD69C33F80AFA8FC5E86072618775FF3C0B9EA2314C9C256576D674DF7496"
	"EA81D3383B4813D692C6E0E0D5D8E250B98BE48E495C1D6089DAD15DC7D7B46154D6B6CE8E"
	"F4AD69B15D4982559B297BCF1885C529F566660E57EC68EDBC3C05726CC02FD4CBF4976EAA"
	"9AFD5138FE8376435B9FC61D2FC0EB06E3",
	"2"
	},
	{ /* 2048 */
	"AC6BDB41324A9A9BF166DE5E1389582FAF72B6651987EE07FC3192943DB56050A37329CBB4"
	"A099ED8193E0757767A13DD52312AB4B03310DCD7F48A9DA04FD50E8083969EDB767B0CF60"
	"95179A163AB3661A05FBD5FAAAE82918A9962F0B93B855F97993EC975EEAA80D740ADBF4FF"
	"747359D041D5C33EA71D281E446B14773BCA97B43A23FB801676BD207A436C6481F1D2B907"
	"8717461A5B9D32E688F87748544523B524B0D57D5EA77A2775D2ECFA032CFBDBF52FB37861"
	"60279004E57AE6AF874E7303CE53299CCC041C7BC308D82A5698F3A8D0C38271AE35F8E9DB"
	"FBB694B5C803D89F7AE435DE236D525F54759B65E372FCD68EF20FA7111F9E4AFF73",
	"2"
	},
 #endif
	{ /* 3072 */
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
	"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
	"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
	"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
	"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
	"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
	"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
	"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
	"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
	"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
	"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
	"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
	"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
	"E0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF",
		"5"
	},
#if HAP_SRP_SUPPORT_ALL_NG_SIZES
	{ /* 4096 */
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
	"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
	"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
	"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
	"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
	"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
	"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
	"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
	"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
	"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
	"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
	"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
	"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
	"E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26"
	"99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB"
	"04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2"
	"233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127"
	"D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934063199"
	"FFFFFFFFFFFFFFFF",
	"5"
	},
	{ /* 8192 */
	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
	"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
	"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
	"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
	"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
	"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
	"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
	"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
	"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
	"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
	"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
	"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
	"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
	"E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26"
	"99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB"
	"04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2"
	"233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127"
	"D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934028492"
	"36C3FAB4D27C7026C1D4DCB2602646DEC9751E763DBA37BDF8FF9406"
	"AD9E530EE5DB382F413001AEB06A53ED9027D831179727B0865A8918"
	"DA3EDBEBCF9B14ED44CE6CBACED4BB1BDB7F1447E6CC254B33205151"
	"2BD7AF426FB8F401378CD2BF5983CA01C64B92ECF032EA15D1721D03"
	"F482D7CE6E74FEF6D55E702F46980C82B5A84031900B1C9E59E7C97F"
	"BEC7E8F323A97A7E36CC88BE0F1D45B7FF585AC54BD407B22B4154AA"
	"CC8F6D7EBF48E1D814CC5ED20F8037E0A79715EEF29BE32806A1D58B"
	"B7C5DA76F550AA3D8A1FBFF0EB19CCB1A313D55CDA56C9EC2EF29632"
	"387FE8D76E3C0468043E8F663F4860EE12BF2D5B0B7474D6E694F91E"
	"6DBE115974A3926F12FEE5E438777CB6A932DF8CD8BEC4D073B931BA"
	"3BC832B68D9DD300741FA7BF8AFC47ED2576F6936BA424663AAB639C"
	"5AE4F5683423B4742BF1C978238F16CBE39D652DE3FDB8BEFC848AD9"
	"22222E04A4037C0713EB57A81A23F0C73473FC646CEA306B4BCBC886"
	"2F8385DDFA9D4B7FA2C087E879683303ED5BDD3A062B3CF5B3A278A6"
	"6D2A13F83F44F82DDF310EE074AB6A364597E899A0255DC164F31CC5"
	"0846851DF9AB48195DED7EA1B1D510BD7EE74D73FAF36BC31ECFA268"
	"359046F4EB879F924009438B481C6CD7889A002ED5EE382BC9190DA6"
	"FC026E479558E4475677E9AA9E3050E2765694DFC81F56E880B96E71"
	"60C980DD98EDD3DFFFFFFFFFFFFFFFFF",
	"13"
	},
#endif
	{0,0} /* null sentinel */
};










// struct HAPSRPUser
// {
//     HAPSRP::hashAlgorithm_t  hashAlgorithm;
//     HAPSRP::NGConstant*      ng;

//     mbedtls_mpi a;
//     mbedtls_mpi A;
//     mbedtls_mpi S;

//     int            authenticated;

//     const char *   username;
//     const uint8_t* password;
//     int            password_len;

//     uint8_t M           [SHA512_DIGEST_LENGTH];
//     uint8_t H_AMK       [SHA512_DIGEST_LENGTH];
//     uint8_t session_key [SHA512_DIGEST_LENGTH];
// };

class HAPSRP {
public:

    struct SRPKeyPair {
        mbedtls_mpi B;
        mbedtls_mpi b;

        SRPKeyPair(){
            mbedtls_mpi_init(&B);
            mbedtls_mpi_init(&b);
        }

        ~SRPKeyPair(){
            clear();
        }

        void clear(){
            mbedtls_mpi_free( &B );
            mbedtls_mpi_free( &b );
        }
    };


    typedef enum {
#if HAP_SRP_SUPPORT_ALL_NG_SIZES
        SRP_NG_512,
        SRP_NG_768,
        SRP_NG_1024,
        SRP_NG_2048,
#endif
	    SRP_NG_3072,
#if HAP_SRP_SUPPORT_ALL_NG_SIZES
        SRP_NG_4096,
        SRP_NG_8192,
#endif
        SRP_NG_CUSTOM,
        SRP_NG_LAST
    } NGType_t;




    struct NGConstant {
        mbedtls_mpi N;
        mbedtls_mpi g;
        bool _isInit = false;

        NGConstant(NGType_t ng_type, const char * N_hex, const char * g_hex){

            mbedtls_mpi_init(&N);
            mbedtls_mpi_init(&g);

            if ( ng_type != SRP_NG_CUSTOM ) {
                N_hex = global_Ng_constants[ ng_type ].N_hex;
                g_hex = global_Ng_constants[ ng_type ].g_hex;
            }

            mbedtls_mpi_read_string( &N, 16, N_hex);
            mbedtls_mpi_read_string( &g, 16, g_hex);

            _isInit = true;
        }

        void clear(){
            if (_isInit == false ) return;
            mbedtls_mpi_free( &N );
            mbedtls_mpi_free( &g );
        }

        NGConstant(NGConstant& rhs){
            mbedtls_mpi_init(&N);
            mbedtls_mpi_init(&g);

            if(!(mbedtls_mpi_copy(&N, &rhs.N)==0  && mbedtls_mpi_copy(&g, &rhs.g)==0)){
                clear();
                return;
            }
            _isInit = true;
        }

        ~NGConstant(){
            clear();
        }


    };


    struct SRPSession {
        NGConstant* ng = NULL;
        HAPHashAlgorithm algorithm;

        void clear(){
            if (ng != NULL) ng->clear();

            ng = NULL;
        }

        ~SRPSession(){
            clear();
        }
    };


    struct SRPVerifier {
        HAPHashAlgorithm algorithm;
        NGConstant*      ng = NULL;

        const char* username;
        int         authenticated = 0;

        uint8_t M           [SHA512_DIGEST_LENGTH] = {0,};
        uint8_t H_AMK       [SHA512_DIGEST_LENGTH] = {0,};
        uint8_t session_key [SHA512_DIGEST_LENGTH] = {0,};

        void clear(){
            if (ng != NULL) ng->clear();
            ng = NULL;
        }


        ~SRPVerifier(){
            clear();
        }
    };

    struct HAPSRPData {
        SRPSession *session = NULL;
        SRPKeyPair *keys = NULL;
        SRPVerifier *verifier = NULL;

        const uint8_t* bytes_s = NULL;
        int len_s = 0;
        const uint8_t* bytes_v = NULL;
        int len_v = 0;
        const uint8_t* bytes_B = NULL;
        int len_B = 0;

        char username[10] = {0,};


        void clear(){
            if (verifier != NULL) {
                //verifier->clear();
                delete verifier;
                verifier = NULL;
            }
            if (session != NULL) {
                //session->clear();
                delete session;
                session = NULL;
            }
            if (keys != NULL) {
                //keys->clear();
                delete keys;
                keys = NULL;
            }

            if (bytes_s != NULL) 	free((uint8_t*) bytes_s);
            if (bytes_v != NULL) 	free((uint8_t*) bytes_v);
            if (bytes_B != NULL) 	free((uint8_t*) bytes_B);

            bytes_s = NULL; len_s = 0;
            bytes_v = NULL; len_v = 0;
            bytes_B = NULL; len_B = 0;
        }

        ~HAPSRPData(){
            clear();
        }

    };

    HAPSRPData*  data;

    HAPSRP();
    ~HAPSRP();

    int getHashLength(SRPSession* session){
        return HAPHash::hashLength(session->algorithm);
    }

    void begin(const char* username);
    void clear();
    void randomSeed(const uint8_t* random_data, size_t data_length);


    SRPKeyPair* createKeyPair(SRPSession *session, const uint8_t* bytes_v, int len_v, const uint8_t** bytes_B, int* len_B);

    void createSaltedVerificationKey( SRPSession *session, const char * username,
                                    const uint8_t * password, int len_password,
                                    const uint8_t ** bytes_s, int * len_s,
                                    const uint8_t ** bytes_v, int * len_v);

    void createSaltedVerificationKey1( SRPSession *session, const char * username,
                                    const uint8_t * password, int len_password,
                                    const uint8_t ** bytes_s, int len_s,
                                    const uint8_t ** bytes_v, int* len_v);


    SRPSession* newSession( HAPHashAlgorithm alg, NGType_t ng_type, const char* N_hex, const char* g_hex);

    SRPVerifier* newVerifier( SRPSession *session,
                                    const char *username,
                                    const unsigned char * bytes_s, int len_s,
                                    const unsigned char * bytes_v, int len_v,
                                    const unsigned char * bytes_A, int len_A,
                                    const unsigned char ** bytes_B, int * len_B);

    SRPVerifier* newVerifier1( SRPSession *session,
                                    const char *username, bool copy_username,
                                    const unsigned char * bytes_s, int len_s,
                                    const unsigned char * bytes_v, int len_v,
                                    const unsigned char * bytes_A, int len_A,
                                    const unsigned char ** bytes_B, int * len_B,
                                    SRPKeyPair *keys);

    int getVerifierIsAuthenticated( SRPVerifier* ver );
    const char* getVerifierUsername( SRPVerifier* ver );
    const uint8_t* getVerifierSessionKey( SRPVerifier* ver, int* key_length );
    int getSessionKeyLength( SRPSession* ses );
    int getVerifierSessionKeyLength( SRPVerifier* ver );
    bool verifySession( SRPVerifier * ver, const uint8_t* user_M, const uint8_t** bytes_HAMK );
    const uint8_t* getVerifierHAMK( SRPVerifier * ver);

protected:
    char* _username;

    static bool                        _isInitialized;
    static mbedtls_entropy_context     _entropy_ctx;
    static mbedtls_ctr_drbg_context    _ctr_drbg_ctx;
    static mbedtls_mpi*                _RR;




    static void H_nn(mbedtls_mpi* bn, HAPHashAlgorithm alg, const mbedtls_mpi* n1, const mbedtls_mpi* n2, bool do_pad);
    static void H_ns(mbedtls_mpi* bn, HAPHashAlgorithm alg,const mbedtls_mpi * n, const uint8_t* bytes, int len_bytes);



    void calculate_x( HAPHashAlgorithm alg, mbedtls_mpi* x, const mbedtls_mpi* salt, const char* username, const uint8_t* password, int password_len );
    static void calculate_M( HAPHashAlgorithm alg, NGConstant *ng, uint8_t* dest, const char* I, const mbedtls_mpi * s, const mbedtls_mpi * A, const mbedtls_mpi * B, const uint8_t* K );
    static void calculate_H_AMK( HAPHashAlgorithm alg, unsigned char *dest, const mbedtls_mpi * A, const uint8_t* M, const uint8_t* K );

    void initRandom();

};

#endif /* HAPSRP_HPP_ */