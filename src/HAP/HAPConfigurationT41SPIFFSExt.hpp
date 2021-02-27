// 
// HAPConfigurationT41SPIFFSExt.hpp
// Teensy_Homekit
//
//  Created on: 18.02.2021
//      Author: michael
//

#ifndef HAPCONFIGURATIONT41SPIFFSEXT_HPP_
#define HAPCONFIGURATIONT41SPIFFSEXT_HPP_

#include <Arduino.h>
#include <vector>
#include <functional>
#include "HAPConfiguration.hpp"
#include <extRAM_t4.h>

#define HAP_SPIFFS_MAGIC_BYTE		0x43E7
#define HAP_SPIFFS_VERSION			0x37


class HAPConfigurationT41SPIFFSExt : public HAPConfiguration {
public:
    HAPConfigurationT41SPIFFSExt();
    // HAPConfigurationT41SPIFFSExt(extRAM_t4* config);
    ~HAPConfigurationT41SPIFFSExt();

	bool begin() override;
	bool getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize) override;
	size_t getDataLengthForPlugin(const char* name) override;

	void reset() override;

	void validateConfig() override;
	bool validConfig() override;

    void formatFlash();
protected:

    bool fileExists(const char* name);

    bool mount();

	size_t writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen) override;
	size_t readBytes(const char* label, uint8_t* output, const size_t expectedDataLen) override;
	size_t getBytesLength(const char* label) override;

    extRAM_t4   _eRAM;
    uint8_t     _config;
};

// static extRAM_t4 eRAM __attribute__ ((unused));

#endif /* HAPCONFIGURATIONT41SPIFFSEXT_HPP_ */