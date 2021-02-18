// 
// HAPConfigurationT41SPIFFSExt.cpp
// Homekit
//
//  Created on: 18.02.2021
//      Author: michael
//

#include "HAPConfigurationT41SPIFFSExt.hpp"
#include "HAPHelper.hpp"
#include "HAPLogger.hpp"

HAPConfigurationT41SPIFFSExt::HAPConfigurationT41SPIFFSExt() : _config(INIT_FLASH_ONLY){
    
}

HAPConfigurationT41SPIFFSExt::~HAPConfigurationT41SPIFFSExt(){

}

bool HAPConfigurationT41SPIFFSExt::begin(){
   return mount();
}

void HAPConfigurationT41SPIFFSExt::formatFlash(){
    int8_t result = _eRAM.begin(_config);
    if(result == 0){
      _eRAM.eraseFlashChip();
    } else {
      _eRAM.eraseDevice();
    }
}


bool HAPConfigurationT41SPIFFSExt::mount(){
    int8_t result = _eRAM.begin(_config);
    if (result == 0){
        _eRAM.fs_mount();
        return true;
    } else {
        return false;
    } 
}

size_t HAPConfigurationT41SPIFFSExt::writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen) {
    // int result = _eRAM.f_writeFile(label, (const char*)input, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);

    spiffs_file file1;
    int result = _eRAM.f_open(file1, label, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR);
    // if (result != 0) return 0;

    result = _eRAM.f_write(file1, (const char*)input, expectedDataLen);
    _eRAM.f_close(file1);
    
    if (result > 0) {        
        return expectedDataLen;
    } else {
        LogE("ERROR: Failed to writeBytes from " + String(label) + ": " + String(result), true);
    }

    return result;
}


size_t HAPConfigurationT41SPIFFSExt::readBytes(const char* label, uint8_t* output, const size_t expectedDataLen) {

    if (!fileExists(label)) return 0;
    
    // int result = _eRAM.f_readFile(label, (const char*)output, expectedDataLen, SPIFFS_RDONLY);

    spiffs_file file1;
    int result = _eRAM.f_open(file1, label, SPIFFS_RDONLY);
    // if (result != 0) return 0;

    result = _eRAM.f_read(file1, (const char*)output, expectedDataLen);    
    _eRAM.f_close(file1);

    if (result > 0) {        
        return expectedDataLen;
    } else {
        LogE("ERROR: Failed to readBytes from " + String(label) + ": " + String(result), true);
    }
    
    return result;
}


size_t HAPConfigurationT41SPIFFSExt::getBytesLength(const char* label){
    if (!fileExists(label)) return 0;
    
    spiffs_stat fileInfo;
    _eRAM.f_info(label, &fileInfo);    

    LogD("Filesize for " + String(label) + ": " + String(fileInfo.size), true);
    return fileInfo.size;
}

size_t HAPConfigurationT41SPIFFSExt::getDataLengthForPlugin(const char* name){
    char label[20];	
	sprintf(label, "p%s", name);
	return getBytesLength(label);
}

bool HAPConfigurationT41SPIFFSExt::getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize){
    
    char label[20];	
	sprintf(label, "p%s", name);

    if (!fileExists(label)) return false;

    size_t read = readBytes(label, data, dataSize);
    if (read == dataSize) return true;

    return false;
}

void HAPConfigurationT41SPIFFSExt::reset(){

}

void HAPConfigurationT41SPIFFSExt::validateConfig() {
    uint8_t magNum[2];
    HAPHelper::u16_to_u8(HAP_SPIFFS_MAGIC_BYTE, magNum);    

    writeBytes("validate", magNum, 2);    
}

bool HAPConfigurationT41SPIFFSExt::validConfig() {

    if (!fileExists("validate")) return false;

    uint8_t magNum[2];
    size_t res = readBytes("validate", magNum, 2);    
    
    if (res == 0) return false;

    uint16_t magicNum = HAPHelper::u8_to_u16(magNum);
    return (magicNum == HAP_SPIFFS_MAGIC_BYTE);
}

bool HAPConfigurationT41SPIFFSExt::fileExists(const char* name){

    spiffs_file fd;
    int result = _eRAM.f_open(fd, name, SPIFFS_RDONLY);
    _eRAM.f_close(fd);

    if (result == SPIFFS_OK) {
        
    } else {        
        LogW("File does not exist: " + String(name), true);
    }
    
    return (result == SPIFFS_OK);
}