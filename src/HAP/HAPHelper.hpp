//
// HAPHelper.hpp
// HomekitAccessoryProtocol
//
//  Created on: 06.08.2017
//      Author: michael
//

#ifndef HAPHELPER_HPP_
#define HAPHELPER_HPP_

#include <Arduino.h>
// #include <ArduinoJson.h>
// #include "HAPGlobals.hpp"

// #if defined( CORE_TEENSY )
// #include <time.h>
// #include <TimeLib.h>
// #endif

#ifndef byte
#define byte uint8_t
#endif

#include "mbedtls/bignum.h"

class HAPHelper {
public:
	HAPHelper();
	~HAPHelper();

	// static union {
	// 	uint32_t bit32;
	// 	uint8_t bit8[4];
	// } HAPBit32to8Converter;


	// static String getValue(String data, char separator, int index) __attribute__ ((deprecated));


	static void binToHex(const unsigned char * in, size_t insz, char * out, size_t outsz);
	static int hexToBin(unsigned char *data, const char* hexstring, size_t len);

	// DEPRECATED functions causing memory leaks
	// static uint8_t* hexToBin(const char* string) __attribute__ ((deprecated));
	// static char* toHex(const unsigned char * in, size_t insz) __attribute__ ((deprecated));

	static void prependZeros(char *dest, const char *src, uint8_t width); // __attribute__ ((deprecated));

	static uint8_t numDigits(const size_t n);
	// static void arrayPrint(uint8_t* a, int len);

	static String wrap(String str);
	static String wrap(const char *str);
	static String arrayWrap(String *s, unsigned short len);
	static String dictionaryWrap(String *key, String *value, unsigned short len);
	static String removeBrackets(String str);

	// static String printUnescaped(String str);

	// static bool containsNestedKey(const JsonObject obj, const char* key);
	// static void mergeJson(JsonDocument& dst, const JsonObject& src);

	static void printHex(const char* suffix, const uint8_t *data, size_t length, bool newline = true);

	static uint32_t u8_to_u32(const uint8_t* bytes) {
		uint32_t u32 = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
		return u32;
	}

	static void u32_to_u8(const uint32_t u32, uint8_t* u8) {
		u8[0] = (u32 & 0xff000000) >> 24;
		u8[1] = (u32 & 0x00ff0000) >> 16;
		u8[2] = (u32 & 0x0000ff00) >> 8;
		u8[3] = u32 & 0x000000ff;
	}

	static void u16_to_u8(const uint16_t u16, uint8_t* u8){
    	u8[0] = (uint8_t)(u16 & 0xFF);
    	u8[1] = (uint8_t)((u16 >> 8) & 0xFF);
	}

	static uint16_t u8_to_u16(const uint8_t* bytes) {
		uint16_t u16 = (bytes[1] | (bytes[0] << 8));
		return u16;
	}


	static size_t base64_enc_len(size_t plainLen);
	static size_t base64_dec_len(char * input, size_t inputLen);

#if defined(ARDUINO_ARCH_ESP32)
	static void getPartionTableInfo();
#endif

	static void mpi_print(const char* tag, const mbedtls_mpi* x);
	static void array_print(const char* tag, const unsigned char* buf, int len);



	static bool isValidFloat(String tString);
	static bool isValidNumber(String str);


	// rounds a number to 2 decimal places
	// example: round(3.14159) -> 3.14
	static double round2(double value) {
   		return (int)(value * 100 + 0.5) / 100.0;
	}

};

// template<typename T>
// struct deleter : std::unary_function<const T*, void>
// {
//   void operator() (const T *ptr) const
//   {
//     delete ptr;
//   }
// };

#endif /* HAPHELPER_HPP_ */
