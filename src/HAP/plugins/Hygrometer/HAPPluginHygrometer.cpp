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

#ifndef HAP_PLUGIN_HYGRO_INTERVAL
#define HAP_PLUGIN_HYGRO_INTERVAL 5000
#endif

#define VERSION_MAJOR       0
#define VERSION_MINOR       8
#define VERSION_REVISION    0
#define VERSION_BUILD       0

//
// Set these values in the HAPGlobals.hpp
//
#ifndef HAP_PLUGIN_HYGROMETER_USE_DUMMY
#define HAP_PLUGIN_HYGROMETER_USE_DUMMY 	0
#endif


#ifndef HAP_PLUGIN_HYGROMTER_REFERENCE
#define HAP_PLUGIN_HYGROMTER_REFERENCE      		2550    // value if sensor is put in a glass of water
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
	LOG_V("Begin plugin %s\n", _config->name);

#if !HAP_PLUGIN_HYGROMETER_USE_DUMMY
    // Init the soil moisture sensor board
    //  VCC
    pinMode(HAP_PLUGIN_HYGROMETER_PIN_VCC, OUTPUT);
    digitalWrite(HAP_PLUGIN_HYGROMETER_PIN_VCC, LOW);

    // ADC
    pinMode(HAP_PLUGIN_HYGROMETER_PIN_ADC, INPUT);
    // analogReadResolution(12);    // 12 is already the default value
#else
	LOG_W("   - Using Hygrometer dummy!\n");
	randomSeed(analogRead(HAP_PLUGIN_HYGROMETER_PIN_ADC));
#endif
	return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginHygrometer::identify(bool oldValue, bool newValue) {
	LOG_I("Start Identify %s\n", _config->name);
}


void HAPPluginHygrometer::changedMoisture(float oldValue, float newValue) {
	LOG_I("[%s] Changed moisture: %.2lf >>> %.2lf\n", _config->name, oldValue, newValue);
}



void HAPPluginHygrometer::handleImpl(bool forced){
	LOG_V("[%s] - Handle plugin [%d]\n", (const char*)_config->name, _config->interval);


	float percentage = 0;

#if HAP_PLUGIN_HYGROMETER_USE_DUMMY
	percentage = random(0, 100);
#else
	uint16_t moisture = readSensorRaw();
	percentage = moistureToPercentage(moisture);
#endif

	_humidityValue->setValue(percentage, true);
	_humidityAverage.addValue(percentage);

	queueNotifyEvent(_humidityValue);

#if HAP_DEBUG_HYGROMETER && !HAP_PLUGIN_HYGROMETER_USE_DUMMY
	LOG_D("[%s] Moisture: %2.lf = %2.lf \%\n", _config->name, moisture, percentage);
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

	LOG_V("[%s] - Initializing accessory for plugin ...\n", _config->name);

	//
	// Unique serial number !!!
	//
    char hex[7] = {'\0',};
#if HAP_PLUGIN_DHT_USE_DUMMY
	sprintf(hex, "%s", "DUMMY");
#else
	sprintf(hex, "%s.%s", STR(HAP_PLUGIN_HYGROMETER_PIN_VCC), STR(HAP_PLUGIN_HYGROMETER_PIN_ADC));
#endif

	const char* snTemp = HAPDeviceID::serialNumber(_config->name, hex).c_str();
	char serialNumber[strlen(snTemp)] = {'\0',};
	strcpy(serialNumber, snTemp);


	char sensorName[strlen(_config->name) + strlen(hex) + 2] = {'\0', };
	sprintf(sensorName, "%s %s", _config->name, hex);



	LOG_V("[%s] - Add new accessory ...", _config->name);
	_accessory = new HAPAccessory();
	auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
	_accessory->addInfoService("Hygrometer", "ACME", "YL-69", serialNumber, callbackIdentify, version());

	LOGRAW_V("OK\n");

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
		LOG_V("[%s] - Add new %s service ...", _config->name, "humidity");

		HAPService* humidityService = new HAPService(HAPServiceType::HumiditySensor);
		_accessory->addService(humidityService);

		LOGRAW_V("OK\n");

		{
			LOG_V("[%s] - Add new %s sensor ...", _config->name, "soil moisture");

			HAPCharacteristic<std::string> *humServiceName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
			humServiceName->setValue("Soil Moisture Sensor");
			_accessory->addCharacteristicToService(humidityService, humServiceName);

			_humidityValue = new HAPCharacteristic<float>(HAPCharacteristicType::CurrentRelativeHumidity, HAPPermission::Read|HAPPermission::Notify, 0, 100, 1, HAPUnit::Percentage);
			_humidityValue->setValue(0.0F);

			_humidityValue->setValueChangeCallback(std::bind(&HAPPluginHygrometer::changedMoisture, this, std::placeholders::_1, std::placeholders::_2));
			_humidityValue->setValueGetCallback(std::bind(&HAPPluginHygrometer::readSensor, this));

			_accessory->addCharacteristicToService(humidityService, _humidityValue);

			LOGRAW_V("OK\n");
		}
	}

#endif


	//
	// FakeGato
	//
	LOG_V("[%s] - Add fakegato ...", _config->name);

	_fakegato.addCharacteristic(new HAPFakegatoCharacteristicHumidity(std::bind(&HAPPluginHygrometer::getAveragedHumidityValue, this)));
	_fakegato.registerFakeGatoService(_accessory, serialNumber);

	auto callbackAddEntry = std::bind(&HAPPluginHygrometer::fakeGatoCallback, this);
	registerFakeGato(&_fakegato, _config->name, callbackAddEntry);

	LOGRAW_V("OK\n");

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
	LogD(HAPTime::timestring() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Get config implementation", true);

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
	LOG_V("[%s] Set defaults\n", _config->name);
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
	LOG_V("[%s] Set configuration\n", _config->name);
	_config = cfg;
}