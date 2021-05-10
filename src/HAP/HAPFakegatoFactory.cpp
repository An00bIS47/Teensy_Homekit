//
// HAPFakegatoFactory.cpp
// Homekit
//
//  Created on: 29.08.2019
//      Author: michael
//

#include "HAPFakegatoFactory.hpp"
#include "HAPLogger.hpp"
#include "HAPTime.hpp"

void HAPFakegatoFactory::handle(bool forced){
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

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoFactory::registerFakeGato(HAPFakegato* fakegato, std::function<bool()> callback, uint32_t interval){
    fakegato->begin();
    fakegato->setInterval(interval);
    fakegato->registerCallbackAddEntry(callback);
    _fakegatos.push_back(fakegato);
    LogD(HAPTime::timeString() + " " + "HAPFakegatoFactory" + "->" + String(__FUNCTION__) + " [   ] " + "Registered fakegato for: " + fakegato->name() + " (" + String(_fakegatos.size()) + ")", true);
}