//
// HAPPluginKNXDevice.hpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#ifndef HAPPLUGINKNXDEVICE_HPP_
#define HAPPLUGINKNXDEVICE_HPP_


#include <Arduino.h>
#include <knx.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include "HAPCharacteristic.hpp"
#include "HAPFakeGato.hpp"
#include "HAPFakeGatoFactory.hpp"
#include "EventManager.h"

enum HAPPluginKNXServiceType {
    HAPPluginKNXServiceTypeNone     = 0x00,
    HAPPluginKNXServiceTypeWeather  = 0x01,
    HAPPluginKNXServiceTypeOutlet   = 0x02,
    HAPPluginKNXServiceTypeSwitch   = 0x03,
};


class HAPPluginKNXDevice {
public:
    HAPPluginKNXDevice();
    
    // HAPPluginKNXDevice(uint8_t id, char name[], bool enableFakegato, uint16_t koTemperature, uint16_t koHumidity, uint16_t koAirPressure);

    ~HAPPluginKNXDevice();
                                
    virtual HAPAccessory* initAccessory() = 0;    
    virtual void handle(bool forced) = 0;

    void identify(bool oldValue, bool newValue);
    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory);    

    static inline String serviceEnumToString(HAPPluginKNXServiceType e){
        switch (e)
        {
        case HAPPluginKNXServiceTypeWeather:
            return "weather";
        
        case HAPPluginKNXServiceTypeOutlet:
            return "outlet";
        
        case HAPPluginKNXServiceTypeSwitch:
            return "switch"; 

        default:
            return "";   
        }    
    }

protected:    
    // std::function<void(NewSettingsPacket)> _callbackSendSettings = NULL;  

    uint8_t _id;
    char _name[41];
    HAPPluginKNXServiceType _type;   

    bool _shouldSend;
    bool _enableFakegato;
    bool _enableSchedule;

    HAPAccessory*           _accessory;
    EventManager*			_eventManager;
    HAPFakeGatoFactory*     _fakegatoFactory;
   
    virtual bool fakeGatoCallback() = 0;  
};

#endif /* HAPPLUGINKNXDEVICE_HPP_ */

