// 
// HAPConfigurationT41SPIFFSInt.hpp
// Teensy_Homekit
//
//  Created on: 15.02.2021
//      Author: michael
//

#ifndef HAPCONFIGURATIONT41SPIFFSINT_HPP_
#define HAPCONFIGURATIONT41SPIFFSINT_HPP_


#include <Arduino.h>
#include <spiffs.h>
#include <vector>
#include <functional>
#include "HAPConfiguration.hpp"

#define HAP_SPIFFS_MAGIC_BYTE		0x43E7
#define HAP_SPIFFS_VERSION			0x37

class HAPConfigurationT41SPIFFSInt : public HAPConfiguration {
public:
    HAPConfigurationT41SPIFFSInt();
    ~HAPConfigurationT41SPIFFSInt();

	bool begin() override;
	bool getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize) override;
	size_t getDataLengthForPlugin(const char* name) override;

	void reset() override;

	void validateConfig() override;
	bool validConfig() override;

    // 
    // SPIFFS CALLBACKS FOR INTERNAL FLASH
    // 
    static s32_t read(u32_t addr, u32_t size, u8_t *dst);
    static s32_t erase(u32_t addr, u32_t size);
    static s32_t write(u32_t addr, u32_t size, u8_t *src);

    
protected:

	size_t writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen) override;
	size_t readBytes(const char* label, uint8_t* output, const size_t expectedDataLen) override;
	size_t getBytesLength(const char* label) override;

    // 
    // SPIFFS INTERNAL FLASH
    //
    static void flash_wait();
    static void flash_write(void *addr, const void *data, uint32_t len);
    static void flash_erase_sector(void *addr);

    bool mount();


    static spiffs _fs;
};


#endif /* HAPCONFIGURATIONT41SPIFFSINT_HPP_ */

