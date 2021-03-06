
//
// main.cpp
// Homekit
//
//  Created on: 08.08.2017
//      Author: michael
//
#include <Arduino.h>
#include "HAP/HAPServer.hpp"

#if TEENSY_DEBUG
#include "TeensyDebug.h"
#pragma GCC optimize ("O0")
#endif



// #if HAP_ENABLE_WEBSERVER_CORE_0
// #include "HAP/HAPWebServer.hpp"
// HAPWebServer* _webserver;
// #endif



// // unsigned long previousMillis = 0;

// // const long interval = 1000;

// #if HAP_ENABLE_WEBSERVER_CORE_0
// void taskWebserver( void * parameter )
// {
// 	_webserver = new HAPWebServer();
// 	_webserver->setAccessorySet(hap.getAccessorySet());
// 	_webserver->begin();

//     while( true ){
//         _webserver->handle();
//         delay(1);
//     }

//     // Serial.println("Ending task 1");
//     // vTaskDelete( NULL );
// }
// #endif

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void setup() {

	Serial.begin(115200);



#if TEENSY_DEBUG
	debug.begin(SerialUSB1);
#endif
	while(!Serial){
		;
	}

#if HAP_DEBUG
	if ( Serial && CrashReport ) { // Make sure Serial is alive and there is a CrashReport stored.
    	Serial.print(CrashReport); // Once called any crash data is cleared
    	// In this case USB Serial is used - but any Stream capable output will work : SD Card or other UART Serial
  	}
#endif

	// Imprint infos to firmware
	Homekit_setFirmware("Homekit", HOMEKIT_VERSION, HOMEKIT_FEATURE_REV);
	Homekit_setBrand(HAP_MANUFACTURER);

#if defined(TEENSY_DEBUG)
	// halt();
#endif

	// Start homekit
	hap.begin();




// #if HAP_ENABLE_WEBSERVER_CORE_0
// 	xTaskCreatePinnedToCore(
// 					taskWebserver,   /* Function to implement the task */
//                     "coreTask", /* Name of the task */
//                     8192,      	/* Stack size in words */
//                     NULL,       /* Task input parameter */
//                     1,          /* Priority of the task */
//                     NULL,       /* Task handle. */
//                     0);  		/* Core where the task should run */
// #endif



}

void loop(){
	hap.handle();
}