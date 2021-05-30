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

#include "HAPFakegato.hpp"
#include "HAPFakegato+ScheduleEnergy.hpp"
#include "HAPFakegatoAverage.hpp"
#include "HAPFakegatoFactory.hpp"

#include "HAPPluginKNXDevice.hpp"
#include "HAPVersion.hpp"

class HAPPluginKNXDeviceOutlet : public HAPPluginKNXDevice{
public:

    HAPPluginKNXDeviceOutlet(uint8_t id_, char name[], bool enableFakegato, bool enableSchedule,  uint16_t koReadState, uint16_t koWriteState, uint16_t koWriteCurrent, uint16_t koWriteActEnergy);

    HAPAccessory* initAccessory() override;

    void handle(bool forced) override;

    // void identify(bool oldValue, bool newValue);
    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory);

    uint32_t getTimestampLastActivity();


	inline float getAveragedTotalPowerValue(){
		return _ttlPowerAverage.getAverage();
	}

	inline float getAveragedCurrentPowerValue(){
		return _curPowerAverage.getAverage();
	}

    inline float getAveragedPowerTenth() {
        return 0.0;
    }

    inline float getAveragedPowerVoltage() {
        return 0.0;
    }

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

    uint32_t                _timestampLastActivity;

    HAPCharacteristic<bool>*    _inUseState;
    HAPCharacteristic<bool>*    _parentalLock;

    HAPCharacteristic<bool>*    _stateValue;


    HAPCharacteristic<float>*   _curPowerValue;
    HAPFakegatoAverage<float>	 _curPowerAverage;

    HAPCharacteristic<float>*   _ttlPowerValue;
    HAPFakegatoAverage<float>	 _ttlPowerAverage;

    // HAPCharacteristicString* 	_lastUpdate;

    HAPFakegatoScheduleEnergy* _fakegato;
    bool fakeGatoCallback() override;

    void setStateFromKNXCallback(GroupObject& go);
    void setCurrentPowerFromKNXCallback(GroupObject& go);
    void setActiveEnergyFromKNXCallback(GroupObject& go);

    void writeStateCallbackFromSchedule(uint16_t state);

	void changedPowerCurrent(float oldValue, float newValue);
	void changedPowerTotal(float oldValue, float newValue);
    void changedState(bool oldValue, bool newValue);

    bool readStateFromKNX();
    float readPowerTotalFromKNX();
    float readPowerCurrentFromKNX();

    // JsonObject scheduleToJson();
    // void scheduleFromJson(JsonObject &root);

    void saveConfig();




};

#endif /* HAPPLUGINKNXDEVICEOUTLET_HPP_ */

