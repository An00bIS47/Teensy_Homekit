//
// HAPPluginRF24Device.cpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#include "HAPPluginKNXDevice.hpp"
#include "HAPServer.hpp"
#include "HAPLogger.hpp"

HAPPluginKNXDevice::HAPPluginKNXDevice(){   
    // _name    = "";    
    _id = 0;
    _type = HAPPluginKNXServiceTypeNone;
    
    _accessory          = nullptr;
    _eventManager       = nullptr;  
    _fakegatoFactory    = nullptr;
}

// HAPPluginRF24Device::HAPPluginRF24Device(uint16_t id_, String name_, uint8_t measureMode_)
// : id(id_)
// , name(name_)
// {
//     type = 0;
//     _accessory          = nullptr;
//     _eventManager       = nullptr;      
//     _fakegatoFactory    = nullptr;

//     sleepInterval       = 1;
//     measureMode         = (enum MeasureMode)measureMode_;
// }

HAPPluginKNXDevice::~HAPPluginKNXDevice(){
}

void HAPPluginKNXDevice::setEventManager(EventManager* eventManager){
      
    _eventManager = eventManager;
    // Serial.printf("event: %p\n", _eventManager);  
}


void HAPPluginKNXDevice::setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory){
    
    _fakegatoFactory = fakegatoFactory;
    // Serial.printf("fakegato: %p\n", _fakegatoFactory);
}   


void HAPPluginKNXDevice::identify(bool oldValue, bool newValue) {
    printf("Start Identify knx: %d\n", _id);
}

