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
HAPService::HAPService(const String& _uuid)
: _uuid(CHAR_TYPE_NULL)
, _uuidString(_uuid)
, _features(0)
{
}


// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// void HAPService::toJson(JsonObject& nested) {
//     // iid
//     nested["iid"] = serviceID;

//     // type
//     if (uuid == 0x00) {
//         nested["type"] = uuidString;
//     } else {
// #if HAP_LONG_UUID
//         char uuidStr[HAP_UUID_LENGTH];
//         snprintf(uuidStr, HAP_UUID_LENGTH, HAP_UUID, uuid);
// #else
//         char uuidStr[8];
//         snprintf(uuidStr, 8, "%X", uuid);
// #endif
//         nested["type"] = uuidStr;
//     }

//     // characteristics
//     JsonArray charsArray = nested.createNestedArray("characteristics");
//     for (int i = 0; i < numberOfCharacteristics(); i++) {
//         // services[i] = _services[i]->describe();
//         _characteristics[i]->addToJsonArray(charsArray);
//     }

//     // hidden
//     nested["hidden"] = hidden ? "true" : "false";

//     // primary
//     nested["primary"] = primary ? "true" : "false";

//     // linked serviceIds
//     if (_linkedServiceIds.size() > 0){
//         JsonArray LinkedIdsArray = nested.createNestedArray("linked");
//         for (int i = 0; i < _linkedServiceIds.size(); i++) {
//             // services[i] = _services[i]->describe();
//             LinkedIdsArray.add(_linkedServiceIds[i]);
//         }
//     }
// }


// #if defined(ARDUINO_TEENSY41)
// FLASHMEM
// #endif
// void HAPService::toJson(JsonArray& array){
//     JsonObject nested = array.createNestedObject();
//     toJson(nested);
// }

// String HAPService::describe() {

//     // String keys[6] = {"iid", "type", "characteristics", "hidden", "primary", "linked"};
//     uint8_t keySize = 5;

//     if (_linkedServiceIds.size() > 0){
//         keySize = 6;
//     }

//     String keys[keySize];
//     String values[keySize];

//     // key: iid
//     {
//         keys[0] = "iid";
//         char temp[8];
//         snprintf(temp, 8, "%d", serviceID);
//         values[0] = temp;
//     }

//     // key: type
//     {
//         keys[1] = "type";
//         if (uuid == 0x00) {
//             String uuidStr = uuidString;
//             values[1] = HAPHelper::wrap(uuidStr);
//         } else {
// #if HAP_LONG_UUID
//             char uuidStr[HAP_UUID_LENGTH];
//             snprintf(uuidStr, HAP_UUID_LENGTH, HAP_UUID, uuid);
// #else
//             char uuidStr[8];
//             snprintf(uuidStr, 8, "%X", uuid);
// #endif
//             values[1] = HAPHelper::wrap(uuidStr);
//         }
//     }

//     // key: characteristics
//     {
//         keys[2] = "characteristics";

//         int no = numberOfCharacteristics();
//         String *chars = new String[no];
//         for (int i = 0; i < no; i++) {
//             chars[i] = _characteristics[i]->describe();
//         }
//         values[2] = HAPHelper::arrayWrap(chars, no);
//         delete [] chars;
//     }

//     // key: hidden
//     {
//         keys[3] = "hidden";
//         values[3] = hidden ? HAPHelper::wrap("true") : HAPHelper::wrap("false");
//     }
//     // key: primary
//     {
//         keys[4] = "primary";
//         values[4] = primary ? HAPHelper::wrap("true") : HAPHelper::wrap("false");
//     }

//     // key: linked serviceIds
//     if (_linkedServiceIds.size() > 0){

//         keys[5] = "linked";

//         int no = _linkedServiceIds.size();
//         String *chars = new String[no];
//         for (int i = 0; i < no; i++) {
//             chars[i] = String(_linkedServiceIds[i]);
//         }
//         values[5] = HAPHelper::arrayWrap(chars, no);
//         delete [] chars;
//     }

//     return HAPHelper::dictionaryWrap(keys, values, keySize);

// }

void HAPService::printTo(Print& print){
	print.print(F("{"));

    // iid
    print.print(F("\"iid\":"));
    print.print(_aid);
    print.print(F(","));

    // type
    print.print(F("\"type\":"));
    if (_uuid == 0x00) {
        print.print(HAPHelper::wrap(_uuidString));
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

