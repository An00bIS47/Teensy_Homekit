// 
// HAPFakeGatoFactory.cpp
// Homekit
//
//  Created on: 29.08.2019
//      Author: michael
//

#include "HAPFakeGatoFactory.hpp"

#include "HAPServer.hpp"
#include "HAPLogger.hpp"


void HAPFakeGatoFactory::handle(bool forced){
    for (auto & gato : _fakegatos) {
        if (forced) {
            gato->handle();
        } else {
            if (gato->isEnabled()) {		
                gato->handle();
            }
        }
			        
	} 
}

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM 
// #endif
// void HAPFakeGatoFactory::registerFakeGato(HAPFakeGato* fakegato, String name, std::function<bool()> callback, uint32_t interval){

//     fakegato->begin();
//     fakegato->setName(name);
//     fakegato->setRefTime(_refTime);    
//     fakegato->setInterval(interval);
//     fakegato->registerCallback(callback);
//     _fakegatos.push_back(fakegato);
//     LogD(HAPTime::timeString() + " " + "HAPFakeGatoFactory" + "->" + String(__FUNCTION__) + " [   ] " + "Registered fakegato for: " + name + " (" + String(_fakegatos.size()) + ")", true);      
// }

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPFakeGatoFactory::registerFakeGato(HAPFakeGato2* fakegato, std::function<bool()> callback, uint32_t interval){
    fakegato->begin();     
    fakegato->setInterval(interval);
    fakegato->registerCallbackAddEntry(callback);
    _fakegatos.push_back(fakegato);
    LogD(HAPTime::timeString() + " " + "HAPFakeGatoFactory" + "->" + String(__FUNCTION__) + " [   ] " + "Registered fakegato for: " + fakegato->name() + " (" + String(_fakegatos.size()) + ")", true);      
}