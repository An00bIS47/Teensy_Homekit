// 
// HAPFakegatoFactory.hpp
// Homekit
//
//  Created on: 29.08.2019
//      Author: michael
//
#ifndef HAPFakegatoFactory_HPP_
#define HAPFakegatoFactory_HPP_

#include <Arduino.h>
#include <vector>
#include <memory>
#include "HAPFakegato.hpp"

class HAPFakegatoFactory {
public:
    HAPFakegatoFactory() {}

    void handle(bool forced = false);
    void registerFakeGato(HAPFakegato* fakegato, std::function<bool()> callback, uint32_t interval = HAP_FAKEGATO_INTERVAL);

private:    
    std::vector<HAPFakegato*> _fakegatos;    
};

#endif /* HAPFakegatoFactory_HPP_ */