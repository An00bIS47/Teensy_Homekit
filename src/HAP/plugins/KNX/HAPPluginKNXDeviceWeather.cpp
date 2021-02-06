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
}



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

        HAPCharacteristicString *tempServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read, HAP_STRING_LENGTH_MAX);
        tempServiceName->setValue("Temperature Sensor " + String(_id));

        _accessory->addCharacteristics(temperatureService, tempServiceName);

        //HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
        _temperatureValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, permission_read|permission_notify, -50, 100, 0.1, unit_celsius);
        _temperatureValue->setValue("0.0");
        auto callbackChangeTemp = std::bind(&HAPPluginKNXDeviceWeather::changeTemp, this, std::placeholders::_1, std::placeholders::_2);
        //_temperatureValue->valueChangeFunctionCall = std::bind(&changeTemp);
        _temperatureValue->valueChangeFunctionCall = callbackChangeTemp;
        _accessory->addCharacteristics(temperatureService, _temperatureValue);

        // if (tmpService == nullptr) tmpService = temperatureService;

        knx.getGroupObject(_koTemperature).dataPointType(DPT_Value_Temp);
        knx.getGroupObject(_koTemperature).callback(std::bind(&HAPPluginKNXDeviceWeather::writeTemperatureCallback, this, std::placeholders::_1));
        
        // Read value from knx
        _temperatureValue->value();
    }



	//
	// Humidity
	//
    if (_koHumidity > 0) {
        HAPService* humidityService = new HAPService(HAP_SERVICE_HUMIDITY_SENSOR);
        _accessory->addService(humidityService);

        HAPCharacteristicString *humServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read, HAP_STRING_LENGTH_MAX);
        humServiceName->setValue("Humidity Sensor " + String(_id));
        _accessory->addCharacteristics(humidityService, humServiceName);

        _humidityValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, permission_read|permission_notify, 0, 100, 0.1, unit_percentage);
        _humidityValue->setValue("0.0");

        auto callbackChangeHum = std::bind(&HAPPluginKNXDeviceWeather::changeHum, this, std::placeholders::_1, std::placeholders::_2);
        //_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
        _humidityValue->valueChangeFunctionCall = callbackChangeHum;
        _accessory->addCharacteristics(humidityService, _humidityValue);

        // if (tmpService == nullptr) tmpService = humidityService;

        if (_useHumdityDPT9) {
            knx.getGroupObject(_koHumidity).dataPointType(DPT_Value_Humidity);
        } else {
            knx.getGroupObject(_koHumidity).dataPointType(DPT_Scaling);
        }

        knx.getGroupObject(_koHumidity).callback(std::bind(&HAPPluginKNXDeviceWeather::writeHumidityCallback, this, std::placeholders::_1));

        // Read value from knx
        _humidityValue->value();
    }

	//
	// AirPressure
	//
    if (_koAirPressure > 0) {
        HAPService* pressureService = new HAPService(HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR);
        _accessory->addService(pressureService);

        HAPCharacteristicString *pressureServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read, HAP_STRING_LENGTH_MAX);
        pressureServiceName->setValue("Pressure Sensor " + String(_id));
        _accessory->addCharacteristics(pressureService, pressureServiceName);
        
        _pressureValue = new HAPCharacteristicUInt16(HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE, permission_read|permission_notify, 0, 1100, 1, unit_hpa);
        _pressureValue->setValue("320");

        auto callbackChangePressure = std::bind(&HAPPluginKNXDeviceWeather::changePressure, this, std::placeholders::_1, std::placeholders::_2);
        //_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
        _pressureValue->valueChangeFunctionCall = callbackChangePressure;
        _accessory->addCharacteristics(pressureService, _pressureValue);

        // if (tmpService == nullptr) tmpService = pressureService;


        knx.getGroupObject(_koAirPressure).dataPointType(DPT_Value_Pres);
        knx.getGroupObject(_koAirPressure).callback(std::bind(&HAPPluginKNXDeviceWeather::writePressureCallback, this, std::placeholders::_1));

        // Read value from knx
        _pressureValue->value();
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
    //     _accessory->addCharacteristics(tmpService, _lastUpdate);
    // }


	//
	// FakeGato
	// 	    
    if (_enableFakegato){
        _fakegato = new HAPFakeGatoWeather();
        _fakegato->registerFakeGatoService(_accessory, _name);    
        auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceWeather::fakeGatoCallback, this);
        _fakegatoFactory->registerFakeGato(_fakegato,  String(_name), callbackAddEntry);
    }
    
	

    return _accessory;
}

void HAPPluginKNXDeviceWeather::setEventManager(EventManager* eventManager){
      
    _eventManager = eventManager;
    // Serial.printf("w event: %p\n", _eventManager);  
}


void HAPPluginKNXDeviceWeather::setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory){
    
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

    if ( (_koTemperature > 0) && (_koHumidity > 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, _temperatureValue->value(), _humidityValue->value(), _pressureValue->value());
    } else if ( (_koTemperature > 0) && (_koHumidity > 0) && (_koAirPressure == 0) ) {
        return _fakegato->addEntry(0x07, _temperatureValue->value(), _humidityValue->value(), "0");
    } else if ( (_koTemperature > 0) && (_koHumidity == 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, _temperatureValue->value(), "0",  _pressureValue->value());
    } else if ( (_koTemperature > 0) && (_koHumidity == 0) && (_koAirPressure == 0) ) {
        return _fakegato->addEntry(0x07, _temperatureValue->value(), "0",  "0");
    } else if ( (_koTemperature == 0) && (_koHumidity > 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, "0", _humidityValue->value(),  _pressureValue->value());
    } else if ( (_koTemperature == 0) && (_koHumidity == 0) && (_koAirPressure > 0) ) {
        return _fakegato->addEntry(0x07, "0", "0",  _pressureValue->value());
    }  else if ( (_koTemperature == 0) && (_koHumidity == 0) && (_koAirPressure == 0) ) {
        return _fakegato->addEntry(0x07, "0", _humidityValue->value(),  "0");
    }   

     return false;   
}

void HAPPluginKNXDeviceWeather::handle(bool forced){

}

void HAPPluginKNXDeviceWeather::writeTemperatureCallback(GroupObject& go){
    float result = go.value();

    // Serial.println("Temperature: " + String(result));
    _temperatureValue->setValue(String(result));

    // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _temperatureValue->iid, String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}

void HAPPluginKNXDeviceWeather::writeHumidityCallback(GroupObject& go){
    uint8_t result = go.value();
    // Serial.println("Humiditiy: " + String(result));

    _humidityValue->setValue(String(result));
        // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _humidityValue->iid, String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}

void HAPPluginKNXDeviceWeather::writePressureCallback(GroupObject& go){
    uint16_t result = go.value();
    // Serial.println("AirPressure: " + String(result));

    _pressureValue->setValue(String(result));
        // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _pressureValue->iid, String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
}