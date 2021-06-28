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
#include "HAPConfigurationItem.hpp"

//
// Platform
//
struct HAPConfigurationPlatform : public HAPConfigurationItem {
public:
	uint32_t	  _refTime  = 0;

	static size_t getDataSize(){
		return sizeof(uint32_t);
	}

	HAPConfigurationPlatform(){
		_refTime  = 0;
	}

	~HAPConfigurationPlatform(){

	}

	HAPConfigurationPlatform(uint32_t refTime)
		: _refTime(refTime)
		{

		}


	void setRefTime(uint32_t refTime){ _refTime = refTime; }
	uint32_t refTime(){ return _refTime; }

	HAPConfigurationPlatform(const HAPConfigurationPlatform& rhs) {
		_refTime  = rhs._refTime;
	}

	HAPConfigurationPlatform operator=(const HAPConfigurationPlatform& rhs) {
		if (this == &rhs) {
			return (*this);
		}

		_refTime  = rhs._refTime;
		return (*this);
	}

	bool operator== (const HAPConfigurationPlatform &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */
		return (
			( this->_refTime == rhs._refTime )
		) ? true : false;
	}


	void clear() override {
		_refTime = 0;
	}

	void setDefaults() override {
		_refTime = 0;
	}


	/*
		{
			"refTime": 0,
		}
	 */
	void toJson(Print& prt) override {
		prt.print("{");

		prt.print("\"refTime\": ");
		prt.print((uint8_t)_refTime);

		prt.print("}");
	}

};

#endif /* HAPCONFIGURATIONPLATFORM_HPP_ */