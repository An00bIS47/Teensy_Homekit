//
// HAPAccessoryType.hpp
// Homekit
//
//  Created on: 10.07.2021
//      Author: michael
//

#ifndef HAPACCESSORYTYPE_HPP_
#define HAPACCESSORYTYPE_HPP_

enum class HAPAccessoryType : uint8_t {
    Unknown              = 0x00,
    Other                = 0x01,
    Bridge               = 0x02,
    Fan                  = 0x03,
    GarageDoorOpener     = 0x04,
    Lightbulb            = 0x05,
    DoorLock             = 0x06,
    Outlet               = 0x07,
    Switch               = 0x08,
    Thermostat           = 0x09,
    Sensor               = 0x0A,
    SecuritySystem       = 0x0B,
    Door                 = 0x0C,
    Window               = 0x0D,
    WindowCovering       = 0x0E,
    ProgrammableSwitch   = 0x0F,
    IpCamera             = 0x11,
    VideoDoorbell        = 0x12,
    AirPurifier          = 0x13,
    Heater               = 0x14,
    AirConditioner       = 0x15,
    Humidifier           = 0x16,
    Dehumidifier         = 0x17,
    Sprinklers           = 0x1C,
    Faucets              = 0x1D,
    ShowerSystems        = 0x1E
};


#endif /* HAPACCESSORYTYPE_HPP_ */
