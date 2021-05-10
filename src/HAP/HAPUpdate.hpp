//
// HAPUpdate.hpp
// Homekit
//
//  Created on: 20.08.2017
//      Author: michael
//

#ifndef HAPUPDATE_HPP_
#define HAPUPDATE_HPP_

#include <Arduino.h>
#if HAP_ENABLE_UPDATE_WEB
#include <HTTPClient.h>
#endif

#include "HAPVersion.hpp"
#include "HAPGlobals.hpp"
#include "HAPConfiguration.hpp"


struct HAPUpdateVersionInfo {
	HAPVersion version;
	String md5;
	uint32_t size;
	uint32_t featureRev;
};


class HAPUpdate {
public:
	HAPUpdate();
	~HAPUpdate();

	void begin(const char* hostname);
	void handle();

	void setConfig(HAPConfigurationArduinoOTA* config){
		_configuration = config;
	}

	void setHostAndPort(const char* url, int port, uint32_t interval = HAP_UPDATE_WEB_INTERVAL) {
		_port = port;
		_host = url;
		_interval = interval;
	}

	void setInterval(uint16_t interval){
		_interval = interval;
	}

	bool updateAvailable();

#if HAP_ENABLE_UPDATE_WEB
	bool checkUpdateAvailable();
	void execWebupdate();
#endif

	String onlineVersion() {
		return _remoteInfo.version.toString();
	}

private:
	bool 			_isOTADone;

	String 			_host;
	uint16_t 		_port;
	uint32_t 		_interval;
	unsigned long 	_previousMillis;
	bool 			_available;

	HAPConfigurationArduinoOTA* _configuration;


 	HAPUpdateVersionInfo _remoteInfo;
#if HAP_ENABLE_UPDATE_WEB
	HTTPClient* _http;
#endif
	// String getHeaderValue(String header, String headerName);
};

#endif /* HAPUPDATE_HPP_ */
