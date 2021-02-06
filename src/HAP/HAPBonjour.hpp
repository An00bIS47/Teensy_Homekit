//
// HAPBonjour.hpp
// Homekit
//
//  Created on: 24.06.2017
//      Author: michael
//

#ifndef HAPBONJOUR_HPP_
#define HAPBONJOUR_HPP_

#if defined(ARDUINO_ARCH_ESP32)

#include "Arduino.h"
#include <ESPmDNS.h>
#include "mdns.h"
#include "HAPConfigurationAccessory.hpp"

class MDNSResponderExt : public MDNSResponder {
public:

	// Added for Homekit
	bool removeServiceTxt(char *name, const char * proto);
	bool addServiceTxtSet(char *name, const char * proto, uint8_t num_items, mdns_txt_item_t *txt);
	bool enableHomekit(uint16_t port, const char* idStr, const char* modelName, uint8_t accessoryType, const char* setupHash, const char* protocalVersion = "1.0");
	bool updateHomekitTxt(bool isPaired, uint32_t configNumber);
	bool updateHomekitTxtItem(const char* key, const char* value);

	
private:

};

extern MDNSResponderExt mDNSExt;
#endif

#endif /* HAPBONJOUR_HPP_ */
