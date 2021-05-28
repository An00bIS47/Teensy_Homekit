//
// HAPPluginKNXDeviceOutlet.cpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#include "HAPPluginKNXDeviceOutlet.hpp"
#include "HAPServer.hpp"
#include "HAPLogger.hpp"
#include "HAPPluginKNX.hpp"
#include "HAPCustomCharacteristics+Services.hpp"

#define HAP_KNX_OUTLET_VERSION "0.1"

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPPluginKNXDeviceOutlet::HAPPluginKNXDeviceOutlet(uint8_t id_, char name[], bool enableFakegato, bool enableSchedule,  uint16_t koReadState, uint16_t koWriteState, uint16_t koWriteCurrent, uint16_t koWriteActEnergy){
    _id = id_;
    strncpy(_name, name, 40);
    _type    			= HAPPluginKNXServiceTypeOutlet;

    _accessory          = nullptr;
    _eventManager       = nullptr;
    _fakegatoFactory    = nullptr;
    _fakegato           = nullptr;

	// _lastUpdate			= nullptr;

    _koWriteState       = koWriteState;
    _koReadState        = koReadState;

    _koWriteCurrent     = koWriteCurrent;
    _koWriteActEnergy   = koWriteActEnergy;

    _enableFakegato     = enableFakegato;
    _enableSchedule     = enableSchedule;

    _shouldSend         = true;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessory* HAPPluginKNXDeviceOutlet::initAccessory(){

    if ( (_koReadState == 0) && (_koWriteState == 0) ) {
        LogE("ERROR: Please set communication objects first!", true);
        return nullptr;
    }

    String sn = HAPDeviceID::serialNumber("knx-2", String(_id));

    // Create accessory if not already created
    
    _accessory = new HAPAccessory();



    //HAPAccessory::addInfoServiceToAccessory(_accessory, "Builtin LED", "ACME", "LED", "123123123", &identify);
    auto callbackIdentify = std::bind(&HAPPluginKNXDevice::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService(String(_name), "KNX", "KNX Outlet", sn, callbackIdentify, HAP_KNX_OUTLET_VERSION);


    // HAPService* tmpService = nullptr;
    //
	// Temperature
	//
    if ( (_koReadState > 0) && (_koWriteState > 0) ) {

        //
        // Outlet Service / Switch Service
        // !!! Fakegato history is only working with switch service !!!
        //
        HAPService* outletService = new HAPService(HAP_SERVICE_OUTLET);
        _accessory->addService(outletService);

        HAPCharacteristicT<String>* plugServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
        plugServiceName->setValue(_name);
        _accessory->addCharacteristicToService(outletService, plugServiceName);

        //
        // Power State
        //
        _stateValue = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_ON, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE|HAP_PERMISSION_NOTIFY);
        _stateValue->setValue(false);

        _stateValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceOutlet::changedState, this, std::placeholders::_1, std::placeholders::_2));
        _stateValue->setValueGetCallback(std::bind(&HAPPluginKNXDeviceOutlet::readStateFromKNX, this));

        // Read value from knx
        _stateValue->value();

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

        _curPowerValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceOutlet::changedPowerCurrent, this, std::placeholders::_1, std::placeholders::_2));
        _curPowerValue->setValueGetCallback(std::bind(&HAPPluginKNXDeviceOutlet::readPowerCurrentFromKNX, this));

        // Read value from knx
        _curPowerValue->value();

        _accessory->addCharacteristicToService(outletService, _curPowerValue);


        //
        // power total (EVE)
        //
        _ttlPowerValue = new HAPCharacteristicT<float>(HAP_CHARACTERISTIC_FAKEGATO_TOTAL_CONSUMPTION, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 0.0, 3600, 0.1, HAP_UNIT_NONE);
        _ttlPowerValue->setValue(0.0);

        _ttlPowerValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceOutlet::changedPowerTotal, this, std::placeholders::_1, std::placeholders::_2));
        _ttlPowerValue->setValueGetCallback(std::bind(&HAPPluginKNXDeviceOutlet::readPowerTotalFromKNX, this));

        // Read value from knx
        _ttlPowerValue->value();

        _accessory->addCharacteristicToService(outletService, _ttlPowerValue);


        //
        // parental Lock
        //
        _parentalLock = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_LOCK_PHYSICAL_CONTROLS, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE);
        _parentalLock->setValue(false);
        _accessory->addCharacteristicToService(outletService, _parentalLock);


        // if (tmpService == nullptr) tmpService = temperatureService;
        knx.getGroupObject(_koWriteState).dataPointType(DPT_Switch);
        knx.getGroupObject(_koReadState).dataPointType(DPT_Switch);

        knx.getGroupObject(_koWriteCurrent).dataPointType(DPT_Value_Electric_Current);
        knx.getGroupObject(_koWriteActEnergy).dataPointType(DPT_ActiveEnergy_kWh);


        knx.getGroupObject(_koWriteState).callback(std::bind(&HAPPluginKNXDeviceOutlet::setStateFromKNXCallback, this, std::placeholders::_1));
        knx.getGroupObject(_koWriteCurrent).callback(std::bind(&HAPPluginKNXDeviceOutlet::setCurrentPowerFromKNXCallback, this, std::placeholders::_1));
        knx.getGroupObject(_koWriteActEnergy).callback(std::bind(&HAPPluginKNXDeviceOutlet::setActiveEnergyFromKNXCallback, this, std::placeholders::_1));



        //
        // FakeGato
        //
        if (_enableFakegato){

            if (_fakegato == nullptr){
                _fakegato = new HAPFakegatoScheduleEnergy();
            }

            _fakegato->addCharacteristic(new HAPFakegatoCharacteristicPowerWatt(std::bind(&HAPPluginKNXDeviceOutlet::getAveragedTotalPowerValue, this)));
            _fakegato->addCharacteristic(new HAPFakegatoCharacteristicPowerVoltage(std::bind(&HAPPluginKNXDeviceOutlet::getAveragedPowerVoltage, this)));
            _fakegato->addCharacteristic(new HAPFakegatoCharacteristicPowerCurrent(std::bind(&HAPPluginKNXDeviceOutlet::getAveragedCurrentPowerValue, this)));
            _fakegato->addCharacteristic(new HAPFakegatoCharacteristicPowerTenth(std::bind(&HAPPluginKNXDeviceOutlet::getAveragedPowerTenth, this)));
            _fakegato->addCharacteristic(new HAPFakegatoCharacteristicPowerOnOff(std::bind(&HAPPluginKNXDeviceOutlet::readStateFromKNX, this)));

            _fakegato->registerFakeGatoService(HAP_SCHEDULE_TYPE_ENERGY_EU_3, _accessory, _name);

            if (_enableSchedule) {
                // Fakegato Schedule
                _fakegato->setSerialNumber(sn);
                _fakegato->setCallbackTimerStart(std::bind(&HAPPluginKNXDeviceOutlet::writeStateCallbackFromSchedule, this, std::placeholders::_1));
                // _fakegato.setCallbackTimerEnd(std::bind(&HAPPluginKNXDeviceOutlet::switchCallback, this));
                _fakegato->setCallbackGetTimestampLastActivity(std::bind(&HAPPluginKNXDeviceOutlet::getTimestampLastActivity, this));

                _fakegato->setCallbackSaveConfig(std::bind(&HAPPluginKNXDeviceOutlet::saveConfig, this));

                // _fakegato->beginSchedule();
            }

            auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceOutlet::fakeGatoCallback, this);
            _fakegatoFactory->registerFakeGato(_fakegato,  callbackAddEntry);
        }

    }
    return _accessory;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginKNXDeviceOutlet::setEventManager(EventManager* eventManager){

    _eventManager = eventManager;
    // Serial.printf("w event: %p\n", _eventManager);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginKNXDeviceOutlet::setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory){

    _fakegatoFactory = fakegatoFactory;
    // Serial.printf("w fakegato: %p\n", _fakegatoFactory);
}


void HAPPluginKNXDeviceOutlet::changedPowerTotal(float oldValue, float newValue){
    Serial.printf("[KNX:%X] New power total: %.2f\n", _id, newValue);
}


void HAPPluginKNXDeviceOutlet::changedState(bool oldValue, bool newValue){
    Serial.printf("[KNX:%X] New power state: %d (send:%d - %d)\n", _id, newValue, _shouldSend, _koReadState);

    knx.getGroupObject(_koReadState).value(newValue);

    if ( oldValue == newValue) return;

    _timestampLastActivity = HAPTime::timestamp();

    if (_enableFakegato){
        //  Add entry
        _fakegato->addEntry(0x10); // only the 5th datapoint == 0x10 =  00010000	16
    }
}

void HAPPluginKNXDeviceOutlet::changedPowerCurrent(float oldValue, float newValue){
    Serial.printf("[KNX:%X] New power current: %.2f\n", _id, newValue);
    if ( oldValue == newValue) return;

    bool inUse;
    inUse = (newValue > 0.01);
    if (_inUseState->value() != inUse){
        _inUseState->setValue(inUse);

        queueNotifyEvent(_inUseState);
    }

}


bool HAPPluginKNXDeviceOutlet::fakeGatoCallback(){
    _fakegato->addEntry(0x05);

    // 0x5	00101	5
    //      |||||
    //      ||||+-> 0b02    ++ HAPFakegatoCharacteristicPowerWatt
    //      |||+-> 0c02     -- HAPFakegatoCharacteristicPowerVoltage    (not yet supported!)
    //      ||+-> 0d02      ++ HAPFakegatoCharacteristicPowerCurrent
    //      |+-> 0702       -- HAPFakegatoCharacteristicPowerTenth      (not yet supported!)
    //      +-> 0e01        -- HAPFakegatoCharacteristicPowerOnOff      (no periodic updates required!)

    // signature
    // 05 0b02 0c02 0d02 0702 0e01

    return true;
}

void HAPPluginKNXDeviceOutlet::handle(bool forced){

}

void HAPPluginKNXDeviceOutlet::setStateFromKNXCallback(GroupObject& go){
    bool result = go.value();

    // Serial.println("Temperature: " + String(result));

    Serial.printf("[KNX:%X] New state: %d\n", _id, result);

    _stateValue->setValue(result, false);
    queueNotifyEvent(_stateValue);


    _timestampLastActivity = HAPTime::timestamp();

    if (_enableFakegato){
        //  Add entry
        _fakegato->addEntry(0x01);
    }
}

void HAPPluginKNXDeviceOutlet::setActiveEnergyFromKNXCallback(GroupObject& go){
    float result = go.value();

    Serial.printf("[KNX:%X] New total power: %.2f\n", _id, result);

    // ToDo: Add proper conversion
    if (result > 3600) result = 3599;

    _ttlPowerValue->setValue(result, false);
    _ttlPowerAverage.addValue(result);
    queueNotifyEvent(_ttlPowerValue);
}

void HAPPluginKNXDeviceOutlet::setCurrentPowerFromKNXCallback(GroupObject& go){
    float result = go.value();

    Serial.printf("[KNX:%X] New current power: %.2f\n", _id, result);


    // ToDo: Add proper conversion
    _curPowerValue->setValue(result, false);
    _curPowerAverage.addValue(result);
    queueNotifyEvent(_curPowerValue);
}


void HAPPluginKNXDeviceOutlet::writeStateCallbackFromSchedule(uint16_t state){
    LogD(HAPTime::timeString() + " " + "HAPPluginKNXDeviceOutlet" + "->" + String(__FUNCTION__) + " [   ] " + "Callback to switch " + String(state == 1 ? "ON" : "OFF"), true);

    _shouldSend = true;

    // _callbackRCSwitchSend(houseAddress, deviceAddress, state);
    // _stateValue->setValue(state == 1 ? "1" : "0");

    knx.getGroupObject(_koReadState).value((state == 1));

}

uint32_t HAPPluginKNXDeviceOutlet::getTimestampLastActivity(){
    return _timestampLastActivity;
}

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// JsonObject HAPPluginKNXDeviceOutlet::scheduleToJson(){
//     return _fakegato->scheduleToJson();
// }

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// void HAPPluginKNXDeviceOutlet::scheduleFromJson(JsonObject &root){
//     _fakegato->scheduleFromJson(root);
// }


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPPluginKNXDeviceOutlet::saveConfig(){
    LogE(HAPTime::timeString() + " " + "HAPPluginKNXDeviceOutlet" + "->" + String(__FUNCTION__) + " [   ] " + "Update config event", true);
    _eventManager->queueEvent( EventManager::kEventUpdatedConfig, HAPEvent());
}

bool HAPPluginKNXDeviceOutlet::readStateFromKNX(){
    Serial.printf("[KNX:%X] Read state\n", _id);
    bool result = knx.getGroupObject(_koReadState).value();
    return result;
}

float HAPPluginKNXDeviceOutlet::readPowerTotalFromKNX(){
    Serial.printf("[KNX:%X] Read power total\n", _id);

    // ToDo: Add proper conversion
    float result = knx.getGroupObject(_koWriteCurrent).value();
    return result;
}

float HAPPluginKNXDeviceOutlet::readPowerCurrentFromKNX(){
    Serial.printf("[KNX:%X] Read power current\n", _id);

    // ToDo: Add proper conversion
    float result = knx.getGroupObject(_koWriteCurrent).value();
    return result;
}