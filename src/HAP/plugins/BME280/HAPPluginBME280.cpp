//
// HAPPluginBME280.cpp
// Homekit
//
//  Created on: 01.08.2019
//      Author: michael
//

#include "HAPPluginBME280.hpp"
#include "HAPServer.hpp"

#define BME280_BASE_ADDRESS        	0x76

#ifndef HAP_PLUGIN_BME280_INTERVAL
#define HAP_PLUGIN_BME280_INTERVAL	5000
#endif

#define SDA_PIN				SDA
#define SCL_PIN				SCL

#define VERSION_MAJOR       0
#define VERSION_MINOR       8	// 8 = std::string; 7 = new fakegato; 6 = new configuration; 2 = FakeGato support
#define VERSION_REVISION    0
#define VERSION_BUILD       0

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginBME280::HAPPluginBME280(){
	_type 				= HAP_PLUGIN_TYPE_ACCESSORY;
	// _config->name 				= "BME280";
	// _isEnabled 			= HAP_PLUGIN_USE_BME280;
	// _config->interval 			= HAP_PLUGIN_BME280_config->interval;
	// _mode 				= HAP_PLUGIN_BME280_INDOOR;
	_previousMillis 	= 0;

    _version.major      = VERSION_MAJOR;
    _version.minor      = VERSION_MINOR;
    _version.revision   = VERSION_REVISION;
    _version.build      = VERSION_BUILD;

	_humidityValue		= nullptr;
	_temperatureValue	= nullptr;
	_pressureValue		= nullptr;

	_timestampLastRead 	= 0;

	_configInternal = new HAPPluginBME280Config();
	_configInternal->mode = HAP_PLUGIN_BME280_INDOOR;

	_config = new HAPConfigurationPlugin("BME280", true, HAP_PLUGIN_BME280_INTERVAL, (uint8_t*)_configInternal, sizeof(HAPPluginBME280Config) );

	// Callback for PRINT internal config to json
	_config->setToJsonCallback(std::bind(&HAPPluginBME280::internalConfigToJson, this, std::placeholders::_1));
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginBME280::~HAPPluginBME280(){
	delete _configInternal;
	delete _config;

	delete _humidityValue;
	delete _temperatureValue;
	delete _pressureValue;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginBME280::identify(bool oldValue, bool newValue) {
	LOG_I("Start Identify %s\n", _config->name);
}


float HAPPluginBME280::readTemperature(){

	float value = 0;
#if HAP_PLUGIN_BME280_USE_DUMMY
	value = random(200, 401) / 10.0;
#else
	if (_timestampLastRead + 1000 <= millis()) {
		_bme->takeForcedMeasurement(); // has no effect in normal mode
		_timestampLastRead = millis();
	}
	value = _bme->readTemperature();
#endif
	return value;
}

float HAPPluginBME280::readHumidity(){
	float value = 0;
#if HAP_PLUGIN_BME280_USE_DUMMY
	value = random(200, 401) / 10.0;
#else
	if (_timestampLastRead + 1000 <= millis()) {
		_bme->takeForcedMeasurement(); // has no effect in normal mode
		_timestampLastRead = millis();
	}
	value = _bme->readHumidity();
#endif
	return value;
}

uint16_t HAPPluginBME280::readPressure(){
	uint16_t value = 0;
#if HAP_PLUGIN_BME280_USE_DUMMY
	value = random(30, 110) * 10;
#else
	if (_timestampLastRead + 1000 <= millis()) {
		_bme->takeForcedMeasurement(); // has no effect in normal mode
		_timestampLastRead = millis();
	}
	value = _bme->readPressure();
#endif
	return value;
}


void HAPPluginBME280::changedTemperature(float oldValue, float newValue) {
	LOG_I("[%s] Changed temperature: %.2lf >>> %.2lf\n", _config->name, oldValue, newValue);
}

void HAPPluginBME280::changedHumidity(float oldValue, float newValue) {
	LOG_I("[%s] Changed humidity: %.2lf >>> %.2lf\n", _config->name, oldValue, newValue);
}

void HAPPluginBME280::changedPressure(uint16_t oldValue, uint16_t newValue) {
	LOG_I("[%s] Changed air pressure: %i >>> %i\n", _config->name, oldValue, newValue);
}

void HAPPluginBME280::handleImpl(bool forced){
	LOG_V("[%s] - Handle plugin [%d]\n", (const char*)_config->name, _config->interval);

	if (_accessory->aid() == 0){
		return;
	}

	float temperature 		= readTemperature();
	_temperatureValue->setValue(temperature, true);
	_temperatureAverage.addValue(temperature);
	queueNotifyEvent(_temperatureValue);

	float relative_humidity = readHumidity();
	_humidityValue->setValue(relative_humidity, true);
	_humidityAverage.addValue(relative_humidity);
	queueNotifyEvent(_humidityValue);

	uint16_t pressure 		= readPressure();
	_pressureValue->setValue(pressure, true);
	_pressureAverage.addValue(pressure);
	queueNotifyEvent(_pressureValue);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginBME280::initAccessory(){
	LOG_V("[%s] - Initializing accessory for plugin ...\n", _config->name);

	//
	// Unique serial number !!!
	//
    char hex[7] = {'\0',};
#if HAP_PLUGIN_BME280_USE_DUMMY
	snprintf(hex, 6, "%s", "DUMMY");
#else
	snprintf(hex, 6, "%X", _bme->sensorID());
#endif

	const char* snTemp = HAPDeviceID::serialNumber(_config->name, hex).c_str();
	char serialNumber[strlen(snTemp)] = {'\0',};
	strncpy(serialNumber, snTemp, strlen(snTemp));


	char sensorName[strlen(_config->name) + strlen(hex) + 2] = {'\0', };
	snprintf(sensorName, strlen(_config->name) + strlen(hex) + 2, "%s %s", _config->name, hex);

	//
	// Add new accessory
	//
	LOG_V("[%s] - Add new accessory ...", _config->name);
	_accessory = new HAPAccessory();
	auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
   	_accessory->addInfoService("Weather", "ACME", sensorName, serialNumber, callbackIdentify, version());

	LOGRAW_V("OK\n");


	//
	// Temperature
	//
	LOG_V("[%s] - Add new %s service ...", _config->name, "temperature");

	HAPService* temperatureService = new HAPService(HAPServiceType::TemperatureSensor);
	// temperatureService->setPrimaryService(true);
	_accessory->addService(temperatureService);

	LOGRAW_V("OK\n");

	{

		LOG_V("[%s] - Add new %s sensor ...", _config->name, "temperature");
		HAPCharacteristic<std::string> *temperatureServiceName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAP_PERMISSION_READ, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		temperatureServiceName->setValue("Temperature Sensor");
		_accessory->addCharacteristicToService(temperatureService, temperatureServiceName);

		_temperatureValue = new HAPCharacteristic<float>(HAPCharacteristicType::CurrentTemperature, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, -50, 100, 0.1, HAPUnit::Celsius);
		_temperatureValue->setValue(0.0F);

		_temperatureValue->setValueChangeCallback(std::bind(&HAPPluginBME280::changedTemperature, this, std::placeholders::_1, std::placeholders::_2));
		_temperatureValue->setValueGetCallback(std::bind(&HAPPluginBME280::readTemperature, this));

		_accessory->addCharacteristicToService(temperatureService, _temperatureValue);

		LOGRAW_V("OK\n");
	}



	//
	// Humidity
	//

	LOG_V("[%s] - Add new %s service ...", _config->name, "humidity");

	HAPService* humidityService = new HAPService(HAPServiceType::HumiditySensor);
	_accessory->addService(humidityService);

	LOGRAW_V("OK\n");


	{

		LOG_V("[%s] - Add new %s sensor ...", _config->name, "humidity");

		HAPCharacteristic<std::string> *humServiceName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAP_PERMISSION_READ, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		humServiceName->setValue("Humidity Sensor");
		_accessory->addCharacteristicToService(humidityService, humServiceName);

		_humidityValue = new HAPCharacteristic<float>(HAPCharacteristicType::CurrentRelativeHumidity, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 100, 1, HAPUnit::Percentage);
		_humidityValue->setValue(0.0F);

		_humidityValue->setValueChangeCallback(std::bind(&HAPPluginBME280::changedHumidity, this, std::placeholders::_1, std::placeholders::_2));
		_humidityValue->setValueGetCallback(std::bind(&HAPPluginBME280::readHumidity, this));

		_accessory->addCharacteristicToService(humidityService, _humidityValue);

		LOGRAW_V("OK\n");
	}




	//
	// AirPressure
	//
	LOG_V("[%s] - Add new %s service ...", _config->name, "air pressure");

	HAPService* pressureService = new HAPService(HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR);
	_accessory->addService(pressureService);

	LOGRAW_V("OK\n");

	{
		LOG_V("[%s] - Add new %s sensor ...", _config->name, "air pressure");

		HAPCharacteristic<std::string> *pressureServiceName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAP_PERMISSION_READ, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
		pressureServiceName->setValue("AirPressure Sensor");
		_accessory->addCharacteristicToService(pressureService, pressureServiceName);

		_pressureValue = new HAPCharacteristic<uint16_t>(HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 1100, 1, HAPUnit::HPA);
		_pressureValue->setValue(320);

		_pressureValue->setValueChangeCallback(std::bind(&HAPPluginBME280::changedPressure, this, std::placeholders::_1, std::placeholders::_2));
		_pressureValue->setValueGetCallback(std::bind(&HAPPluginBME280::readPressure, this));

		_accessory->addCharacteristicToService(pressureService, _pressureValue);

		LOGRAW_V("OK\n");
	}



	//
	// Link services
	//
	LOG_V("[%s] - Link services ...", _config->name);

	temperatureService->addLinkedServiceId(humidityService->aid());
	temperatureService->addLinkedServiceId(pressureService->aid());

	LOGRAW_V("OK\n");


	//
	// FakeGato
	//
	LOG_V("[%s] - Add fakegato ...", _config->name);

	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicTemperature(std::bind(&HAPPluginBME280::getAveragedTemperatureValue, this)));
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginBME280::getAveragedHumidityValue, this)));
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicAirPressure(std::bind(&HAPPluginBME280::getAveragedPressureValue, this)));

	_fakegato.registerFakeGatoService(_accessory, serialNumber);

	auto callbackAddEntry = std::bind(&HAPPluginBME280::fakeGatoCallback, this);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);

	LOGRAW_V("OK\n");

	return _accessory;
}

#if HAP_ENABLE_WEBSERVER
HAPConfigurationValidationResult HAPPluginBME280::validateConfig(JsonObject object){
    HAPConfigurationValidationResult result;

    result = HAPPlugin::validateConfig(object);
    if (result.valid == false) {
        return result;
    }

	/*
        "HAPPluginBME280": {
            "enabled": true,
            "interval": 1000,
            "mode": 1,
        }
     */

	result.valid = false;

	// plugin._config->name.mode
    if (object.containsKey("mode") && !object["mode"].is<uint8_t>()) {
        result.reason = "plugins." + String(_config->name) + ".mode is not a integer";
        return result;
    }

	if (object["mode"].as<uint8_t>() > 3) {
        result.reason = "plugins." + String(_config->name) + ".mode is not a valid value";
        return result;
    }

	result.valid = true;
    return result;
}
#endif


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPPluginBME280::begin(){
	LOG_V("[%s] - Begin plugin\n", _config->name);
	_bme = new Adafruit_BME280();

#if HAP_PLUGIN_BME280_USE_DUMMY
	LOG_W("   - Using BME280 dummy!\n");
#else

	Wire.begin(SDA_PIN, SCL_PIN);

    uint8_t status = _bme->begin(BME280_BASE_ADDRESS, &Wire);
    char hex[5];
    sprintf(hex, "%x", _bme->sensorID());

    if (!status) {
        LOG_E("Could not find a valid BME280 sensor, check wiring, address, sensor ID!\n");
        LOG_E("SensorID was: 0x%s\n", hex);
        LOG_E("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        LOG_E("   ID of 0x56-0x58 represents a BMP 280\n");
        LOG_E("        ID of 0x60 represents a BME 280\n");
        LOG_E("        ID of 0x61 represents a BME 680\n");
        LOG_E("Disabling BME280 Sensor Plugin\n");
		_config->enabled = false;

		_eventManager->removeListener(&_listenerMemberFunctionPlugin);

		return false;
    }


	if (_configInternal->mode == HAP_PLUGIN_BME280_WEATHER) {
		// Table 7: Settings and performance for weather monitoring
		// Suggested settings for weather monitoring
		// 		Sensor mode: forced mode, 1 sample / minute
		// 		Oversampling settings: pressure *1, temperature *1, humidity *1
		// 		IIR filter settings: filter off
		// 	Performance for suggested settings
		// 		Current consumption 0.16 µA
		// 		RMS Noise 3.3 Pa / 30 cm, 0.07 %RH
		// 		Data output rate: 1/60 Hz
		_bme->setSampling(Adafruit_BME280::MODE_FORCED,
						Adafruit_BME280::SAMPLING_X1,  // temperature
						Adafruit_BME280::SAMPLING_X1, // pressure
						Adafruit_BME280::SAMPLING_X1,  // humidity
						Adafruit_BME280::FILTER_OFF );
		// suggested rate is 1/60Hz (1m)
		_config->interval = 60000; // in milliseconds

	} else if (_configInternal->mode == HAP_PLUGIN_BME280_HUMIDITY) {
		// Serial.println("-- Humidity Sensing Scenario --");
		// Serial.println("forced mode, 1x temperature / 1x humidity / 0x pressure oversampling");
		// Serial.println("= pressure off, filter off");
    	_bme->setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,   // temperature
                    Adafruit_BME280::SAMPLING_NONE, // pressure
                    Adafruit_BME280::SAMPLING_X1,   // humidity
                    Adafruit_BME280::FILTER_OFF );

    	// suggested rate is 1Hz (1s)
    	// _config->interval = 1000;  // in milliseconds

	} else if (_configInternal->mode == HAP_PLUGIN_BME280_INDOOR) {
		// Serial.println("-- Indoor Navigation Scenario --");
		// Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
		// Serial.println("0.5ms standby period, filter 16x");
	    _bme->setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,  // temperature
                    Adafruit_BME280::SAMPLING_X16, // pressure
                    Adafruit_BME280::SAMPLING_X1,  // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5 );

		// suggested rate is 25Hz
		// 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
		// T_ovs = 2
		// P_ovs = 16
		// H_ovs = 1
		// = 40ms (25Hz)
		// with standby time that should really be 24.16913... Hz
		// _config->interval = 41;

	} else if (_configInternal->mode == HAP_PLUGIN_BME280_GAMING) {
		// gaming
		// Serial.println("-- Gaming Scenario --");
		// Serial.println("normal mode, 4x pressure / 1x temperature / 0x humidity oversampling,");
		// Serial.println("= humidity off, 0.5ms standby period, filter 16x");
		_bme->setSampling(Adafruit_BME280::MODE_NORMAL,
						Adafruit_BME280::SAMPLING_X1,   // temperature
						Adafruit_BME280::SAMPLING_X4,   // pressure
						Adafruit_BME280::SAMPLING_NONE, // humidity
						Adafruit_BME280::FILTER_X16,
						Adafruit_BME280::STANDBY_MS_0_5 );

		// Suggested rate is 83Hz
		// 1 + (2 * T_ovs) + (2 * P_ovs + 0.5)
		// T_ovs = 1
		// P_ovs = 4
		// = 11.5ms + 0.5ms standby
		// _config->interval = 12;
	} else {
		_config->interval = HAP_PLUGIN_BME280_INTERVAL;
	}


#endif

	return true;
}

bool HAPPluginBME280::fakeGatoCallback(){
	// 0102 0202 0302
	//	|	  |	   +-> Pressure
	//  |	  +-> Humidity
	//  +-> Temp
	//
	// 0x07 => all			= 111
	// 0x01 => temp			= 001
	// 0x02 => hum			= 010
	// 0x04 => pressure		= 100
	_fakegato.addEntry(_fakegato.getBitmaskForAll());

	return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPConfigurationPlugin* HAPPluginBME280::setDefaults(){
	LOG_V("[%s] Set defaults\n", _config->name);
	_configInternal->mode = HAP_PLUGIN_BME280_INDOOR;
	_config->enabled  = HAP_PLUGIN_USE_BME280;
	_config->interval = HAP_PLUGIN_BME280_INTERVAL;
	_config->dataPtr = (uint8_t*)_configInternal;
	_config->dataSize = sizeof(_configInternal);
	return _config;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginBME280::internalConfigToJson(Print& prt){
	/*
		{ >>> is already printed before
			"mode": 1
		} >>> will be printed after

	*/
	prt.print("\"mode\":");
	prt.print(_configInternal->mode);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginBME280::setConfiguration(HAPConfigurationPlugin* cfg){
	LOG_V("[%s] Set configuration\n", _config->name);

	_config = cfg;
	_configInternal = (HAPPluginBME280Config*)_config->dataPtr;
	_config->setToJsonCallback(std::bind(&HAPPluginBME280::internalConfigToJson, this, std::placeholders::_1));

	// Serial.print("BME280 interval:");
	// Serial.println(_config->interval);
	// Serial.print("BME280 dataSize:");
	// Serial.println(_config->dataSize);
	// Serial.print("BME280 mode:");
	// Serial.println(_configInternal->mode);
}

