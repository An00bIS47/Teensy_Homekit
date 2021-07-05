//
// HAPPlugins.cpp
// Homekit
//
//  Created on: 18.08.2017
//      Author: michael
//

#include "HAPPlugins.hpp"

HAPPluginFactory& HAPPluginFactory::Instance() {
	static HAPPluginFactory instance;
	return instance;
}

void HAPPluginFactory::registerPlugin(IPluginRegistrar* registrar, std::string name) {
	_registry[name] = registrar;
}

std::unique_ptr<HAPPlugin> HAPPluginFactory::getPlugin(std::string name) {
	/* throws out_of_range if plugin unknown */
	IPluginRegistrar* registrar = _registry.at(name);
	return registrar->getPlugin();
}

std::vector<std::string> HAPPluginFactory::names(){
	std::vector<std::string> v;
	for(std::map<std::string, IPluginRegistrar*>::iterator it = _registry.begin(); it != _registry.end(); ++it) {
		v.push_back(it->first);
	}
	return v;
}

