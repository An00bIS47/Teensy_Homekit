//
// HAPPluginRCSwitchDevice.hpp
// Homekit
//
//  Created on: 20.12.2019
//      Author: michael
//

#ifndef HAPPLUGINRCSWITCHDEVICE_HPP_
#define HAPPLUGINRCSWITCHDEVICE_HPP_


#include <Arduino.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"

#include "HAPFakegato.hpp"
#include "HAPFakegato+ScheduleEnergy.hpp"
#include "HAPFakegatoAverage.hpp"
#include "HAPFakegatoFactory.hpp"

#include "HAPCharacteristicBase.hpp"
#include "EventManager.h"



class HAPPluginRCSwitchDevice {
public:

    HAPPluginRCSwitchDevice();
    HAPPluginRCSwitchDevice(uint8_t houseAddress_, uint8_t deviceAddress_, String name_);

    ~HAPPluginRCSwitchDevice();


    HAPAccessory* initAccessory();    

    void setState(String pwrState);    

    void identify(bool oldValue, bool newValue);
    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory);

    // void switchOn();
    // void switchOff();
    void switchCallback(uint16_t state);

    uint32_t getTimestampLastActivity();

    void setRCSwitchSendCallback(std::function<void(uint8_t, uint8_t, bool)> callback){
        _callbackRCSwitchSend = callback;
    }

    JsonObject scheduleToJson();
    void scheduleFromJson(JsonObject &root);

    void saveConfig();

    uint8_t             houseAddress;
    uint8_t             deviceAddress;
    String              name;


	inline float getAveragedTotalPowerValue(){
		return 0.0;
	}

	inline float getAveragedCurrentPowerValue(){
		return 0.0;
	}

    inline float getAveragedPowerTenth() {
        return 0.0;
    }

    inline float getAveragedPowerVoltage() {
        return 0.0;
    }  


protected:    

    bool getPowerState();

    inline void queueNotifyEvent(HAPCharacteristicBase* characteristic){
        if (characteristic->notifiable()){
            if (_eventManager){
                struct HAPEvent event = HAPEvent(nullptr, _accessory->aid(), characteristic->iid(), characteristic->valueString());							            
                _eventManager->queueEvent( EventManager::kEventNotifyController, event);
            }
			
		}	
    }

    std::function<void(uint8_t, uint8_t, uint8_t)> _callbackRCSwitchSend = NULL;  

    HAPFakegatoScheduleEnergy  _fakegato;
    
    HAPAccessory*           _accessory;
    EventManager*			_eventManager;
    HAPFakegatoFactory*     _fakegatoFactory;

    HAPCharacteristicT<bool>*    _stateValue;
    HAPCharacteristicT<bool>*    _inUseState;
    HAPCharacteristicT<bool>*    _parentalLock;
    HAPCharacteristicT<float>*   _curPowerValue;
    HAPCharacteristicT<float>*   _ttlPowerValue;


    uint32_t                _timestampLastActivity;

    void changedPowerState(bool oldValue, bool newValue);
	void changedPowerCurrent(float oldValue, float newValue);
	void changedPowerTotal(float oldValue, float newValue);
    void changedState(bool oldValue, bool newValue);
   
    bool fakeGatoCallback();  
};

#endif /* HAPPLUGINRCSWITCHDEVICE_HPP_ */

