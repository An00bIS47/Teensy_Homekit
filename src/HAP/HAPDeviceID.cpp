//
// HAPDeviceID.cpp
// Homekit
//
//  Created on: 23.08.2017
//      Author: michael
//

#include "HAPDeviceID.hpp"
#include "HAPHelper.hpp"


uint8_t HAPDeviceID::_mac[6] = {'\0', };


uint8_t* HAPDeviceID::mac() {

    if (_mac[0] == '\0')
#if defined(ARDUINO_ARCH_ESP32)
        esp_read_mac(_mac, ESP_MAC_WIFI_STA);
#elif defined ( CORE_TEENSY )
        teensyMAC(_mac);
#endif

    return _mac;
}


void HAPDeviceID::deviceID(char* baseMacChr){
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
    baseMacChr[17] = '\0';
}

void HAPDeviceID::chipID(char* baseMacChr){
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", _mac[5], _mac[4], _mac[3], _mac[2], _mac[1], _mac[0]);
    baseMacChr[12] = '\0';
}

void HAPDeviceID::serialNumber(const char* type, const char* id, char* serialNumberStr, size_t* len){
    *len = (6 + 2 + strlen(type) + strlen(id)) + 1;

    if (serialNumberStr == nullptr ) return;

    sprintf(serialNumberStr, "%02X%02X%02X-%s-%s", _mac[3], _mac[4], _mac[5], type, id);
    serialNumberStr[*len - 1] = '\0';
}

#if defined(ARDUINO_ARCH_ESP32)
std::string HAPDeviceID::provisioningID(const char* prefix){
    char provId[6 + 1 + strlen(prefix)];
    sprintf(provId, "%s%02X%02X%02X", prefix, _deviceID[3], _deviceID[4], _deviceID[5]);
    return std::string(provId);
}
#endif