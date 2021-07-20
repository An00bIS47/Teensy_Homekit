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
}


HAPPluginKNXDeviceWeather::~HAPPluginKNXDeviceWeather(){
    if (_fakegato != nullptr) delete _fakegato;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginKNXDeviceWeather::initAccessory(){

    if ( (_koTemperature == 0) && (_koHumidity == 0) && (_koAirPressure == 0) ) {
        LogE("ERROR: Please set communication objects first!", true);
        return nullptr;
    }

    // knx-*1* cause of WEATHER
    String sn = HAPDeviceID::serialNumber("knx-1", String(_id));

    // Create accessory if not already created
    _accessory = new HAPAccessory();
    auto callbackIdentify = std::bind(&HAPPluginKNXDeviceWeather::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService(String(_name), "KNX", "KNX Weather", sn, callbackIdentify, HAP_KNX_WEATHER_VERSION);



    //
	// Temperature
	//
    if (_koTemperature > 0) {
        HAPService* temperatureService = new HAPService(HAP_SERVICE_TEMPERATURE_SENSOR);
        _accessory->addService(temperatureService);

        HAPCharacteristic<String> *tempServiceName = new HAPCharacteristic<String>(HAP_CHARACTERISTIC_NAME, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
        tempServiceName->setValue("Temperature Sensor " + String(_id));

        _accessory->addCharacteristicToService(temperatureService, tempServiceName);

        //HAPCharacteristicFloat(uint8_t _type, int _permission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _permission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
        _temperatureValue = new HAPCharacteristic<float>(HAP_CHARACTERISTIC_CURRENT_TEMPERATURE, HAPPermission::Read|HAPPermission::Notify, -50, 100, 0.1, HAP_UNIT_CELSIUS);
        _temperatureValue->setValue(0.0F);

        _temperatureValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceWeather::changedTemperature, this, std::placeholders::_1, std::placeholders::_2));

        _accessory->addCharacteristicToService(temperatureService, _temperatureValue);

        knx.getGroupObject(_koTemperature).dataPointType(DPT_Value_Temp);
        knx.getGroupObject(_koTemperature).callback(std::bind(&HAPPluginKNXDeviceWeather::setTemperatureFromKNXCallback, this, std::placeholders::_1));

        // Read value from knx
        _temperatureValue->value(true);
    }



	//
	// Humidity
	//
    if (_koHumidity > 0) {
        HAPService* humidityService = new HAPService(HAP_SERVICE_HUMIDITY_SENSOR);
        _accessory->addService(humidityService);

        HAPCharacteristic<String> *humServiceName = new HAPCharacteristic<String>(HAP_CHARACTERISTIC_NAME, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
        humServiceName->setValue("Humidity Sensor " + String(_id));
        _accessory->addCharacteristicToService(humidityService, humServiceName);

        _humidityValue = new HAPCharacteristic<float>(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, HAPPermission::Read|HAPPermission::Notify, 0, 100, 0.1, HAP_UNIT_PERCENTAGE);
        _humidityValue->setValue(0.0);

        _humidityValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceWeather::changedHumidity, this, std::placeholders::_1, std::placeholders::_2));

        _accessory->addCharacteristicToService(humidityService, _humidityValue);

        if (_useHumdityDPT9) {
            knx.getGroupObject(_koHumidity).dataPointType(DPT_Value_Humidity);
        } else {
            knx.getGroupObject(_koHumidity).dataPointType(DPT_Scaling);
        }

        knx.getGroupObject(_koHumidity).callback(std::bind(&HAPPluginKNXDeviceWeather::setHumidityFromKNXCallback, this, std::placeholders::_1));

        // Read value from knx
        _humidityValue->value(true);
    }

	//
	// AirPressure
	//
    if (_koAirPressure > 0) {
        HAPService* pressureService = new HAPService(HAP_SERVICE_FAKEGATO_AIR_PRESSURE_SENSOR);
        _accessory->addService(pressureService);

        HAPCharacteristic<String> *pressureServiceName = new HAPCharacteristic<String>(HAP_CHARACTERISTIC_NAME, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
        pressureServiceName->setValue("Pressure Sensor " + String(_id));
        _accessory->addCharacteristicToService(pressureService, pressureServiceName);

        _pressureValue = new HAPCharacteristic<uint16_t>(HAP_CHARACTERISTIC_FAKEGATO_AIR_PRESSURE, HAPPermission::Read|HAPPermission::Notify, 0, 1100, 1, HAP_UNIT_HPA);
        _pressureValue->setValue(320);

        _pressureValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceWeather::changedPressure, this, std::placeholders::_1, std::placeholders::_2));

        _accessory->addCharacteristicToService(pressureService, _pressureValue);

        knx.getGroupObject(_koAirPressure).dataPointType(DPT_Value_Pres);
        knx.getGroupObject(_koAirPressure).callback(std::bind(&HAPPluginKNXDeviceWeather::setPressureFromKNXCallback, this, std::placeholders::_1));

        // Read value from knx
        _pressureValue->value(true);
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
    if (_enableFakegato) {

        if (_fakegato == nullptr) {
            _fakegato = new HAPFakegato();
        }


        if (_koTemperature > 0) _fakegato->addCharacteristic(new HAPFakegatoCharacteristicTemperature(std::bind(&HAPPluginKNXDeviceWeather::getAveragedTemperatureValue, this)));
        if (_koHumidity > 0)    _fakegato->addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginKNXDeviceWeather::getAveragedHumidityValue, this)));
        if (_koAirPressure > 0) _fakegato->addCharacteristic(new HAPFakegatoCharacteristicAirPressure(std::bind(&HAPPluginKNXDeviceWeather::getAveragedPressureValue, this)));

        _fakegato->registerFakeGatoService(_accessory, String(_name));

        auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceWeather::fakeGatoCallback, this);
        _fakegatoFactory->registerFakeGato(_fakegato, callbackAddEntry);
    }

    return _accessory;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginKNXDeviceWeather::setEventManager(EventManager* eventManager){
    _eventManager = eventManager;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginKNXDeviceWeather::setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory){
    _fakegatoFactory = fakegatoFactory;
}


void HAPPluginKNXDeviceWeather::identify(bool oldValue, bool newValue){
    Serial.printf("[KNX:%X] Identify device: %d\n", _id, newValue);
}


void HAPPluginKNXDeviceWeather::changedTemperature(float oldValue, float newValue) {
	Serial.printf("[KNX:%X] New temperature: %f\n", _id, newValue);
}

void HAPPluginKNXDeviceWeather::changedHumidity(float oldValue, float newValue) {
	Serial.printf("[KNX:%X] New humidity: %f\n", _id, newValue);
}

void HAPPluginKNXDeviceWeather::changedPressure(uint16_t oldValue, uint16_t newValue) {
	Serial.printf("[KNX:%X] New pressure: %d\n", _id, newValue);
}

// void HAPPluginKNXDeviceWeather::changeLastUpdate(String oldValue, String newValue){
//     Serial.printf("[KNX:%X] New LastUpdate: %s\n", id, newValue.c_str());
// }


bool HAPPluginKNXDeviceWeather::fakeGatoCallback(){

    uint8_t bitmask = 0;
    if (_koTemperature > 0) bitmask += 1 << 0;
    if (_koHumidity > 0)    bitmask += 1 << 1;
    if (_koAirPressure > 0) bitmask += 1 << 2;

    _fakegato->addEntry(bitmask);
    return true;
}

void HAPPluginKNXDeviceWeather::handle(bool forced){

}

/**
 * @brief
 *  Reads the values WRITTEN into this group object and sets the temperature characteristic accordingly
 *
 * @param go
 */
void HAPPluginKNXDeviceWeather::setTemperatureFromKNXCallback(GroupObject& go){
    float result = go.value();

    // Serial.println("Temperature: " + String(result));

    _temperatureValue->setValue(result);
    _temperatureAverage.addValue(result);
    queueNotifyEvent(_temperatureValue);
}


/**
 * @brief
 *  Reads the values WRITTEN into this group object and sets the humidity characteristic accordingly
 *
 * @param go
 */
void HAPPluginKNXDeviceWeather::setHumidityFromKNXCallback(GroupObject& go){
    uint8_t result = go.value();
    float floatResult = result * 1.0;

    // Serial.println("Humiditiy: " + String(floatResult));

    _humidityValue->setValue(floatResult);
    _humidityAverage.addValue(floatResult);
    queueNotifyEvent(_humidityValue);
}


/**
 * @brief
 *  Reads the values WRITTEN into this group object and sets the air pressure characteristic accordingly
 *
 * @param go
 */
void HAPPluginKNXDeviceWeather::setPressureFromKNXCallback(GroupObject& go){
    uint16_t result = go.value();

    // Serial.println("AirPressure: " + String(result));

    _pressureValue->setValue(result);
    _pressureAverage.addValue(result);
    queueNotifyEvent(_pressureValue);
}

