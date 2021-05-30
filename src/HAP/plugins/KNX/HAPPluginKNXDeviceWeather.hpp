//
// HAPPluginKNXDeviceWeather.hpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#ifndef HAPPLUGINKNXDEVICEWEATHER_HPP_
#define HAPPLUGINKNXDEVICEWEATHER_HPP_

#define HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER 0

#include <Arduino.h>
#include "HAPAccessory.hpp"
#include "HAPService.hpp"
#include "HAPCharacteristic.hpp"
#include "EventManager.h"

#include "HAPFakegato.hpp"
#include "HAPFakegatoAverage.hpp"
#include "HAPFakegatoFactory.hpp"

#include "HAPPluginKNXDevice.hpp"
#include "HAPVersion.hpp"

class HAPPluginKNXDeviceWeather : public HAPPluginKNXDevice{
public:

    HAPPluginKNXDeviceWeather(uint8_t id_, char name[], bool enableFakegato, uint16_t koTemperature, uint16_t koHumidity, uint16_t koAirPressure, bool useHumidityDPT9 = false);
    ~HAPPluginKNXDeviceWeather();

    HAPAccessory* initAccessory() override;

    void handle(bool forced) override;

    void identify(bool oldValue, bool newValue);

    void setEventManager(EventManager* eventManager);
    void setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory);

	inline float getAveragedTemperatureValue(){
		return _temperatureAverage.getAverage();
	}

	inline float getAveragedHumidityValue(){
		return _humidityAverage.getAverage();
	}

	inline uint16_t getAveragedPressureValue(){
		return _pressureAverage.getAverage();
	}

protected:

    // Default 0
    // if != 0: sensor is available
    uint16_t                _koTemperature;
    uint16_t                _koHumidity;
    uint16_t                _koAirPressure;

    bool                    _useHumdityDPT9;

    HAPCharacteristic<float>*		_humidityValue;
	HAPFakegatoAverage<float>		_humidityAverage;

	HAPCharacteristic<float>*		_temperatureValue;
	HAPFakegatoAverage<float>		_temperatureAverage;


	HAPCharacteristic<uint16_t>*	_pressureValue;
	HAPFakegatoAverage<uint16_t>	_pressureAverage;


    HAPFakegato*     _fakegato;


	void changedTemperature(float oldValue, float newValue);
	void changedHumidity(float oldValue, float newValue);
	void changedPressure(uint16_t oldValue, uint16_t newValue);


    void setTemperatureFromKNXCallback(GroupObject& go);
    void setHumidityFromKNXCallback(GroupObject& go);
    void setPressureFromKNXCallback(GroupObject& go);



    bool fakeGatoCallback() override;
};

#endif /* HAPPLUGINKNXDEVICEWEATHER_HPP_ */

