//
// HAPService.cpp
// Homekit
//
//  Created on: 22.04.2018
//      Author: michael
//

#include "HAPService.hpp"
#include "HAPHelper.hpp"

HAPService::HAPService(uint8_t _uuid)
: uuid(_uuid), uuidString("") {
    hidden = false;
    primary = false;
}

HAPService::HAPService(const String& _uuid)
: uuid(CHAR_TYPE_NULL), uuidString(_uuid) {
    hidden = false;
    primary = false;
}



void HAPService::toJson(JsonObject& nested) {
    // iid
    nested["iid"] = serviceID;
    
    // type
    if (uuid == 0x00) { 
        nested["type"] = uuidString;                
    } else {
#if HAP_LONG_UUID    
        char uuidStr[HAP_UUID_LENGTH];
        snprintf(uuidStr, HAP_UUID_LENGTH, HAP_UUID, uuid);        
#else    
        char uuidStr[8];
        snprintf(uuidStr, 8, "%X", uuid);    
#endif  
        nested["type"] = uuidStr;                                  
    }

    // characteristics
    JsonArray charsArray = nested.createNestedArray("characteristics");
    for (int i = 0; i < numberOfCharacteristics(); i++) {
        // services[i] = _services[i]->describe();
        _characteristics[i]->addToJsonArray(charsArray);
    }  

    // hidden
    nested["hidden"] = hidden ? "true" : "false";
    
    // primary
    nested["primary"] = primary ? "true" : "false";

    // linked serviceIds
    if (_linkedServiceIds.size() > 0){
        JsonArray LinkedIdsArray = nested.createNestedArray("linked");
        for (int i = 0; i < _linkedServiceIds.size(); i++) {
            // services[i] = _services[i]->describe();
            LinkedIdsArray.add(_linkedServiceIds[i]);
        } 
    }  
}

void HAPService::toJson(JsonArray& array){
    JsonObject nested = array.createNestedObject();
    toJson(nested); 
}

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
	print.print("{");
    
    // iid
    print.print("\"iid\":");
    print.print(serviceID);    
    print.print(",");

    // type
    print.print("\"type\":");    
    if (uuid == 0x00) { 
        print.print(HAPHelper::wrap(uuidString));
    } else {
#if HAP_LONG_UUID    
        char uuidStr[HAP_UUID_LENGTH];
        snprintf(uuidStr, HAP_UUID_LENGTH, HAP_UUID, uuid);        
#else    
        char uuidStr[8];
        snprintf(uuidStr, 8, "%X", uuid);    
#endif  
        print.print(HAPHelper::wrap(uuidStr));                                
    }
    print.print(",");

    // hidden
    print.print("\"hidden\":");
    if (hidden) {
        print.print(1);    
    } else {
        print.print(0);    
    }    
    print.print(",");

    // primary
    print.print("\"primary\":");
    if (primary) {
        print.print(1);    
    } else {
        print.print(0);    
    }    
    print.print(",");

    // linked
    if (_linkedServiceIds.size() > 0 ){
        print.print("\"linked\":[");
        for (uint8_t i = 0; i < _linkedServiceIds.size(); i++){
            print.print(_linkedServiceIds[i]);
            if ( i + 1 < _linkedServiceIds.size()){
                print.print(",");
            }
        }
        print.print("]");
        print.print(",");
    }

    // _characteristics
    print.print("\"characteristics\":[");    
	for (int i = 0; i < numberOfCharacteristics(); i++) {
        _characteristics[i]->printTo(print);

		if (i+1 < numberOfCharacteristics()){
			print.print(",");
		}
    }
	print.print("]");

    print.print("}");
}