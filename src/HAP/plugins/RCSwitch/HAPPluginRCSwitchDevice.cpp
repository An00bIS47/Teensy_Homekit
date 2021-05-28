//
// HAPPluginRCSwitchDevice.cpp
// Homekit
//
//  Created on: 14.09.2019
//      Author: michael
//

#include "HAPPluginRCSwitchDevice.hpp"
#include "HAPServer.hpp"
#include "HAPLogger.hpp"

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginRCSwitchDevice::HAPPluginRCSwitchDevice(){
    name    = "";
    houseAddress = 0;
    deviceAddress = 0;

    _accessory          = nullptr;
    _eventManager       = nullptr;
    _fakegatoFactory    = nullptr;

    _stateValue         = nullptr;
    _inUseState         = nullptr;
    _parentalLock       = nullptr;
    _curPowerValue      = nullptr;
    _ttlPowerValue      = nullptr;

    _timestampLastActivity = 0;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginRCSwitchDevice::HAPPluginRCSwitchDevice(uint8_t houseAddress_, uint8_t deviceAddress_, String name_)
: houseAddress(houseAddress_)
, deviceAddress(deviceAddress_)
, name(name_)
{
    _accessory          = nullptr;
    _eventManager       = nullptr;
    _fakegatoFactory    = nullptr;

    _stateValue         = nullptr;
    _inUseState         = nullptr;
    _parentalLock       = nullptr;
    _curPowerValue      = nullptr;
    _ttlPowerValue      = nullptr;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginRCSwitchDevice::~HAPPluginRCSwitchDevice(){
    // if (_configInternal != nullptr) delete _configInternal;
    // if (_config != nullptr) delete _config;

    delete _accessory;
    delete _stateValue;
    delete _inUseState;
    delete _parentalLock;
    delete _curPowerValue;
    delete _ttlPowerValue;


    // _configInternal = nullptr;
    // _config = nullptr;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginRCSwitchDevice::initAccessory(){

    String sn = HAPDeviceID::serialNumber("RC", String(houseAddress) + String(deviceAddress));

    // Create accessory if not already created
    _accessory = new HAPAccessory();
    //HAPAccessory::addInfoServiceToAccessory(_accessory, "Builtin LED", "ACME", "LED", "123123123", &identify);
    auto callbackIdentify = std::bind(&HAPPluginRCSwitchDevice::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService(name, "ACME", "RCSwitch", sn, callbackIdentify, "1.0");

    // //
    // // Outlet Service / Switch Service
    // // !!! Fakegato history is only working with switch service !!!
    // //
    // HAPService* outletService = new HAPService(HAP_SERVICE_SWITCH);
    // _accessory->addService(outletService);

    // stringCharacteristics *plugServiceName = new stringCharacteristics(HAP_CHARACTERISTIC_NAME, permission_read, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
    // plugServiceName->setValue(name);
    // _accessory->addCharacteristicToService(outletService, plugServiceName);

    // //
    // // Power State
    // //
    // _stateValue = new boolCharacteristics(HAP_CHARACTERISTIC_ON, permission_read|permission_write|permission_notify);
    // _stateValue->setValue("0");

    // auto callbackState = std::bind(&HAPPluginRCSwitchDevice::changedState, this, std::placeholders::_1, std::placeholders::_2);
    // _stateValue->valueChangeFunctionCall = callbackState;
    // _accessory->addCharacteristicToService(outletService, _stateValue);


    // //
    // // FakeGato History
    // //
    // _fakegato.registerFakeGatoService(_accessory, "RCSwitch " + String(houseAddress) + String(deviceAddress));
	// auto callbackAddEntry = std::bind(&HAPPluginRCSwitchDevice::fakeGatoCallback, this);
	// _fakegatoFactory->registerFakeGato(&_fakegato,  "RCSwitch " + String(houseAddress) + String(deviceAddress), callbackAddEntry);

    //
    // Outlet Service
    //
    HAPService* outletService = new HAPService(HAP_SERVICE_OUTLET);
    _accessory->addService(outletService);

    HAPCharacteristicT<String> *plugServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, 32);
    plugServiceName->setValue("RCSwitch " + String(houseAddress) + String(deviceAddress));
    _accessory->addCharacteristicToService(outletService, plugServiceName);

    //
    // Power State
    //
    _stateValue = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_ON, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE|HAP_PERMISSION_NOTIFY);
    _inUseState->setValue(true);

    auto callbackState = std::bind(&HAPPluginRCSwitchDevice::changedState, this, std::placeholders::_1, std::placeholders::_2);
    _stateValue->setValueChangeCallback(callbackState);
    _accessory->addCharacteristicToService(outletService, _stateValue);


    //
    // in use State
    //
    _inUseState = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_OUTLET_IN_USE, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY);
    // auto callbackState = std::bind(&HAPPluginRCSwitchDevice::setValue, this, std::placeholders::_1, std::placeholders::_2);
    // _inUseState->valueChangeFunctionCall = callbackState;
    _inUseState->setValue(true);
    _accessory->addCharacteristicToService(outletService, _inUseState);

    //
    // power current (EVE)
    //
    _curPowerValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_FAKEGATO_ELECTRIC_CURRENT, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0.0, 3600, 0.1, HAP_UNIT_NONE);
    _curPowerValue->setValue(0.0);

    auto callbackChangeCurPower = std::bind(&HAPPluginRCSwitchDevice::changedPowerCurrent, this, std::placeholders::_1, std::placeholders::_2);
    _curPowerValue->setValueChangeCallback(callbackChangeCurPower);
    _accessory->addCharacteristicToService(outletService, _curPowerValue);


    //
    // power total (EVE)
    //
    _ttlPowerValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_FAKEGATO_TOTAL_CONSUMPTION, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0.0, 3600, 0.1, HAP_UNIT_NONE);
    _ttlPowerValue->setValue(0.0);

    auto callbackChangeTtlPower = std::bind(&HAPPluginRCSwitchDevice::changedPowerTotal, this, std::placeholders::_1, std::placeholders::_2);
    _ttlPowerValue->setValueChangeCallback(callbackChangeTtlPower);
    _accessory->addCharacteristicToService(outletService, _ttlPowerValue);


    //
    // parental Lock
    //
    _parentalLock = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_LOCK_PHYSICAL_CONTROLS, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE);
    _inUseState->setValue(false);
    // auto callbackChangeTtlPower = std::bind(&HAPPluginRCSwitchDevice::changedPowerTotal, this, std::placeholders::_1, std::placeholders::_2);
    // _ttlPowerValue->valueChangeFunctionCall = callbackChangeTtlPower;
    _accessory->addCharacteristicToService(outletService, _parentalLock);



    //
    // FakeGato History
    //

    _fakegato.addCharacteristic(new HAPFakegatoCharacteristicPowerWatt(std::bind(&HAPPluginRCSwitchDevice::getAveragedTotalPowerValue, this)));
    _fakegato.addCharacteristic(new HAPFakegatoCharacteristicPowerVoltage(std::bind(&HAPPluginRCSwitchDevice::getAveragedPowerVoltage, this)));
    _fakegato.addCharacteristic(new HAPFakegatoCharacteristicPowerCurrent(std::bind(&HAPPluginRCSwitchDevice::getAveragedCurrentPowerValue, this)));
    _fakegato.addCharacteristic(new HAPFakegatoCharacteristicPowerTenth(std::bind(&HAPPluginRCSwitchDevice::getAveragedPowerTenth, this)));
    _fakegato.addCharacteristic(new HAPFakegatoCharacteristicPowerOnOff(std::bind(&HAPPluginRCSwitchDevice::getPowerState, this)));


    _fakegato.registerFakeGatoService(_accessory, "RCSwitch " + String(houseAddress) + String(deviceAddress), true);

    // Fakegato Schedule
    _fakegato.setSerialNumber(sn);
    _fakegato.setCallbackTimerStart(std::bind(&HAPPluginRCSwitchDevice::switchCallback, this, std::placeholders::_1));
    // _fakegato.setCallbackTimerEnd(std::bind(&HAPPluginRCSwitchDevice::switchCallback, this));
    _fakegato.setCallbackGetTimestampLastActivity(std::bind(&HAPPluginRCSwitchDevice::getTimestampLastActivity, this));

    _fakegato.setCallbackSaveConfig(std::bind(&HAPPluginRCSwitchDevice::saveConfig, this));

    // _fakegato.beginSchedule();

	auto callbackAddEntry = std::bind(&HAPPluginRCSwitchDevice::fakeGatoCallback, this);
	_fakegatoFactory->registerFakeGato(&_fakegato, callbackAddEntry);


    return _accessory;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginRCSwitchDevice::setEventManager(EventManager* eventManager){
    _eventManager = eventManager;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginRCSwitchDevice::setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory){
    _fakegatoFactory = fakegatoFactory;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginRCSwitchDevice::identify(bool oldValue, bool newValue) {
    printf("Start Identify rcswitch:%d-%d\n", houseAddress, deviceAddress);
}


void HAPPluginRCSwitchDevice::changedPowerCurrent(float oldValue, float newValue){

    if (oldValue != newValue) {
        queueNotifyEvent(_curPowerValue);

        String inUse;
        newValue > 0.1 ? inUse = "1" : inUse = "0";
        if (_inUseState->valueString() != inUse){
            _inUseState->setValue(inUse);

            queueNotifyEvent(_inUseState);
        }
    }
}

void HAPPluginRCSwitchDevice::changedPowerTotal(float oldValue, float newValue){
    if (oldValue != newValue) {
        queueNotifyEvent(_ttlPowerValue);
    }
}


void HAPPluginRCSwitchDevice::changedState(bool oldValue, bool newValue){

    LogD(HAPTime::timeString() + " " + "HAPPluginRCSwitchDevice" + "->" + String(__FUNCTION__) + " [   ] " + "Setting new value to " + String(newValue), true);

    if (oldValue != newValue) {
        _callbackRCSwitchSend(houseAddress, deviceAddress, newValue);

        _timestampLastActivity = HAPTime::timestamp();

        // Add entry to fakegato
        _fakegato.addEntry(0x01);
        queueNotifyEvent(_stateValue);
    }
}

bool HAPPluginRCSwitchDevice::fakeGatoCallback(){
    // LogD(HAPTime::timeString() + " " + "HAPPluginPCA301Device" + "->" + String(__FUNCTION__) + " [   ] " + "fakeGatoCallback()", true);

    // Serial.println("power: " + _curPowerValue->value());
    _fakegato.addEntry(0x1F);
    return true;
}

void HAPPluginRCSwitchDevice::switchCallback(uint16_t state){
    LogD(HAPTime::timeString() + " " + "HAPPluginRCSwitchDevice" + "->" + String(__FUNCTION__) + " [   ] " + "Callback to switch " + String(state == 1 ? "ON" : "OFF"), true);
    // _callbackRCSwitchSend(houseAddress, deviceAddress, state);
    _stateValue->setValue(state);
}

uint32_t HAPPluginRCSwitchDevice::getTimestampLastActivity(){
    return _timestampLastActivity;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
JsonObject HAPPluginRCSwitchDevice::scheduleToJson(){
    return _fakegato.scheduleToJson();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginRCSwitchDevice::scheduleFromJson(JsonObject &root){
    _fakegato.scheduleFromJson(root);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginRCSwitchDevice::saveConfig(){
    LogE(HAPTime::timeString() + " " + "HAPPluginRCSwitchDevice" + "->" + String(__FUNCTION__) + " [   ] " + "Update config event", true);
    _eventManager->queueEvent( EventManager::kEventUpdatedConfig, HAPEvent());
}


bool HAPPluginRCSwitchDevice::getPowerState(){
    return _stateValue->value(false);
}