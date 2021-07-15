//
// HAPFakegatoFactory.cpp
// Homekit
//
//  Created on: 29.08.2019
//      Author: michael
//

#include "HAPFakegatoFactory.hpp"
#include "HAPLogging.hpp"

void HAPFakegatoFactory::handle(bool forced){
    for (auto & gato : _fakegatos) {
        if (gato->isEnabled() || forced) {
            gato->handle();
        }
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoFactory::registerFakeGato(HAPFakegato* fakegato, std::function<bool()> callback, uint32_t interval){
    fakegato->setInterval(interval);
    fakegato->registerCallbackAddEntry(callback);
    _fakegatos.push_back(fakegato);
    LOG_I("Registered Fakegato for %s with size %d\n", fakegato->name(), _fakegatos.size());
}