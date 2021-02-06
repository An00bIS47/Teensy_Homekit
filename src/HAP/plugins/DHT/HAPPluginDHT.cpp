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
#define VERSION_MINOR       6
#define VERSION_REVISION    0
#define VERSION_BUILD       0


HAPPluginDHT::HAPPluginDHT(){
	_type 				= HAP_PLUGIN_TYPE_ACCESSORY;
	_previousMillis 	= 0;

    _version.major      = VERSION_MAJOR;
    _version.minor      = VERSION_MINOR;
    _version.revision   = VERSION_REVISION;
    _version.build      = VERSION_BUILD;

	_humidityValue		= nullptr;
	_temperatureValue	= nullptr;

#if HAP_PLUGIN_DHT_USE_PRESSURE	
	_pressureValue		= nullptr;
#endif	

	_config = new HAPConfigurationPlugin("DHT", true, HAP_PLUGIN_DHT_INTERVAL);
}


bool HAPPluginDHT::begin(){
	LogV(HAPServer::timeString() + " " + String(_config->name) + "->" + String(__FUNCTION__) + " [   ] " + "begin()", true);

#if HAP_PLUGIN_DHT_USE_DUMMY
	LogW("   - Using DHT dummy!", true);
	randomSeed(analogRead(DHTPIN));
#else
	_dht = new DHT_Unified(DHTPIN, DHTTYPE);
	_dht->begin();
#endif
	return true;
}


void HAPPluginDHT::identify(bool oldValue, bool newValue) {
	Serial.printf("Start Identify %s\n", _config->name);
}

void HAPPluginDHT::changeTemp(float oldValue, float newValue) {
	Serial.printf("[%s] New temperature: %f\n", _config->name, newValue);
}

void HAPPluginDHT::changeHum(float oldValue, float newValue) {
	Serial.printf("[%s] New humidity: %f\n", _config->name, newValue);
}

#if HAP_PLUGIN_DHT_USE_PRESSURE
void HAPPluginDHT::changePressure(uint16_t oldValue, uint16_t newValue) {
	Serial.printf("New pressure: %d\n", newValue);
}
#endif


void HAPPluginDHT::handleImpl(bool forced){	

	LogV(HAPServer::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);
	
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
		setValue(_temperatureValue->iid, _temperatureValue->value(), String(sensorEventTemp.temperature) );
	}

	if (!isnan(sensorEventHum.relative_humidity)) {
		setValue(_humidityValue->iid, _humidityValue->value(), String(sensorEventHum.relative_humidity) );
	}

#if HAP_PLUGIN_DHT_USE_PRESSURE

	sensorEventPressure.pressure 		= random(30, 110) * 10;

	if (!isnan(sensorEventPressure.pressure)) {
		setValue(_pressureValue->iid, getValue(_pressureValue->iid), String(sensorEventPressure.pressure) );
	}
#endif


}

// void HAPPluginDHT::handleEvents(int eventCode, struct HAPEvent eventParam){
// 	LogE("!!!!!!!!!!! HANDLE PLUGIN EVENT !!!!!!!!!!!!!!!", true);
// }


void HAPPluginDHT::setValue(int iid, String oldValue, String newValue){
	if (iid == _temperatureValue->iid) {		
		_temperatureValue->setValue(newValue);
	} else if (iid == _humidityValue->iid) {
		//LogW("Setting DHT HUMIDITY oldValue: " + oldValue + " -> newValue: " + newValue, true);
		_humidityValue->setValue(newValue);
	} 
#if HAP_PLUGIN_DHT_USE_PRESSURE	
	else if (iid == _pressureValue->iid) {
		//LogW("Setting DHT HUMIDITY oldValue: " + oldValue + " -> newValue: " + newValue, true);
		_pressureValue->setValue(newValue);
	}
#endif
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, iid, newValue);							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);	
}



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
		HAPCharacteristicString *tempServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read, strlen(serviceName));
		tempServiceName->setValue(serviceName);

		_accessory->addCharacteristics(temperatureService, tempServiceName);

		//HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
		_temperatureValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, permission_read|permission_notify, -50, 100, 0.1, unit_celsius);
		_temperatureValue->setValue("0.0");
		auto callbackChangeTemp = std::bind(&HAPPluginDHT::changeTemp, this, std::placeholders::_1, std::placeholders::_2);
		//_temperatureValue->valueChangeFunctionCall = std::bind(&changeTemp);
		_temperatureValue->valueChangeFunctionCall = callbackChangeTemp;
		_accessory->addCharacteristics(temperatureService, _temperatureValue);

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
		_humidityValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, permission_read|permission_notify, 0, 100, 0.1, unit_percentage);
		_humidityValue->setValue("0.0");

		auto callbackChangeHum = std::bind(&HAPPluginDHT::changeHum, this, std::placeholders::_1, std::placeholders::_2);
		_humidityValue->valueChangeFunctionCall = callbackChangeHum;
		// _accessory->addCharacteristics(humidityService, _humidityValue);
		_accessory->addCharacteristics(temperatureService, _humidityValue);
	}

	

#if HAP_PLUGIN_DHT_USE_PRESSURE
	//
	// AirPressure Service
	//
	{	
		
		HAPService* pressureService = new HAPService(serviceType_FG_pressure);
		_accessory->addService(pressureService);

		const char* serviceName = "DHT AirPressure Sensor";
		HAPCharacteristicString *pressureServiceName = new HAPCharacteristicString(charType_serviceName, permission_read, strlen(serviceName));
		pressureServiceName->setValue(serviceName);
		_accessory->addCharacteristics(pressureService, pressureServiceName);

		_pressureValue = new HAPCharacteristicUInt16(charType_FG_airPressure, permission_read|permission_notify, 300, 1100, 1, unit_hpa);
		_pressureValue->setValue("700");

		auto callbackChangePressure = std::bind(&HAPPluginDHT::changePressure, this, std::placeholders::_1, std::placeholders::_2);
		//_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
		_pressureValue->valueChangeFunctionCall = callbackChangePressure;
		_accessory->addCharacteristics(pressureService, _pressureValue);
	}
#endif



	//
	// FakeGato
	// 		
	_fakegato.registerFakeGatoService(_accessory, _config->name);
	auto callbackAddEntry = std::bind(&HAPPluginDHT::fakeGatoCallback, this);
	// _fakegato.registerCallback(callbackAddEntry);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);

	return _accessory;
}

HAPConfigurationValidationResult HAPPluginDHT::validateConfig(JsonObject object){
    return HAPPlugin::validateConfig(object);
}


bool HAPPluginDHT::fakeGatoCallback(){	
	// return _fakegato.addEntry(_temperatureValue->value(), _humidityValue->value(), _pressureValue->value());
#if HAP_PLUGIN_DHT_USE_PRESSURE	
	return _fakegato.addEntry(0x07, _temperatureValue->value(), _humidityValue->value(), _pressureValue->value());
#else
	return _fakegato.addEntry(0x03, _temperatureValue->value(), _humidityValue->value(), "0");
	// 0102 0202 0302
	//	|	  |	   +-> Pressure	
	//  |	  +-> Humidity
	//  +-> Temp
	// 
	// 0x07 => all			= 111
	// 0x01 => temp			= 001
	// 0x02 => hum			= 010
	// 0x04 => pressure		= 100
#endif
}

HAPConfigurationPlugin* HAPPluginDHT::setDefaults(){	
	_config->enabled  = HAP_PLUGIN_USE_DHT;
	_config->interval = HAP_PLUGIN_DHT_INTERVAL;	
	_config->dataPtr = nullptr;
	_config->dataSize = 0;
	return _config;
}