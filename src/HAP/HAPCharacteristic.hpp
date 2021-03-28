// HAPCharacteristic.hpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#ifndef HAPCHARACTERISTIC_HPP_
#define HAPCHARACTERISTIC_HPP_

#include <Arduino.h>
#include "HAPGlobals.hpp"
#include <functional>
#include <ArduinoJson.h>
#include "HAPHelper.hpp"

#include "HAPCharacteristicBase.hpp"


#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#ifndef HAP_UUID
#define HAP_UUID        "%08X-0000-1000-8000-0026BB765291"
#endif

#ifndef HAP_UUID_LENGTH
#define HAP_UUID_LENGTH 36
#endif

#ifndef HAP_LONG_UUID
#define HAP_LONG_UUID   0
#endif

#define CHAR_TYPE_NULL  0x00

#ifndef HAP_ADD_DESC_TO_JSON
#define HAP_ADD_DESC_TO_JSON		1		// Adds description for each chr to accessory.json
#endif

enum {
	permission_read             = 1,        // Paired read
	permission_write            = 1 << 1,   // Paired write
	permission_notify           = 1 << 2,   // Notify/Events = Accessory will notice the controller
	permission_hidden           = 1 << 3,   // Hidden 
	permission_additional_auth  = 1 << 4,   // This characteristic supports Additional Authorization  
	permission_timed_write      = 1 << 5,   // This characteristic allows only timed write procedure
	permission_write_response   = 1 << 6    // This characteristic supports write response
};

typedef enum {
	unit_none = 0,          // none
	unit_celsius,           // °C
	unit_percentage,        // %
	unit_arcDegree,         // °
	unit_lux,               // lux
	unit_seconds,           // sec

	unit_hpa,               // hPa
	unit_watt,              // W
	unit_voltage,           // V
	unit_kwh,               // kWh
	unit_kmh,               // km/h
	unit_km,                // km
	unit_m,                 // m
	unit_mm,                // mm    
	unit_kelvin,            // K
	unit_DU,                // DU
	unit_mired,             // Mired
	unit_ppm,               // ppm    
} unit;

typedef enum {
	battery_level_normal    = 0,
	battery_level_low       = 1    
} battery_level;

typedef enum {
	NOT_CHARGING            = 0,
	CHARGING                = 1,
	NOT_CHARGEABLE          = 2
} charging_state;


class HAPCharacteristic {
public:   
	uint8_t iid;
	uint16_t type;
	const uint8_t permission;
	
	char* typeString = nullptr;    

#if HAP_ADD_DESC_TO_JSON
	char* desc;
#endif    

	// std::function<void(int, void*, void*)> genericValueChangeFunctionCall = NULL;
	
	std::function<void(void)> valueGetFunctionCall = NULL;

	HAPCharacteristic(uint16_t _type, uint8_t _permission): type(_type), permission(_permission) {
		type = _type;
		typeString = nullptr;
		desc = nullptr;
	}

	HAPCharacteristic(const char* _typeString, uint8_t _permission): type(CHAR_TYPE_NULL), permission(_permission) {        
		type = CHAR_TYPE_NULL;
		
		typeString = new char[HAP_UUID_LENGTH + 1];
		strncpy(typeString, _typeString, HAP_UUID_LENGTH + 1);
		typeString[HAP_UUID_LENGTH] = '\0';
	  
		desc = nullptr;
	}    


	virtual ~HAPCharacteristic(){
		if (typeString) delete[] typeString;
		if (desc) delete[] desc;
	}


	// 
	// Character specific
	// 
	virtual String valueString() = 0;
	virtual void setValueString(const String& str) = 0;    
	virtual void addCharacteristicSpecToJson(JsonObject& root, bool meta = true) = 0;

	// 
	// Desc
	// 
	// virtual void setDescription(const String& str){
	//     strcpy(desc, str.c_str());
	// }
	virtual void setDescription(const char* str){

		if (desc) delete[] desc;

		desc = new char[HAP_HOMEKIT_DEFAULT_STRING_LENGTH + 1];
		strncpy(desc, str, HAP_HOMEKIT_DEFAULT_STRING_LENGTH + 1);
		desc[strlen(str)] = '\0';
	}

	
	// 
	// Json 
	//
	virtual void addToJsonArray(JsonArray& array);
	virtual void toJson(JsonObject& root, bool type_ = false, bool perms = false, bool event = false, bool meta = false, bool addDesc = false);    
	virtual void printTo(Print& print);
	virtual void addPermsToJson(JsonObject& root);

	// 
	// Permissions
	// 
	bool writable()       { return permission & permission_write;           }
	bool notifiable()     { return permission & permission_notify;          }
	bool readable()       { return permission & permission_read;            }
	bool hidden()         { return permission & permission_hidden;          }
	bool timedWrite()     { return permission & permission_timed_write;     }
	bool writeResponse()  { return permission & permission_write_response;  }
	bool additionalAuth() { return permission & permission_additional_auth; }

	// 
	// Units
	// 
	virtual const char* unitString(unit unitValue);
	virtual const char* unitJson(unit unitValue);


	// 
	// NEW and DELETE overloads
	// 
	void* operator new(size_t size)
    {
        Serial.printf(PSTR("Overloading new operator with size: %d\n"), size);
        //void * p = ::operator new(size);

#if defined(ARDUINO_TEENSY41)
		void* ptr = extmem_malloc(size);		
#else		
        void* ptr = malloc(size); // will also work fine
#endif     
        return ptr;
    }
 
    void operator delete(void* ptr)
    {
        Serial.println(F("Overloading delete operator"));
        
#if defined(ARDUINO_TEENSY41)
		extmem_free(ptr);
#else		
        free(ptr);
#endif 		
    }

};



#endif /* HAPCHARACTERISTIC_HPP_ */