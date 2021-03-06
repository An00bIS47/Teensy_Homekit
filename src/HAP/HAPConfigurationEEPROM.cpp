//
// HAPConfigurationEEPROM.cpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#include "HAPConfigurationEEPROM.hpp"
#include "HAPPlugins.hpp"
#include "HAPLogging.hpp"

#define HAP_TEENSY_EEPROM_SIZE 4284

#define HAP_EEPROM_PARTITION_NAME 	"HKeeprom"
#define HAP_EEPROM_PARTITION_SIZE 	4096
#define HAP_EEPROM_MAGIC_BYTE		0x43E7
#define HAP_EEPROM_VERSION			0x01

// #define HAP_EEPROM_OFFSET			0
#define HAP_EEPROM_OFFSET_INTERNAL	HAP_EEPROM_OFFSET + 5	// uin32_t Magic byte + uint8_t version


HAPConfigurationEEPROM::HAPConfigurationEEPROM(){
#if defined (ARDUINO_ARCH_ESP32)
	_eeprom = nullptr;
#endif
}


HAPConfigurationEEPROM::~HAPConfigurationEEPROM(){

#if defined (ARDUINO_ARCH_ESP32)
	if (_eeprom == nullptr) {
		delete _eeprom;
		_eeprom = nullptr;
	}
#endif

}

bool HAPConfigurationEEPROM::begin() {
	buildDataMap();

	// printDataMapTo(Serial);
	logDataMap();

#if defined (ARDUINO_ARCH_ESP32)
	//EEPROM.begin(KNX_FLASH_SIZE);

	if (_eeprom == nullptr){
		_eeprom = new EEPROMClass(HAP_EEPROM_PARTITION_NAME, HAP_EEPROM_PARTITION_SIZE);
		_eeprom->begin(HAP_EEPROM_PARTITION_SIZE);
	}

#else
	EEPROM.begin();
#endif

	return true;
}

void HAPConfigurationEEPROM::validateConfig(){
#if defined (ARDUINO_ARCH_ESP32)
	if (_eeprom == nullptr) {
		buildDataMap();
		_eeprom = new EEPROMClass(HAP_EEPROM_PARTITION_NAME, HAP_EEPROM_PARTITION_SIZE);
		_eeprom->begin(HAP_EEPROM_PARTITION_SIZE);
	}

	_eeprom->writeUShort(HAP_EEPROM_OFFSET+0, HAP_EEPROM_MAGIC_BYTE);
	_eeprom->writeUChar(HAP_EEPROM_OFFSET+2, HAP_EEPROM_VERSION);

	_eeprom->commit();
#else
	writeLong(HAP_EEPROM_OFFSET + 0, HAP_EEPROM_MAGIC_BYTE);
	EEPROM.write(HAP_EEPROM_OFFSET + 4, HAP_EEPROM_VERSION);
#endif
}


bool HAPConfigurationEEPROM::validConfig(){

	uint32_t magicByte;

#if defined (ARDUINO_ARCH_ESP32)
	if (_eeprom == nullptr){
		buildDataMap();
		_eeprom = new EEPROMClass(HAP_EEPROM_PARTITION_NAME, HAP_EEPROM_PARTITION_SIZE);
		_eeprom->begin(HAP_EEPROM_PARTITION_SIZE);
	}

	magicByte = _eeprom->readLong(HAP_EEPROM_OFFSET + 0);

#else
	magicByte = readLong(HAP_EEPROM_OFFSET);
#endif
	// HAPHelper::printHex("magic byte ", (uint8_t*)magicByte, 2);
	LOG_D("Magic Byte: %X\n", magicByte);

	return (magicByte == HAP_EEPROM_MAGIC_BYTE);
}

size_t HAPConfigurationEEPROM::getBytesLength(const char* label) {
	int index = getIndexForLabel(label);
	if (index >= 0){
		return _dataMap[index].size;
	}
	return 0;
}


size_t HAPConfigurationEEPROM::getAddressForLabel(const char* label) {
	int index = getIndexForLabel(label);
	if (index >= 0){
		return _dataMap[index].offset;
	}
	return 0;
}

size_t HAPConfigurationEEPROM::getDataLengthForPlugin(const char* name) {
	char label[20];
	sprintf(label, "p%s", name);
	return getBytesLength(label);
}

int HAPConfigurationEEPROM::getIndexForLabel(const char* name){
	for (uint8_t i=0; i < _dataMap.size(); i++){
		if (strncmp(name, _dataMap[i].name, 20) == 0){
			return i;
		}
	}
	return -1;
}


void HAPConfigurationEEPROM::buildDataMap(){
	size_t offset = HAP_EEPROM_OFFSET_INTERNAL;

	// Platform
	{
		DataEntry newEntry = DataEntry(offset, HAPConfigurationPlatform::getDataSize(), "cPltfrm");
		offset += HAPConfigurationPlatform::getDataSize();
		_dataMap.push_back(newEntry);
	}


	// Accessory
	{
		DataEntry newEntry = DataEntry(offset, HAPConfigurationAccessory::getDataSize(), "cAcc");
		offset += HAPConfigurationAccessory::getDataSize();
		_dataMap.push_back(newEntry);
	}

#if HAP_ENABLE_WIFI
	// WiFi
	{
		DataEntry newEntry = DataEntry(offset, HAPConfigurationWiFi::getDataSize(), "cWiFi");
		offset += HAPConfigurationWiFi::getDataSize();
		_dataMap.push_back(newEntry);
	}
#endif

#if HAP_ENABLE_KEYSTORE
	// OTA
	{
		DataEntry newEntry = DataEntry(offset, HAPConfigurationKeystore::getDataSize(), "cKeySt");
		offset += HAPConfigurationKeystore::getDataSize();
		_dataMap.push_back(newEntry);
	}
#endif

#if HAP_ENABLE_WEBSERVER
	// WebServer
	{
		DataEntry newEntry = DataEntry(offset, HAPConfigurationWebServer::getDataSize(), "cWebSvr");
		offset += HAPConfigurationWebServer::getDataSize();
		_dataMap.push_back(newEntry);
	}
#endif

#if HAP_ENABLE_UPDATE_OTA
	// OTA
	{
		DataEntry newEntry = DataEntry(offset, HAPConfigurationArduinoOTA::getDataSize(), "cOTA");
		offset += HAPConfigurationArduinoOTA::getDataSize();
		_dataMap.push_back(newEntry);
	}
#endif



	auto &factory = HAPPluginFactory::Instance();
    std::vector<std::string> names = factory.names();


    for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it) {
    	auto plugin = factory.getPlugin(*it);
		char label[20];
		sprintf(label, "p%s", plugin->name());
		HAPConfigurationPlugin* pluginConfig = plugin->getConfiguration();

		size_t dataSize = pluginConfig->getCommonSize() + pluginConfig->getDataSize();

		DataEntry newEntry = DataEntry(offset, dataSize, label);
		offset += dataSize;
		_dataMap.push_back(newEntry);
    }
}

bool HAPConfigurationEEPROM::getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize){
	char label[20];
	sprintf(label, "p%s",name);

	// Serial.println("Loading data for " + String(name) + " EEPROM: ");
	// Serial.print("name: ");
	// Serial.println(name);
	// Serial.print("label: ");
	// Serial.println(label);
	// Serial.print("dataSize: ");
	// Serial.println(dataSize);
	// Serial.print("address: ");
	// Serial.println(getAddressForLabel(label));

	// uint8_t buffer[dataSize];
	size_t read = readBytes(label, data, dataSize);

#if HAP_DEBUG_CONFIGURATION
	char text[32];
	sprintf(text, "LOAD PLUGIN %s", name);
	LOGARRAY_D(text, (uint8_t*)data, dataSize);
#endif

	if (read == dataSize) {
		return true;
	}
	return false;
}

size_t HAPConfigurationEEPROM::readBytes(const char* label, uint8_t* output, const size_t expectedDataLen){
	if (getIndexForLabel(label) == -1) return 0;

	size_t address = getAddressForLabel(label);
	for (int i=0; i < expectedDataLen; i++){

#if defined (ARDUINO_ARCH_ESP32)
		output[i] = _eeprom->read(address + i);
#else
		output[i] = EEPROM.read(address + i);
#endif
	}
	return expectedDataLen;

	// EEPROM.begin();
	// EepromStream s(address, expectedDataLen);
	// size_t read = s.readBytes(output, expectedDataLen);

	// EEPROM.end();
	// return read;
}




size_t HAPConfigurationEEPROM::writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen){
	if (getIndexForLabel(label) == -1) return 0;

	size_t address = getAddressForLabel(label);


#if defined (ARDUINO_ARCH_ESP32)
	// for (int i=0; i < expectedDataLen; i++){
	// 	// EEPROM.write(address + i, input[i]);
	// 	_eeprom->write(address + i, input[i]);
	// }

	_eeprom->writeBytes(address, input, expectedDataLen);

	//EEPROM.commit();
	_eeprom->commit();
	return expectedDataLen;
#else
	for (int i=0; i < expectedDataLen; i++){
		EEPROM.update(address + i, input[i]);
	}
	return expectedDataLen;
#endif


	// EEPROM.begin();
	// EepromStream s(address, expectedDataLen);
	// size_t written = 0;
	// if (expectedDataLen > 0){
	// 	written = s.write(input, expectedDataLen);
	// 	Serial.print(">>>>>>>>>> WRITE BYTES 4: written ");
	// 	Serial.println(written);
	// 	Serial.send_now();
	// }

	// s.flush();
	// EEPROM.end();

	// return written;
}

void HAPConfigurationEEPROM::reset(){
	size_t lastAddress = 0;
	lastAddress += _dataMap[_dataMap.size()-1].offset;
	lastAddress += _dataMap[_dataMap.size()-1].size;


#if defined (ARDUINO_ARCH_ESP32)
	for (int i=0; i < lastAddress; i++){
		// EEPROM.write(i, 0xFF);

		_eeprom->write(i, 0xFF);
	}
	// EEPROM.commit();
	_eeprom->commit();
#else
	for (int i=0; i < lastAddress; i++){
		EEPROM.update(i, 0xFF);
	}
#endif


	// EEPROM.begin();
	// EepromStream s(0, lastAddress);
	// for (int i=0; i < lastAddress; i++){
	// 	s.print(0x00);
	// }
	// s.flush();
	// EEPROM.end();
}

void HAPConfigurationEEPROM::printDataMapTo(Print& prt){
	prt.println("DataMap:");
	for (uint8_t i=0; i < _dataMap.size(); i++){
		_dataMap[i].printTo(prt);
	}
}

void HAPConfigurationEEPROM::logDataMap(){
	LOG_D("DataMap:\n");
	for (uint8_t i=0; i < _dataMap.size(); i++){
		LOG_D(" - name: %-20s - offset: %4d - size: %4d\n", _dataMap[i].name, _dataMap[i].offset, _dataMap[i].size);
	}

}


void HAPConfigurationEEPROM::writeLong(int address, uint32_t value)
{
    uint8_t four = (value & 0xFF);
    uint8_t three = ((value>>8)&0xFF);
    uint8_t two = ((value>>16)&0xFF);
    uint8_t one = ((value>>24)&0xFF);

    EEPROM.write(address , four);
    EEPROM.write(address+1, three);
    EEPROM.write(address+2, two);
    EEPROM.write(address+3, one);
}

uint32_t HAPConfigurationEEPROM::readLong(int address)
{
    uint8_t four = EEPROM.read(address);
    uint8_t three = EEPROM.read(address+1);
    uint8_t two = EEPROM.read(address+2);
    uint8_t one = EEPROM.read(address+3);

    return ((four<<0)&0xFF)+((three<<8)&0xFFFF)+((two<<16)&0xFFFFFF)+((one<<24)&0xFFFFFFFF);
}