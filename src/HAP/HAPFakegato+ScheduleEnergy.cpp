//
// HAPFakegato+ScheduleEnergy.cpp
// Homekit
//
//  Created on: 05.04.2021
//      Author: michael
//

#include "HAPFakegato+ScheduleEnergy.hpp"
#include "HAPLogger.hpp"
#include "HAPHelper.hpp"
#include "HAPTLV8.hpp"
#include "HAPTime.hpp"

#if defined ( ARDUINO_ARCH_ESP32 )
#if ESP_IDF_VERSION_MAJOR == 4
#include "mbedtls/base64.h"
#else
extern "C" {
    #include "crypto/base64.h"
}

#endif
#elif defined(CORE_TEENSY)
#include <Base64.h>
#endif



#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPFakegatoScheduleEnergy::decodeToggleOnOff(uint8_t* data){
	return data[1] & 0x01;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoScheduleEnergy::decodeDays(uint8_t *data){
	uint32_t daysnumber = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
	daysnumber = daysnumber >> 4;

	_days = HAPFakeGatoScheduleDays(daysnumber);

#if HAP_DEBUG_FAKEGATO_SCHEDULE
	Serial.printf("M T W T F S S \n");
	Serial.printf("%d %d %d %d %d %d %d \n", _days.mon, _days.tue, _days.wed, _days.thu, _days.fri, _days.sat, _days.sun);
#endif
}


/**
 * @brief Load Programs from data
 *
 * @param data
 */
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoScheduleEnergy::decodePrograms(uint8_t* data){
	// clear all old programs and timers
	clear();
	// ToDo: update to daily timer vector
	_timers.clear();

	uint8_t programCount = data[1];
	programCount = programCount - 1;

	uint8_t pcount = 0;
	uint8_t curIndex = 7;

	for (uint8_t i = 0; i < programCount; i++) {

		HAPFakeGatoScheduleProgramEvent program;
		program.id = pcount;

		uint16_t timerCount = (data[curIndex] | (data[curIndex+1] << 8)) >> 7;

		// printf("%x %x\n", data[curIndex], data[curIndex+1]);
		// printf("timerCount: %d\n", timerCount);

		curIndex = curIndex + 2;


		for (uint8_t j = 0; j < timerCount; ++j){
			// printf("%x%x", data[curIndex], data[curIndex+1]);

			uint16_t timer = data[curIndex] | (data[curIndex+1] << 8);

			// printf("timer: %d: %X \n", timer, timer);

			HAPFakeGatoScheduleTimerEvent tEvent;

			tEvent.state  = (timer & 0x1F) >> 2;
			tEvent.type   = static_cast<HAPFakeGatoScheduleTimerType>( ((timer & 0x1F) & 0x02 ) >> 1);

			if ((timer & 0x1F) == 1 || (timer & 0x1F) == 5) {

				tEvent.minute = (timer >> 5) % 60;
				tEvent.hour   = ((timer >> 5) - tEvent.minute) / 60;

				// printf("offset:: %d\n", ((timer >> 5) * 60) );
				tEvent.offset = ((timer >> 5) * 60);

#if HAP_DEBUG_FAKEGATO_SCHEDULE
				Serial.printf("hour: %d, min: %d, offset: %d state: %d type: %d \n", tEvent.hour, tEvent.minute, tEvent.offset, tEvent.state, tEvent.type);
#endif
			} else if ((timer & 0x1F) == 7 || (timer & 0x1F) == 3) {
				tEvent.sunrise = static_cast<HAPFakeGatoScheduleSunriseType>(((timer >> 5) & 0x01));    // 1 = sunrise, 0 = sunset

				tEvent.offset  = ((timer >> 6) & 0x01 ? ~((timer >> 7) * 60) + 1 : (timer >> 7) * 60);   // offset from sunrise/sunset (plus/minus value)

#if HAP_DEBUG_FAKEGATO_SCHEDULE
				Serial.printf("sunrise: %d, offset: %d state: %d type: %d \n", tEvent.sunrise, tEvent.offset, tEvent.state, tEvent.type);
#endif
			}


			program.timerEvents.push_back(tEvent);
			curIndex = curIndex + 2;
		}



		_programEvents.push_back(program);
		pcount++;
	}


	programTimers();
	// for (int i = 0; i < _programEvents.size(); i++){
	// 	printf("   %d:\n", _programEvents[i].id + 1);

	// 	for (int j = 0; j < _programEvents[i].timerEvents.size(); ++j){
	// 		HAPFakeGatoScheduleTimerEvent tEvent = _programEvents[i].timerEvents[j];

	// 		if (tEvent.type == TIME) {
	// 			printf("   >>> hour: %d, min: %d, offset: %d state: %d type: %d \n", tEvent.hour, tEvent.minute, tEvent.offset, tEvent.state, tEvent.type);
	// 		} else {
	// 			printf("   >>> sunrise: %d, offset: %d state: %d type: %d \n", tEvent.sunrise, tEvent.offset, tEvent.state, tEvent.type);
	// 		}
	// 	}
	// }

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
uint32_t HAPFakegatoScheduleEnergy::encodeTimerCount(uint8_t timerCount){
	uint32_t result = 0;
	if (timerCount < 3) {
		result = (timerCount * 128) + timerCount;
	} else {
		result = (timerCount * 128) + 3;
	}
	return result;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
uint8_t HAPFakegatoScheduleEnergy::encodeProgramCount(uint8_t programCount){
	return programCount + 1;
}


/**
 * @brief Put programs to data
 *
 * @param data 			Output for programs
 * @param dataSize 		length of data
 */
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoScheduleEnergy::encodePrograms(uint8_t* data, size_t *dataSize){
	// uint8_t programCount = data[1] | data[2] << 8;
	uint8_t programCount = _programEvents.size();
	// printf("%0x\n", programCount);

	uint8_t totalTimerCount = 0;

	uint16_t timerCount[programCount];

	for (uint8_t i = 0; i < _programEvents.size(); i++){
		timerCount[i] = _programEvents[i].timerEvents.size();
		totalTimerCount += timerCount[i];
	}

	// printf("%d\n", totalTimerCount);
	// uint8_t dataSize = 1 + 1 + 5;
	*dataSize = 1 + 1 + 5;
	*dataSize += (programCount * 2) + (totalTimerCount * 2);
	// printf("%d\n", dataSize);

	// uint8_t data[dataSize];
	if (data == nullptr) {
		return;
	}

	memset(data, 0x00, *dataSize);

	data[ 0 ] 	= 0x05;
	data[ 1 ] 	= (programCount + 1);

	uint8_t curIndex = 7;
	for (int i = 0; i < programCount; i++){

		// printf("timerCount %d\n", timerCount[i]);

		uint8_t addition = 0;
		if (timerCount[i] < 3){
			addition = timerCount[i];
		} else {
			addition = 3;
		}

		data[curIndex++] = ((timerCount[i] << 7) & 0xFF) + addition;
		data[curIndex++] = ((timerCount[i] << 7 ) >> 8);

		for (int j = 0; j < _programEvents[i].timerEvents.size(); j++){

			HAPFakeGatoScheduleTimerEvent tEvent = _programEvents[i].timerEvents[j];

			uint16_t timerData = 0;

			tEvent.state == true ? timerData += 4 : timerData += 0;

			// tEvent.type  == TIME ? timerData += 1 : timerData += 3;
			if (tEvent.type  == TIME){
				timerData += (tEvent.offset / 60) << 5;
				timerData += 1;
			} else {
				// printf("offset: %d - %02X \n", tEvent.offset, tEvent.offset);

				if (tEvent.offset < 0) {
					timerData += (~((tEvent.offset / 60) << 7) + 1 + 0x40);
					// printf("negatte: %d\n", timerData);

				} else {
					timerData += (tEvent.offset / 60) << 7;
				}

				timerData += 0x03 + ((uint8_t)tEvent.sunrise * 0x1F) + (uint8_t)tEvent.sunrise;
			}

			data[curIndex++] = (timerData & 0xFF);
			data[curIndex++] = (timerData >> 8);

			// printf("%d - %d %02X %02X %02X\n", tEvent.offset, timerData, timerData, (timerData & 0xFF), (timerData >> 8));
		}
	}
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
String HAPFakegatoScheduleEnergy::scheduleRead(){
    TLV8 tlv;

	//
	// Device Type
	//
    tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_DEVICE_TYPE, {_deviceType, 0x00});


    tlv.encode(0x03, {0xB8, 0x04});


	//
    // Serial Number
	//
    // tlv.encode(0x04, {0x42, 0x56, 0x31, 0x32, 0x4A, 0x31, 0x41, 0x30, 0x37, 0x32, 0x31, 0x32});
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_SERIALNUMBER, _serialNumber.length(), (uint8_t*)_serialNumber.c_str());

	//
	// Number of history entries
	//
    // tlv.encode(0x06, {0xFB, 0x0A});
	ui16_to_ui8 memoryUsed;
    memoryUsed.ui16 = _entries.size();
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_USED_MEMORY, 2, memoryUsed.ui8);

	//
	// Number of rolled over index
	//
    // tlv.encode(0x07, {0x0C, 0x10, 0x00, 0x00});
	ui32_to_ui8 rolledOverIndex;
	if (_entries.size() == _entries.capacity) {
		rolledOverIndex.ui32 = 1;
	} else {
		rolledOverIndex.ui32 = 0;
	}
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_ROLLED_OVER_INDEX, 4, rolledOverIndex.ui8);

    tlv.encode(0x0B, {0x00, 0x00});
    tlv.encode(0x05, {0x00});
    tlv.encode(0x02, {0x90, 0x27, 0x00, 0x00});
    tlv.encode(0x5F, {0x00, 0x00, 0x00, 0x00});
    tlv.encode(0x19, {0x96, 0x00});
    tlv.encode(0x14, {0x03});
    tlv.encode(0x0F, {0x00, 0x00, 0x00, 0x00});

	//
    // Programs
	//
    //tlv.encode(0x45, {0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x3C, 0x05, 0x96});
	size_t dataSize = 0;
	encodePrograms(nullptr, &dataSize);
	uint8_t data[dataSize];
	encodePrograms(data, &dataSize);
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_PROGRAMS, dataSize, data);

	//
    // Days
	//
    // tlv.encode(0x46, {0x05, 0x15, 0x1C, 0x2C, 0x9F, 0x24, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
	uint8_t daysHex[84];
	memset(daysHex, 0, 84);

	uint32_t daysNo = _days.daysnumber();
	daysHex[0] = 0x05;  // ??
	daysHex[1] = 0x15;  // ??
	daysHex[2] = 0x1C;	// ??
	daysHex[3] = 0x2C;	// ??
	daysHex[4] = daysNo;
	daysHex[5] = daysNo >>  8;
	daysHex[6] = daysNo >> 16;
	daysHex[7] = daysNo >> 24;

	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_DAYS, 84, daysHex);

	//
    // Commands
    // tlv.encode(0x44, {0x05, 0x0C, 0x00, 0x05, 0x03, 0x3C, 0x00, 0x00, 0x00, 0x32, 0xC2, 0x42, 0x42, 0xA1, 0x93, 0x34, 0x41});
	// 						    |
	//						    +-> 0x0C = OFF -- 0xOD = ON
	// ToDo:
	// Logitude and Latitude calcualtion
	if (_timers.isEnabled()) {
		tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_TOGGLE_SCHEDULE, {0x05, 0x0D, 0x00, 0x05, 0x03, 0x3C, 0x00, 0x00, 0x00, 0x32, 0xC2, 0x42, 0x42, 0xA1, 0x93, 0x34, 0x41});
	} else {
		tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_TOGGLE_SCHEDULE, {0x05, 0x0C, 0x00, 0x05, 0x03, 0x3C, 0x00, 0x00, 0x00, 0x32, 0xC2, 0x42, 0x42, 0xA1, 0x93, 0x34, 0x41});
	}


	//
    // DST
	//
    // tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_DST, {0x05, 0x73, 0x1B, 0x45, 0x1C, 0xDF, 0x1C, 0xB8, 0x1D, 0xB4, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00});
	uint8_t offsetDST = 0;
	uint8_t DST[19];

	// first 0x05
	DST[0] = 0x05;
	offsetDST += 1;

	tmElements_t curTime;
	breakTime(now(), curTime);

	// current year DST begin
	ui16_to_ui8 dstBeginCurYear;
	dstBeginCurYear.ui16 = HAPTime::getDaysToDST((curTime.Year + 1970), 3);
	memcpy(DST + offsetDST, dstBeginCurYear.ui8, 2);
	offsetDST += 2;

	// current year DST end
	ui16_to_ui8 dstEndCurYear;
	dstEndCurYear.ui16 = HAPTime::getDaysToDST((curTime.Year + 1970), 10);
	memcpy(DST + offsetDST, dstEndCurYear.ui8, 2);
	offsetDST += 2;

	// next year DST begin
	ui16_to_ui8 dstBeginNextYear;
	dstBeginNextYear.ui16 = HAPTime::getDaysToDST((curTime.Year + 1970 + 1), 3);
	memcpy(DST + offsetDST, dstBeginNextYear.ui8, 2);
	offsetDST += 2;

	// next year DST end
	ui16_to_ui8 dstEndNextYear;
	dstEndNextYear.ui16 = HAPTime::getDaysToDST((curTime.Year + 1970 + 1), 10);
	memcpy(DST + offsetDST, dstEndNextYear.ui8, 2);
	offsetDST += 2;

	// time when changing clock
	// 0x00000078 = 120 = 02:00 time
	ui32_to_ui8 clockChangeTime;
	clockChangeTime.ui32 = (HAPTime::getUTCOffset() * 60) + 60;
	memcpy(DST + offsetDST, clockChangeTime.ui8, 4);
	offsetDST += 4;

	// time changing offset => hardcoded to 60
	ui32_to_ui8 clockOffset;
	clockOffset.ui32 = 60;
	memcpy(DST + offsetDST, clockOffset.ui8, 4);
	offsetDST += 4;

	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_DST, offsetDST, DST);


	// ??
	tlv.encode(0x48, {0x05, 0x00, 0x00, 0x00, 0x00, 0x00});
    tlv.encode(0x4A, {0x05, 0x00, 0x00, 0x00, 0x00, 0x00});
    tlv.encode(0x1A, {0x00, 0x00, 0x00, 0x00});

	//
    // Status LED
	//
    // tlv.encode(0x60, {0x64});
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_STATUS_LED, 1, _statusLED);

	//
    // last activity On switch ?
	//
    // tlv.encode(0xD0, {0x99, 0x6C, 0x21, 0x00});
	ui32_to_ui8 secsLastAct;
	if ((_callbackGetTimestampLastActivity != nullptr)) {
		secsLastAct.ui32 = _callbackGetTimestampLastActivity() - HAPTime::refTime();
	} else {
		secsLastAct.ui32 = 0;
	}
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_LAST_ACTIVITY, 4, secsLastAct.ui8); // offset ?

#if HAP_DEBUG_FAKEGATO_SCHEDULE
	// HAPHelper::array_print("secsLastAct", secsLastAct.ui8, 4);
#endif

	//
    //  EVE Time
	//
	//tlv.encode(0x9B, {0xFB, 0x2C, 0x19, 0x00}); // offset ?
	ui32_to_ui8 secs;
    secs.ui32 = (timestampLastEntry() - HAPTime::refTime());
	tlv.encode(HAP_FAKEGATO_SCHEDULE_TYPE_EVE_TIME, 4, secs.ui8); // offset ?

#if HAP_DEBUG_FAKEGATO_SCHEDULE
	// HAPHelper::array_print("secs", secs.ui8, 4);
#endif

	//
    // end mark
	//
    uint8_t endBytes[2] = {HAP_FAKEGATO_SCHEDULE_TYPE_END_MARK, 0x00};

    size_t decodedLen = 0;
	uint8_t out[tlv.size() + 2];

	tlv.decode(out, &decodedLen);

    // attach endingBytes
    memcpy(out + decodedLen, endBytes, 2);
    decodedLen = decodedLen + 2;

	tlv.clear();

#if HAP_DEBUG_FAKEGATO_SCHEDULE
    HAPHelper::array_print("tlv", out, decodedLen);
#endif


#if defined( ARDUINO_ARCH_ESP32 )
	return base64::encode(out, decodedLen);
#elif defined( CORE_TEENSY )
	int encodedLen = base64_enc_len(decodedLen);
	char encoded[encodedLen];

	base64_encode(encoded, (char*)out, decodedLen);
	return String(encoded);
#endif
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPFakegatoScheduleEnergy::scheduleWrite(String oldValue, String newValue){
    LogD(HAPTime::timeString() + " " + "HAPFakegatoScheduleEnergy" + "->" + "scheduleWrite" + " [   ] " + "Schedule Write " + _name + " ..." , true);

    // size_t outputLength = 0;
    // mbedtls_base64_decode(NULL, 0, &outputLength, (const uint8_t*)newValue.c_str(), newValue.length());
    // uint8_t decoded[outputLength];

    // mbedtls_base64_decode(decoded, sizeof(decoded), &outputLength, (const uint8_t*)newValue.c_str(), newValue.length());

	size_t outputLength = base64_dec_len((char*)newValue.c_str(), newValue.length());
	uint8_t decoded[outputLength];
	base64_decode((char*)decoded, (char*)newValue.c_str(), newValue.length());

#if HAP_DEBUG_FAKEGATO_SCHEDULE
    HAPHelper::array_print("decoded", decoded, outputLength);
#endif

    TLV8 tlv;
    tlv.encode(decoded, outputLength);

    if (tlv.hasType(HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_TOGGLE_SCHEDULE)){
        TLV8Entry* tlvEntry = tlv.getType(HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_TOGGLE_SCHEDULE);
        enable(decodeToggleOnOff(tlvEntry->value));
        _shouldSave = true;
    }

    if (tlv.hasType(HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_STATUS_LED)){
        TLV8Entry* tlvEntry = tlv.getType(HAP_FAKEGATO_SCHEDULE_TYPE_COMMAND_STATUS_LED);
        setStatusLED(tlvEntry->value[0]);
    }

    if (tlv.hasType(HAP_FAKEGATO_SCHEDULE_TYPE_DAYS)){
        TLV8Entry* tlvEntry = tlv.getType(HAP_FAKEGATO_SCHEDULE_TYPE_DAYS);
        decodeDays(tlvEntry->value);
        _shouldSave = true;
    }

    if (tlv.hasType(HAP_FAKEGATO_SCHEDULE_TYPE_PROGRAMS)){
        TLV8Entry* tlvEntry = tlv.getType(HAP_FAKEGATO_SCHEDULE_TYPE_PROGRAMS);
        decodePrograms(tlvEntry->value);
        _shouldSave = true;
    }

    // _configReadCharacteristics->setValue(_schedule->buildScheduleString(), false);
	saveConfig();

	LogD("OK", true);

	tlv.clear();
}
