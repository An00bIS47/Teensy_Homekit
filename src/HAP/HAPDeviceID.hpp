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

	static String deviceID();	// mac address -> change to random
	static String chipID();		// basically mac reverse

	static String serialNumber(String type, String id);

#if defined(ARDUINO_ARCH_ESP32)
	static String provisioningID(const char* prefix);
#endif

private:
	static uint8_t _deviceID[6];
};

#endif /* HAPDEVICEID_HPP_ */
