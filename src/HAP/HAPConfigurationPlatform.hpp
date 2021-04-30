//
// HAPConfigurationPlatform.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONPLATFORM_HPP_
#define HAPCONFIGURATIONPLATFORM_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include "HAPLogger.hpp"
#include "HAPConfigurationItem.hpp"

//
// Platform
//
struct HAPConfigurationPlatform : public HAPConfigurationItem {
public:
	enum LogLevel _logLevel = LogLevel::DEBUG;
	uint32_t	  _refTime  = 0;

	static size_t getDataSize(){
		return sizeof(uint8_t) + sizeof(uint32_t);
	}

	HAPConfigurationPlatform(){
		_logLevel = LogLevel::DEBUG;
		_refTime  = 0;
	}

	~HAPConfigurationPlatform(){

	}

	HAPConfigurationPlatform(uint8_t loglvl, uint32_t refTime)
		: _logLevel((enum LogLevel)loglvl)
		, _refTime(refTime)
		{

		}

	uint8_t logLevel(){ return (uint8_t)_logLevel; }
	void 	setLogLevel(uint8_t lvl){ _logLevel = (enum LogLevel)lvl; }

	void setRefTime(uint32_t refTime){ _refTime = refTime; }
	uint32_t refTime(){ return _refTime; }

	HAPConfigurationPlatform(const HAPConfigurationPlatform& rhs) {
		_logLevel = rhs._logLevel;
		_refTime  = rhs._refTime;
	}

	HAPConfigurationPlatform operator=(const HAPConfigurationPlatform& rhs) {
		if (this == &rhs) {
			return (*this);
		}

		_logLevel = rhs._logLevel;
		_refTime  = rhs._refTime;
		return (*this);
	}

	bool operator== (const HAPConfigurationPlatform &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */
		return (
			( this->_logLevel == rhs._logLevel ) &&
			( this->_refTime == rhs._refTime )
		) ? true : false;
	}


	void clear() override {
		_logLevel = LogLevel::DEBUG;
		_refTime = 0;
	}

	void setDefaults() override {
		_logLevel = LogLevel::DEBUG;
		_refTime = 0;
	}


	/*
		{
			"loglevel": 1,
			"refTime": 0,
		}
	 */
	void toJson(Print& prt) override {
		prt.print("{");

		prt.print("\"loglevel\": ");
		prt.print((uint8_t)_logLevel);
		prt.print(",");

		prt.print("\"refTime\": ");
		prt.print((uint8_t)_refTime);

		prt.print("}");
	}

};

#endif /* HAPCONFIGURATIONPLATFORM_HPP_ */