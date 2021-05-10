//
// HAPPrintEncrypted.cpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//
//
// buffer:
// 	 												0								   1024
//  ----------------------------------------------------------------------------------------------------------------------
//  | AD | AD | ER | ER | ER | ER | ER | ER | ER |  B  |  U  |  F  |  F  |  E  |  R  | ... | 7x ER | ... | 16x TAG | ... |
//  ----------------------------------------------------------------------------------------------------------------------
// 	  |			|									|										  |      +--> 16 x TAG
//    |			|                                   |										  +--> 7 x ENCODING RESERVE
//    |			|                                   +--> 1024 x BUFFER
//    |			+-->  7 x ENCODING RESERVE
//    +--> 2 x AEAD
//
// 		\r\n123\r\n	= 7 ER  	= l = 3
//								  WRITE					ENCRYPT
//	    			=> 2 AD		= buffer -l -2			- NO -
// 					=> l ER		= buffer -l				- YES -
// 		....        => 1024 		= buffer	= s		- YES - buffer - l, s + l
// 					=> 16 TAG	= buffer + s 			- ADD - tag, _pos + l + s
//
#include "HAPPrintEncrypted.hpp"
#include "HAPHelper.hpp"

#define ENCODING_RESERVE 7
#define AEAD_RESERVE	 2
#define TAG_RESERVE		 16
// 					   + 7 		for extra encoding reserve at the end of the response
// 					  + =====
// 					1024 + 7 + 7 + 2 + 16 = 1056 Bytes


HAPPrintEncrypted::HAPPrintEncrypted(Print &target, uint8_t* buffer, size_t size)
: HAPPrintBuffered(target, buffer + AEAD_RESERVE + ENCODING_RESERVE, size - ENCODING_RESERVE - AEAD_RESERVE - TAG_RESERVE - ENCODING_RESERVE){
	memset(_key, 0, CHACHA20_POLY1305_KEY_BYTES);
}


void HAPPrintEncrypted::setKey(const uint8_t key[CHACHA20_POLY1305_KEY_BYTES]){

	memcpy(_key, key, CHACHA20_POLY1305_KEY_BYTES);
}


void HAPPrintEncrypted::endHeader() {
	flush();
	_headerEnded = true;
	_firstChunk = true;
}


void HAPPrintEncrypted::begin(){
	_firstChunk = true;
	_ended = false;
}


void HAPPrintEncrypted::end() {
	if (_ended == true) return;

	print(F("\r\n0\r\n\r\n"));

	_bodyEnded = true;

	if (_pos > 0) {
		flush();
	}

	_bodyEnded = false;
	_ended = true;
}

void HAPPrintEncrypted::flush() {

	if (!_pos)
		return;

	uint8_t l = 0;

	if (_bodyEnded == true) {
		_pos = _pos - 7;			// ignore the \r\n0\r\n\r\n at the end !!
		l = (_firstChunk ? 2 : 4) + (_pos > 0xFF ? 3 : (_pos <= 0xF ? 1 : 2));
		sprintf_P((char*)_buffer - l, (_firstChunk ? PSTR("%x\r") : PSTR("\r\n%x\r")), _pos);
		(_buffer - 1)[0] = '\n';
		_pos = _pos + 7;			// don't ignore anylonger!
	}
	else if (_headerEnded == true ) {
		l = (_firstChunk ? 2 : 4) + (_pos > 0xFF ? 3 : (_pos <= 0xF ? 1 : 2));
		sprintf_P((char*)_buffer - l, (_firstChunk ? PSTR("%x\r") : PSTR("\r\n%x\r")), _pos);
		(_buffer - 1)[0] = '\n';
	}

	size_t chunk_size = _pos;
	chunk_size += l;

#if HAP_DEBUG_HOMEKIT
	//Serial.write(_buffer - l, chunk_size);
	HAPHelper::array_print("Response", _buffer - l, chunk_size);
#endif


	uint8_t* encrypted = _buffer - l - AEAD_RESERVE;
	encrypted[0] = chunk_size & 0xff;	// AEAD[0]
	encrypted[1] = chunk_size >> 8;  	// AEAD[1]


	// Nonce
	uint8_t nonce[12];
	memset(nonce, 0, sizeof(nonce));
	uint8_t i = 4;
	int x = _encryptCount++;
	while (x) {
		nonce[i++] = x % 256;
		x /= 256;
	}

	mbedtls_chachapoly_context chachapoly_ctx;
	mbedtls_chachapoly_init(&chachapoly_ctx);
	mbedtls_chachapoly_setkey(&chachapoly_ctx,_key);

	int ret = mbedtls_chachapoly_encrypt_and_tag(&chachapoly_ctx, chunk_size, nonce, encrypted, HAP_ENCRYPTION_AAD_SIZE, _buffer - l, encrypted + 2, encrypted + 2 + chunk_size);
	if (ret != 0) {
		mbedtls_chachapoly_free(&chachapoly_ctx);
		return;
	}


#if HAP_DEBUG_ENCRYPTION
	HAPHelper::array_print("encrypted", _buffer - l - 2, chunk_size + 16 + 2);
#endif

	_target.write(encrypted, chunk_size + 16 + 2);
	// target.write(buffer - l - 2, chunk_size + 16 + 2);

	mbedtls_chachapoly_free(&chachapoly_ctx);

	if (_firstChunk) {
		_firstChunk = false;
	}
	_pos = 0;
}

size_t HAPPrintEncrypted::write(uint8_t b){
	_buffer[_pos++] = b;

	if (_pos == _size) {
		flush();
	}
	return 1;
}






