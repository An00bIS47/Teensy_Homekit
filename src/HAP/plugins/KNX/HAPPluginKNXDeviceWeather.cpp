//
// HAPPluginRF24DeviceWeather.cpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#include "HAPPluginKNXDeviceWeather.hpp"
#include "HAPServer.hpp"
#include "HAPLogger.hpp"
#include "HAPPluginKNX.hpp"
#include "HAPCustomCharacteristics+Services.hpp"

#define HAP_KNX_WEATHER_VERSION "0.1"

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPPluginKNXDeviceWeather::HAPPluginKNXDeviceWeather(uint8_t id_, char name[], bool enableFakegato, uint16_t koTemperature, uint16_t koHumidity, uint16_t koAirPressure, bool useHumidityDPT9){
    _id = id_;
    strncpy(_name, name, 40);          
    _type    			= HAPPluginKNXServiceTypeWeather;	

    _accessory          = nullptr;
    _eventManager       = nullptr;      
    _fakegatoFactory    = nullptr;
    _fakegato           = nullptr;

	// _lastUpdate			= nullptr;

    _koTemperature      = koTemperature;
    _koHumidity         = koHumidity;
    _koAirPressure      = koAirPressure;

    _enableFakegato     = enableFakegato;
    _enableSchedule     = false;

    _useHumdityDPT9     = useHumidityDPT9;

    _shouldSend         = true;

#if HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER
    resetAverage();
#endif    
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPAccessory* HAPPluginKNXDeviceWeather::initAccessory(){

    if ( (_koTemperature == 0) && (_koHumidity == 0) && (_koAirPressure == 0) ) {
        LogE("ERROR: Please set communication objects first!", true);        
        return nullptr;
    }

    String sn = HAPDeviceID::serialNumber("knx-1", String(_id));

    // Create accessory if not already created
    _accessory = new HAPAccessory();
    //HAPAccessory::addInfoServiceToAccessory(_accessory, "Builtin LED", "ACME", "LED", "123123123", &identify);
    auto callbackIdentify = std::bind(&HAPPluginKNXDevice::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService(String(_name), "KNX", "KNX Weather", sn, callbackIdentify, HAP_KNX_WEATHER_VERSION);


    // HAPService* tmpService = nullptr;
    //
	// Temperature
	//
    if (_koTemperature > 0) {
        HAPService* temperatureService = new HAPService(HAP_SERVICE_TEMPERATURE_SENSOR);
        _accessory->addService(temperatureService);

        HAPCharacteristicT<String> *tempServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, HAP_STRING_LENGTH_MAX);
        tempServiceName->setValue("Temperature Sensor " + String(_id));

        _accessory->addCharacteristicToService(temperatureService, tempServiceName);

        //HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
        _temperatureValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, -50, 100, 0.1, HAP_UNIT_CELSIUS);
        _temperatureValue->setValue(0.0);
        auto callbackChangeTemp = std::bind(&HAPPluginKNXDeviceWeather::changeTemp, this, std::placeholders::_1, std::placeholders::_2);
        //_temperatureValue->valueChangeFunctionCall = std::bind(&changeTemp);
        _temperatureValue->setValueChangeCallback(callbackChangeTemp);
        _accessory->addCharacteristicToService(temperatureService, _temperatureValue);

        // if (tmpService == nullptr) tmpService = temperatureService;

        knx.getGroupObject(_koTemperature).dataPointType(DPT_Value_Temp);
        knx.getGroupObject(_koTemperature).callback(std::bind(&HAPPluginKNXDeviceWeather::writeTemperatureCallback, this, std::placeholders::_1));
        
        // Read value from knx
        _temperatureValue->valueString();
    }



	//
	// Humidity
	//
    if (_koHumidity > 0) {
        HAPService* humidityService = new HAPService(HAP_SERVICE_HUMIDITY_SENSOR);
        _accessory->addService(humidityService);

        HAPCharacteristicT<String> *humServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, HAP_STRING_LENGTH_MAX);
        humServiceName->setValue("Humidity Sensor " + String(_id));
        _accessory->addCharacteristicToService(humidityService, humServiceName);

        _humidityValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 100, 0.1, HAP_UNIT_PERCENTAGE);
        _humidityValue->setValue(0.0);

        auto callbackChangeHum = std::bind(&HAPPluginKNXDeviceWeather::changeHum, this, std::placeholders::_1, std::placeholders::_2);
        //_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
        _humidityValue->setValueChangeCallback(callbackChangeHum);
        _accessory->addCharacteristicToService(humidityService, _humidityValue);

        // if (tmpService == nullptr) tmpService = humidityService;

        if (_useHumdityDPT9) {
            knx.getGroupObject(_koHumidity).dataPointType(DPT_Value_Humidity);
        } else {
            knx.getGroupObject(_koHumidity).dataPointType(DPT_Scaling);
        }

        knx.getGroupObject(_koHumidity).callback(std::bind(&HAPPluginKNXDeviceWeather::writeHumidityCallback, this, std::placeholders::_1));

        // Read value from knx
        _humidityValue->valueString();
    }

	//
	// AirPressure
	//
    if (_koAirPressure > 0) {
        HAPService* pressureService = new HAPService(HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR);
        _accessory->addService(pressureService);

        HAPCharacteristicT<String> *pressureServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, HAP_STRING_LENGTH_MAX);
        pressureServiceName->setValue("Pressure Sensor " + String(_id));
        _accessory->addCharacteristicToService(pressureService, pressureServiceName);
        
        _pressureValue = new HAPCharacteristicT<uint16_t>(HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 1100, 1, HAP_UNIT_HPA);
        _pressureValue->setValue(320);

        auto callbackChangePressure = std::bind(&HAPPluginKNXDeviceWeather::changePressure, this, std::placeholders::_1, std::placeholders::_2);
        //_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
        _pressureValue->setValueChangeCallback(callbackChangePressure);
        _accessory->addCharacteristicToService(pressureService, _pressureValue);

        // if (tmpService == nullptr) tmpService = pressureService;


        knx.getGroupObject(_koAirPressure).dataPointType(DPT_Value_Pres);
        knx.getGroupObject(_koAirPressure).callback(std::bind(&HAPPluginKNXDeviceWeather::writePressureCallback, this, std::placeholders::_1));

        // Read value from knx
        _pressureValue->valueString();
    }


    // 
    // Last Update characteristics  (Custom)
    // is bound to temperature service
    // 
    // if (tmpService != nullptr) {
    //     _lastUpdate = new HAPCharacteristicString(HAP_CHARACTERISTIC_FAKEGATO_OBSERVATION_TIME, permission_read|permission_notify, 32);
    //     _lastUpdate->setDescription("Observation Time");
    //     _lastUpdate->setValue("Never");

    //     auto callbackChangeLastUpdate = std::bind(&HAPPluginKNXDeviceWeather::changeLastUpdate, this, std::placeholders::_1, std::placeholders::_2);
    //     _lastUpdate->valueChangeFunctionCall = callbackChangeLastUpdate;
    //     _accessory->addCharacteristicToService(tmpService, _lastUpdate);
    // }


	//
	// FakeGato
	// 	    
    // if (_enableFakegato){
    //     _fakegato = new HAPFakeGatoWeather();
    //     _fakegato->registerFakeGatoService(_accessory, _name);    
    //     auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceWeather::fakeGatoCallback, this);
    //     _fakegatoFactory->registerFakeGato(_fakegato,  String(_name), callbackAddEntry);
    // }
    
	

    return _accessory;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceWeather::setEventManager(EventManager* eventManager){
      
    _eventManager = eventManager;
    // Serial.printf("w event: %p\n", _eventManager);  
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceWeather::setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory){
    
    _fakegatoFactory = fakegatoFactory;
    // Serial.printf("w fakegato: %p\n", _fakegatoFactory);
}   


void HAPPluginKNXDeviceWeather::changeTemp(float oldValue, float newValue) {
	Serial.printf("[KNX:%X] New temperature: %f\n", _id, newValue);
}

void HAPPluginKNXDeviceWeather::changeHum(float oldValue, float newValue) {
	Serial.printf("[KNX:%X] New humidity: %f\n", _id, newValue);
}

void HAPPluginKNXDeviceWeather::changePressure(uint16_t oldValue, uint16_t newValue) {
	Serial.printf("[KNX:%X] New pressure: %d\n", _id, newValue);
}

// void HAPPluginKNXDeviceWeather::changeLastUpdate(String oldValue, String newValue){
//     Serial.printf("[KNX:%X] New LastUpdate: %s\n", id, newValue.c_str());
// }


bool HAPPluginKNXDeviceWeather::fakeGatoCallback(){	


    float avgTemp = 0;
    float avgHum = 0;
    uint16_t avgPres = 0;

#if HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER
	
    if (_measurementCountTemperature > 0) avgTemp = _averageTemperature / _measurementCountTemperature;
	
    if (_measurementCountHumidity > 0) avgHum = _averageHumidity / _measurementCountHumidity;
    
	if (_measurementCountPressure > 0) avgPres = _averagePressure / _measurementCountPressure;

	resetAverage();
#else	
	// return _fakegato.addEntry(0x07, String(avgTemp), String(avgHum), String(avgPres));

    if (_koTemperature > 0) avgTemp = _temperatureValue->value();
    if (_koHumidity > 0) avgHum = _humidityValue->value();
    if (_koAirPressure > 0) avgTemp = _pressureValue->value();
#endif

    if ( (_koTemperature > 0) && (_koHumidity > 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, String(avgTemp), String(avgHum), String(avgPres));
    } else if ( (_koTemperature > 0) && (_koHumidity > 0) && (_koAirPressure == 0) ) {
        return _fakegato->addEntry(0x07, String(avgTemp), String(avgHum), "0");
    } else if ( (_koTemperature > 0) && (_koHumidity == 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, String(avgTemp), "0",  String(avgPres));
    } else if ( (_koTemperature > 0) && (_koHumidity == 0) && (_koAirPressure == 0) ) {
        return _fakegato->addEntry(0x07, String(avgTemp), "0",  "0");
    } else if ( (_koTemperature == 0) && (_koHumidity > 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, "0", String(avgHum),  String(avgPres));
    } else if ( (_koTemperature == 0) && (_koHumidity == 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, "0", "0",  String(avgPres));
    }  else if ( (_koTemperature == 0) && (_koHumidity == 0) && (_koAirPressure == 0) ) {
        return _fakegato->addEntry(0x07, "0",String(avgHum),  "0");
    }   

     return false;   
}

void HAPPluginKNXDeviceWeather::handle(bool forced){

}

void HAPPluginKNXDeviceWeather::writeTemperatureCallback(GroupObject& go){
    float result = go.value();

#if HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER
    _averageTemperature += result;
    _measurementCountTemperature++;
#endif


    // Serial.println("Temperature: " + String(result));
    _temperatureValue->setValue(result);

    // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid(), _temperatureValue->iid(), String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}

void HAPPluginKNXDeviceWeather::writeHumidityCallback(GroupObject& go){
    uint8_t result = go.value();
    
#if HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER    
    _averageHumidity += result;
    _measurementCountHumidity++;
    // Serial.println("Humiditiy: " + String(result));
#endif

    _humidityValue->setValue(result);
        // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid(), _humidityValue->iid(), String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}

void HAPPluginKNXDeviceWeather::writePressureCallback(GroupObject& go){
    uint16_t result = go.value();

#if HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER
    _averagePressure += result;
    _measurementCountPressure++;
    // Serial.println("AirPressure: " + String(result));
#endif


    _pressureValue->setValue(result);
        // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid(), _pressureValue->iid(), String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}


#if HAP_PLUGIN_KNX_ENABLE_AVERAGE_FOR_WEATHER
void HAPPluginKNXDeviceWeather::addToAverage(float temperature, float humidity, uint16_t pressure){
	_averageTemperature += temperature;
	_averageHumidity    += humidity;	
	_averagePressure    += pressure;

	_measurementCountTemperature++;
    _measurementCountHumidity++;
    _measurementCountPressure++;    
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceWeather::resetAverage(){
	_averageTemperature = 0;
	_averageHumidity    = 0;	
	_averagePressure    = 0;

    _measurementCountTemperature = 0;
    _measurementCountHumidity   = 0;
    _measurementCountPressure   = 0;
}
#endif