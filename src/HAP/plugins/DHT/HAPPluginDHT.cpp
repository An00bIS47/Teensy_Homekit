//
// HAPPluginDHT.cpp
// Homekit
//
//  Created on: 29.04.2018
//      Author: michael
//
#include "HAPPluginDHT.hpp"
#include "HAPServer.hpp"

#define HAP_PLUGIN_DHT_INTERVAL 5000

#define VERSION_MAJOR       0
#define VERSION_MINOR       7
#define VERSION_REVISION    0
#define VERSION_BUILD       0

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginDHT::HAPPluginDHT(){
	_type 				= HAP_PLUGIN_TYPE_ACCESSORY;
	_previousMillis 	= 0;

    _version.major      = VERSION_MAJOR;
    _version.minor      = VERSION_MINOR;
    _version.revision   = VERSION_REVISION;
    _version.build      = VERSION_BUILD;

	_humidityValue		= nullptr;
	_temperatureValue	= nullptr;

	_config = new HAPConfigurationPlugin("DHT", true, HAP_PLUGIN_DHT_INTERVAL);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPPluginDHT::begin(){
	LogV(HAPTime::timeString() + " " + String(_config->name) + "->" + String(__FUNCTION__) + " [   ] " + "begin()", true);

#if HAP_PLUGIN_DHT_USE_DUMMY
	LogW("   - Using DHT dummy!", true);
	randomSeed(analogRead(DHTPIN));
#else
	_dht = new DHT_Unified(DHTPIN, DHTTYPE);
	_dht->begin();
#endif
	return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginDHT::identify(bool oldValue, bool newValue) {
	Serial.printf("Start Identify %s\n", _config->name);
}

void HAPPluginDHT::changedTemperature(float oldValue, float newValue) {
	Serial.println("[" + String(_config->name) + "] Changed temperature " + String(oldValue) + " >>> " + String(newValue));
}

void HAPPluginDHT::changedHumidity(float oldValue, float newValue) {
	Serial.println("[" + String(_config->name) + "] Changed humidity " + String(oldValue) + " >>> " + String(newValue));
}


void HAPPluginDHT::handleImpl(bool forced){

	LogV(HAPTime::timeString() + " " + _config->name + "->" + "handleImpl" + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);

	sensors_event_t sensorEventTemp;
	sensors_event_t sensorEventHum;
#if HAP_PLUGIN_DHT_USE_PRESSURE
	sensors_event_t sensorEventPressure;
#endif

#if HAP_PLUGIN_DHT_USE_DUMMY

	sensorEventTemp.temperature 		= random(200, 401) / 10.0;
	sensorEventHum.relative_humidity 	= random(200, 401) / 10.0;
	// setValue(_temperatureValue->iid, getValue(_temperatureValue->iid), String(sensorEvent.temperature) );
	// setValue(_humidityValue->iid, getValue(_humidityValue->iid), String(sensorEvent.relative_humidity) );
#else

	_dht->temperature().getEvent(&sensorEventTemp);
	_dht->humidity().getEvent(&sensorEventHum);
#endif

	if (!isnan(sensorEventTemp.temperature)) {
		_temperatureValue->setValue(sensorEventTemp.temperature, false);
		_temperatureAverage.addValue(sensorEventTemp.temperature);
		queueNotifyEvent(_temperatureValue);
	}

	if (!isnan(sensorEventHum.relative_humidity)) {
		_humidityValue->setValue(sensorEventHum.relative_humidity, false);
		_humidityAverage.addValue(sensorEventHum.relative_humidity);
		queueNotifyEvent(_humidityValue);

	}

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginDHT::initAccessory(){
	LogV("\nInitializing accessory for plugin: " + String(_config->name) + " ...", true);

#if HAP_PLUGIN_DHT_USE_DUMMY
#else
	sensor_t sensor;
	_dht->temperature().getSensor(&sensor);
#endif

	//
	// Add new accessory
	//
	_accessory = new HAPAccessory();
	auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
#if HAP_PLUGIN_DHT_USE_DUMMY
	String sn = HAPDeviceID::serialNumber("DHT", "DUMMY");
	_accessory->addInfoService("DHT Sensor", "ACME", "DHT", sn, callbackIdentify, version());
#else
	String sn = HAPDeviceID::serialNumber("DHT", String(sensor.version));
	_accessory->addInfoService("DHT Sensor", "ACME", sensor.name, sn, callbackIdentify, version());
#endif

	//
	// Temperature Service
	//
	HAPService* temperatureService = new HAPService(HAP_SERVICE_TEMPERATURE_SENSOR);
	_accessory->addService(temperatureService);
	{
		const char* serviceName = "DHT Temperature Sensor";
		HAPCharacteristic<String> *temperatureServiceName = new HAPCharacteristic<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
		temperatureServiceName->setValue(serviceName);

		_accessory->addCharacteristicToService(temperatureService, temperatureServiceName);

		//HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
		_temperatureValue = new HAPCharacteristic<float>(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, -50, 100, 0.1, HAP_UNIT_CELSIUS);
		_temperatureValue->setValue(0.0F);

		_temperatureValue->setValueChangeCallback(std::bind(&HAPPluginDHT::changedTemperature, this, std::placeholders::_1, std::placeholders::_2));
		_temperatureValue->setValueGetCallback(std::bind(&HAPPluginDHT::readTemperature, this));

		_accessory->addCharacteristicToService(temperatureService, _temperatureValue);

	}


#if HAP_PLUGIN_DHT_USE_DUMMY
#else
	_dht->humidity().getSensor(&sensor);
#endif

	//
	// Humidity
	//
	// is bound to the temperature service
	{
		_humidityValue = new HAPCharacteristic<float>(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 100, 0.1, HAP_UNIT_PERCENTAGE);		_humidityValue->setValue(0.0);
		_humidityValue->setValue(0.0F);

		_humidityValue->setValueChangeCallback(std::bind(&HAPPluginDHT::changedHumidity, this, std::placeholders::_1, std::placeholders::_2));
		_humidityValue->setValueGetCallback(std::bind(&HAPPluginDHT::readHumidity, this));


		_accessory->addCharacteristicToService(temperatureService, _humidityValue);
	}


	//
	// FakeGato
	//
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicTemperature(std::bind(&HAPPluginDHT::getAveragedTemperatureValue, this)));
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginDHT::getAveragedHumidityValue, this)));

	_fakegato.registerFakeGatoService(_accessory, sn);

	auto callbackAddEntry = std::bind(&HAPPluginDHT::fakeGatoCallback, this);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);

	return _accessory;
}

#if HAP_ENABLE_WEBSERVER
HAPConfigurationValidationResult HAPPluginDHT::validateConfig(JsonObject object){
    return HAPPlugin::validateConfig(object);
}
#endif

bool HAPPluginDHT::fakeGatoCallback(){
	_fakegato.addEntry(0x03);
	return true;
	// 0102 0202 0302
	//	|	  |	   +-> Pressure
	//  |	  +-> Humidity
	//  +-> Temp
	//
	// 0x03 => all			= 11
	// 0x01 => temp			= 01
	// 0x02 => hum			= 10
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPConfigurationPlugin* HAPPluginDHT::setDefaults(){
	_config->enabled  = HAP_PLUGIN_USE_DHT;
	_config->interval = HAP_PLUGIN_DHT_INTERVAL;
	_config->dataPtr = nullptr;
	_config->dataSize = 0;
	return _config;
}

float HAPPluginDHT::readTemperature(){

	float value = 0;
#if HAP_PLUGIN_BME280_USE_DUMMY
	value = random(200, 401) / 10.0;
#else
	sensors_event_t sensorEventTemp;
	if (_timestampLastRead + 2000 <= millis()) {
		_dht->temperature().getEvent(&sensorEventTemp);
		_timestampLastRead = millis();

		if (!isnan(sensorEventTemp.temperature)) {
			value = sensorEventTemp.temperature;
		}
	} else {
		value = _temperatureValue->value(false);
	}

#endif
	return value;
}

float HAPPluginDHT::readHumidity(){
	float value = 0;
#if HAP_PLUGIN_BME280_USE_DUMMY
	value = random(200, 401) / 10.0;
#else
	sensors_event_t sensorEventHum;
	if (_timestampLastRead + 2000 <= millis()) {
		_dht->humidity().getEvent(&sensorEventHum);
		_timestampLastRead = millis();

		if (!isnan(sensorEventHum.relative_humidity)) {
			value = sensorEventHum.relative_humidity;
		}
	} else {
		value = _humidityValue->value(false);
	}
#endif
	return value;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginDHT::setConfiguration(HAPConfigurationPlugin* cfg){
	_config = cfg;
}