//
// HAPBonjour.cpp
// Homekit
//
//  Created on: 24.06.2017
//      Author: michael
//

#include <stdlib.h>
#include "HAPBonjour.hpp"
#include "HAPLogger.hpp"
#include "HAPGlobals.hpp"
#include "HAPDeviceID.hpp"

#if defined(ARDUINO_ARCH_ESP32)

bool MDNSResponderExt::addServiceTxtSet(char *name, const char * proto, uint8_t num_items, mdns_txt_item_t *txt){

	// if(mdns_service_txt_set(name, proto, txt, num_items)) {
	// 	log_e("Failed setting service TXT set");
	// 	return false;
	// }
	return true;
}

bool MDNSResponderExt::removeServiceTxt(char *name, const char * proto){
	if(mdns_service_remove(name, proto)) {
		log_w("Failed removing TXT service");
		return false;
	}
	return true;
}

bool MDNSResponderExt::enableHomekit(uint16_t port, const char* idStr, const char* modelName, uint8_t accessoryType, const char* setupHash, const char* protocalVersion){

	char idString[18];
	HAPDeviceID::deviceID(idString);

	char ciString[3];
	itoa(accessoryType, ciString, 10);

    mdns_txt_item_t hapTxtData[9] = {
        {(char*)"id"	,(char*)idString},
        {(char*)"md"	,(char*)modelName},
		{(char*)"sh"   	,(char*)setupHash},
		{(char*)"ci"	,(char*)ciString},
		{(char*)"pv"   	,(char*)"1.0"},
        {(char*)"c#"	,(char*)"0"},
		{(char*)"ff"	,(char*)"0"},
		{(char*)"s#"   	,(char*)"1"},
		{(char*)"sf"   	,(char*)"1"},
    };

	int errorCode = mdns_service_add(NULL, "_hap", "_tcp", port, hapTxtData, 9);
    if (errorCode != 0) {
        LogE("ERROR: Failed adding Homekit service", true);
		return false;
    }

    // if (auth && mdns_service_txt_item_set("_hap", "_tcp", "auth_upload", "yes")){
	// 	LogE("ERROR: Failed setting Arduino txt item", true);
	// 	return false;
    // }

	return true;
}

bool MDNSResponderExt::updateHomekitTxt(bool isPaired, uint32_t configNumber){


	{
		// c# - Configuration Number
		char value[32];
		itoa(configNumber, value, 10);
		if (!updateHomekitTxtItem("c#", value)) return false;
	}


	// {
	// 	// ff - Feature Flag
	// 	char value[3];
	// 	itoa((uint8_t)0, value, 10);
	// 	if (!updateHomekitTxtItem("ff", value)) return false;
	// }


	{
		// sf - state number
		// Status flags (e.g. "0x04" for bit 3). Value should be an unsigned integer.
		// Required if non-zero.
		// 1 if not paired
		// 0 if paired ??
		char value[3];
		itoa((uint8_t)!isPaired, value, 10);
		if (!updateHomekitTxtItem("sf", value)) return false;
	}


	return true;
}

bool MDNSResponderExt::updateHomekitTxtItem(const char* key, const char* value){
	if (mdns_service_txt_item_set("_hap", "_tcp", key, value)){
		LogE("ERROR: Failed setting Homekit txt item:" + String(key) + "="  + String(value), true);
		return false;
	}
	return true;
}




MDNSResponderExt mDNSExt;

#endif