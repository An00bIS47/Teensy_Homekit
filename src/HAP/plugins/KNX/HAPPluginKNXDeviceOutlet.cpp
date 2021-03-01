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

        HAPCharacteristicString *plugServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read, HAP_STRING_LENGTH_MAX);
        plugServiceName->setValueString(_name);
        _accessory->addCharacteristics(outletService, plugServiceName);

        //
        // Power State 
        // 
        _stateValue = new HAPCharacteristicBool(HAP_CHARACTERISTIC_ON, permission_read|permission_write|permission_notify);            
        _stateValue->setValueString("0");

        auto callbackState = std::bind(&HAPPluginKNXDeviceOutlet::changedState, this, std::placeholders::_1, std::placeholders::_2);        
        _stateValue->valueChangeFunctionCall = callbackState;

        // Read value from knx
        _stateValue->valueString();

        auto callbackReadState = std::bind(&HAPPluginKNXDeviceOutlet::readState, this);        
        _stateValue->valueGetFunctionCall = callbackReadState;

        _accessory->addCharacteristics(outletService, _stateValue);
  

        //
        // in use State
        //
        _inUseState = new HAPCharacteristicBool(HAP_CHARACTERISTIC_OUTLET_IN_USE, permission_read|permission_notify);        
        // auto callbackState = std::bind(&HAPPluginRCSwitchDevice::setValue, this, std::placeholders::_1, std::placeholders::_2);        
        // _inUseState->valueChangeFunctionCall = callbackState;
        _inUseState->setValueString("1");
        _accessory->addCharacteristics(outletService, _inUseState);

        //
        // power current (EVE)
        //
        _curPowerValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_FAKEGATO_ELECTRIC_CURRENT, permission_read|permission_notify, 0.0, 3600, 0.1, unit_none);
        _curPowerValue->setValueString("0.0");
        
        auto callbackChangeCurPower = std::bind(&HAPPluginKNXDeviceOutlet::changedPowerCurrent, this, std::placeholders::_1, std::placeholders::_2);
        _curPowerValue->valueChangeFunctionCall = callbackChangeCurPower;

        auto callbackReadPowerCurrent = std::bind(&HAPPluginKNXDeviceOutlet::readPowerCurrent, this);        
        _curPowerValue->valueGetFunctionCall = callbackReadPowerCurrent;
        
        // Read value from knx
        _curPowerValue->valueString();

        _accessory->addCharacteristics(outletService, _curPowerValue);
        

        //
        // power total (EVE)
        //
        _ttlPowerValue = new HAPCharacteristicFloat(HAP_CHARACTERISTIC_FAKEGATO_TOTAL_CONSUMPTION, permission_read|permission_notify, 0.0, 3600, 0.1, unit_none);
        _ttlPowerValue->setValueString("0.0");
        
        auto callbackChangeTtlPower = std::bind(&HAPPluginKNXDeviceOutlet::changedPowerTotal, this, std::placeholders::_1, std::placeholders::_2);
        _ttlPowerValue->valueChangeFunctionCall = callbackChangeTtlPower;
        
        auto callbackReadPowerTotal = std::bind(&HAPPluginKNXDeviceOutlet::readPowerTotal, this);        
        _ttlPowerValue->valueGetFunctionCall = callbackReadPowerTotal;
        
        // Read value from knx
        _ttlPowerValue->valueString();


        _accessory->addCharacteristics(outletService, _ttlPowerValue);


        //
        // parental Lock
        //
        _parentalLock = new HAPCharacteristicBool(HAP_CHARACTERISTIC_LOCK_PHYSICAL_CONTROLS, permission_read|permission_write);        
        _parentalLock->setValueString("0");    
        _accessory->addCharacteristics(outletService, _parentalLock);


        // if (tmpService == nullptr) tmpService = temperatureService;
        knx.getGroupObject(_koWriteState).dataPointType(DPT_Switch);
        knx.getGroupObject(_koReadState).dataPointType(DPT_Switch);

        knx.getGroupObject(_koWriteCurrent).dataPointType(DPT_Value_Electric_Current);
        knx.getGroupObject(_koWriteActEnergy).dataPointType(DPT_ActiveEnergy_kWh);
        

        knx.getGroupObject(_koWriteState).callback(std::bind(&HAPPluginKNXDeviceOutlet::writeStateCallback, this, std::placeholders::_1));        
        knx.getGroupObject(_koWriteCurrent).callback(std::bind(&HAPPluginKNXDeviceOutlet::writeCurrentCallback, this, std::placeholders::_1));        
        knx.getGroupObject(_koWriteActEnergy).callback(std::bind(&HAPPluginKNXDeviceOutlet::writeActiveEnergyCallback, this, std::placeholders::_1));        
    


        //
        // FakeGato
        // 	
        if (_enableFakegato){
            _fakegato = new HAPFakeGatoEnergy();
            _fakegato->registerFakeGatoService(_accessory, _name, true);    


            if (_enableSchedule) {
                // Fakegato Schedule
                // _fakegato->setSerialNumber(sn);        
                _fakegato->setCallbackTimerStart(std::bind(&HAPPluginKNXDeviceOutlet::writeStateCallbackFromSchedule, this, std::placeholders::_1));
                // _fakegato.setCallbackTimerEnd(std::bind(&HAPPluginKNXDeviceOutlet::switchCallback, this));
                _fakegato->setCallbackGetTimestampLastActivity(std::bind(&HAPPluginKNXDeviceOutlet::getTimestampLastActivity, this));
                
                _fakegato->setCallbackSaveConfig(std::bind(&HAPPluginKNXDeviceOutlet::saveConfig, this));

                _fakegato->beginSchedule();
            }

            auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceOutlet::fakeGatoCallback, this);
            _fakegatoFactory->registerFakeGato(_fakegato,  String(_name), callbackAddEntry);
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
void HAPPluginKNXDeviceOutlet::setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory){
    
    _fakegatoFactory = fakegatoFactory;
    // Serial.printf("w fakegato: %p\n", _fakegatoFactory);
}   


void HAPPluginKNXDeviceOutlet::changedPowerTotal(float oldValue, float newValue){
    Serial.printf("[KNX:%X] New power total: %.2f\n", _id, newValue);

    if ( oldValue == newValue) return;

    struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _ttlPowerValue->iid, String(newValue));							
    _eventManager->queueEvent( EventManager::kEventNotifyController, event); 
}


void HAPPluginKNXDeviceOutlet::changedState(bool oldValue, bool newValue){
    Serial.printf("[KNX:%X] New power state: %d (send:%d - %d)\n", _id, newValue, _shouldSend, _koReadState);

    if (_shouldSend) {
        knx.getGroupObject(_koReadState).value(newValue);
    }        

    _shouldSend = true;

    if ( oldValue == newValue) return;

    _timestampLastActivity = HAPServer::timestamp();

    // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _stateValue->iid, String(newValue));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);

    if (_enableFakegato){
        // Add entry to fakegato
        _fakegato->addEntry(0x01, "0", "0", "0", "0", newValue == true ? "1" : "0");
    }     
}

void HAPPluginKNXDeviceOutlet::changedPowerCurrent(float oldValue, float newValue){
    Serial.printf("[KNX:%X] New power current: %.2f\n", _id, newValue);
    if ( oldValue == newValue) return;

    struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _curPowerValue->iid, String(newValue));							
    _eventManager->queueEvent( EventManager::kEventNotifyController, event);


    String inUse;
    newValue > 0.01 ? inUse = "1" : inUse = "0";    
    if (_inUseState->valueString() != inUse){
        _inUseState->setValueString(inUse);

        struct HAPEvent eventInUse = HAPEvent(nullptr, _accessory->aid, _inUseState->iid, String(inUse));							
        _eventManager->queueEvent( EventManager::kEventNotifyController, eventInUse);
    }
    
}


bool HAPPluginKNXDeviceOutlet::fakeGatoCallback(){	
    // return _fakegato->addEntry(_stateValue->value());   
    // return _fakegato->addEntry(0x1F, "0", "0", "0", "0", _stateValue->value());   

    // bool HAPFakeGatoEnergy::addEntry(uint8_t bitmask, String powerWatt, String powerVoltage, String powerCurrent, String stringPower10th, String status)
    return _fakegato->addEntry(0x1F, "0", "0", _curPowerValue->valueString(), _ttlPowerValue->valueString(), _stateValue->valueString());   
}

void HAPPluginKNXDeviceOutlet::handle(bool forced){

}

void HAPPluginKNXDeviceOutlet::writeStateCallback(GroupObject& go){
    bool result = go.value();

    _shouldSend = false;
    Serial.println("writeStateCallback: " + String(result));
    _stateValue->setValueString(String(result));
}

void HAPPluginKNXDeviceOutlet::writeActiveEnergyCallback(GroupObject& go){
    float result = go.value();

    _shouldSend = false;
    Serial.println("writeActiveEnergyCallback: " + String(result));

    // ToDo: Add proper conversion
    if (result > 3600) result = 3599;
    _ttlPowerValue->setValueString(String(result));
}

void HAPPluginKNXDeviceOutlet::writeCurrentCallback(GroupObject& go){
    float result = go.value();

    _shouldSend = false;
    Serial.println("writeCurrentCallback: " + String(result));

    // ToDo: Add proper conversion
    _curPowerValue->setValueString(String(result));
}


void HAPPluginKNXDeviceOutlet::writeStateCallbackFromSchedule(uint16_t state){
    LogD(HAPServer::timeString() + " " + "HAPPluginKNXDeviceOutlet" + "->" + String(__FUNCTION__) + " [   ] " + "Callback to switch " + String(state == 1 ? "ON" : "OFF"), true);

    _shouldSend = true;

    // _callbackRCSwitchSend(houseAddress, deviceAddress, state);
    // _stateValue->setValue(state == 1 ? "1" : "0");

    knx.getGroupObject(_koReadState).value((state == 1));
}

uint32_t HAPPluginKNXDeviceOutlet::getTimestampLastActivity(){
    return _timestampLastActivity;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
JsonObject HAPPluginKNXDeviceOutlet::scheduleToJson(){
    return _fakegato->scheduleToJson();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceOutlet::scheduleFromJson(JsonObject &root){
    _fakegato->scheduleFromJson(root);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceOutlet::saveConfig(){ 
    LogE(HAPServer::timeString() + " " + "HAPPluginKNXDeviceOutlet" + "->" + String(__FUNCTION__) + " [   ] " + "Update config event", true);		
    _eventManager->queueEvent( EventManager::kEventUpdatedConfig, HAPEvent());
}

void HAPPluginKNXDeviceOutlet::readState(){
    Serial.printf("[KNX:%X] Read state\n", _id);
    _stateValue->_value = knx.getGroupObject(_koReadState).value();
}

void HAPPluginKNXDeviceOutlet::readPowerTotal(){
    Serial.printf("[KNX:%X] Read power total\n", _id);
    _ttlPowerValue->_value = knx.getGroupObject(_koWriteCurrent).value();
}

void HAPPluginKNXDeviceOutlet::readPowerCurrent(){
    Serial.printf("[KNX:%X] Read power current\n", _id);
    _curPowerValue->_value = knx.getGroupObject(_koWriteCurrent).value();
}