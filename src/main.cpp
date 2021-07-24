
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


#if TEENSY_DEBUG
// Create an IntervalTimer object
IntervalTimer debugLEDTimer;
const int ledPin = 41;  // the pin with a LED
// The interrupt will blink the LED, and keep
// track of how many times it has blinked.
int ledState = LOW;
volatile unsigned long blinkCount = 0; // use volatile for shared variables

// functions called by IntervalTimer should be short, run as quickly as
// possible, and should avoid calling other functions if possible.
void blinkLED() {
	if (ledState == LOW) {
		ledState = HIGH;
		blinkCount = blinkCount + 1;  // increase when LED turns on
	} else {
		ledState = LOW;
	}
	digitalWrite(ledPin, ledState);
}

#endif


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void setup() {

#if HAP_DEBUG
	Serial.begin(115200);
#endif


#if TEENSY_DEBUG
	debug.begin(SerialUSB1);
#endif

#if HAP_DEBUG
	while(!Serial){
		;
	}


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

#if TEENSY_DEBUG
	pinMode(ledPin, OUTPUT);
  	debugLEDTimer.begin(blinkLED, 150000);  // blinkLED to run every 0.15 seconds
#endif

}

void loop(){

	hap.handle();

#if TEENSY_DEBUG
	unsigned long blinkCopy;  // holds a copy of the blinkCount

	// to read a variable which the interrupt code writes, we
	// must temporarily disable interrupts, to be sure it will
	// not change while we are reading.  To minimize the time
	// with interrupts off, just quickly make a copy, and then
	// use the copy while allowing the interrupt to keep working.
	noInterrupts();
	blinkCopy = blinkCount;
	interrupts();

	Serial.print("blinkCount = ");
	Serial.println(blinkCopy);
#endif
}