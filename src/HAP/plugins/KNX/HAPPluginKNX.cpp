//
// HAPPluginKNX.cpp
// Homekit
//
//  Created on: 20.12.2019
//      Author: michael
//

#include "HAPPluginKNX.hpp"
#include "HAPServer.hpp"
#include "HAPWebServerBodyParserMultipart.hpp"
#include "HAPWebServerBodyParserURLEncoded.hpp"


#if !HAP_WEBSERVER_USE_SPIFFS  
#include "HAPWebServerFiles.hpp"
#endif

#include <knx.h>
#include <EEPROM.h>         // can be removed once knx loads porperly...

#define VERSION_MAJOR       0
#define VERSION_MINOR       6
#define VERSION_REVISION    0
#define VERSION_BUILD       0


#ifndef HAP_PLUGIN_KNX_INTERVAL
#define HAP_PLUGIN_KNX_INTERVAL 1
#endif

#include "HAPPluginKNXMemory.hpp"


HAPPluginKNX::HAPPluginKNX(){
    _type           = HAP_PLUGIN_TYPE_ACCESSORY;
	_previousMillis = 0;

    _version.major      = VERSION_MAJOR;
    _version.minor      = VERSION_MINOR;
    _version.revision   = VERSION_REVISION;
    _version.build      = VERSION_BUILD;    

    _config = new HAPConfigurationPlugin("KNX", true, HAP_PLUGIN_KNX_INTERVAL);
    _config->setToJsonCallback(std::bind(&HAPPluginKNX::internalConfigToJson, this, std::placeholders::_1));
}


bool HAPPluginKNX::begin(){


#if HAP_SERVER_USE_INTEGRATED_KNX == 0
    // Transmitter is connected to Arduino Pin #10  
    ArduinoPlatform::SerialDebug = &Serial;
	randomSeed(millis());


    // read adress table, association table, groupobject table and parameters from eeprom
    knx.readMemory();

  
    Serial.print(">>>>>>>>> knx.configured(): ");
    Serial.println(knx.configured());

    Serial.printf("Group Obj Table: LoadState: %d\n", knx.bau().groupObjectTable().loadState());
    Serial.printf("App Program: LoadState: %d\n", knx.bau().parameters().loadState());
    // Serial.printf("Device Object: LoadState: %d\n", knx.bau().deviceObject().loadState());
    // Serial.printf("idx 1: LoadState: %d\n", knx.bau().getInterfaceObject(1)->loadState());
    // Serial.printf("idx 2: LoadState: %d\n", knx.bau().getInterfaceObject(2)->loadState());

    Serial.print("_addrTable: ");
    Serial.println(knx.bau()._addrTable.loadState());

    Serial.print("_assocTable: ");
    Serial.println(knx.bau()._assocTable.loadState());

    Serial.print("_groupObjTable: ");
    Serial.println(knx.bau()._groupObjTable.loadState());

    Serial.print("_appProgram: ");
    Serial.println(knx.bau()._appProgram.loadState());

    // int offset = 0;
    // for (int i = 0; i< KNX_FLASH_SIZE; i++) {
        
    //     uint8_t data[1];
    //     data[0] = EEPROM.read(offset++);
        
    //     if (offset % 8 == 0){
    //         printHex("", data,1, true);
    //     } else {
    //         printHex("", data,1, false);
    //     }        
    // }

    // is the led active on HIGH or low? Default is LOW
    knx.ledPinActiveOn(HIGH);    
	knx.buttonPin(6);
    
#endif

    return true;
}


HAPAccessory* HAPPluginKNX::initAccessory() {
    LogV("\nInitializing accessory for plugin: " + String(_config->name) + " ...", true);
    
    // print values of parameters if device is already configured
    if (knx.configured()){		

#if HAP_DEBUG
        printHex("configuration ", knx.paramData(0), ((ETS_HK_CHANNEL_PARAMETER_SIZE * HAP_PLUGIN_KNX_MAX_DEVICES) + 1 ));
#endif   
        uint8_t idCounter = 1;

        // Serial.print("StartupDelay: ");
        // Serial.println(knx.paramByte(StartupDelay));

        // Serial.print("StartupDelaySelection: ");
        // Serial.println(knx.paramByte(StartupDelaySelection));

        // Serial.print("Heartbeat: ");
        // Serial.println(knx.paramByte(Heartbeat));

        for (uint8_t i = 0; i < HAP_PLUGIN_KNX_MAX_DEVICES; i++) {

            uint32_t offset = (i * ETS_HK_CHANNEL_PARAMETER_SIZE);
            uint32_t koOffset = (i * ETS_HK_CHANNEL_COMOBJECT_SIZE);

            Serial.print("========================= ");
            Serial.print("Sensor ");
            Serial.print(i + 1);
            Serial.println(" =========================");
            Serial.print(">>> offset: ");
            Serial.println(offset);
            Serial.print(">>> koOffset: ");
            Serial.println(koOffset);

            // Channel specific values 
            Serial.print("Service Type: ");
            enum HAPPluginKNXServiceType serviceType = (HAPPluginKNXServiceType)knx.paramByte( (offset + ETS_HK_OFFSET_SERVICE_TYPE) );            
            Serial.print(HAPPluginKNXDevice::serviceEnumToString(serviceType));
            Serial.println(" RAW: " + String(knx.paramByte( (offset + ETS_HK_OFFSET_SERVICE_TYPE))));

            Serial.print("Service Name: ");
            char serviceName[41];
            memcpy(serviceName, knx.paramData( (offset + ETS_HK_OFFSET_SERVICE_NAME) ) , 40);
            serviceName[40] = '\0';
            Serial.println(serviceName);
            

            uint8_t availableFeatures = knx.paramByte( (offset + ETS_HK_OFFSET_ENABLE_FAKEGATO) );
            bool enableFakegato = (availableFeatures & 0x80);
            bool enableSchedule = (availableFeatures & 0x40);

            Serial.println("Features: ");
            Serial.println("    Fakegato: " + String(enableFakegato));
            Serial.println("    Schedule: " + String(enableSchedule)); 


            // Type specific values 
            if (serviceType == HAPPluginKNXServiceTypeWeather) {
                Serial.println("Available Sensors: ");        

                uint8_t availableSensors = knx.paramByte((offset + ETS_HK_OFFSET_HAS_TEMPERATURE_SENSOR));
                bool hasTemperatureSensor   = (availableSensors & 0x80);
                bool hasHumiditySensor      = (availableSensors & 0x40);
                bool hasAirPressureSensor   = (availableSensors & 0x20);

                Serial.println("    hasTemperatureSensor: " + String(hasTemperatureSensor));
                Serial.println("    hasHumidtySensor:     " + String(hasHumiditySensor));
                Serial.println("    hasAirPressureSensor:    " + String(hasAirPressureSensor));

                Serial.println("DTP: ");
                Serial.print("    Humidity: ");
                Serial.println(knx.paramByte((offset + ETS_HK_OFFSET_HUMIDITY_DPT_TYPE)));     // 9 oder 5

                Serial.println("#### ComObject: Temperature:  " + String((koOffset + ETS_HK_KO_OFFSET_REMOTE_TEMPERATURE)));
                Serial.println("#### ComObject: Humidity:     " + String((koOffset + ETS_HK_KO_OFFSET_REMOTE_HUMIDITY)));
                Serial.println("#### ComObject: Air Pressure: " + String((koOffset + ETS_HK_KO_OFFSET_REMOTE_AIRPRESSURE)));

        
                HAPPluginKNXDeviceWeather* newDevice = new HAPPluginKNXDeviceWeather(
                    idCounter++, 
                    serviceName, 
                    enableFakegato,
                    hasTemperatureSensor == 1 ? (koOffset + ETS_HK_KO_OFFSET_REMOTE_TEMPERATURE) : 0, 
                    hasHumiditySensor  == 1 ? (koOffset + ETS_HK_KO_OFFSET_REMOTE_HUMIDITY) : 0, 
                    hasAirPressureSensor  == 1 ? (koOffset + ETS_HK_KO_OFFSET_REMOTE_AIRPRESSURE) : 0,
                    (uint8_t)knx.paramByte((offset + ETS_HK_OFFSET_HUMIDITY_DPT_TYPE)) == 9 ? true : false
                );
                _devices.push_back(newDevice);
            } else if (serviceType == HAPPluginKNXServiceTypeOutlet) {


                Serial.println("#### ComObject: Switch Read:  " + String((koOffset + ETS_HK_KO_OFFSET_SWITCH_READ)));
                Serial.println("#### ComObject: Switch Write: " + String((koOffset + ETS_HK_KO_OFFSET_SWITCH_WRITE)));

                Serial.println("#### ComObject: Current:  " + String((koOffset + ETS_HK_KO_OFFSET_OUTLET_CURRENT)));
                Serial.println("#### ComObject: actEnergy: " + String((koOffset + ETS_HK_KO_OFFSET_OUTLET_ACT_ENERGY)));


                HAPPluginKNXDeviceOutlet* newDevice = new HAPPluginKNXDeviceOutlet(
                    idCounter++, 
                    serviceName, 
                    enableFakegato,
                    enableSchedule,
                    (koOffset + ETS_HK_KO_OFFSET_SWITCH_READ), 
                    (koOffset + ETS_HK_KO_OFFSET_SWITCH_WRITE), 
                    (koOffset + ETS_HK_KO_OFFSET_OUTLET_CURRENT),
                    (koOffset + ETS_HK_KO_OFFSET_OUTLET_ACT_ENERGY)
                );
                _devices.push_back(newDevice);
            } else if (serviceType == HAPPluginKNXServiceTypeSwitch) {  

                Serial.println("#### ComObject: Switch Read:  " + String((koOffset + ETS_HK_KO_OFFSET_SWITCH_READ)));
                Serial.println("#### ComObject: Switch Write: " + String((koOffset + ETS_HK_KO_OFFSET_SWITCH_WRITE)));

                HAPPluginKNXDeviceSwitch* newDevice = new HAPPluginKNXDeviceSwitch(
                    idCounter++, 
                    serviceName, 
                    enableFakegato,
                    (koOffset + ETS_HK_KO_OFFSET_SWITCH_READ), 
                    (koOffset + ETS_HK_KO_OFFSET_SWITCH_WRITE)
                );
                _devices.push_back(newDevice);

            } else  {
                LogE("NOT YET SUPPORTED!!!", true);
            }
        }            
    } else {
		LogE("ERROR: KNX is not configured! Use ETS to parameterize it!", true);
	}	


#if HAP_SERVER_USE_INTEGRATED_KNX == 0
#if HAP_PLUGIN_KNX_USE_TASK == 0
    // start the framework.
    knx.start();
#else
	xTaskCreatePinnedToCore(
					HAPPluginKNX::taskKNX,   /* Function to implement the task */
                    "taskKNX", /* Name of the task */
                    8192,      	/* Stack size in words */
                    NULL,       /* Task input parameter */
                    1,          /* Priority of the task */
                    NULL,       /* Task handle. */
                    !ARDUINO_RUNNING_CORE);  		/* Core where the task should run */


#endif
#endif

    for (auto& dev : _devices){           
        dev->setFakeGatoFactory(_fakeGatoFactory);
        dev->setEventManager(_eventManager);
        
        // auto callbackSend = std::bind(&HAPPluginKNX::sendDeviceCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);        
        // dev->setRCSwitchSendCallback(callbackSend);

        _accessorySet->addAccessory(dev->initAccessory());        
    }



    return nullptr;
}



void HAPPluginKNX::handleImpl(bool forced){


#if HAP_SERVER_USE_INTEGRATED_KNX == 0
#if HAP_PLUGIN_KNX_USE_TASK == 0
    // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS    
    knx.loop();

    // only run the application code if the device was configured with ETS
    if (!knx.configured())
        return;
#endif
#endif    

    for (auto& dev : _devices){           
        dev->handle(forced);
    }
}	

HAPConfigurationValidationResult HAPPluginKNX::validateConfig(JsonObject object){

    /*
        {
            "enabled": true,
            "interval": 1000,
            "devices": [
                {
                    "houseAddress": 17,
                    "deviceAddress": 16,
                    "name": "test1"
                }
            ]
        }
     */


    HAPConfigurationValidationResult result;

    result = HAPPlugin::validateConfig(object);
    if (result.valid == false) {
        return result;
    }

    result.valid = false;

       // plugin._name.devices
    if (object.containsKey("devices") && !object["devices"].is<JsonArray>()) {
        result.reason = "plugins." + String(_config->name) + ".devices is not an array";
        return result;
    }

    // plugin._name.devices array
    uint8_t count = 0;
    for( const auto& value : object["devices"].as<JsonArray>() ) {
        
        // plugin._name.devices.count.house
        if (!value.containsKey("houseAddress") ) {
            result.reason = "plugins." + String(_config->name) + ".devices." + String(count) + ".houseAddress is required";
            return result;
        }
        if (value.containsKey("houseAddress") && !value["houseAddress"].is<uint8_t>()) {
            result.reason = "plugins." + String(_config->name) + ".devices." + String(count) + ".houseAddress is not an integer";
            return result;
        }

        // plugin._name.devices.count.device
        if (!value.containsKey("deviceAddress") ) {
            result.reason = "plugins." + String(_config->name) + ".devices." + String(count) + ".deviceAddress is required";
            return result;
        }
        if (value.containsKey("deviceAddress") && !value["deviceAddress"].is<uint8_t>()) {
            result.reason = "plugins." + String(_config->name) + ".devices." + String(count) + ".deviceAddress is not an integer";
            return result;
        }    

        // optional
        // plugin._name.devices.count.name
        if (value.containsKey("name") && !value["name"].is<const char*>()) {
            result.reason = "plugins." + String(_config->name) + ".devices." + String(count) + ".name is not a string";
            return result;
        }   

        // plugin._name.devices.count.name - length
        if (value.containsKey("name")) {
            if (strlen(value["name"]) + 1 > HAP_STRING_LENGTH_MAX) {
                result.reason = "plugins." + String(_config->name) + ".devices." + String(count) + ".name is too long";
                return result;
            } 
        }   

        // plugin._name.devices.count.timer
        if (value.containsKey("timer") && !value["timer"].is<JsonObject>() && !value["timer"].isNull()) {
            result.reason = "plugins." + String(_config->name) + ".devices.timer is not an object";
            return result;
        }        

        if (value["timer"].containsKey("enabled") && !value["timer"]["enabled"].is<bool>()) {
            result.reason = "plugins." + String(_config->name) + ".devices.timer.enabled is not a bool";
            return result;
        }

        if (value["timer"].containsKey("days") && !value["timer"]["days"].is<uint32_t>()) {
            result.reason = "plugins." + String(_config->name) + ".devices.timer.enabled is not an uint32_t";
            return result;
        }        

        if (value["timer"].containsKey("programs") && !value["timer"]["programs"].is<String>()) {
            result.reason = "plugins." + String(_config->name) + ".devices.timer.enabled is not a string";
            return result;
        }

        // const char* data = value["timer"]["programs"].as<const char*>();
        // if ( (data[0] != '0') && (data[1] != '5') && (data[4] != '0') && (data[5] != '0')) {
        //     result.reason = "plugins." + _config->name + ".devices.timer.programs is not valid program";
        //     return result;
        // }

        // if ( (data[2] == '0') && (data[3] == '1') ) {
        //     result.reason = "plugins." + _config->name + ".devices.timer.programs - program count is 0";
        //     return result;
        // }

        count++;
    }

    result.valid = true;
    return result;
}



int HAPPluginKNX::indexOfDevice(HAPPluginKNXDevice* device){
    // Check if element 22 exists in vector
	std::vector<HAPPluginKNXDevice*>::iterator it = std::find(_devices.begin(), _devices.end(), device);
 
	if (it != _devices.end())
	{		
		// Get index of element from iterator
		return std::distance(_devices.begin(), it);		
	} else {
        return -1;
    }
}



#if HAP_ENABLE_WEBSERVER
void HAPPluginKNX::handleHTTPGet(HTTPRequest * req, HTTPResponse * res){        
//     // template processing    
//     auto callback = std::bind(&HAPPluginKNX::handleHTTPGetKeyProcessor, this, std::placeholders::_1, std::placeholders::_2);
    
    
// #if HAP_WEBSERVER_USE_SPIFFS        
//     HAPWebServerTemplateProcessor::processAndSend(res, "/index.html", callback);
// #else
//     HAPWebServerTemplateProcessor::processAndSendEmbedded(res, html_template_index_start, html_template_index_end, callback);
// #endif  

}


void HAPPluginKNX::handleHTTPGetKeyProcessor(const String& key, HTTPResponse * res){
    // if (key == "VAR_TITLE") {
    //     res->print("Plugins - " + _config->name);
    // } else if (key == "VAR_NAVIGATION") {
    //     res->print(HAPWebServer::buildNavigation());
    // } else if (key == "VAR_CONTENT") {        
    //     res->print("<div class=\"pure-u-1 pure-u-md-1\"><p>Paired Devices:</p>");
    //     res->print("<table class=\"pure-table pure-table-horizontal\">");
    //     res->print("<thead> <tr> <th>Name</th> <th>House Code</th> <th>Device Code</th> </tr> </thead>");
    //     res->print("<tbody>");
    //     res->print("");
    //     for (auto& dev : _devices){ 

    //         char houseCode[6];
    //         char deviceCode[6];

    //         prependZeros(houseCode, String(dev->houseAddress, BIN), 5);
    //         prependZeros(deviceCode, String(dev->deviceAddress, BIN), 5);

    //         res->printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", dev->name.c_str(),houseCode ,deviceCode);
    //     }
    //     res->print("</tbody>");
    //     res->print("</table>");
    //     res->print("</div>");
        
    //     res->print("<br>");
    //     res->print("<br>");


    //     // Add new device        
    //     res->print("<div class=\"pure-u-1 pure-u-md-1\">");
    //     res->print("<form class=\"pure-form\" action=\"/plugin/rcswitch\" method=\"POST\">");
    //     res->print(" <fieldset>");
    //     res->print("<legend>Add another outlet</legend>");
    //     res->print("<input type=\"text\" id=\"name\" placeholder=\"Name\">");
    //     res->print("<input type=\"text\" id=\"houseAddress\" placeholder=\"System Address\">");
    //     res->print("<input type=\"text\" id=\"deviceAddress\" placeholder=\"Device Address\">");
    //     res->print("<button type=\"submit\" class=\"pure-button pure-button-primary\">Add</button>");
    //     res->print("</fieldset>");
    //     res->print("</form>");
    //     res->print("</div>");

    // } else {
    //     res->print("");
    // }
}
#endif



#if HAP_ENABLE_WEBSERVER
void HAPPluginKNX::handleHTTPPost(HTTPRequest * req, HTTPResponse * res){    


//     if (req->getHeader("Content-Type") == "application/x-www-form-urlencoded") {
//         HAPWebServerBodyParserURLEncoded urlParser;
//         std::vector<std::pair<std::string, std::string>> parameters = urlParser.processAndParse(req);

//         if (_newDevice == nullptr) {
//             _newDevice = new HAPPluginKNXDevice();
//         }
        
//         HAPConfigurationValidationResult result;
//         result.valid = true;
        
//         for (auto param : parameters) {

//             if (param.first == "name") {
//                 result = validateName(param.second.c_str());

//                 if (result.valid){
//                     _newDevice->name = param.second.c_str();
//                 } else {
//                     break;
//                 }
                
//             } else if (param.first == "deviceAddress") {
//                 result = validateDeviceAddress(param.second.c_str());

//                 if (result.valid){
//                     _newDevice->deviceAddress = strtol(param.second.c_str(), (char**) NULL, 2);
//                 } else {
//                     break;
//                 }              
                
//             } else if (param.first == "houseAddress") {
//                 result = validateHouseAddress(param.second.c_str());
                
//                 if (result.valid){
//                     _newDevice->houseAddress = strtol(param.second.c_str(), (char**) NULL, 2);
//                 } else {
//                     break;
//                 }
//             }
//         }

//         if (result.valid) {

//             std::vector<HAPPluginKNXDevice*>::iterator it = find(_devices.begin(), _devices.end(), _newDevice);
//             if(it == _devices.end()) {
//                 LogD("Adding new KNX device with name: " + _newDevice->name + " hA: " + String(_newDevice->houseAddress) + " dA: " + String(_newDevice->deviceAddress), true);

//                 _newDevice->setFakeGatoFactory(_fakeGatoFactory);
//                 _newDevice->setEventManager(_eventManager);
                
//                 // auto callbackSend = std::bind(&HAPPluginKNX::sendDeviceCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);        
//                 // _newDevice->setRCSwitchSendCallback(callbackSend);

//                 _accessorySet->addAccessory(_newDevice->initAccessory());
                
//                 _devices.push_back(_newDevice);

//                 struct HAPEvent event = HAPEvent(nullptr, 0, 0, "");							
//                 _eventManager->queueEvent( EventManager::kEventUpdatedConfig, event, EventManager::kLowPriority);        
//             }

//         } else {
//             LogE("Could not add new KNX device. Reason: " + result.reason, true);
//         }

//         _newDevice = nullptr;
//     } else {
//         auto callbackFormField = std::bind(&HAPPluginKNX::handleHTTPFormField, this, std::placeholders::_1, std::placeholders::_2);
//         HAPWebServerBodyParserMultipart parser;
//         parser.processAndParse(req, callbackFormField);
        
//         if (_newDevice != nullptr) {                    

//             std::vector<HAPPluginKNXDevice*>::iterator it = find(_devices.begin(), _devices.end(), _newDevice);
//             if(it == _devices.end()) {

//                 LogD("Adding new KNX device with name: " + _newDevice->name + " hA: " + String(_newDevice->houseAddress) + " dA: " + String(_newDevice->deviceAddress), true);

//                 _newDevice->setFakeGatoFactory(_fakeGatoFactory);
//                 _newDevice->setEventManager(_eventManager);
                
//                 // auto callbackSend = std::bind(&HAPPluginKNX::sendDeviceCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);        
//                 // _newDevice->setRCSwitchSendCallback(callbackSend);

//                 _accessorySet->addAccessory(_newDevice->initAccessory());
                
//                 _devices.push_back(_newDevice);

//             }
    
//             // _newDevice = nullptr;
//             delete _newDevice;

//             struct HAPEvent event = HAPEvent(nullptr, 0, 0, "");							
//             _eventManager->queueEvent( EventManager::kEventUpdatedConfig, event, EventManager::kLowPriority);                    
//         }
//     }

//     auto callbackGet = std::bind(&HAPPluginKNX::handleHTTPGetKeyProcessor, this, std::placeholders::_1, std::placeholders::_2);
// #if HAP_WEBSERVER_USE_SPIFFS        
//     // template processing    
//     HAPWebServerTemplateProcessor::processAndSend(res, "/index.html", callbackGet);
// #else
    
//     HAPWebServerTemplateProcessor::processAndSendEmbedded(res, html_template_index_start, html_template_index_end, callbackGet);
// #endif 

    
}

void HAPPluginKNX::handleHTTPFormField(const std::string& fieldName, const std::string& fieldValue) { 

    // if (_newDevice == nullptr) {
    //     _newDevice = new HAPPluginKNXDevice();
    // }

    // HAPConfigurationValidationResult result;
    // result.valid = true;
    
    // if (fieldName == "name") {
    //     result = validateName(fieldValue.c_str());
    //     if (result.valid) {
    //         _newDevice->name = fieldValue.c_str();
    //     } else {
    //         _newDevice = nullptr;
    //         return;
    //     }            
    // } else if (fieldName == "houseAddress") {  

    //     result = validateName(fieldValue.c_str());
    //     if (result.valid) {
    //         _newDevice->houseAddress = strtol(fieldValue.c_str(), (char**) NULL, 2);
    //     } else {
    //         _newDevice = nullptr;
    //         return;
    //     }
    // } else if (fieldName == "deviceAddress") {
    //     result = validateName(fieldValue.c_str());
    //     if (result.valid) {
    //         _newDevice->deviceAddress = strtol(fieldValue.c_str(), (char**) NULL, 2);        
    //     } else {
    //         _newDevice = nullptr;
    //         return;
    //     }
    // }

    // std::vector<HAPPluginKNXDevice*>::iterator it = find(_devices.begin(), _devices.end(), _newDevice);
    // if(it == _devices.end()) {

    //     LogD("Adding new KNX device with name: " + _newDevice->name + " hA: " + String(_newDevice->houseAddress) + " dA: " + String(_newDevice->deviceAddress), true);

    //     _newDevice->setFakeGatoFactory(_fakeGatoFactory);
    //     _newDevice->setEventManager(_eventManager);
        
    //     // auto callbackSend = std::bind(&HAPPluginKNX::sendDeviceCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);        
    //     // _newDevice->setRCSwitchSendCallback(callbackSend);

    //     _accessorySet->addAccessory(_newDevice->initAccessory());
        
    //     _devices.push_back(_newDevice);

    // }

}


std::vector<HAPWebServerPluginNode*> HAPPluginKNX::getResourceNodes(){
    std::vector<HAPWebServerPluginNode*> vector;

    auto callbackGet = std::bind(&HAPPluginKNX::handleHTTPGet, this, std::placeholders::_1, std::placeholders::_2);
    HAPWebServerPluginNode* getRequest = new HAPWebServerPluginNode("KNX", "knx", "GET", callbackGet);    
    vector.push_back(getRequest);


    auto callbackPost = std::bind(&HAPPluginKNX::handleHTTPPost, this, std::placeholders::_1, std::placeholders::_2);
    HAPWebServerPluginNode* postRequest = new HAPWebServerPluginNode("KNX", "knx", "POST", callbackPost);    
    vector.push_back(postRequest);

    return vector;
}
#endif


#if HAP_PLUGIN_KNX_USE_TASK
void HAPPluginKNX::taskKNX( void * parameter )
{
    Serial.println(">>>>> Started task now");
	knx.start();    

    while( true ){
        knx.loop(); 
        //delay(HAP_PLUGIN_KNX_INTERVAL);   
        //vTaskDelay(1);    
    }
 
    // Serial.println("Ending task 1");
    // vTaskDelete( NULL );
}
#endif

HAPConfigurationPlugin* HAPPluginKNX::setDefaults(){
	_config->enabled  = HAP_PLUGIN_USE_KNX;
	_config->interval = HAP_PLUGIN_KNX_INTERVAL;	
	_config->dataPtr = nullptr;
	_config->dataSize = 0;
	return _config;
}

void HAPPluginKNX::internalConfigToJson(Print& prt){
	/*
		{ >>> is already printed before

            "enabled": 1,
            "interval": 1,            
			"devices": [
                {
                    "type": "weather",
                    "name": "abcd",
                    "enableFakegato": 1,
                    "hasTemperture": 1,
                    "hasHumidity": 1,
                    "hasAirPressure": 1,
                }, {
                    "type": "outlet",
                    "name": "abcd",
                    "enableFakegato": 1,
                    "enableSchedule": 1,
                }, 
                ...
            ]
		} >>> will be printed after
	
	*/

    if (!knx.configured()) return;

	prt.print("\"devices\": [");
	
    for (uint8_t i = 0; i < HAP_PLUGIN_KNX_MAX_DEVICES; i++) {

        uint32_t offset = (i * ETS_HK_CHANNEL_PARAMETER_SIZE);
          
        enum HAPPluginKNXServiceType serviceType = (HAPPluginKNXServiceType)knx.paramByte( (offset + ETS_HK_OFFSET_SERVICE_TYPE) );            
        prt.print("{");
        prt.print("\"type\":");
        prt.print(HAPHelper::wrap(HAPPluginKNXDevice::serviceEnumToString(serviceType)));

        prt.print(",");
        prt.print("\"name\":");
        char serviceName[41];
        memcpy(serviceName, knx.paramData( (offset + ETS_HK_OFFSET_SERVICE_NAME) ) , 40);
        serviceName[40] = '\0';
        prt.print(HAPHelper::wrap(serviceName));
        

        uint8_t availableFeatures = knx.paramByte( (offset + ETS_HK_OFFSET_ENABLE_FAKEGATO) );
        bool enableFakegato = (availableFeatures & 0x80);
        bool enableSchedule = (availableFeatures & 0x40);
        prt.print(",");
        prt.print("\"enableFakegato\":");
        prt.print(enableFakegato);
        prt.print(",");
        prt.print("\"enableSchedule\":");
        prt.print(enableSchedule);

        if (serviceType == HAPPluginKNXServiceTypeWeather){
            uint8_t availableSensors = knx.paramByte((offset + ETS_HK_OFFSET_HAS_TEMPERATURE_SENSOR));
            bool hasTemperatureSensor   = (availableSensors & 0x80);
            bool hasHumiditySensor      = (availableSensors & 0x40);
            bool hasAirPressureSensor   = (availableSensors & 0x20);

            prt.print(",");
            prt.print("\"hasTemperatureSensor\":");
            prt.print(hasTemperatureSensor);

            prt.print(",");
            prt.print("\"hasHumidtySensor\":");
            prt.print(hasHumiditySensor);

            prt.print(",");
            prt.print("\"hasAirPressureSensor\":");
            prt.print(hasAirPressureSensor);

            if (hasHumiditySensor){
                prt.print(",");
                prt.print("\"humidityDPT\":");
                prt.print(knx.paramByte((offset + ETS_HK_OFFSET_HUMIDITY_DPT_TYPE)));     // 9 / 5
            }
            
        }

        prt.print("}");

        if (i + 1 < HAP_PLUGIN_KNX_MAX_DEVICES){
            prt.print(",");
        }
    }
    prt.print("]");
}

void HAPPluginKNX::setConfiguration(HAPConfigurationPlugin* cfg){
	_config = cfg;
	_config->setToJsonCallback(std::bind(&HAPPluginKNX::internalConfigToJson, this, std::placeholders::_1));

	// Serial.print("BME280 interval:");
	// Serial.println(_config->interval);
	// Serial.print("BME280 dataSize:");
	// Serial.println(_config->dataSize);	
	// Serial.print("BME280 mode:");
	// Serial.println(_configInternal->mode);	
}