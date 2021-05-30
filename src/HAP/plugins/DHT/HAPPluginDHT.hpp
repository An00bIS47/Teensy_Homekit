//
// HAPPluginDHT.hpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//
// Pinout:
//  _______
// |#######|
// |#######|
// |#######|
// |_______|
//  | | | |
//  | | | |
// 3v H - GND
//    H | = Not connected
//    H => Resistor: 10k
//    H
//    |
//    15
//


#ifndef HAPPLUGINDHT_HPP_
#define HAPPLUGINDHT_HPP_

#include <Arduino.h>
#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"

#include "HAPFakegato.hpp"
#include "HAPFakegatoAverage.hpp"
#include "HAPGlobals.hpp"

//
// Set these values in the HAPGlobals.hpp
//
#ifndef HAP_PLUGIN_DHT_USE_DUMMY
#define HAP_PLUGIN_DHT_USE_DUMMY 	0
#endif

#ifndef DHTPIN
#define DHTPIN 		A8 	// 15
#endif

#ifndef DHTTYPE
#define DHTTYPE    	DHT22
#endif

#include <Adafruit_Sensor.h>

#if HAP_PLUGIN_DHT_USE_DUMMY
#else
#include <DHT.h>
#include <DHT_U.h>
#endif

#include "HAPFakegato.hpp"
#include "HAPFakegatoAverage.hpp"

class HAPPluginDHT: public HAPPlugin {
public:

	HAPPluginDHT();

	bool begin();
	HAPAccessory* initAccessory() override;

	void changedTemperature(float oldValue, float newValue);
	void changedHumidity(float oldValue, float newValue);

	void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);

	float readHumidity();
	float readTemperature();

	inline float getAveragedTemperatureValue(){
		return _temperatureAverage.getAverage();
	}

	inline float getAveragedHumidityValue(){
		return _humidityAverage.getAverage();
	}

#if HAP_ENABLE_WEBSERVER
	HAPConfigurationValidationResult validateConfig(JsonObject object);
#endif

	HAPConfigurationPlugin* setDefaults();
	void setConfiguration(HAPConfigurationPlugin* cfg) override;

private:

	HAPCharacteristic<float>*		_humidityValue;
	HAPFakegatoAverage<float>		_humidityAverage;

	HAPCharacteristic<float>*		_temperatureValue;
	HAPFakegatoAverage<float>		_temperatureAverage;

#if HAP_PLUGIN_DHT_USE_DUMMY
#else
	DHT_Unified* _dht;
#endif

	bool fakeGatoCallback();

	uint32_t _timestampLastRead;

	HAPFakegato _fakegato;

};

REGISTER_PLUGIN(HAPPluginDHT)

#endif /* HAPPLUGINS_HPP_ */