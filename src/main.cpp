#include <Arduino.h>
#include "HAP/HAPLogger.hpp"
#include "HAP/HAPServer.hpp"
#include "HAP/HAPGlobals.hpp"
#include "HAP/HAPHelper.hpp"
#include "HAP/HAPVersion.hpp"

#include <knx.h>

// create named references for easy access to group objects
#define goCurrent knx.getGroupObject(1)
#define goMax knx.getGroupObject(2)
#define goMin knx.getGroupObject(3)
#define goReset knx.getGroupObject(4)

float currentValue = 0;
float maxValue = 0;
float minValue = RAND_MAX;
long lastsend = 0;

void measureTemp()
{
    long now = millis();
    if ((now - lastsend) < 2000)
        return;

    lastsend = now;
    int r = rand();
    currentValue = (r * 1.0) / (RAND_MAX * 1.0);
    currentValue *= 100 * 100;

    // write new value to groupobject
    goCurrent.value(currentValue);

    if (currentValue > maxValue)
    {
        maxValue = currentValue;
        goMax.value(maxValue);
    }

    if (currentValue < minValue)
    {
        minValue = currentValue;
        goMin.value(minValue);
    }
}

// callback from reset-GO
void resetCallback(GroupObject& go)
{
    if (go.value())
    {
        maxValue = 0;
        minValue = 10000;
    }
}

void setup()
{
    Serial.begin(115200);

    Serial.print("Starting on Teensy ...");
    while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo only  
    }
    Serial.println( "OK");
    
    ArduinoPlatform::SerialDebug = &Serial;


    // Imprint infos to firmware
	Homekit_setFirmware("Homekit", HOMEKIT_VERSION, HOMEKIT_FEATURE_REV);
	Homekit_setBrand(HAP_MANUFACTURER);

	LogI( F("Starting Homekit "), false);
	LogI( hap.versionString() + String( " ..."), true);
	LogI( F("Log level: "), false);
	LogI( String(HAPLogger::getLogLevel() ), true);
    Serial.send_now();

    randomSeed(millis());

    // read adress table, association table, groupobject table and parameters from eeprom
    knx.readMemory();

    // print values of parameters if device is already configured
    if (knx.configured())
    {
        // register callback for reset GO
        goReset.callback(resetCallback);
        goReset.dataPointType(DPT_Trigger);
        goCurrent.dataPointType(DPT_Value_Temp);
        goMin.dataPointType(DPT_Value_Temp);
        goMax.dataPointType(DPT_Value_Temp);

        Serial.print("Timeout: ");
        Serial.println(knx.paramByte(0));
        Serial.print("Zykl. senden: ");
        Serial.println(knx.paramByte(1));
        Serial.print("Min/Max senden: ");
        Serial.println(knx.paramByte(2));
        Serial.print("Aenderung senden: ");
        Serial.println(knx.paramByte(3));
        Serial.print("Abgleich: ");
        Serial.println(knx.paramByte(4));
    }

    // pin or GPIO the programming led is connected to. Default is LED_BUILTIN
    // knx.ledPin(LED_BUILTIN);
    // is the led active on HIGH or low? Default is LOW
    // knx.ledPinActiveOn(HIGH);
    // pin or GPIO programming button is connected to. Default is 0
    // knx.buttonPin(0);

    // start the framework.
    knx.start();

    // Start homekit
	hap.begin();

}

void loop()
{

    // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS
    // knx.loop();

    hap.handle();
    
    // only run the application code if the device was configured with ETS
    if (!knx.configured())
        return;


    measureTemp();
}
