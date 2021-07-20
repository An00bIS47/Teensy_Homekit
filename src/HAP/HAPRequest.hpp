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


struct HAPServerMethod {
	enum Type : uint8_t {
		Unknown 	= 0x00,
		Post		= 0x01,
		Get			= 0x02,
		Put			= 0x03,
		Delete		= 0x04
	};
};


class HAPRequest {
	
public:
	HAPRequest();
	~HAPRequest();


	HAPServerMethod::Type 	method;
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
