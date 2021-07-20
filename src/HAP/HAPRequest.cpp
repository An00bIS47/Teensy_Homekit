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
	method = HAPServerMethod::Unknown;
}

HAPRequest::~HAPRequest() {
	clear();
}

#if HAP_DEBUG_HOMEKIT
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
std::string HAPRequest::toString() const {
	std::string result = "method: ";
	result += method;

	result += "\npath: ";
	result += path;

	result += "\ncontentType: ";
	result += contentType;

	result += "\ncontentLength: ";
	result += contentLength;

	return result;
}
#endif

void HAPRequest::clear(){
	path = "";
	contentType = "";
	contentLength = 0;
	method = HAPServerMethod::Unknown;

	tlv.clear();
	params.clear();
}
