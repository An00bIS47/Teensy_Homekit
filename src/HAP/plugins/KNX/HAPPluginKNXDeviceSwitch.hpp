//
// HAPPluginKNXDeviceSwitch.hpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#ifndef HAPPLUGINKNXDEVICESWITCH_HPP_
#define HAPPLUGINKNXDEVICESWITCH_HPP_


#include <Arduino.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include "HAPCharacteristicBase.hpp"
#include "EventManager.h"

#include "HAPFakegato.hpp"
#include "HAPFakegatoFactory.hpp"

#include "HAPPluginKNXDevice.hpp"
#include "HAPVersion.hpp"

class HAPPluginKNXDeviceSwitch : public HAPPluginKNXDevice{
public:

    HAPPluginKNXDeviceSwitch(uint8_t id_, char name[], bool enableFakegato, uint16_t koReadState, uint16_t koWriteState);

    HAPAccessory* initAccessory() override;    
	
    void handle(bool forced) override;

    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory);

    bool getStateValue();

protected:    
    
    // Default 0
    // if != 0: sensor is available
    uint16_t                _koReadState;
    uint16_t                _koWriteState;    

    HAPCharacteristicT<bool>*  _stateValue;

    // HAPCharacteristicString* 	_lastUpdate;

    HAPFakegato*     _fakegato;

    void changedState(bool oldValue, bool newValue);    
    void writeStateToKNX(bool newValue);

    bool readStateFromKNX();
    void setStateFromKNXCallback(GroupObject& go);

    bool fakeGatoCallback() override;  
};

#endif /* HAPPLUGINKNXDEVICESWITCH_HPP_ */

