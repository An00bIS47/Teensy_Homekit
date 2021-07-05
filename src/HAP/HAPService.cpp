//
// HAPService.cpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//

#include "HAPService.hpp"
#include "HAPHelper.hpp"

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPService::HAPService(uint8_t uuid)
: _uuid(uuid)
, _uuidString("")
, _features(0)
{
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPService::HAPService(const char* _uuid)
: _uuid(CHAR_TYPE_NULL)
, _uuidString(_uuid)
, _features(0)
{
}

void HAPService::printTo(Print& print){
	print.print(F("{"));

    // iid
    print.print(F("\"iid\":"));
    print.print(_aid);
    print.print(F(","));

    // type
    print.print(F("\"type\":"));
    if (_uuid == 0x00) {
        print.print(HAPHelper::wrap(_uuidString.c_str()));
    } else {
#if HAP_LONG_UUID
        char uuidStr[HAP_UUID_LENGTH];
        snprintf(uuidStr, HAP_UUID_LENGTH, HAP_UUID, uuid);
#else
        char uuidStr[8];
        snprintf(uuidStr, 8, PSTR("%X"), _uuid);
#endif
        print.print(HAPHelper::wrap(uuidStr));
    }
    print.print(F(","));

    // hidden
    print.print(F("\"hidden\":"));
    print.print((uint8_t)isHidden());
    print.print(F(","));

    // primary
    print.print(F("\"primary\":"));
    print.print((uint8_t)isPrimary());
    print.print(F(","));

    // linked
    if (_linkedServiceIds.size() > 0 ){
        print.print(F("\"linked\":["));
        for (uint8_t i = 0; i < _linkedServiceIds.size(); i++){
            print.print(_linkedServiceIds[i]);
            if ( i + 1 < _linkedServiceIds.size()){
                print.print(F(","));
            }
        }
        print.print(F("]"));
        print.print(F(","));
    }

    // _characteristics
    print.print(F("\"characteristics\":["));
	for (int i = 0; i < numberOfCharacteristics(); i++) {
        _characteristics[i]->printTo(print);

		if (i+1 < numberOfCharacteristics()){
			print.print(F(","));
		}
    }
	print.print(F("]"));

    print.print(F("}"));
}

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// void* HAPService::operator new(size_t size)
// {
//     Serial.printf(PSTR("Overloading new operator with size: %d\n"), size);
//     //void * p = ::operator new(size);

// #if defined(ARDUINO_TEENSY41)
//     void* ptr = extmem_malloc(size);
// #else
//     void* ptr = malloc(size); // will also work fine
// #endif
//     return ptr;
// }

// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// void HAPService::operator delete(void* ptr)
// {
//     Serial.println(F("Overloading delete operator"));

// #if defined(ARDUINO_TEENSY41)
//     extmem_free(ptr);
// #else
//     free(ptr);
// #endif
// }

