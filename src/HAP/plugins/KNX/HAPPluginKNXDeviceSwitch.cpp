//
// HAPPluginKNXDeviceSwitch.cpp
// Homekit
//
//  Created on: 20.05.2020
//      Author: michael
//

#include "HAPPluginKNXDeviceSwitch.hpp"
#include "HAPServer.hpp"
#include "HAPLogger.hpp"
#include "HAPPluginKNX.hpp"
#include "HAPCustomCharacteristics+Services.hpp"

#define HAP_KNX_SWITCH_VERSION "0.1"

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPPluginKNXDeviceSwitch::HAPPluginKNXDeviceSwitch(uint8_t id_, char name[], bool enableFakegato, uint16_t koReadState, uint16_t koWriteState){
    _id = id_;
    strncpy(_name, name, 40);          
    _type    			= HAPPluginKNXServiceTypeSwitch;	

    _accessory          = nullptr;
    _eventManager       = nullptr;      
    _fakegatoFactory    = nullptr;
    _fakegato           = nullptr;

	// _lastUpdate			= nullptr;


    _koWriteState       = koWriteState;
    _koReadState        = koReadState;

    _enableFakegato     = enableFakegato;  
    _enableSchedule     = false;
    _shouldSend         = true;  
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
HAPAccessory* HAPPluginKNXDeviceSwitch::initAccessory(){

    if ( (_koReadState == 0) && (_koWriteState == 0) ) {
        LogE("ERROR: Please set communication objects first!", true);        
        return nullptr;
    }
    
    String sn = HAPDeviceID::serialNumber("knx-3", String(_id));

    // Create accessory if not already created
    _accessory = new HAPAccessory();
    //HAPAccessory::addInfoServiceToAccessory(_accessory, "Builtin LED", "ACME", "LED", "123123123", &identify);
    auto callbackIdentify = std::bind(&HAPPluginKNXDevice::identify, this, std::placeholders::_1, std::placeholders::_2);
    _accessory->addInfoService(String(_name), "KNX", "KNX Switch", sn, callbackIdentify, HAP_KNX_SWITCH_VERSION);

    
    //
	// Switch
	//
    if ( (_koReadState > 0) && (_koWriteState > 0) ) {

        // 
        // Outlet Service / Switch Service
        // !!! Fakegato history is only working with switch service !!!
        // 
        HAPService* switchService = new HAPService(HAP_SERVICE_SWITCH);
        _accessory->addService(switchService);

        HAPCharacteristicT<String> *plugServiceName = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, HAP_STRING_LENGTH_MAX);
        plugServiceName->setValue(_name);
        _accessory->addCharacteristicToService(switchService, plugServiceName);

        //
        // Power State 
        // 
        _stateValue = new HAPCharacteristicT<bool>(HAP_CHARACTERISTIC_ON, HAP_PERMISSION_READ|HAP_PERMISSION_WRITE|HAP_PERMISSION_NOTIFY);            
        _stateValue->setValue(false);

        // 
        // callbacks        
        _stateValue->setValueChangeCallback(std::bind(&HAPPluginKNXDeviceSwitch::changedState, this, std::placeholders::_1, std::placeholders::_2));
        _stateValue->setValueGetCallback(std::bind(&HAPPluginKNXDeviceSwitch::readStateFromKNX, this));

        // Read value from knx
        _stateValue->value(true);

        _accessory->addCharacteristicToService(switchService, _stateValue);
  

        knx.getGroupObject(_koWriteState).dataPointType(DPT_Switch);
        knx.getGroupObject(_koReadState).dataPointType(DPT_Switch);

        knx.getGroupObject(_koWriteState).callback(std::bind(&HAPPluginKNXDeviceSwitch::setStateFromKNXCallback, this, std::placeholders::_1));        
    



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
        if (_enableFakegato){

            if (_fakegato == nullptr) {
                _fakegato = new HAPFakegato();
            }

            _fakegato->enablePeriodicUpdates(false);
            _fakegato->addCharacteristic(new HAPFakegatoCharacteristicSwitch(std::bind(&HAPPluginKNXDeviceSwitch::getStateValue, this)));        

            _fakegato->registerFakeGatoService(_accessory, _name);            
            
            auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceSwitch::fakeGatoCallback, this);
            _fakegatoFactory->registerFakeGato(_fakegato, callbackAddEntry);
        }

    }
    return _accessory;
}


bool HAPPluginKNXDeviceSwitch::getStateValue(){
    return _stateValue->value();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceSwitch::setEventManager(EventManager* eventManager){
      
    _eventManager = eventManager;
    // Serial.printf("w event: %p\n", _eventManager);  
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPPluginKNXDeviceSwitch::setFakeGatoFactory(HAPFakegatoFactory* fakegatoFactory){
    
    _fakegatoFactory = fakegatoFactory;
    // Serial.printf("w fakegato: %p\n", _fakegatoFactory);
}   



void HAPPluginKNXDeviceSwitch::changedState(bool oldValue, bool newValue){
    Serial.printf("[KNX:%X] New state: %d\n", _id, newValue);
    writeStateToKNX(newValue);
}


bool HAPPluginKNXDeviceSwitch::readStateFromKNX(){
    Serial.printf("[KNX:%X] Read state\n", _id);
    
    bool result = knx.getGroupObject(_koReadState).value();    
    return result;
}


// void HAPPluginKNXDeviceSwitch::changeLastUpdate(String oldValue, String newValue){
//     Serial.printf("[KNX:%X] New LastUpdate: %s\n", _id, newValue.c_str());
// }


bool HAPPluginKNXDeviceSwitch::fakeGatoCallback(){	
    _fakegato->addEntry(0x01);   
    return true;   
}

void HAPPluginKNXDeviceSwitch::handle(bool forced){

}

void HAPPluginKNXDeviceSwitch::setStateFromKNXCallback(GroupObject& go){
    bool result = go.value();

    // Serial.println("Temperature: " + String(result));
    
    Serial.printf("[KNX:%X] New state: %d\n", _id, result);

    _stateValue->setValue(result, false);
    queueNotifyEvent(_stateValue);

    if (_enableFakegato){
        //  Add entry 
        _fakegato->addEntry(0x01);    
    }  

    
}

void HAPPluginKNXDeviceSwitch::writeStateToKNX(bool newValue){
    knx.getGroupObject(_koReadState).value(newValue);
}

