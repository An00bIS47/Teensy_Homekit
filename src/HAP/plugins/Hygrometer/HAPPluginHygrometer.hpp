//
// HAPPluginHygrometer.hpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//

#ifndef HAPPLUGINHYGROMETER_HPP_
#define HAPPLUGINHYGROMETER_HPP_

#include <Arduino.h>
#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"

#include "HAPFakegato.hpp"
#include "HAPFakegatoAverage.hpp"

#include "HAPCustomCharacteristics+Services.hpp"
#include "HAPGlobals.hpp"


class HAPPluginHygrometer: public HAPPlugin {
public:

	HAPPluginHygrometer();

	bool begin();
	HAPAccessory* initAccessory() override;
	
	void changedHumidity(float oldValue, float newValue);

	void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);	
	
	inline float getAveragedHumidityValue(){
		return _humidityAverage.getAverage();
	}

#if HAP_ENABLE_WEBSERVER	
	HAPConfigurationValidationResult validateConfig(JsonObject object);
	JsonObject getConfigImpl();
	void setConfigImpl(JsonObject root);
#endif

	HAPConfigurationPlugin* setDefaults();
	void setConfiguration(HAPConfigurationPlugin* cfg) override;

private:


	HAPCharacteristicT<float>*		_humidityValue;
	HAPFakegatoAverage<float>		_humidityAverage;

// #if HAP_HYGROMETER_LEAK_SENSOR_ENABLED	
// 	HAPCharacteristicUInt8*	_leakSensor;	
// #endif

	bool _leakSensorEnabled;
	
	bool fakeGatoCallback();

	HAPFakegato _fakegato;

    float readSensor();
	uint16_t readSensorRaw();
	float moistureToPercentage(uint16_t moisture);
};

REGISTER_PLUGIN(HAPPluginHygrometer)

#endif /* HAPPLUGINHYGROMETER_HPP_ */ 