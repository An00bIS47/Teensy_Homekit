//
// HAPPluginKNXDeviceOutlet.hpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#ifndef HAPPLUGINKNXDEVICEOUTLET_HPP_
#define HAPPLUGINKNXDEVICEOUTLET_HPP_


#include <Arduino.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include "HAPCharacteristic.hpp"
#include "EventManager.h"
#include "HAPFakeGato.hpp"
#include "HAPFakeGatoFactory.hpp"
#include "HAPFakeGatoEnergy.hpp"
#include "HAPPluginKNXDevice.hpp"
#include "HAPVersion.hpp"

class HAPPluginKNXDeviceOutlet : public HAPPluginKNXDevice{
public:

    HAPPluginKNXDeviceOutlet(uint8_t id_, char name[], bool enableFakegato, bool enableSchedule,  uint16_t koReadState, uint16_t koWriteState, uint16_t koWriteCurrent, uint16_t koWriteActEnergy);

    HAPAccessory* initAccessory() override;    
	
    void handle(bool forced) override;
    
    // void identify(bool oldValue, bool newValue);
    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory);

    uint32_t getTimestampLastActivity();

    // uint8_t id;
    // char name[41];
    // enum HAPPluginKNXServiceType type;  

protected:    
    
    // Default 0
    // if != 0: sensor is available
    uint16_t                _koReadState;
    uint16_t                _koWriteState;

    uint16_t                _koWriteCurrent;
    uint16_t                _koWriteActEnergy;    
    
    

    HAPCharacteristicBool*    _stateValue;
    HAPCharacteristicBool*    _inUseState;
    HAPCharacteristicBool*    _parentalLock;
    HAPCharacteristicFloat*   _curPowerValue;
    HAPCharacteristicFloat*   _ttlPowerValue;
    // HAPCharacteristicString* 	_lastUpdate;

    uint32_t                _timestampLastActivity;

    void writeStateCallback(GroupObject& go);
    void writeCurrentCallback(GroupObject& go);
    void writeActiveEnergyCallback(GroupObject& go);

    void writeStateCallbackFromSchedule(uint16_t state);

	void changedPowerCurrent(float oldValue, float newValue);
	void changedPowerTotal(float oldValue, float newValue);
    void changedState(bool oldValue, bool newValue);

    void readState();
    void readPowerTotal();
    void readPowerCurrent();
    
    JsonObject scheduleToJson();
    void scheduleFromJson(JsonObject &root);

    void saveConfig();

    HAPFakeGatoEnergy* _fakegato;

    bool fakeGatoCallback() override;  
};

#endif /* HAPPLUGINKNXDEVICEOUTLET_HPP_ */

