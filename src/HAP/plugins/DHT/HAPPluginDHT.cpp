//
// HAPPluginDHT.cpp
// Homekit
//
//  Created on: 29.04.2018
//      Author: michael
//
#include "HAPPluginDHT.hpp"
#include "HAPServer.hpp"

#ifndef HAP_PLUGIN_DHT_INTERVAL
#define HAP_PLUGIN_DHT_INTERVAL 5000
#endif

#define VERSION_MAJOR       0
#define VERSION_MINOR       8
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
	LOG_V("Begin plugin %s\n", _config->name);

#if HAP_PLUGIN_DHT_USE_DUMMY
	LOG_W("   - Using DHT dummy!\n");
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
	LOG_I("Start Identify %s\n", _config->name);
}

void HAPPluginDHT::changedTemperature(float oldValue, float newValue) {
	LOG_I("[%s] Changed temperature: %.2lf >>> %.2lf\n", _config->name, oldValue, newValue);
}

void HAPPluginDHT::changedHumidity(float oldValue, float newValue) {
	LOG_I("[%s] Changed humidity: %.2lf >>> %.2lf\n", _config->name, oldValue, newValue);
}



void HAPPluginDHT::handleImpl(bool forced){

	LOG_V("[%s] - Handle plugin [%d]\n", (const char*)_config->name, _config->interval);

	sensors_event_t sensorEventTemp;
	sensors_event_t sensorEventHum;
#if HAP_PLUGIN_DHT_USE_PRESSURE
	sensors_event_t sensorEventPressure;
#endif

#if HAP_PLUGIN_DHT_USE_DUMMY

	sensorEventTemp.temperature 		= random(200, 401) / 10.0;
	sensorEventHum.relative_humidity 	= random(200, 401) / 10.0;
#else

	_dht->temperature().getEvent(&sensorEventTemp);
	_dht->humidity().getEvent(&sensorEventHum);
#endif

	if (!isnan(sensorEventTemp.temperature)) {
		_temperatureValue->setValue(sensorEventTemp.temperature, true);
		_temperatureAverage.addValue(sensorEventTemp.temperature);
		queueNotifyEvent(_temperatureValue);
	}

	if (!isnan(sensorEventHum.relative_humidity)) {
		_humidityValue->setValue(sensorEventHum.relative_humidity, true);
		_humidityAverage.addValue(sensorEventHum.relative_humidity);
		queueNotifyEvent(_humidityValue);

	}

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginDHT::initAccessory(){

	LOG_V("[%s] - Initializing accessory for plugin ...\n", _config->name);

#if HAP_PLUGIN_DHT_USE_DUMMY
#else
	sensor_t sensor;
	_dht->temperature().getSensor(&sensor);
#endif


	//
	// Unique serial number !!!
	//
    char hex[6] = {'\0',};
#if HAP_PLUGIN_BME280_USE_DUMMY
	sprintf(hex, "%s", "DUMMY");
#else
	sensor_t sensor;
	_dht->temperature().getSensor(&sensor);
    sprintf(hex, "%x", sensor.sensor_id);
#endif
	char sensorName[20] = {'\0', };
	sprintf(sensorName, "DHT %s", hex);

#if HAP_PLUGIN_DHT_USE_DUMMY
	const char* sn = HAPDeviceID::serialNumber("DHT", "DY").c_str();
#else
	char versionStr[6] = {'\0', };
	sprintf(versionStr, "%d", sensor.version);
	const char* sn = HAPDeviceID::serialNumber("DHT", versionStr).c_str();
#endif

	//
	// Add new accessory
	//
	LOG_V("[%s] - Add new accessory ...", _config->name);
	_accessory = new HAPAccessory();
	auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
	_accessory->addInfoService("DHT Sensor", "ACME", sensorName, sn, callbackIdentify, version());
	LOGRAW_V("OK\n");


	//
	// Temperature Service
	//
	LOG_V("[%s] - Add new %s service ...", _config->name, "temperature");
	HAPService* temperatureService = new HAPService(HAPServiceType::TemperatureSensor);
	_accessory->addService(temperatureService);
	LOGRAW_V("OK\n");

	{
		LOG_V("[%s] - Add new %s sensor ...", _config->name, "temperature");

		const char* serviceName = "DHT Temperature Sensor";
		HAPCharacteristic<std::string> *temperatureServiceName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAP_PERMISSION_READ, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		temperatureServiceName->setValue(serviceName);

		_accessory->addCharacteristicToService(temperatureService, temperatureServiceName);

		//HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
		_temperatureValue = new HAPCharacteristic<float>(HAPCharacteristicType::CurrentTemperature, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, -50, 100, 0.1, HAPUnit::Celsius);
		_temperatureValue->setValue(0.0F);

		_temperatureValue->setValueChangeCallback(std::bind(&HAPPluginDHT::changedTemperature, this, std::placeholders::_1, std::placeholders::_2));
		_temperatureValue->setValueGetCallback(std::bind(&HAPPluginDHT::readTemperature, this));

		_accessory->addCharacteristicToService(temperatureService, _temperatureValue);

		LOGRAW_V("OK\n");
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
		LOG_V("[%s] - Add new %s sensor ...", _config->name, "humidity");

		_humidityValue = new HAPCharacteristic<float>(HAPCharacteristicType::CurrentRelativeHumidity, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 100, 1, HAPUnit::Percentage);		_humidityValue->setValue(0.0);
		_humidityValue->setValue(0.0F);

		_humidityValue->setValueChangeCallback(std::bind(&HAPPluginDHT::changedHumidity, this, std::placeholders::_1, std::placeholders::_2));
		_humidityValue->setValueGetCallback(std::bind(&HAPPluginDHT::readHumidity, this));


		_accessory->addCharacteristicToService(temperatureService, _humidityValue);

		LOGRAW_V("OK\n");
	}


	//
	// FakeGato
	//
	LOG_V("[%s] - Add fakegato ...", _config->name);
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicTemperature(std::bind(&HAPPluginDHT::getAveragedTemperatureValue, this)));
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginDHT::getAveragedHumidityValue, this)));

	_fakegato.registerFakeGatoService(_accessory, sn);

	auto callbackAddEntry = std::bind(&HAPPluginDHT::fakeGatoCallback, this);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);

	LOGRAW_V("OK\n");


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
	LOG_V("[%s] Set defaults\n", _config->name);
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
	LOG_V("[%s] Set configuration\n", _config->name);
	_config = cfg;
}