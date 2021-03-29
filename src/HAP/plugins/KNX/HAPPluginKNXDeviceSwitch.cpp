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


    // HAPService* tmpService = nullptr;
    //
	// Temperature
	//
    if ( (_koReadState > 0) && (_koWriteState > 0) ) {

        // 
        // Outlet Service / Switch Service
        // !!! Fakegato history is only working with switch service !!!
        // 
        HAPService* switchService = new HAPService(HAP_SERVICE_SWITCH);
        _accessory->addService(switchService);

        HAPCharacteristicString *plugServiceName = new HAPCharacteristicString(HAP_CHARACTERISTIC_NAME, permission_read, HAP_STRING_LENGTH_MAX);
        plugServiceName->setValueString(_name);
        _accessory->addCharacteristicToService(switchService, plugServiceName);

        //
        // Power State 
        // 
        _stateValue = new HAPCharacteristicBool(HAP_CHARACTERISTIC_ON, permission_read|permission_write|permission_notify);            
        _stateValue->setValueString("0");

        // 
        // Value Changed Function Call (callback when value changed from KNX)
        // 
        auto callbackState = std::bind(&HAPPluginKNXDeviceSwitch::changedState, this, std::placeholders::_1, std::placeholders::_2);        
        _stateValue->valueChangeFunctionCall = callbackState;


        // Read value from knx
        _stateValue->valueString();

        // 
        // Value GET Function Call (callback to read state from KNX)
        // 
        auto callbackReadState = std::bind(&HAPPluginKNXDeviceSwitch::readState, this);        
        _stateValue->valueGetFunctionCall = callbackReadState;


        _accessory->addCharacteristicToService(switchService, _stateValue);
  



        // if (tmpService == nullptr) tmpService = temperatureService;
        knx.getGroupObject(_koWriteState).dataPointType(DPT_Switch);
        knx.getGroupObject(_koReadState).dataPointType(DPT_Switch);

        knx.getGroupObject(_koWriteState).callback(std::bind(&HAPPluginKNXDeviceSwitch::writeStateCallback, this, std::placeholders::_1));        
    



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
            _fakegato = new HAPFakeGatoSwitch();
            _fakegato->registerFakeGatoService(_accessory, _name);    
            auto callbackAddEntry = std::bind(&HAPPluginKNXDeviceSwitch::fakeGatoCallback, this);
            _fakegatoFactory->registerFakeGato(_fakegato,  String(_name), callbackAddEntry);
        }

    }
    return _accessory;
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
void HAPPluginKNXDeviceSwitch::setFakeGatoFactory(HAPFakeGatoFactory* fakegatoFactory){
    
    _fakegatoFactory = fakegatoFactory;
    // Serial.printf("w fakegato: %p\n", _fakegatoFactory);
}   



void HAPPluginKNXDeviceSwitch::changedState(bool oldValue, bool newValue){
    Serial.printf("[KNX:%X] New state: %d\n", _id, newValue);

    if (_shouldSend) {
         knx.getGroupObject(_koReadState).value(newValue);
    }
    
    _shouldSend = true;
}


void HAPPluginKNXDeviceSwitch::readState(){
    Serial.printf("[KNX:%X] Read state\n", _id);
    _stateValue->_value = knx.getGroupObject(_koReadState).value();
}


// void HAPPluginKNXDeviceSwitch::changeLastUpdate(String oldValue, String newValue){
//     Serial.printf("[KNX:%X] New LastUpdate: %s\n", _id, newValue.c_str());
// }


bool HAPPluginKNXDeviceSwitch::fakeGatoCallback(){	


    return _fakegato->addEntry(_stateValue->valueString());      
}

void HAPPluginKNXDeviceSwitch::handle(bool forced){

}

void HAPPluginKNXDeviceSwitch::writeStateCallback(GroupObject& go){
    bool result = go.value();


    _shouldSend = false;
    // Serial.println("Temperature: " + String(result));
    _stateValue->setValueString(String(result));

    // Add event
	struct HAPEvent event = HAPEvent(nullptr, _accessory->aid, _stateValue->iid, String(result));							
	_eventManager->queueEvent( EventManager::kEventNotifyController, event);

    if (_enableFakegato){
        //  Add entry 
        _fakegato->addEntry(_stateValue->valueString());    
    }  
}

