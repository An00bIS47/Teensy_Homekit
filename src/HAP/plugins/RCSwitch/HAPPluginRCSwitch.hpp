//
// HAPPluginRCSwitch.hpp
// Homekit
//
//  Created on: 20.12.2019
//      Author: michael
//
//

#ifndef HAPPLUGINRCSWITCH_HPP_
#define HAPPLUGINRCSWITCH_HPP_

#include <Arduino.h>
#include <RCSwitch.h>
#include <vector>
#include <algorithm>

#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"
#include "HAPFakegato.hpp"


#include "HAPPluginRCSwitchDevice.hpp"


struct HAPPluginRCSwitchConfig {
	uint8_t* data = {0,};
	size_t dataLen = 0;
};


class HAPPluginRCSwitch: public HAPPlugin {
public:

	HAPPluginRCSwitch();
	~HAPPluginRCSwitch();
    // HAPPluginRCSwitch(String name_);

	bool begin();

	HAPAccessory* initAccessory() override;

	void setValue(int iid, String oldValue, String newValue);

	void identify(bool oldValue, bool newValue);
    void handleImpl(bool forced = false);

#if HAP_ENABLE_WEBSERVER
	HAPConfigurationValidationResult validateConfig(JsonObject object);
	JsonObject getConfigImpl();
	void setConfigImpl(JsonObject root);
#endif


	HAPConfigurationPlugin* setDefaults() override;
	void internalConfigToJson(Print& prt);
	void setConfiguration(HAPConfigurationPlugin* cfg) override;



    // void handleRoot(HTTPRequest * req, HTTPResponse * res);


#if HAP_ENABLE_WEBSERVER
	std::vector<HAPWebServerPluginNode*> getResourceNodes();

	void handleHTTPGet(HTTPRequest * req, HTTPResponse * res);
	void handleHTTPGetKeyProcessor(const String& key, HTTPResponse * res);

	void handleHTTPPost(HTTPRequest * req, HTTPResponse * res);
	void handleHTTPFormField(const std::string& fieldName, const std::string& fieldValue);
#endif


    void sendDeviceCallback(uint8_t houseAddress_, uint8_t deviceAddress_, bool on_);


    static void prependZeros(char *dest, String src, uint8_t width);
private:

	// boolCharacteristics*	_stateValue;

	int indexOfDevice(HAPPluginRCSwitchDevice* device);
    void configAccessory(uint8_t devPtr);
	std::vector<HAPPluginRCSwitchDevice*>	_devices;

    uint32_t bitStringToUInt32(char* input_binary_string);
    String uint32ToBitString(uint32_t dec);

	HAPConfigurationValidationResult validateName(const char* name);
	HAPConfigurationValidationResult validateDeviceAddress(const char* deviceAddress);
	HAPConfigurationValidationResult validateHouseAddress(const char* houseAddress);

    RCSwitch _rcSwitch;

	HAPPluginRCSwitchDevice* _newDevice;

	struct HAPPluginRCSwitchConfig* _configInternal;

	bool fakeGatoCallback();
	// HAPFakeGatoWeather _fakegato;

};

REGISTER_PLUGIN(HAPPluginRCSwitch)

#endif /* HAPPLUGINRCSWITCH_HPP_ */