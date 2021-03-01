// HAPCharacteristic.cpp
// Homekit
//
//  Created on: 31.03.2018
//      Author: michael
//

#include "HAPCharacteristic.hpp"


void HAPCharacteristic::addToJsonArray(JsonArray& array) {
	JsonObject root = array.createNestedObject();
	toJson(root, true, true, false, true, true);
}

void HAPCharacteristic::printTo(Print& print){
	DynamicJsonDocument doc(1024);
	JsonObject root = doc.to<JsonObject>();
	toJson(root, true, true, false, true, true);
	serializeJson(doc, print);
}

void HAPCharacteristic::toJson(JsonObject& root, bool type_, bool perms, bool event, bool meta, bool addDesc){

	root["iid"] = iid;

	       
	addCharacteristicSpecToJson(root, true);
	
	
	if (perms){
		addPermsToJson(root);      
	}

	if (event) {
		root["ev"] = (uint8_t)notifiable();
	}
		
	if (type_){
		if (type == CHAR_TYPE_NULL) {
			root["type"] = typeString;
		} else {
			char hexType[5];
			sprintf(hexType, "%X", type);
			root["type"] = hexType;
		}            
	}

	if (addDesc) {      
		if ( (desc) && (strcmp(desc, "") != 0) ) {
			root["description"] = desc;
		}
	}  
	
}
	
void HAPCharacteristic::addPermsToJson(JsonObject& root){
	JsonArray permsArray = root.createNestedArray("perms");
	if (writable())
		permsArray.add("pw");
	if (readable())
		permsArray.add("pr");
	if (notifiable())
		permsArray.add("ev");   
	if (hidden())
		permsArray.add("hd"); 
	if (timedWrite())
		permsArray.add("tw"); 
	if (writeResponse())
		permsArray.add("wr"); 
	if (additionalAuth())
		permsArray.add("aa"); 
}


const char* HAPCharacteristic::unitString(unit unitValue) {
	switch (unitValue) {
		case unit_arcDegree:                
			return "°";                
		case unit_celsius:                
			return "°C";                    
		case unit_percentage:
			return "%";
		case unit_lux:
			return "lux";             
		case unit_seconds:
			return "seconds";
			
		case unit_hpa:
			return "hPa";
		case unit_watt:
			return "W";
		case unit_voltage:
			return "V";
		case unit_kwh:
			return "kWh";
		case unit_kmh:
			return "km/h";
		case unit_km:
			return "km";
		case unit_m:
			return "m";
		case unit_mm:
			return "mm";
		case unit_kelvin:
			return "K";
		case unit_DU:
			return "DU";            
		case unit_mired:
			return "Mired";
		case unit_ppm:
			return "ppm";

		case unit_none:
			return "";                
		default:
			return "";
	}
}


const char* HAPCharacteristic::unitJson(unit unitValue) {
	switch (unitValue) {
		case unit_arcDegree:                
			return "arcdegrees";                
		case unit_celsius:                
			return "celsius";                    
		case unit_percentage:
			return "percentage";
		case unit_lux:
			return "lux";             
		case unit_seconds:
			return "seconds";

		case unit_hpa:
			return "hPa";
		case unit_watt:
			return "W";
		case unit_voltage:
			return "V";
		case unit_kwh:
			return "kWh";
		case unit_kmh:
			return "km/h";
		case unit_km:
			return "km";
		case unit_m:
			return "m";
		case unit_mm:
			return "mm";
		case unit_kelvin:
			return "K";
		case unit_DU:
			return "DU";            
		case unit_mired:
			return "Mired";
		case unit_ppm:
			return "ppm";

		case unit_none:
			return "";                
		default:
			return "";
	}
}