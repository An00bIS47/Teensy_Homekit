//
// HAPPluginHygrometer.cpp
// Homekit
//
//  Created on: 29.04.2018
//      Author: michael
//
// Pinout:
// The YL-39 module has 4 pins:
// - VCC: 3.3-5V
// - GND
// - D0 : digital pin that goes LOW or HIGH depending on a preset value
// - A0 : analog output that can be easily read by Arduino
//
// On ESP32s when using WiFi only ADC1 pins can be used! ADC2 is disabled when using WIFI
//
// The issue with such sensors is that the probe itself work by trying to measure the current
// that goes from one side of it to the other. Because of this electrolysis occurs so it can destroy
// the probe (YL-69) pretty fast in high-moisture soils. To bypass this,
// instead of directly linking the VCC to the Arduino's VCC/5V we simply link it to a digital pin
// and power it (digital pin goes HIGH) only before we do a readout (see the code for this).
//
//
// ESP32 Pinout:
// 14 -> VCC
// A0 -> GPIO 32
//
//

#include "HAPPluginHygrometer.hpp"
#include "HAPServer.hpp"

#define HAP_PLUGIN_HYGRO_INTERVAL 5000

#define VERSION_MAJOR       0
#define VERSION_MINOR       7
#define VERSION_REVISION    0
#define VERSION_BUILD       0

//
// Set these values in the HAPGlobals.hpp
//
#ifndef HAP_PLUGIN_HYGROMETER_USE_DUMMY
#define HAP_PLUGIN_HYGROMETER_USE_DUMMY 	0
#endif


#ifndef HAP_PLUGIN_HYGROMTER_REFERENCE
#define HAP_PLUGIN_HYGROMTER_REFERENCE      		2550    // value if put in a glass of water
#endif

#ifndef HAP_PLUGIN_HYGROMETER_PIN_VCC
#define HAP_PLUGIN_HYGROMETER_PIN_VCC       		A6	// 14
#endif

#ifndef HAP_PLUGIN_HYGROMETER_PIN_ADC
#define HAP_PLUGIN_HYGROMETER_PIN_ADC       		A7	// 32
#endif

#ifndef HAP_HYGROMETER_LEAK_SENSOR_ENABLED
#define HAP_HYGROMETER_LEAK_SENSOR_ENABLED  		1		// enable leak sensor for hygrometer
#endif

#ifndef HAP_HYGROMETER_LEAK_PERCENTAGE
#define HAP_HYGROMETER_LEAK_PERCENTAGE				35		// Level when a "leak" notification is triggered
#endif

#ifndef HAP_DEBUG_HYGROMETER
#define HAP_DEBUG_HYGROMETER 0
#endif

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginHygrometer::HAPPluginHygrometer(){
	_type = HAP_PLUGIN_TYPE_ACCESSORY;
	// _name = "Hygrometer";
	// _isEnabled = HAP_PLUGIN_USE_HYGROMETER;
	// _interval = HAP_PLUGIN_INTERVAL;
	_previousMillis = 0;

    _version.major      = VERSION_MAJOR;
    _version.minor      = VERSION_MINOR;
    _version.revision   = VERSION_REVISION;
    _version.build      = VERSION_BUILD;

	_humidityValue 		= nullptr;

#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED
	// _leakSensor			= nullptr;
	// _leakSensorEnabled  = true;
#else
	_leakSensorEnabled  = false;
#endif

	_config = new HAPConfigurationPlugin("Hygro", true, HAP_PLUGIN_HYGRO_INTERVAL);

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPPluginHygrometer::begin(){
	LogV(HAPTime::timeString() + " " + String(_config->name) + "->" + String(__FUNCTION__) + " [   ] " + "begin()", true);

#if !HAP_PLUGIN_HYGROMETER_USE_DUMMY
    // Init the soil moisture sensor board
    //  VCC
    pinMode(HAP_PLUGIN_HYGROMETER_PIN_VCC, OUTPUT);
    digitalWrite(HAP_PLUGIN_HYGROMETER_PIN_VCC, LOW);

    // ADC
    pinMode(HAP_PLUGIN_HYGROMETER_PIN_ADC, INPUT);
    // analogReadResolution(12);    // 12 is already the default value
#else
	LogI("Using Hygrometer dummy!", true);
	randomSeed(analogRead(HAP_PLUGIN_HYGROMETER_PIN_ADC));
#endif
	return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginHygrometer::identify(bool oldValue, bool newValue) {
	Serial.printf("Start Identify %s\n", _config->name);
}


void HAPPluginHygrometer::changedHumidity(float oldValue, float newValue) {
	Serial.printf("[%s] New soil moisture: %f\n",  _config->name, newValue);
}



void HAPPluginHygrometer::handleImpl(bool forced){

    LogV(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);

	float percentage = 0;

#if HAP_PLUGIN_HYGROMETER_USE_DUMMY
	percentage = random(0, 100);
#else

	uint16_t moisture = readSensorRaw();
	percentage = moistureToPercentage(moisture);
	_humidityValue->setValue(percentage, false);
	_humidityAverage.addValue(percentage);
	queueNotifyEvent(_humidityValue);

#if HAP_DEBUG_HYGROMETER
    Serial.print("[" + _config->name + "] Moisture: ");
    Serial.print(moisture);
    Serial.print(" = ");
    Serial.print(percentage);
    Serial.println(" %");
#endif

#endif

#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED

	// if (_leakSensorEnabled) {
	// 	if (percentage < HAP_HYGROMETER_LEAK_PERCENTAGE) {
	// 		_leakSensor->setValueString("1");
	// 	} else {
	// 		_leakSensor->setValueString("0");
	// 	}
	// }


#endif

}



#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginHygrometer::initAccessory(){

#if HAP_PLUGIN_HYGROMETER_USE_DUMMY
	String sn = HAPDeviceID::serialNumber("HY", "DUMMY");
#else
	String sn = HAPDeviceID::serialNumber("HY", String(HAP_PLUGIN_HYGROMETER_PIN_VCC) + String(HAP_PLUGIN_HYGROMETER_PIN_VCC));
#endif

	_accessory = new HAPAccessory();
	auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
	_accessory->addInfoService("Hygrometer", "ACME", "YL-69", sn, callbackIdentify, version());


	//
	// Leak Service
	//
#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED
	// HAPService* leakService = new HAPService(HAP_SERVICE_LEAK_SENSOR);
	// _accessory->addService(leakService);

	//
	// Leak Sensor
	//
	// {
	// 	HAPCharacteristicString *leakServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
	// 	leakServiceName->setValueString("Hygro Leak");
	// 	_accessory->addCharacteristicToService(leakService, leakServiceName);

	// 	uint8_t validValues[] = {0, 1};	// 0 = no Leak detected , 1 = leak detected
	// 	_leakSensor = new HAPCharacteristicUInt8(HAP_CHARACTERISTIC_LEAK_DETECTED, permission_read|permission_notify, 0, 1, 1, unit_none, 2, validValues);
	// 	_leakSensor->setValueString("0");
	// 	_accessory->addCharacteristicToService(leakService, _leakSensor);
	// }

	//
	// Humidity Service
	//
	// {
	// 	HAPCharacteristicString *humServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
	// 	humServiceName->setValueString("Hygro Sensor");
	// 	_accessory->addCharacteristicToService(leakService, humServiceName);

	// 	_humidityValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, permission_read|permission_notify, 0, 100, 0.1, unit_percentage);
	// 	_humidityValue->setValueString("0.0");

	// 	auto callbackChangeHum = std::bind(&HAPPluginHygrometer::changeHum, this, std::placeholders::_1, std::placeholders::_2);
	// 	//_humidityValue->valueChangeFunctionCall = std::bind(&changeHum);
	// 	_humidityValue->valueChangeFunctionCall = callbackChangeHum;
	// 	_accessory->addCharacteristicToService(leakService, _humidityValue);
	// }

#else


	{
		//
		// Humidity Servicee
		//
		HAPService* humidityService = new HAPService(HAP_SERVICE_HUMIDITY_SENSOR);
		_accessory->addService(humidityService);
		{
			HAPCharacteristicT<String> *humServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
			humServiceName->setValue("Soil Moisture Sensor");
			_accessory->addCharacteristicToService(humidityService, humServiceName);

			_humidityValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0, 100, 0.1, HAP_UNIT_PERCENTAGE);
			_humidityValue->setValue(0.0F);

			_humidityValue->setValueChangeCallback(std::bind(&HAPPluginHygrometer::changedHumidity, this, std::placeholders::_1, std::placeholders::_2));
			_humidityValue->setValueGetCallback(std::bind(&HAPPluginHygrometer::readSensor, this));

			_accessory->addCharacteristicToService(humidityService, _humidityValue);
		}
	}

#endif


	//
	// FakeGato
	//
	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginHygrometer::getAveragedHumidityValue, this)));
	_fakegato.registerFakeGatoService(_accessory, sn);

	auto callbackAddEntry = std::bind(&HAPPluginHygrometer::fakeGatoCallback, this);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);


	return _accessory;
}


#if HAP_ENABLE_WEBSERVER
HAPConfigurationValidationResult HAPPluginHygrometer::validateConfig(JsonObject object){

#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED
    HAPConfigurationValidationResult result;


    result = HAPPlugin::validateConfig(object);
    if (result.valid == false) {
        return result;
    }
    result.valid = false;

    // plugin._name.leakEnabled
    if (object.containsKey("leakEnabled") && !object["leakEnabled"].is<bool>()) {
        result.reason = "plugins." + String(_config->name) + ".leakEnabled is not a bool";
        return result;
    }

    result.valid = true;
    return result;
#else
	return HAPPlugin::validateConfig(object);
#endif

}


JsonObject HAPPluginHygrometer::getConfigImpl(){
	LogD(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Get config implementation", true);

    DynamicJsonDocument doc(128);

#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED
    doc["leakEnabled"] = _leakSensorEnabled;
#endif

#if HAP_DEBUG_CONFIG
    serializeJson(doc, Serial);
    Serial.println();
#endif

    doc.shrinkToFit();
	return doc.as<JsonObject>();
}

void HAPPluginHygrometer::setConfigImpl(JsonObject root){

#if HAP_HYGROMETER_LEAK_SENSOR_ENABLED
	if (root.containsKey("leakEnabled")){
        // LogD(" -- password: " + String(root["password"]), true);
        _leakSensorEnabled = root["leakEnabled"].as<bool>();
    }
#endif

}
#endif

bool HAPPluginHygrometer::fakeGatoCallback(){
	// return _fakegato.addEntry(0x02, "0", _humidityValue->value(), "0");
	_fakegato.addEntry(0x01);
	return true;
}


float HAPPluginHygrometer::readSensor() {
	return moistureToPercentage(readSensorRaw());
}

float HAPPluginHygrometer::moistureToPercentage(uint16_t moisture){
	float percentage = map(moisture, HAP_PLUGIN_HYGROMTER_REFERENCE, 0, 100, 0) * 1.0;
	return percentage;
}


uint16_t HAPPluginHygrometer::readSensorRaw() {

    digitalWrite(HAP_PLUGIN_HYGROMETER_PIN_VCC, HIGH);
    delay(100);

    uint16_t moisture = 4095 - analogRead(HAP_PLUGIN_HYGROMETER_PIN_ADC);
    digitalWrite(HAP_PLUGIN_HYGROMETER_PIN_VCC, LOW);
	return moisture;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPConfigurationPlugin* HAPPluginHygrometer::setDefaults(){
	_config->enabled  = HAP_PLUGIN_USE_HYGROMETER;
	_config->interval = HAP_PLUGIN_HYGRO_INTERVAL;
	_config->dataPtr = nullptr;
	_config->dataSize = 0;
	return _config;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginHygrometer::setConfiguration(HAPConfigurationPlugin* cfg){
	_config = cfg;
}