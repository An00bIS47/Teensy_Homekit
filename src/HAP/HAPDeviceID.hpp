//
// HAPDeviceID.hpp
// Homekit
//
//  Created on: 23.08.2017
//      Author: michael
//

#ifndef HAPDEVICEID_HPP_
#define HAPDEVICEID_HPP_

#include <Arduino.h>
#include <string>
#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include "esp_system.h"
#elif defined ( CORE_TEENSY )
#include <TeensyID.h>
#endif

class HAPDeviceID {
public:

	static uint8_t* generateID();
	//static byte* deviceID();
	//static const char* deviceID();
	static void deviceID(char baseMacChr[18]);

	static std::string deviceID();	// mac address -> change to random
	static std::string chipID();		// basically mac reverse

	static std::string serialNumber(const char* type, const char* id);

#if defined(ARDUINO_ARCH_ESP32)
	static std::string provisioningID(const char* prefix);
#endif

private:
	static uint8_t _deviceID[6];
};

#endif /* HAPDEVICEID_HPP_ */
