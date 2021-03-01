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

#include "HAPFakeGato.hpp"
#include "HAPFakeGatoHygrometer.hpp"
#include "HAPCustomCharacteristics+Services.hpp"
#include "HAPGlobals.hpp"


class HAPPluginHygrometer: public HAPPlugin {
public:

	HAPPluginHygrometer();

	bool begin();
	HAPAccessory* initAccessory() override;
	
	void setValue(int iid, String oldValue, String newValue);	
	
	void changeHum(float oldValue, float newValue);

	void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);	
	
#if HAP_ENABLE_WEBSERVER	
	HAPConfigurationValidationResult validateConfig(JsonObject object);
	JsonObject getConfigImpl();
	void setConfigImpl(JsonObject root);
#endif

	HAPConfigurationPlugin* setDefaults();

private:


	HAPCharacteristicFloat*	_humidityValue;

#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED	
	HAPCharacteristicUInt8*	_leakSensor;	
#endif

	bool _leakSensorEnabled;
	
	bool fakeGatoCallback();

	HAPFakeGatoHygrometer _fakegato;

    uint16_t readSensor();
};

REGISTER_PLUGIN(HAPPluginHygrometer)

#endif /* HAPPLUGINHYGROMETER_HPP_ */ 