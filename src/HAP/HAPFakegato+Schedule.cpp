//
// HAPFakegato+Schedule.cpp
// Homekit
//
//  Created on: 05.04.2021
//      Author: michael
//

#include "HAPFakegato+Schedule.hpp"
#include "HAPTime.hpp"
#include "HAPLogging.hpp"

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPService* HAPFakegatoSchedule::registerFakeGatoService(HAPFakegatoScheduleDeviceType::Type deviceType, HAPAccessory* accessory, const char* name){

	HAPService* fgService = HAPFakegato::registerFakeGatoService(accessory, name);
	
	_deviceType = deviceType;

	uint8_t zeroValue[1] = {0x00};

	// Config Read
	_scheduleRead = new HAPCharacteristicData(HAP_CHARACTERISTIC_FAKEGATO_CONFIG_READ, HAPPermission::Read|HAPPermission::Notify|HAPPermission::Hidden, HAP_FAKEGATO_CHUNK_BUFFER_SIZE);
	_scheduleRead->setDescription("EVE Schedule Read");
	_scheduleRead->setValue(zeroValue, 1, false);
	// auto callbackConfigRead = std::bind(&HAPFakegato::scheduleRead, this, std::placeholders::_1, std::placeholders::_2);
	// _configRead->setValueChangeCallback(callbackConfigRead);

	auto callbackGetConfig = std::bind(&HAPFakegatoSchedule::callbackGetSchedule, this, std::placeholders::_1, std::placeholders::_2);
	_scheduleRead->setDataGetCallback(callbackGetConfig);

	accessory->addCharacteristicToService(fgService, _scheduleRead);


	// Config Write
	_scheduleWrite = new HAPCharacteristicData(HAP_CHARACTERISTIC_FAKEGATO_CONFIG_WRITE, HAPPermission::Write|HAPPermission::Hidden, HAP_FAKEGATO_CHUNK_BUFFER_SIZE / 2);
	_scheduleWrite->setDescription("EVE Schedule Write");

	auto callbackConfigWrite = std::bind(&HAPFakegatoSchedule::callbackSetSchedule, this, std::placeholders::_1, std::placeholders::_2);
	_scheduleWrite->setDataChangeCallback(callbackConfigWrite);
	accessory->addCharacteristicToService(fgService, _scheduleWrite);

	return fgService;
}



#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoSchedule::callbackTimerStart(uint16_t state){
#if HAP_DEBUG_FAKEGATO_SCHEDULE
	// LogI(HAPTime::timestring() + " " + "HAPFakegatoSchedule" + "->" + "callbackTimerStart" + " [   ] " + "Timed action: START", true);
	LOG_I("Timed action: START\n");
#endif
	if (_callbackTimerStart) _callbackTimerStart(state);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoSchedule::callbackTimerEnd(uint16_t state){
#if HAP_DEBUG_FAKEGATO_SCHEDULE
	// LogI(HAPTime::timestring() + " " + "HAPFakegatoSchedule" + "->" + "callbackTimerStart" + " [   ] " + "Timed action: END", true);
	LOG_I("Timed action: END\n");
#endif
	if (_callbackTimerEnd)  _callbackTimerEnd(state);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoSchedule::programTimers() {

	for (size_t i = 0; i < _programEvents.size(); i++){

#if HAP_DEBUG_FAKEGATO_SCHEDULE
		Serial.printf("M T W T F S S \n");
		Serial.printf("%d %d %d %d %d %d %d \n", _days.mon, _days.tue, _days.wed, _days.thu, _days.fri, _days.sat, _days.sun);
#endif

		/*SMTWTFSS*/
		uint8_t daysMask = 0;
		daysMask  = (_days.sun == (i+1)) << 7;
		daysMask |= (_days.mon == (i+1)) << 6;
		daysMask |= (_days.tue == (i+1)) << 5;
		daysMask |= (_days.wed == (i+1)) << 4;
		daysMask |= (_days.thu == (i+1)) << 3;
		daysMask |= (_days.fri == (i+1)) << 2;
		daysMask |= (_days.sat == (i+1)) << 1;
		//0b10000000,

#if HAP_DEBUG_FAKEGATO_SCHEDULE
		Serial.printf("daysMask: %d\n", daysMask);
#endif

		for (size_t j = 0; j < _programEvents[i].timerEvents.size(); j++){
			// add dailytimer here !!
			HAPDailyTimer timer(_programEvents[i].timerEvents[j].hour,
								_programEvents[i].timerEvents[j].minute,
								daysMask,
								FIXED,
								std::bind(&HAPFakegatoSchedule::callbackTimerStart, this, std::placeholders::_1),
								// [](uint16_t i){Serial.println("Serial Print Timer just Fired!");},
								(uint16_t)_programEvents[i].timerEvents[j].state);

			// timer.setDaysActive(daysMask);
			timer.begin();
			_timers.addTimer(timer);
		}
	}
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoSchedule::clear(){
	for (int i = 0; i < _programEvents.size(); i++){
		_programEvents[i].timerEvents.clear();
	}

	_programEvents.clear();
}

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// void HAPFakegatoSchedule::scheduleFromJson(JsonObject &root){

// 	if (root["timer"].isNull()) return;

// 	enable(root["timer"]["enabled"].as<bool>());
// 	_days = HAPFakeGatoScheduleDays(root["timer"]["days"].as<uint32_t>());

// 	String dataStr = root["timer"]["programs"].as<String>();

// 	uint8_t data[dataStr.length() / 2];
// 	HAPHelper::hexToBin(data, dataStr.c_str(), dataStr.length());

// 	decodePrograms(data);

// }

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// JsonObject HAPFakegatoSchedule::scheduleToJson(){
// 	/*
// 		"days": uint32_t,
// 		"programs": "0503....."
// 	*/
// 	const size_t capacity = 256;
// 	DynamicJsonDocument doc(capacity);

// 	if (_programEvents.size() == 0) {
// 	    doc.shrinkToFit();
//     	return doc.as<JsonObject>();
// 	}

// 	doc["enabled"] = isEnabled();
// 	doc["days"] = _days.daysnumber();

// 	size_t dataSize = 0;
// 	encodePrograms(nullptr, &dataSize);
// 	uint8_t data[dataSize];
// 	encodePrograms(data, &dataSize);

// 	// HAPHelper::array_print("CONFIG SAVE data", data, dataSize);

// 	char hexString[(dataSize * 2) + 1];
// 	HAPHelper::binToHex(data, dataSize, hexString, (dataSize * 2) + 1);

// 	doc["programs"] = hexString;
// 	return doc.as<JsonObject>();
// }

