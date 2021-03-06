//
// HAPPluginBME280.hpp
// Homekit
//
//  Created on: 01.08.2019
//      Author: michael
//
// Wiring:
//
//  BME280       ESP32 Module
//  VIN      ->  3V3
//  GND      ->  GND
//  SCL      ->  22
//  SDA      ->  21
//
// All Qwiic cables have the following color scheme and arrangement:
//
// 	Black = GND
// 	Red = 3.3V
// 	Blue = SDA
// 	Yellow = SCL
//

#ifndef HAPPLUGINBME280_HPP_
#define HAPPLUGINBME280_HPP_


#include <Arduino.h>
#include "HAPPlugins.hpp"
#include "HAPLogging.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"
#include "HAPFakegato.hpp"
#include "HAPFakegatoAverage.hpp"
// #include "HAPFakeGatoWeather.hpp"


#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//
// Set these values in the HAPGlobals.hpp
//
#ifndef HAP_PLUGIN_BME280_USE_DUMMY
#define HAP_PLUGIN_BME280_USE_DUMMY		0	// if 0 then use real sensor,
											// if 1 then use random values without any real sensor connected
#endif

#ifndef SEALEVELPRESSURE_HPA
#define SEALEVELPRESSURE_HPA 			(1013.25)
#endif

enum HAP_PLUGIN_BME280_MODE {
	HAP_PLUGIN_BME280_WEATHER 	= 0x00,
	HAP_PLUGIN_BME280_HUMIDITY 	= 0x01,
	HAP_PLUGIN_BME280_INDOOR 	= 0x02,
	HAP_PLUGIN_BME280_GAMING 	= 0x03,
};

struct HAPPluginBME280Config {
	uint8_t mode = (uint8_t)HAP_PLUGIN_BME280_INDOOR;
};

class HAPPluginBME280: public HAPPlugin {
public:

	HAPPluginBME280();
	~HAPPluginBME280();

	bool begin();

	HAPAccessory* initAccessory() override;

	void changedTemperature(float oldValue, float newValue);
	void changedHumidity(float oldValue, float newValue);
	void changedPressure(uint16_t oldValue, uint16_t newValue);

	float readTemperature();
	float readHumidity();
	uint16_t readPressure();

	void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);

#if HAP_ENABLE_WEBSERVER
	HAPConfigurationValidationResult validateConfig(JsonObject object);

#endif

	float getAveragedTemperatureValue(){
		return _temperatureAverage.getAverage();
	}

	float getAveragedHumidityValue(){
		return _humidityAverage.getAverage();
	}

	uint16_t getAveragedPressureValue(){
		return _pressureAverage.getAverage();
	}


	HAPConfigurationPlugin* setDefaults() override;
	void internalConfigToJson(Print& prt);
	void setConfiguration(HAPConfigurationPlugin* cfg) override;

protected:


 	struct HAPPluginBME280Config* _configInternal;

	HAPCharacteristic<float>*		_humidityValue;
	HAPFakegatoAverage<float>		_humidityAverage;

	HAPCharacteristic<float>*		_temperatureValue;
	HAPFakegatoAverage<float>		_temperatureAverage;


	HAPCharacteristic<uint16_t>*	_pressureValue;
	HAPFakegatoAverage<uint16_t>	_pressureAverage;

	Adafruit_BME280* _bme;
	uint32_t _timestampLastRead;

	bool fakeGatoCallback();
	HAPFakegato _fakegato;
};

REGISTER_PLUGIN(HAPPluginBME280)

#endif /* HAPPLUGINS_HPP_ */