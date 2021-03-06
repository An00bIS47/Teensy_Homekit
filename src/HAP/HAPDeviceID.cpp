//
// HAPDeviceID.cpp
// Homekit
//
//  Created on: 23.08.2017
//      Author: michael
//

#include "HAPDeviceID.hpp"
#include "HAPHelper.hpp"


uint8_t HAPDeviceID::_deviceID[] = {'\0'};

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
uint8_t* HAPDeviceID::generateID() {

    if (_deviceID[0] == '\0')
#if defined(ARDUINO_ARCH_ESP32)
        esp_read_mac(_deviceID, ESP_MAC_WIFI_STA);
#elif defined ( CORE_TEENSY )
        teensyMAC(_deviceID);
#endif

    return _deviceID;
}

/*
const char* HAPDeviceID::deviceID(){
    char baseMacChr[18];
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", _deviceID[0], _deviceID[1], _deviceID[2], _deviceID[3], _deviceID[4], _deviceID[5]);
    return baseMacChr;
}
*/

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
std::string HAPDeviceID::deviceID(){
    char baseMacChr[18];
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", _deviceID[0], _deviceID[1], _deviceID[2], _deviceID[3], _deviceID[4], _deviceID[5]);
    return std::string(baseMacChr);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPDeviceID::deviceID(char baseMacChr[18]){
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", _deviceID[0], _deviceID[1], _deviceID[2], _deviceID[3], _deviceID[4], _deviceID[5]);
    baseMacChr[18] = '\0';
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
std::string HAPDeviceID::chipID(){
    char baseMacChr[18];
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", _deviceID[5], _deviceID[4], _deviceID[3], _deviceID[2], _deviceID[1], _deviceID[0]);
    return std::string(baseMacChr);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
std::string HAPDeviceID::serialNumber(const char* type, const char* id){
    char serialNumber[6 + 2 + strlen(type) + strlen(id)];
    sprintf(serialNumber, "%02X%02X%02X-%s-%s", _deviceID[3], _deviceID[4], _deviceID[5], type, id);
    return std::string(serialNumber);
}

#if defined(ARDUINO_ARCH_ESP32)
std::string HAPDeviceID::provisioningID(const char* prefix){
    char provId[6 + 1 + strlen(prefix)];
    sprintf(provId, "%s%02X%02X%02X", prefix, _deviceID[3], _deviceID[4], _deviceID[5]);
    return std::string(provId);
}
#endif