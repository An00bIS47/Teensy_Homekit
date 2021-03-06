//
// HAPRequest.hpp
// Homekit
//
//  Created on: 12.08.2017
//      Author: michael
//

#ifndef HAPREQUEST_HPP_
#define HAPREQUEST_HPP_

#include <Arduino.h>
#include <map>

#include "HAPTLV8.hpp"

enum HAPServerMethod {
	METHOD_UNKNOWN = 0,
	METHOD_POST,
	METHOD_GET,
	METHOD_PUT,
	METHOD_DELETE,
};

class HAPRequest {
public:
	HAPRequest();
	~HAPRequest();


	HAPServerMethod 	method;
	std::string 		path;
	std::string 		contentType;
	uint16_t 			contentLength;
	TLV8				tlv;

	std::map<std::string, std::string> params;

	void clear();

#if HAP_DEBUG_HOMEKIT
	std::string toString() const;
#endif
private:

};

#endif /* HAPREQUEST_HPP_ */
