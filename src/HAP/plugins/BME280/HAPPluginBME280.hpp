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
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"
#include "HAPFakeGato.hpp"
#include "HAPFakeGatoWeather.hpp"


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

	void changeTemp(float oldValue, float newValue);
	void changeHum(float oldValue, float newValue);
	void changePressure(uint16_t oldValue, uint16_t newValue);

	void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);	

#if HAP_ENABLE_WEBSERVER	
	HAPConfigurationValidationResult validateConfig(JsonObject object);

#endif	
	
	HAPConfigurationPlugin* setDefaults() override;
	void internalConfigToJson(Print& prt); 	
	void setConfiguration(HAPConfigurationPlugin* cfg) override;
	
protected:
	
 	struct HAPPluginBME280Config* _configInternal;
	

	void setValue(int iid, String oldValue, String newValue);

	HAPCharacteristicFloat*		_humidityValue;
	HAPCharacteristicFloat*		_temperatureValue;
	HAPCharacteristicUInt16*	_pressureValue;

	Adafruit_BME280* _bme;

	bool fakeGatoCallback();
	HAPFakeGatoWeather _fakegato;
};

REGISTER_PLUGIN(HAPPluginBME280)

#endif /* HAPPLUGINS_HPP_ */ 