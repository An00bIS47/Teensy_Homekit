//
// HAPPluginLED.hpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//

#ifndef HAPPLUGINLED_HPP_
#define HAPPLUGINLED_HPP_

#include <Arduino.h>
#include "HAPPlugins.hpp"
#include "HAPLogging.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"

#ifndef HAP_PLUGIN_LED_SUPPORTS_ENABLED
#define HAP_PLUGIN_LED_SUPPORTS_ENABLED 0
#endif

class HAPPluginLED: public HAPPlugin {
public:

	HAPPluginLED();
	HAPAccessory* initAccessory() override;

	bool begin();

	void changeState(bool oldValue, bool newValue);

#if HAP_PLUGIN_LED_SUPPORTS_ENABLED
	void changeEnabled(bool oldValue, bool newValue);
#endif

	void handleImpl(bool forced=false);
	void identify( bool oldValue, bool newValue);

	HAPConfigurationValidationResult validateConfig(JsonObject object);

	HAPConfigurationPlugin* setDefaults();
	void setConfiguration(HAPConfigurationPlugin* cfg) override;


protected:
	HAPCharacteristic<bool>* 	_powerState;

#if HAP_PLUGIN_LED_SUPPORTS_ENABLED
	HAPCharacteristic<bool>* 	_enabledState;
#endif

	bool 	_blinkingEnabled;
	uint8_t _gpio;
	bool 	_isOn;
};

REGISTER_PLUGIN(HAPPluginLED)

#endif /* HAPPLUGINLED_HPP_ */