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
	_brightnessState    = nullptr;

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
        
        setValue(_powerState->iid, "1", "0");

        struct HAPEvent event = HAPEvent(nullptr, 0, 0, "");							
        _eventManager->queueEvent( EventManager::kEventUpdatedConfig, event, EventManager::kLowPriority);                           
    }     
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginLED::changeBrightness(int oldValue, int newValue){
    printf("New brightness state: %d\n", newValue);
}

void HAPPluginLED::handleImpl(bool forced){
    
    if (isEnabled() && _blinkingEnabled) {
        LogV(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Handle plguin [" + String(_config->interval) + "]", true);

        if (_isOn) {            
            setValue(_powerState->iid, "1", "0");
        } else {
            setValue(_powerState->iid, "0", "1");            
        }

        // Add event
        // struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _powerState->iid, _powerState->value());							
        // _eventManager->queueEvent( EventManager::kEventNotifyController, event);

#if HAP_LED_ENABLE_BRIGHTNESS  
        uint32_t freeMem = ESP.getFreeHeap();        
        uint8_t percentage = ( freeMem * 100) / 245000;        
        // _brightnessState->setValue(String(percentage));
        setValue(charType_brightness, _brightnessState->getValue(), percentage);

        // Add event
        // struct HAPEvent eventB = HAPEvent(nullptr, _accessory->aid, _brightnessState->iid, _brightnessState->value());							
        // _eventManager->queueEvent( EventManager::kEventNotifyController, eventB);
#endif  
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


    HAPCharacteristicString *lightServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
    lightServiceName->setValueString("LED");
    _accessory->addCharacteristicToService(_service, lightServiceName);

    _powerState = new HAPCharacteristicBool(HAP_CHARACTERISTIC_ON, permission_read|permission_write|permission_notify);
    if (_isOn)
        _powerState->setValueString("1");
    else
        _powerState->setValueString("0");
    
    auto callbackPowerState = std::bind(&HAPPluginLED::changePower, this, std::placeholders::_1, std::placeholders::_2);        
    _powerState->valueChangeFunctionCall = callbackPowerState;
    _accessory->addCharacteristicToService(_service, _powerState);

    
    _brightnessState = new HAPCharacteristicInt(HAP_CHARACTERISTIC_BRIGHTNESS, permission_read|permission_write|permission_notify, 0, 100, 1, unit_percentage);
        //_brightnessState->valueChangeFunctionCall = &changeBrightness;

#if HAP_LED_ENABLE_BRIGHTNESS   
    _brightnessState->setValueString("50");
    auto callbackBrightness = std::bind(&HAPPluginLED::changeBrightness, this, std::placeholders::_1, std::placeholders::_2);        
    _brightnessState->valueChangeFunctionCall = callbackBrightness;
    _accessory->addCharacteristicToService(_service, _brightnessState);    
#endif

    // 
    // Outlet Service / Switch Service
    // 
    HAPService* switchService = new HAPService(HAP_SERVICE_SWITCH);
    _accessory->addService(switchService);

    HAPCharacteristicString *plugServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read);
    plugServiceName->setValueString("LED");
    _accessory->addCharacteristicToService(switchService, plugServiceName);

    _enabledState = new HAPCharacteristicBool(HAP_CHARACTERISTIC_ON, permission_read|permission_write|permission_notify);
    _enabledState->setDescription("Enabled");
    if (_blinkingEnabled)
        _enabledState->setValueString("1");
    else
        _enabledState->setValueString("0");

    auto callbackEnabledState = std::bind(&HAPPluginLED::changeEnabled, this, std::placeholders::_1, std::placeholders::_2);        
    _enabledState->valueChangeFunctionCall = callbackEnabledState;
    _accessory->addCharacteristicToService(switchService, _enabledState);

    // Link switchService to _service
    _service->addLinkedServiceId(switchService->serviceID);

	LogD("OK", true);

	return _accessory;
}


void HAPPluginLED::setValue(int iid, String oldValue, String newValue){
    LogV(HAPTime::timeString() + " " + _config->name + "->" + String(__FUNCTION__) + " [   ] " + "Setting iid " + String(iid) +  " oldValue: " + oldValue + " -> newValue: " + newValue, true);

     if (iid == _powerState->iid) {
        
        if (newValue == "1"){
            _isOn = true;
        } else {
            _isOn = false;
        }    

        _powerState->setValueString(newValue);

        struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _powerState->iid, _powerState->valueString());							
	    _eventManager->queueEvent( EventManager::kEventNotifyController, event);
 
    }
    //  else if (type == charType_brightness) {        
    //     _brightnessState->setValue(newValue);

    //     struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _brightnessState->iid, _brightnessState->value());							
	// 	_eventManager->queueEvent( EventManager::kEventNotifyController, event);
    // }

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginLED::identify(bool oldValue, bool newValue) {
    printf(F("Start Identify Light from member\n"));
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