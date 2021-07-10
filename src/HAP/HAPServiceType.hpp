//
// HAPServiceType.hpp
// Homekit
//
//  Created on: 10.07.2021
//      Author: michael
//

#ifndef HAPSERVICETYPE_HPP_
#define HAPSERVICETYPE_HPP_

enum class HAPServiceType : uint16_t {
    AccessoryInformation                     = 0x003E,
    Fan                                      = 0x0040,
    GarageDoorOpener                         = 0x0041,
    Lightbulb                                = 0x0043,
    LockManagement                           = 0x0044,
    LockMechanism                            = 0x0045,
    Outlet                                   = 0x0047,
    Switch                                   = 0x0049,
    Thermostat                               = 0x004A,
    SecuritySystem                           = 0x007E,
    CarbonMonoxideSensor                     = 0x007F,
    ContactSensor                            = 0x0080,
    Door                                     = 0x0081,
    HumiditySensor                           = 0x0082,
    LeakSensor                               = 0x0083,
    LightSensor                              = 0x0084,
    MotionSensor                             = 0x0085,
    OccupancySensor                          = 0x0086,
    SmokeSensor                              = 0x0087,
    StatelessProgrammableSwitch              = 0x0089,
    TemperatureSensor                        = 0x008A,
    Window                                   = 0x008B,
    WindowCovering                           = 0x008C,
    AirQualitySensor                         = 0x008D,
    BatteryService                           = 0x0096,
    CarbonDioxideSensor                      = 0x0097,
    FanV2                                    = 0x00B7,
    Slat                                     = 0x00B9,
    FilterMaintenance                        = 0x00BA,
    AirPurifier                              = 0x00BB,
    HeaterCooler                             = 0x00BC,
    HumidifierDehumidifier                   = 0x00BD,
    ServiceLabel                             = 0x00CC,
    IrrigationSystem                         = 0x00CF,
    Valve                                    = 0x00D0,
    Faucet                                   = 0x00D7,
    CameraRtpStreamManagement                = 0x0110,
    Microphone                               = 0x0112,
    Speaker                                  = 0x0113,
    Doorbell                                 = 0x0121
};


#endif /* HAPSERVICETYPE_HPP_ */