//
// HAPVerifyContext.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPVERIFYCONTEXT_HPP_
#define HAPVERIFYCONTEXT_HPP_

#include <Arduino.h>
#include "m_ed25519.h"
#include "HAPGlobals.hpp"

struct HAPLongTermContext {
	// uint8_t publicKey[ED25519_PUBLIC_KEY_LENGTH];
	uint8_t* publicKey;
	uint8_t publicKeyLength;

	// uint8_t privateKey[ED25519_PRIVATE_KEY_LENGTH];
	uint8_t* privateKey;
	uint8_t privateKeyLength;

	uint8_t* LTSK;
	uint8_t LTSKLength;

	HAPLongTermContext() 
	: publicKeyLength(ED25519_PUBLIC_KEY_LENGTH)
	, privateKeyLength(ED25519_PRIVATE_KEY_LENGTH)
	, LTSKLength(ED25519_BYTES) {
		publicKey = (uint8_t*) malloc(sizeof(uint8_t) * publicKeyLength);
		privateKey = (uint8_t*) malloc(sizeof(uint8_t) * privateKeyLength);
		LTSK = (uint8_t*) malloc(sizeof(uint8_t) * LTSKLength);
	}

	~HAPLongTermContext(){
		free(publicKey);
		free(privateKey);
		free(LTSK);
	}
};


struct HAPEncryptionContext {
	uint8_t encryptKey[CURVE25519_SECRET_LENGTH];
	uint8_t decryptKey[CURVE25519_SECRET_LENGTH];

	uint16_t encryptCount;
    uint16_t decryptCount;

    HAPEncryptionContext()
    : encryptCount(0)
	, decryptCount(0){
		memset(encryptKey, 0, CURVE25519_SECRET_LENGTH);
		memset(decryptKey, 0, CURVE25519_SECRET_LENGTH);
	}	
};


struct HAPVerifyContext {
	uint8_t secret[HKDF_KEY_LEN];
	uint8_t sessionKey[CURVE25519_SECRET_LENGTH]; 	
	uint8_t accessoryLTPK[ED25519_PUBLIC_KEY_LENGTH];	
	uint8_t deviceLTPK[ED25519_PUBLIC_KEY_LENGTH];
	
	HAPVerifyContext() {
		memset(secret, 0, HKDF_KEY_LEN);
		memset(sessionKey, 0, CURVE25519_SECRET_LENGTH);
		memset(accessoryLTPK, 0, ED25519_PUBLIC_KEY_LENGTH);
		memset(deviceLTPK, 0, ED25519_PUBLIC_KEY_LENGTH);
	}
};

#endif /* HAPVERIFYCONTEXT_HPP_ */
