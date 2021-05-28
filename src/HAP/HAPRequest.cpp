//
// HAPRequest.cpp
// Homekit
//
//  Created on: 12.08.2017
//      Author: michael
//

#include "HAPRequest.hpp"
#include <WString.h>



HAPRequest::HAPRequest() {
	path = "";
	contentType = "";
	contentLength = 0;
	method = METHOD_UNKNOWN;
}

HAPRequest::~HAPRequest() {
	clear();
}

#if HAP_DEBUG_HOMEKIT
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
String HAPRequest::toString() const {
	String result = F("method: ");
	result += method;

	result += F("\npath: ");
	result += path;

	result += F("\ncontentType: ");
	result += contentType;

	result += F("\ncontentLength: ");
	result += contentLength;

	return result;
}
#endif

void HAPRequest::clear(){
	path = "";
	contentType = "";
	contentLength = 0;
	method = METHOD_UNKNOWN;

	tlv.clear();
	params.clear();
}
