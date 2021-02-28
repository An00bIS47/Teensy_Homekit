//
// HAPConfigurationEEPROM.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONEEPROM_HPP_
#define HAPCONFIGURATIONEEPROM_HPP_

#include <Arduino.h>
#include <vector>
#include <functional>
#include "HAPConfiguration.hpp"
#include <EEPROM.h>
// #include <StreamUtils.h>

struct DataEntry {
	size_t offset;
	size_t size;
	char name[20+1] = {0,};

	DataEntry(size_t offset_, size_t size_, const char* name_)
		: offset(offset_)
		, size(size_)
		{			
		strncpy(name, name_, 20);
	}

	void printTo(Print& prt){

		prt.printf("name: %12s", name);
		prt.printf(" - offset: %4d", offset);
		prt.printf(" - size: %4d", size);
		
		prt.println("");
	}
};

class HAPConfigurationEEPROM : public HAPConfiguration {
public:
	

	HAPConfigurationEEPROM();
	~HAPConfigurationEEPROM();

	bool begin() override;
	bool getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize) override;
	size_t getDataLengthForPlugin(const char* name) override;

	void reset() override;

	void validateConfig() override;
	bool validConfig() override;


	void printDataMapTo(Print& prt);
protected:	

#if defined (ARDUINO_ARCH_ESP32)
	EEPROMClass* _eeprom;
#endif

	
	void buildDataMap();

	size_t writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen) override;
	size_t readBytes(const char* label, uint8_t* output, const size_t expectedDataLen) override;
	size_t getBytesLength(const char* label) override;
	
	size_t getAddressForLabel(const char* label);
	int getIndexForLabel(const char* name);

	std::vector<DataEntry> _dataMap;
};




#endif /* HAPCONFIGURATIONEEPROM_HPP_ */