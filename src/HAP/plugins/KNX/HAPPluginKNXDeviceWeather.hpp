//
// HAPPluginKNXDeviceWeather.hpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#ifndef HAPPLUGINKNXDEVICEWEATHER_HPP_
#define HAPPLUGINKNXDEVICEWEATHER_HPP_


#include <Arduino.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include "HAPCharacteristic.hpp"
#include "EventManager.h"
#include "HAPFakeGato.hpp"
#include "HAPFakeGatoFactory.hpp"
#include "HAPFakeGatoWeather.hpp"
#include "HAPPluginKNXDevice.hpp"
#include "HAPVersion.hpp"

class HAPPluginKNXDeviceWeather : public HAPPluginKNXDevice{
public:

    HAPPluginKNXDeviceWeather(uint8_t id_, char name[], bool enableFakegato, uint16_t koTemperature, uint16_t koHumidity, uint16_t koAirPressure, bool useHumidityDPT9 = false);
    
    HAPAccessory* initAccessory() override;    
	
    void handle(bool forced) override;

    // void changeLastUpdate(String oldValue, String newValue);
    
    // void identify(bool oldValue, bool newValue);
    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory);


    // uint8_t id;
    // char name[41];
    // enum HAPPluginKNXServiceType type;  

protected:    
    
    // Default 0
    // if != 0: sensor is available
    uint16_t                _koTemperature;
    uint16_t                _koHumidity;    
    uint16_t                _koAirPressure;

    bool                    _useHumdityDPT9;
   
	HAPCharacteristicFloat*	_humidityValue;
	HAPCharacteristicFloat*	_temperatureValue;
	HAPCharacteristicUInt16*	_pressureValue;

    // HAPCharacteristicString* 	_lastUpdate;

    HAPFakeGatoWeather*     _fakegato;


	void changeTemp(float oldValue, float newValue);
	void changeHum(float oldValue, float newValue);
	void changePressure(uint16_t oldValue, uint16_t newValue);
    

    void writeTemperatureCallback(GroupObject& go);
    void writeHumidityCallback(GroupObject& go);
    void writePressureCallback(GroupObject& go);


    bool fakeGatoCallback() override;  
};

#endif /* HAPPLUGINKNXDEVICEWEATHER_HPP_ */

