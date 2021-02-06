//
// HAPConfigurationPreferences.cpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//
//  Used partitions: 
// 		* config			
// 
//  Used labels:
//    Configuration		
// 		* cPltfrm		= 0x01	
// 		* cAcc			= 0x02	
// 		* cWiFi			= 0x03	
// 		* cOTA			= 0x04
// 		* cWebSvr		= 0x05
// 		* cKeySt		= 0x06
// 

#include "HAPPlugins.hpp"
#include "HAPConfigurationPreferences.hpp"

HAPConfigurationPreferences::HAPConfigurationPreferences(){

}

HAPConfigurationPreferences::~HAPConfigurationPreferences(){
	clear();
}

size_t HAPConfigurationPreferences::readBytes(const char* label, uint8_t* output, const size_t expectedDataLen){
	_prefs.begin("config", true);
	size_t dataLen = _prefs.getBytesLength(label);
	if (dataLen != expectedDataLen) {
		_prefs.end();
		return 0;
	}

	size_t read = _prefs.getBytes(label, output, expectedDataLen);
	_prefs.end();
	return read;
}

size_t HAPConfigurationPreferences::writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen){
	_prefs.begin("config", false);
	size_t dataLen = _prefs.getBytesLength(label);
	
	if (dataLen == expectedDataLen) {
		uint8_t buffer[dataLen];
		readBytes(label, buffer, dataLen);

		if (memcmp(buffer, input, expectedDataLen) == 0) {
			_prefs.end();
			return expectedDataLen;
		}
	} 

	size_t written = _prefs.putBytes(label, input, expectedDataLen);
	_prefs.end();
	return written;
}

size_t HAPConfigurationPreferences::getBytesLength(const char* label){
	_prefs.begin("config", true);
	size_t dataLen = _prefs.getBytesLength(label);
	_prefs.end();
	return dataLen;
}


void HAPConfigurationPreferences::reset() {
	_prefs.begin("config", false);
	_prefs.remove("config");
	_prefs.end();
}


bool HAPConfigurationPreferences::getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize){	
	char label[20];	
	sprintf(label, "p%s",name);

	// Serial.println("Loading data from EEPROM: ");
	// Serial.print("name: ");
	// Serial.println(name);
	// Serial.print("label: ");
	// Serial.println(label);
	// Serial.print("dataSize: ");
	// Serial.println(dataSize);

	// uint8_t buffer[dataSize];
	size_t read = readBytes(label, data, dataSize);		

#if HAP_DEBUG_CONFIGURATION
	char text[32];
	sprintf(text, "LOAD PLUGIN %s", name);
	HAPHelper::array_print(text, (uint8_t*)data, dataSize);
#endif

	if (read == dataSize) {
		return true;
	}
	return false;
}

size_t HAPConfigurationPreferences::getDataLengthForPlugin(const char* name){
	char label[20];	
	sprintf(label, "p%s", name);
	return getBytesLength(label);	
}