//
// HAPPluginKNX.hpp
// Homekit
//
//  Created on: 20.12.2019
//      Author: michael
//
//

#ifndef HAPPLUGINKNX_HPP_
#define HAPPLUGINKNX_HPP_

#include <Arduino.h>
#include <vector>
#include <algorithm>

#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"
#include "HAPFakegato.hpp"

#include <knx.h>
#include "HAPPluginKNXDevice.hpp"
#include "HAPPluginKNXDeviceWeather.hpp"
#include "HAPPluginKNXDeviceSwitch.hpp"
#include "HAPPluginKNXDeviceOutlet.hpp"


#if defined(ARDUINO_ARCH_ESP32)
  #ifndef HAP_PLUGIN_KNX_USE_TASK
    #define HAP_PLUGIN_KNX_USE_TASK 1
  #endif
#else
  #define HAP_PLUGIN_KNX_USE_TASK 0
#endif

class HAPPluginKNX: public HAPPlugin {
public:

	HAPPluginKNX();
    ~HAPPluginKNX();

	bool begin();

	HAPAccessory* initAccessory() override;

	// void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);


#if HAP_ENABLE_WEBSERVER
	HAPConfigurationValidationResult validateConfig(JsonObject object);
#endif

	HAPConfigurationPlugin* setDefaults();
	void internalConfigToJson(Print& prt);
	void setConfiguration(HAPConfigurationPlugin* cfg) override;

	// void remoteTempCallback(GroupObject& go);

#if HAP_ENABLE_WEBSERVER
	std::vector<HAPWebServerPluginNode*> getResourceNodes();

	void handleHTTPGet(HTTPRequest * req, HTTPResponse * res);
	void handleHTTPGetKeyProcessor(const String& key, HTTPResponse * res);

	void handleHTTPPost(HTTPRequest * req, HTTPResponse * res);
	void handleHTTPFormField(const std::string& fieldName, const std::string& fieldValue);
#endif

#if HAP_PLUGIN_KNX_USE_TASK
	static void taskKNX( void * parameter );
#endif

private:

#if SMALL_GROUPOBJECT
	void callbackReceived(GroupObject& go);
#endif

	int indexOfDevice(HAPPluginKNXDevice* device);

	std::vector<HAPPluginKNXDevice*> _devices;

	bool fakeGatoCallback();
};

REGISTER_PLUGIN(HAPPluginKNX)

#endif /* HAPPLUGINKNX_HPP_ */