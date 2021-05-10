//
// HAPConfigurationPluginData.hpp
// Homekit
//
//  Created on: 14.12.2018
//      Author: michael
//

#ifndef HAPCONFIGURATIONPLUGINDATA_HPP_
#define HAPCONFIGURATIONPLUGINDATA_HPP_

#include <Arduino.h>
#include <vector>
#include <functional>
#include "HAPGlobals.hpp"
#include "HAPLogger.hpp"

#include "HAPConfigurationItem.hpp"

// //
// // HAPConfigurationPluginData
// //
// struct HAPConfigurationPluginData {
// 	uint8_t* 	dataPtr = nullptr;
// 	size_t 		dataSize = 0;

// 	std::function<bool(void)> _callbackSave;
// 	std::function<void(Print&)> _callbackToJson;

// 	void setSaveCallback(std::function<bool(void)> callback){
// 		_callbackSave = callback;
// 	}

// 	void setToJsonCallback(std::function<void(Print&)> callback){
// 		_callbackToJson = callback;
// 	}

// 	void toJson(Print& prt){
// 		if (_callbackToJson){
// 			_callbackToJson(prt);
// 		}
// 	}

// 	bool save(){
// 		if (_callbackSave){
// 			return _callbackSave();
// 		}
// 		return false;
// 	}

// 	HAPConfigurationPluginData(){

// 	}

// 	HAPConfigurationPluginData(uint8_t* dataPtr_, size_t dataSize_)
// 		: dataPtr(dataPtr_)
// 		, dataSize(dataSize_) {
// 		}

// 	HAPConfigurationPluginData(const HAPConfigurationPluginData& rhs) {
// 		dataPtr = rhs.dataPtr;
// 		dataSize = rhs.dataSize;
// 	}


// 	HAPConfigurationPluginData operator=(const HAPConfigurationPluginData& rhs) {
// 		if (this == &rhs) {
// 			return (*this);
// 		}

// 		dataPtr = rhs.dataPtr;
// 		dataSize = rhs.dataSize;
// 		return (*this);
// 	}

// 	bool operator== (const HAPConfigurationPluginData &rhs) const {
// 		/* your logic for comparision between "*this" and "rhs" */
// 		return (
// 			( this->dataSize == rhs.dataSize ) &&
// 			( memcmp(this->dataPtr, rhs.dataPtr, this->dataSize) == 0 )
// 		) ? true : false;
// 	}
// };



class HAPConfigurationPlugin : public HAPConfigurationItem {
public:
	char 		name[20 + 1] = {0, };
	bool 		enabled = true;
	uint32_t 	interval = 0;
	size_t 		dataSize = 0;
	uint8_t* 	dataPtr = nullptr;

	// HAPConfigurationPluginData* data;


	std::function<void(Print&)> _callbackToJson;


	HAPConfigurationPlugin(){

	}

	~HAPConfigurationPlugin(){
		if (dataPtr != nullptr){
			delete[] dataPtr;
		}
	}

	HAPConfigurationPlugin(const char* name_,  bool enabled_, uint32_t interval_, uint8_t* dataPtr_ = nullptr, size_t dataSize_ = 0){
		uint8_t size = strlen(name_);

		if (size > 20) {
			LogE(F("ERROR: NAME is too long!"), true);
			return;
		}

		// common
		strncpy(name, name_, 20);
		enabled = enabled_;
		interval = interval_;
		setData(dataPtr_, dataSize_);
	}

	void setToJsonCallback(std::function<void(Print&)> callback){
		_callbackToJson = callback;
	}

	bool setName(const char* name_){

		uint8_t size = strlen(name_);

		if (size > 20) {
			LogE(F("ERROR: NAME is too long!"), true);
			return false;
		}
		strncpy(name, name_, 20);

		return true;
	}

	void setData(const uint8_t* data, size_t dataLen ){
		if (dataPtr != nullptr) delete[] dataPtr;

		dataPtr = new uint8_t[dataLen];
		memcpy(dataPtr, data, dataLen);
		dataSize = dataLen;
	}


	HAPConfigurationPlugin(const HAPConfigurationPlugin& rhs)
		: enabled(rhs.enabled)
		, interval(rhs.interval)
	{
		setName(rhs.name);
		setData(rhs.dataPtr, rhs.dataSize);
	}

	size_t getCommonSize(){
		return (sizeof(name) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t));
	}

	size_t getDataSize(){
		return dataSize;
	}


	HAPConfigurationPlugin operator=(const HAPConfigurationPlugin& rhs) {
		if (this == &rhs) {
			return (*this);
		}

		enabled = rhs.enabled;
		interval = rhs.interval;

		setName(rhs.name);
		setData(rhs.dataPtr, rhs.dataSize);

		return (*this);
	}

	bool operator== (const HAPConfigurationPlugin &rhs) const {
		/* your logic for comparision between "*this" and "rhs" */
		return (
			( this->dataSize == rhs.dataSize ) &&
			( memcmp(this->dataPtr, rhs.dataPtr, this->dataSize) == 0 ) &&
			( strncmp(this->name, rhs.name, strlen(this->name)) == 0 ) &&
			( this->enabled == rhs.enabled ) &&
			( this->interval == rhs.interval )
		) ? true : false;
	}

	/*
        {
			"name": "ABCD",
			"enabled": true,
            "interval": 1000,
            "internal" {
				"mode": 1
			}
        }
     */
	void toJson(Print& prt) override {
		prt.print("{");
		prt.print("\"name\": ");
		prt.print(HAPHelper::wrap(name));


		prt.print(",");
		prt.print("\"enabled\": ");
		prt.print(enabled);


		prt.print(",");
		prt.print("\"interval\": ");
		prt.print(interval);

		prt.print(",");
		prt.print("\"internal\": {");
			if (_callbackToJson){
				_callbackToJson(prt);
			}
		prt.print("}");

		prt.print("}");
	}

	void clear() override {

	}

	void setDefaults() override {
		enabled = true;
		interval = 0;
	}

};

#endif /* HAPCONFIGURATIONPLUGINDATA_HPP_ */