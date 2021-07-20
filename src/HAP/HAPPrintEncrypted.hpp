//
// HAPPrintEncrypted.hpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//

#ifndef HAPPRINTENCRYPTED_HPP_
#define HAPPRINTENCRYPTED_HPP_

#include <Arduino.h>
#include "HAPPrintBuffered.hpp"
#include "HAPGlobals.hpp"
// #include "m_chacha20_poly1305.h"
#include "mbedtls/chachapoly.h"




#ifndef CHACHA20_POLY1305_KEY_BYTES
#define CHACHA20_POLY1305_KEY_BYTES       32
#endif

#ifndef CHACHA20_POLY1305_NONCE_BYTES_MAX
#define CHACHA20_POLY1305_NONCE_BYTES_MAX 12
#endif

#ifndef CHACHA20_POLY1305_TAG_BYTES
#define CHACHA20_POLY1305_TAG_BYTES       16
#endif



class HAPPrintEncrypted : public HAPPrintBuffered {
private:

	bool _firstChunk = false;

	bool _headerEnded = false;
	bool _bodyEnded = false;
	bool _ended	= true;			// is set at begin() to false and at end() to true

	uint8_t _encryptCount = 0;

	uint8_t _key[CHACHA20_POLY1305_KEY_BYTES];



public:
	HAPPrintEncrypted(Print &target, uint8_t* buffer, size_t size);
	~HAPPrintEncrypted() { end(); }

	void setEncryptCount(uint8_t encryptCount) { _encryptCount = encryptCount; }
	uint8_t encryptCount(){ return _encryptCount; }

	void setKey(const uint8_t key[CHACHA20_POLY1305_KEY_BYTES]);

	void begin();
	void endHeader();

	size_t write(uint8_t b) override;
	size_t write(const uint8_t* buffer, size_t size);

	void end();

	void flush() override;

};






#endif /* HAPPRINTENCRYPTED_HPP_ */