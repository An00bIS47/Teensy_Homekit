// 
// HAPFakeGatoFactory.hpp
// Homekit
//
//  Created on: 29.08.2019
//      Author: michael
//
#ifndef HAPFAKEGATOFACTORY_HPP_
#define HAPFAKEGATOFACTORY_HPP_

#include <Arduino.h>
#include <vector>
#include <memory>

#include "HAPFakeGato2.hpp"

class HAPFakeGatoFactory {
public:
    HAPFakeGatoFactory() {}

    void handle(bool forced = false);
    void registerFakeGato(HAPFakeGato2* fakegato, std::function<bool()> callback, uint32_t interval = HAP_FAKEGATO_INTERVAL);

private:    
    std::vector<HAPFakeGato2*> _fakegatos;    
};

#endif /* HAPFAKEGATOFACTORY_HPP_ */