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
#define HAP_PLUGIN_BME280_INTERVAL	5000

#define SDA_PIN				SDA	
#define SCL_PIN				SCL

#define VERSION_MAJOR       0
#define VERSION_MINOR       7	// 7 = new fakegato; 6 = new configuration; 2 = FakeGato support
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
	Serial.printf("Start Identify %s\n", _config->name);
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
	// LogI(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Change temperature from " + String(oldValue) + " to " + String(newValue), true);
	Serial.println("[" + String(_config->name) + "] Changed temperature " + String(oldValue) + " >>> " + String(newValue));

}

void HAPPluginBME280::changedHumidity(float oldValue, float newValue) {		
	// LogI(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Change humidity from " + String(oldValue) + " to " + String(newValue), true);
	Serial.println("[" + String(_config->name) + "] Changed humidity " + String(oldValue) + " >>> " + String(newValue));
}

void HAPPluginBME280::changedPressure(uint16_t oldValue, uint16_t newValue) {
	// LogI(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Change pressure from " + String(oldValue) + " to " + String(newValue), true);
	Serial.println("[" + String(_config->name) + "] Changed pressure " + String(oldValue) + " >>> " + String(newValue));
}

void HAPPluginBME280::handleImpl(bool forced){	
	// if (shouldHandle() || forced) {		
	LogV(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);

	if (_accessory->aid() == 0){
		return;
	}

	float temperature 		= readTemperature();
	_temperatureValue->setValue(temperature, false);
	_temperatureAverage.addValue(temperature);
	queueNotifyEvent(_temperatureValue);

	float relative_humidity = readHumidity();
	_humidityValue->setValue(relative_humidity, false);
	_humidityAverage.addValue(relative_humidity);
	queueNotifyEvent(_humidityValue);
	
	uint16_t pressure 		= readPressure();
	_pressureValue->setValue(pressure, false);
	_pressureAverage.addValue(pressure);
	queueNotifyEvent(_pressureValue);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPAccessory* HAPPluginBME280::initAccessory(){
	LogV("\nInitializing accessory for plugin: " + String(_config->name) + " ...", true);
	
	// 
	// Unique serial number !!!
	// 
    char hex[6];
#if HAP_PLUGIN_BME280_USE_DUMMY
	sprintf(hex, "%s", "DUMMY");
#else
    sprintf(hex, "%x", _bme->sensorID());
#endif	
	String sn = HAPDeviceID::serialNumber("BME", String(hex));

	// 
	// Add new accessory
	// 
	_accessory = new HAPAccessory();
	auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
   	_accessory->addInfoService("Weather", "ACME", "BME280 " + String(hex), sn, callbackIdentify, version());

	
	//
	// Temperature
	//
	HAPService* temperatureService = new HAPService(HAP_SERVICE_TEMPERATURE_SENSOR);
	// temperatureService->setPrimaryService(true);
	_accessory->addService(temperatureService);
	{
		HAPCharacteristicT<String> *temperatureServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
		temperatureServiceName->setValue("BME280 Temperature Sensor");
		_accessory->addCharacteristicToService(temperatureService, temperatureServiceName);
		
		_temperatureValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, -50, 100, 0.1, HAP_UNIT_CELSIUS);
		_temperatureValue->setValue(0.0F);

		_temperatureValue->setValueChangeCallback(std::bind(&HAPPluginBME280::changedTemperature, this, std::placeholders::_1, std::placeholders::_2));
		_temperatureValue->setValueGetCallback(std::bind(&HAPPluginBME280::readTemperature, this));

		_accessory->addCharacteristicToService(temperatureService, _temperatureValue);
	}

	
	//
	// Humidity
	//
	HAPService* humidityService = new HAPService(HAP_SERVICE_HUMIDITY_SENSOR);
	_accessory->addService(humidityService);
	{	
		HAPCharacteristicT<String> *humServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
		humServiceName->setValue("BME280 Humidity Sensor");
		_accessory->addCharacteristicToService(humidityService, humServiceName);

		_humidityValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 100, 0.1, HAP_UNIT_PERCENTAGE);
		_humidityValue->setValue(0.0F);

		_humidityValue->setValueChangeCallback(std::bind(&HAPPluginBME280::changedHumidity, this, std::placeholders::_1, std::placeholders::_2));
		_humidityValue->setValueGetCallback(std::bind(&HAPPluginBME280::readHumidity, this));

		_accessory->addCharacteristicToService(humidityService, _humidityValue);
	}

	
	//
	// AirPressure
	//
	HAPService* pressureService = new HAPService(HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR);
	_accessory->addService(pressureService);
	{	
		HAPCharacteristicT<String> *pressureServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
		pressureServiceName->setValue("BME280 AirPressure Sensor");
		_accessory->addCharacteristicToService(pressureService, pressureServiceName);
		
		_pressureValue = new HAPCharacteristicT<uint16_t>(HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 1100, 1, HAP_UNIT_HPA);
		_pressureValue->setValue(320);

		_pressureValue->setValueChangeCallback(std::bind(&HAPPluginBME280::changedPressure, this, std::placeholders::_1, std::placeholders::_2));
		_pressureValue->setValueGetCallback(std::bind(&HAPPluginBME280::readPressure, this));

		_accessory->addCharacteristicToService(pressureService, _pressureValue);
	}

	// 
	// Link services
	// 
	temperatureService->addLinkedServiceId(humidityService->aid());
	temperatureService->addLinkedServiceId(pressureService->aid());

	//
	// FakeGato
	// 			
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicTemperature(std::bind(&HAPPluginBME280::getAveragedTemperatureValue, this)));
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginBME280::getAveragedHumidityValue, this)));
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicAirPressure(std::bind(&HAPPluginBME280::getAveragedPressureValue, this)));
	
	_fakegato.registerFakeGatoService(_accessory, "BME280 " + String(hex));
			
	auto callbackAddEntry = std::bind(&HAPPluginBME280::fakeGatoCallback, this);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);



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
	LogV(HAPTime::timeString() + " " + String(_config->name) + "->" + String(__FUNCTION__) + " [   ] " + "begin()", true);

	_bme = new Adafruit_BME280();

#if HAP_PLUGIN_BME280_USE_DUMMY

	LogW("   - Using BME280 dummy!", true);
	_config->interval = HAP_PLUGIN_BME280_INTERVAL;
#else
	
	Wire.begin(SDA_PIN, SCL_PIN);
	
    uint8_t status = _bme->begin(BME280_BASE_ADDRESS, &Wire);  
    char hex[5];
    sprintf(hex, "%x", _bme->sensorID());

    if (!status) {        
        LogE("\nCould not find a valid BME280 sensor, check wiring, address, sensor ID!", true);
        LogE("SensorID was: 0x" + String(hex), true);
        LogE("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085", true);
        LogE("   ID of 0x56-0x58 represents a BMP 280", true);
        LogE("        ID of 0x60 represents a BME 280", true);
        LogE("        ID of 0x61 represents a BME 680", true);
        LogE("Disabling BME280 Sensor Plugin", true);
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

	// Serial.println("ADD FAKEGATO ENTRY");
	// addToAverage(_temperatureValue->value(), _humidityValue->value(), _pressureValue->value());
	
	// float avgTemp = _averageTemperature / _measurementTempCount;
	// float avgHum = _averageHumidity / _measurementHumCount;
	// float avgPres = _averagePressure / _measurementPresCount;
	
	// resetAverage();
	_fakegato.addEntry(0x07); //, "thp", avgTemp, avgHum, avgPres);
	
	return true;
	// return _fakegato.addEntry(0x07, String(avgTemp), String(avgHum), String(avgPres));
	// 0102 0202 0302
	//	|	  |	   +-> Pressure	
	//  |	  +-> Humidity
	//  +-> Temp
	// 
	// 0x07 => all			= 111
	// 0x01 => temp			= 001
	// 0x02 => hum			= 010
	// 0x04 => pressure		= 100

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPConfigurationPlugin* HAPPluginBME280::setDefaults(){
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

