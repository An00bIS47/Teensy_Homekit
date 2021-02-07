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
#define VERSION_MINOR       6	// 6 = new configuration; 2 = FakeGato support
#define VERSION_REVISION    0
#define VERSION_BUILD       0

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

	_configInternal = new HAPPluginBME280Config();
	_configInternal->mode = HAP_PLUGIN_BME280_INDOOR;

	_config = new HAPConfigurationPlugin("BME280", true, HAP_PLUGIN_BME280_INTERVAL, (uint8_t*)_configInternal, sizeof(HAPPluginBME280Config) );

	// Callback for PRINT internal config to json
	_config->setToJsonCallback(std::bind(&HAPPluginBME280::internalConfigToJson, this, std::placeholders::_1));
}

HAPPluginBME280::~HAPPluginBME280(){
	delete _configInternal;
	delete _config;

	delete _humidityValue;
	delete _temperatureValue;
	delete _pressureValue;
}

void HAPPluginBME280::identify(bool oldValue, bool newValue) {
	Serial.printf("Start Identify %s\n", _config->name);
}

void HAPPluginBME280::changeTemp(float oldValue, float newValue) {
	Serial.printf("[%s] New temperature: %f\n", _config->name, newValue);
}

void HAPPluginBME280::changeHum(float oldValue, float newValue) {
	Serial.printf("[%s] New humidity: %f\n", _config->name, newValue);
}

void HAPPluginBME280::changePressure(uint16_t oldValue, uint16_t newValue) {
	Serial.printf("[%s] New pressure: %d\n", _config->name, newValue);
}

void HAPPluginBME280::handleImpl(bool forced){	
	// if (shouldHandle() || forced) {		
	LogV(HAPServer::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);

		if (_accessory->aid == 0){
			return;
		}

#if HAP_PLUGIN_BME280_USE_DUMMY
		float temperature 			= random(200, 401) / 10.0;
		float relative_humidity 	= random(200, 401) / 10.0;
		float pressure 				= random(30, 110) * 10;

		// Serial.print(">>>>>>>>>>>>>>>>>> temperature: ");
		// Serial.print(temperature);
		// Serial.print(" ");
		// Serial.print(String(temperature));
		// Serial.print(" relative_humidity: ");
		// Serial.print(relative_humidity);
		// Serial.print(" pressure: ");
		// Serial.println(pressure);

		setValue(_temperatureValue->iid, _temperatureValue->value(), String(temperature) );
#else

		// Only needed in forced mode! In normal mode, you can remove the next line.
    	_bme->takeForcedMeasurement(); // has no effect in normal mode
        
		setValue(_temperatureValue->iid, _temperatureValue->value(), String(_bme->readTemperature()) );
#endif		
		// sensors_event_t sensorEvent;		
		// _dht->temperature().getEvent(&sensorEvent);
  		// if (!isnan(sensorEvent.temperature)) {
    	// 	setValue(charType_currentTemperature, getValue(charType_currentTemperature), String(sensorEvent.temperature) );        
  		// }

#if HAP_PLUGIN_BME280_USE_DUMMY
		setValue(_humidityValue->iid, _humidityValue->value(), String(relative_humidity) );
#else
        setValue(_humidityValue->iid, _humidityValue->value(), String(_bme->readHumidity()) );
		//  _dht->humidity().getEvent(&sensorEvent);  
		//  if (!isnan(sensorEvent.relative_humidity)) {
    	// 	setValue(charType_currentHumidity, getValue(charType_currentHumidity), String(sensorEvent.relative_humidity) );
  		// }
#endif


#if HAP_PLUGIN_BME280_USE_DUMMY
		uint16_t pres = pressure;
		setValue(_pressureValue->iid, _pressureValue->value(), String(pres) );
#else		  
		float pressure = _bme->readPressure() / 100.0F;
		// Serial.print("pressure: ");
		// Serial.println(pressure);
		// uint16_t pres = (uint16_t)pressure;
		setValue(_pressureValue->iid, _pressureValue->value(), String(pressure) );
	// }
#endif

}


// void HAPPluginDHT::handleEvents(int eventCode, struct HAPEvent eventParam){
// 	LogE("!!!!!!!!!!! HANDLE PLUGIN EVENT !!!!!!!!!!!!!!!", true);
// }



void HAPPluginBME280::setValue(int iid, String oldValue, String newValue){
	if (iid == _temperatureValue->iid) {		
		_temperatureValue->setValue(newValue);
	} else if (iid == _humidityValue->iid) {
		_humidityValue->setValue(newValue);
	} else if (iid == _pressureValue->iid) {
		_pressureValue->setValue(newValue);
	}

	// Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, iid, newValue);							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}


HAPAccessory* HAPPluginBME280::initAccessory(){
	LogV("\nInitializing accessory for plugin: " + String(_config->name) + " ...", true);
	
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
		HAPCharacteristicString *tempServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
		tempServiceName->setValue("BME280 Temperature Sensor");
		_accessory->addCharacteristics(temperatureService, tempServiceName);

		//HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
		_temperatureValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, permission_read|permission_notify, -50, 100, 0.1, unit_celsius);
		_temperatureValue->setValue("0.0");
		auto callbackChangeTemp = std::bind(&HAPPluginBME280::changeTemp, this, std::placeholders::_1, std::placeholders::_2);
		//_temperatureValue->valueChangeFunctionCall = std::bind(&changeTemp);
		_temperatureValue->valueChangeFunctionCall = callbackChangeTemp;
		_accessory->addCharacteristics(temperatureService, _temperatureValue);
	}


	//
	// Humidity
	//
	HAPService* humidityService = new HAPService(HAP_SERVICE_HUMIDITY_SENSOR);
	_accessory->addService(humidityService);
	{	
		HAPCharacteristicString *humServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
		humServiceName->setValue("BME280 Humidity Sensor");
		_accessory->addCharacteristics(humidityService, humServiceName);

		_humidityValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, permission_read|permission_notify, 0, 100, 0.1, unit_percentage);
		_humidityValue->setValue("0.0");

		auto callbackChangeHum = std::bind(&HAPPluginBME280::changeHum, this, std::placeholders::_1, std::placeholders::_2);
		//_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
		_humidityValue->valueChangeFunctionCall = callbackChangeHum;
		_accessory->addCharacteristics(humidityService, _humidityValue);
	}

	//
	// AirPressure
	//
	HAPService* pressureService = new HAPService(HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR);
	_accessory->addService(pressureService);

	{	
		HAPCharacteristicString *pressureServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
		pressureServiceName->setValue("BME280 AirPressure Sensor");
		_accessory->addCharacteristics(pressureService, pressureServiceName);
		
		_pressureValue = new HAPCharacteristicUInt16(HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE, permission_read|permission_notify, 0, 1100, 1, unit_hpa);
		_pressureValue->setValue("320");

		auto callbackChangePressure = std::bind(&HAPPluginBME280::changePressure, this, std::placeholders::_1, std::placeholders::_2);
		//_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
		_pressureValue->valueChangeFunctionCall = callbackChangePressure;
		_accessory->addCharacteristics(pressureService, _pressureValue);
	}

	// 
	// Link services
	// 
	temperatureService->addLinkedServiceId(humidityService->serviceID);
	temperatureService->addLinkedServiceId(pressureService->serviceID);

	//
	// FakeGato
	// 		
	_fakegato.registerFakeGatoService(_accessory, _config->name);
	auto callbackAddEntry = std::bind(&HAPPluginBME280::fakeGatoCallback, this);
	// _fakegato.registerCallback(callbackAddEntry);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);


	return _accessory;
}

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


bool HAPPluginBME280::begin(){
	LogV(HAPServer::timeString() + " " + String(_config->name) + "->" + String(__FUNCTION__) + " [   ] " + "begin()", true);

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
	return _fakegato.addEntry(0x07, _temperatureValue->value(), _humidityValue->value(), _pressureValue->value());
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

HAPConfigurationPlugin* HAPPluginBME280::setDefaults(){
	_configInternal->mode = HAP_PLUGIN_BME280_INDOOR;
	_config->enabled  = HAP_PLUGIN_USE_BME280;
	_config->interval = HAP_PLUGIN_BME280_INTERVAL;	
	_config->dataPtr = (uint8_t*)_configInternal;
	_config->dataSize = sizeof(_configInternal);
	return _config;
}


void HAPPluginBME280::internalConfigToJson(Print& prt){
	/*
		{ >>> is already printed before
			"mode": 1
		} >>> will be printed after
	
	*/
	prt.print("\"mode\":");
	prt.print(_configInternal->mode);
}

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