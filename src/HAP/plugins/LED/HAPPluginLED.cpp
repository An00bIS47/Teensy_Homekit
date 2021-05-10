//
// HAPPluginLED.cpp
// Homekit
//
//  Created on: 01.08.2019
//      Author: michael
//
#include "HAPPluginLED.hpp"
#include "HAPServer.hpp"

#define HAP_PLUGIN_LED_INTERVAL 1000

#if 0
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
#define VERSION_MINOR       6	// 2 = FakeGato support
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

    _enabledState       = nullptr;
    _blinkingEnabled    = true;

    _config = new HAPConfigurationPlugin("LED", true, HAP_PLUGIN_LED_INTERVAL);
}


void HAPPluginLED::changePower(bool oldValue, bool newValue) {
    // LogD(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Setting iid " + String(iid) +  " oldValue: " + oldValue + " -> newValue: " + newValue, true);

    if (newValue == true) {
        digitalWrite(_gpio, HIGH);     // dont know why to put low here, maybe because of SPI ?
    } else {
        digitalWrite(_gpio, LOW);
    }
}

void HAPPluginLED::changeEnabled(bool oldValue, bool newValue) {
    // LogD(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Setting iid " + String(iid) +  " oldValue: " + oldValue + " -> newValue: " + newValue, true);

    if (newValue != oldValue) {
        _blinkingEnabled = newValue;

        _powerState->setValue(newValue);

        // Config update notification
        struct HAPEvent event = HAPEvent(nullptr, 0, 0, "");
        _eventManager->queueEvent( EventManager::kEventUpdatedConfig, event, EventManager::kLowPriority);
    }
}



void HAPPluginLED::handleImpl(bool forced){

    if (isEnabled() && _blinkingEnabled) {
        LogV(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);

        _powerState->setValue(_isOn);
        queueNotifyEvent(_powerState);

    }



}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPPluginLED::begin(){
    LogV(HAPTime::timeString() + " " + String(_config->name) + "->" + String(__FUNCTION__) + " [   ] " + "begin()", true);

    pinMode(_gpio, OUTPUT);
    digitalWrite(_gpio, _isOn);

    return true;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginLED::initAccessory(){

    LogV("\nInitializing accessory for plugin: " + String(_config->name) + " ...", true);

    String sn = HAPDeviceID::serialNumber("LED", String(_gpio));

    //
    // Add new accessory
    //
	_accessory = new HAPAccessory();
	//HAPAccessory::addInfoServiceToAccessory(_accessory, "Builtin LED", "ACME", "LED", "123123123", &identify);
    auto callbackIdentify = std::bind(&HAPPlugin::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService("Builtin LED", "ACME", "LED", sn, callbackIdentify, version());


    //
    // Lightbulb Service
    //
    HAPService* _service = new HAPService(HAP_SERVICE_LIGHTBULB);
    // _service->setPrimaryService(true);
    _accessory->addService(_service);


    HAPCharacteristicT<String>* lightServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
    lightServiceName->setValue("LED");
    _accessory->addCharacteristicToService(_service, lightServiceName);

    _powerState = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_ON, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE|HAP_PERMISSION_NOTIFY);
    _powerState->setValue(_isOn);

    auto callbackPowerState = std::bind(&HAPPluginLED::changePower, this, std::placeholders::_1, std::placeholders::_2);
    _powerState->setValueChangeCallback(callbackPowerState);
    _accessory->addCharacteristicToService(_service, _powerState);



    //
    // Outlet Service / Switch Service
    //
    HAPService* switchService = new HAPService(HAP_SERVICE_SWITCH);
    _accessory->addService(switchService);

    HAPCharacteristicT<String>* plugServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ);
    plugServiceName->setValue("LED");
    _accessory->addCharacteristicToService(switchService, plugServiceName);

    _enabledState = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_ON, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE|HAP_PERMISSION_NOTIFY);
    _enabledState->setDescription("Enabled");
    _enabledState->setValue(_blinkingEnabled);

    auto callbackEnabledState = std::bind(&HAPPluginLED::changeEnabled, this, std::placeholders::_1, std::placeholders::_2);
    _enabledState->setValueChangeCallback(callbackEnabledState);
    _accessory->addCharacteristicToService(switchService, _enabledState);

    // Link switchService to _service
    _service->addLinkedServiceId(switchService->aid());

	LogD("OK", true);

	return _accessory;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginLED::identify(bool oldValue, bool newValue) {
    Serial.printf(F("Start Identify Light from member\n"));
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

    LogD(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Get config implementation", true);

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
	_config = cfg;
}