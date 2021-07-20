//
// HAPPluginLED.cpp
// Homekit
//
//  Created on: 01.08.2019
//      Author: michael
//
#include "HAPPluginLED.hpp"
#include "HAPServer.hpp"


#ifndef HAP_PLUGIN_LED_INTERVAL
#define HAP_PLUGIN_LED_INTERVAL 1000
#endif

#if defined (CORE_TEENSY)
#ifndef BUILTIN_LED
#define BUILTIN_LED 13
#endif
#else
#ifndef BUILTIN_LED
#define BUILTIN_LED 5
#endif
#endif


#ifndef HAP_LED_PIN
#define HAP_LED_PIN BUILTIN_LED
#endif

#define VERSION_MAJOR       0
#define VERSION_MINOR       8	// 8 = std::string
#define VERSION_REVISION    0
#define VERSION_BUILD       0

#ifndef HAP_LED_ENABLE_BRIGHTNESS
#define HAP_LED_ENABLE_BRIGHTNESS   0
#endif

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginLED::HAPPluginLED(){
    _type               = HAP_PLUGIN_TYPE_ACCESSORY;
    _previousMillis     = 0;
    _isOn               = false;
    _gpio               = HAP_LED_PIN;

    _version.major      = VERSION_MAJOR;
    _version.minor      = VERSION_MINOR;
    _version.revision   = VERSION_REVISION;
    _version.build      = VERSION_BUILD;

    _powerState         = nullptr;

#if HAP_PLUGIN_LED_SUPPORTS_ENABLED
    _enabledState       = nullptr;
#endif

    _blinkingEnabled    = true;

    _config = new HAPConfigurationPlugin("LED", true, HAP_PLUGIN_LED_INTERVAL);
}


void HAPPluginLED::changeState(bool oldValue, bool newValue) {

    LOG_I("[%s] Changed state: %d >>> %d\n", _config->name, oldValue, newValue);
}

#if HAP_PLUGIN_LED_SUPPORTS_ENABLED
void HAPPluginLED::changeEnabled(bool oldValue, bool newValue) {

    LOG_I("[%s] Changed enabled: %d >>> %d\n", _config->name, oldValue, newValue);

    if (newValue != oldValue) {
        _blinkingEnabled = newValue;

        _powerState->setValue(newValue, true);
	    queueNotifyEvent(_powerState);
    }
}
#endif


void HAPPluginLED::handleImpl(bool forced){

    if (isEnabled() && _blinkingEnabled) {
        LOG_V("[%s] - Handle plugin [%d]\n", (const char*)_config->name, _config->interval);

        _isOn == true ? _isOn = false : _isOn = true;

        digitalWrite(HAP_LED_PIN, _isOn);

        bool withCallback = false;
#if HAP_DEBUG
        withCallback = true;
#endif

        _powerState->setValue(_isOn, withCallback);
        queueNotifyEvent(_powerState);
    }
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPPluginLED::begin(){
    LOG_V("[%s] - Begin plugin\n", _config->name);
    pinMode(_gpio, OUTPUT);
    digitalWrite(_gpio, _isOn);

    return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginLED::initAccessory(){

    LOG_V("[%s] - Initializing accessory for plugin ...\n", _config->name);


    //
	// Unique serial number !!!
	//
    char hex[7] = {'\0', };
    sprintf(hex, "%d", _gpio);

	const char* snTemp = HAPDeviceID::serialNumber(_config->name, hex).c_str();
	char serialNumber[strlen(snTemp) + 1] = {'\0',};
	strcpy(serialNumber, snTemp);

	char sensorName[strlen(_config->name) + strlen(hex) + 2] = {'\0', };
	sprintf(sensorName, "%s %s", _config->name, hex);


    //
    // Add new accessory
    //
    LOG_V("[%s] - Add new accessory ...", _config->name);

	_accessory = new HAPAccessory();
	//HAPAccessory::addInfoServiceToAccessory(_accessory, "Builtin LED", "ACME", "LED", "123123123", &identify);
    auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService("Builtin LED", "ACME", "Builtin LED", serialNumber, callbackIdentify, version());

    LOGRAW_V("OK\n");

    //
    // Lightbulb Service
    //
    LOG_V("[%s] - Add new %s service ...", _config->name, "light bulb");
    HAPService* _service = new HAPService(HAPServiceType::Lightbulb);
    // _service->setPrimaryService(true);
    _accessory->addService(_service);

    LOGRAW_V("OK\n");

    {
        LOG_V("[%s] - Add new %s sensor ...", _config->name, "led");

        HAPCharacteristic<std::string>* lightServiceName = new HAPCharacteristic<std::string>(HAPCharacteristicType::Name, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
        lightServiceName->setValue("LED");
        _accessory->addCharacteristicToService(_service, lightServiceName);

        _powerState = new HAPCharacteristic<bool>(HAPCharacteristicType::On, HAPPermission::Read|HAPPermission::Write|HAPPermission::Notify);
        _powerState->setValue(_isOn);

        auto callbackPowerState = std::bind(&HAPPluginLED::changeState, this, std::placeholders::_1, std::placeholders::_2);
        _powerState->setValueChangeCallback(callbackPowerState);
        _accessory->addCharacteristicToService(_service, _powerState);

        LOGRAW_V("OK\n");
    }


#if HAP_PLUGIN_LED_SUPPORTS_ENABLED
    //
    // Outlet Service / Switch Service
    //
    HAPService* switchService = new HAPService(HAP_SERVICE_SWITCH);
    _accessory->addService(switchService);

    HAPCharacteristic<std::string>* plugServiceName = new HAPCharacteristic<std::string>(HAP_CHARACTERISTIC_NAME, HAPPermission::Read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
    plugServiceName->setValue("LED");
    _accessory->addCharacteristicToService(switchService, plugServiceName);

    _enabledState = new HAPCharacteristic<bool>(HAP_CHARACTERISTIC_ON, HAPPermission::Read|HAPPermission::Write|HAPPermission::Notify);
    _enabledState->setDescription("Enabled");
    _enabledState->setValue(_blinkingEnabled);

    auto callbackEnabledState = std::bind(&HAPPluginLED::changeEnabled, this, std::placeholders::_1, std::placeholders::_2);
    _enabledState->setValueChangeCallback(callbackEnabledState);
    _accessory->addCharacteristicToService(switchService, _enabledState);

    // Link switchService to _service
    _service->addLinkedServiceId(switchService->aid());
#endif

	LOGRAW_D("OK\n");

	return _accessory;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginLED::identify(bool oldValue, bool newValue) {
    LOG_I("Start Identify %s\n", _config->name);
}

#if HAP_ENABLE_WEBSERVER

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPConfigurationValidationResult HAPPluginLED::validateConfig(JsonObject object){
    HAPConfigurationValidationResult result;

    result = HAPPlugin::validateConfig(object);
    if (result.valid == false) {
        return result;
    }
    result.valid = false;

    // plugin._name.username
    if (object.containsKey("gpio") && !object["gpio"].is<uint8_t>()) {
        result.reason = "plugins." + String(_config->name) + ".gpio is not an integer";
        return result;
    }

    result.valid = true;
    return result;
}

JsonObject HAPPluginLED::getConfigImpl(){

    LogD(HAPTime::timestring() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Get config implementation", true);

    DynamicJsonDocument doc(128);
    doc["gpio"] = _gpio;
    doc["blinkingEnabled"] = _blinkingEnabled;

#if HAP_DEBUG_CONFIG
    serializeJson(doc, Serial);
    Serial.println();
#endif

#if defined(ARDUINO_ARCH_ESP32)
	doc.shrinkToFit();
#endif
	return doc.as<JsonObject>();
}

void HAPPluginLED::setConfigImpl(JsonObject root){
    if (root.containsKey("gpio")){
        // LogD(" -- password: " + String(root["password"]), true);
        _gpio = root["gpio"].as<uint8_t>();
        _blinkingEnabled = root["blinkingEnabled"].as<bool>();
    }
}

#endif


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPConfigurationPlugin* HAPPluginLED::setDefaults(){

    LOG_V("[%s] Set defaults\n", _config->name);

	_config->enabled  = HAP_PLUGIN_USE_LED;
	_config->interval = HAP_PLUGIN_LED_INTERVAL;
	_config->dataPtr = nullptr;
	_config->dataSize = 0;
	return _config;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginLED::setConfiguration(HAPConfigurationPlugin* cfg){
    LOG_V("[%s] Set configuration\n", _config->name);
	_config = cfg;
    //_config->setToJsonCallback(std::bind(&HAPPluginBME280::internalConfigToJson, this, std::placeholders::_1));
}