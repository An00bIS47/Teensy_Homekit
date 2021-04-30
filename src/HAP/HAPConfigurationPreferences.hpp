//
// HAPConfigurationPreferences.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONPREFERENCES_HPP_
#define HAPCONFIGURATIONPREFERENCES_HPP_

#include <Arduino.h>
#include "HAPConfiguration.hpp"
#include <Preferences.h>

class HAPConfigurationPreferences : public HAPConfiguration {
public:
	HAPConfigurationPreferences();
	~HAPConfigurationPreferences();

	void reset() override;

	bool getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize) override;
	size_t getDataLengthForPlugin(const char* name) override;

protected:
	Preferences _prefs;

	size_t writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen) override;
	size_t readBytes(const char* label, uint8_t* output, const size_t expectedDataLen) override;
	size_t getBytesLength(const char* label) override;
};




#endif /* HAPCONFIGURATIONPREFERENCES_HPP_ */