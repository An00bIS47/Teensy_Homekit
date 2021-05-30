//
// HAPCharacteristics.hpp
// Homekit
//
//  Generated on: 23.09.2019
//

#ifndef HAPCHARACTERISTICS_HPP_
#define HAPCHARACTERISTICS_HPP_

#include <Arduino.h>

#ifndef HAP_ENABLE_BLUETOOTH
#define HAP_ENABLE_BLUETOOTH 0
#endif

#ifdef HAP_ENABLE_BLUETOOTH
#include "HAPCharacteristicsBluetooth.hpp"
#endif

typedef enum {
    HAP_CHARACTERISTIC_ACCESSORY_FLAGS                          = 0xA6,     //    uint32      pr|ev
    HAP_CHARACTERISTIC_ACTIVE                                   = 0xB0,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_ADMINISTRATOR_ONLY_ACCESS                = 0x1,      //    bool        pr|pw|ev
    HAP_CHARACTERISTIC_AIR_PARTICULATE_DENSITY                  = 0x64,     //    float       pr|ev
    HAP_CHARACTERISTIC_AIR_PARTICULATE_SIZE                     = 0x65,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_AIR_QUALITY                              = 0x95,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_AUDIO_FEEDBACK                           = 0x5,      //    bool        pr|pw|ev
    HAP_CHARACTERISTIC_BATTERY_LEVEL                            = 0x68,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_BRIGHTNESS                               = 0x8,      //    int         pr|pw|ev
    HAP_CHARACTERISTIC_CARBON_DIOXIDE_DETECTED                  = 0x92,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CARBON_DIOXIDE_LEVEL                     = 0x93,     //    float       pr|ev
    HAP_CHARACTERISTIC_CARBON_DIOXIDE_PEAK_LEVEL                = 0x94,     //    float       pr|ev
    HAP_CHARACTERISTIC_CARBON_MONOXIDE_DETECTED                 = 0x69,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CARBON_MONOXIDE_LEVEL                    = 0x90,     //    float       pr|ev
    HAP_CHARACTERISTIC_CARBON_MONOXIDE_PEAK_LEVEL               = 0x91,     //    float       pr|ev
    HAP_CHARACTERISTIC_CHARGING_STATE                           = 0x8F,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_COLOR_TEMPERATURE                        = 0xCE,     //    uint32      pr|pw|ev
    HAP_CHARACTERISTIC_CONTACT_SENSOR_STATE                     = 0x6A,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_COOLING_THRESHOLD_TEMPERATURE            = 0xD,      //    float       pr|pw|ev
    HAP_CHARACTERISTIC_CURRENT_AIR_PURIFIER_STATE               = 0xA9,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_AMBIENT_LIGHT_LEVEL              = 0x6B,     //    float       pr|ev
    HAP_CHARACTERISTIC_CURRENT_DOOR_STATE                       = 0xE,      //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_FAN_STATE                        = 0xAF,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_HEATER_COOLER_STATE              = 0xB1,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_HEATING_COOLING_STATE            = 0xF,      //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_HORIZONTAL_TILT_ANGLE            = 0x6C,     //    int         pr|ev
    HAP_CHARACTERISTIC_CURRENT_HUMIDIFIER_DEHUMIDIFIER_STATE    = 0xB3,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_POSITION                         = 0x6D,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY                = 0x10,     //    float       pr|ev
    HAP_CHARACTERISTIC_CURRENT_SLAT_STATE                       = 0xAA,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_CURRENT_TEMPERATURE                      = 0x11,     //    float       pr|ev
    HAP_CHARACTERISTIC_CURRENT_TILT_ANGLE                       = 0xC1,     //    int         pr|ev
    HAP_CHARACTERISTIC_CURRENT_VERTICAL_TILT_ANGLE              = 0x6E,     //    int         pr|ev
    HAP_CHARACTERISTIC_DIGITAL_ZOOM                             = 0x11D,    //    float       pr|pw|ev
    HAP_CHARACTERISTIC_FILTER_CHANGE_INDICATION                 = 0xAC,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_FILTER_LIFE_LEVEL                        = 0xAB,     //    float       pr|ev
    HAP_CHARACTERISTIC_FIRMWARE_REVISION                        = 0x52,     //    string      pr
    HAP_CHARACTERISTIC_HARDWARE_REVISION                        = 0x53,     //    string      pr
    HAP_CHARACTERISTIC_HEATING_THRESHOLD_TEMPERATURE            = 0x12,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_HOLD_POSITION                            = 0x6F,     //    bool        pw
    HAP_CHARACTERISTIC_HUE                                      = 0x13,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_IDENTIFY                                 = 0x14,     //    bool        pw
    HAP_CHARACTERISTIC_IMAGE_MIRRORING                          = 0x11F,    //    bool        pr|pw|ev
    HAP_CHARACTERISTIC_IMAGE_ROTATION                           = 0x11E,    //    float       pr|pw|ev
    HAP_CHARACTERISTIC_IN_USE                                   = 0xD2,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_IS_CONFIGURED                            = 0xD6,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_LEAK_DETECTED                            = 0x70,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_LOCK_CONTROL_POINT                       = 0x19,     //    tlv8        pw
    HAP_CHARACTERISTIC_LOCK_CURRENT_STATE                       = 0x1D,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_LOCK_LAST_KNOWN_ACTION                   = 0x1C,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_LOCK_MANAGEMENT_AUTO_SECURITY_TIMEOUT    = 0x1A,     //    uint32      pr|pw|ev
    HAP_CHARACTERISTIC_LOCK_PHYSICAL_CONTROLS                   = 0xA7,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_LOCK_TARGET_STATE                        = 0x1E,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_LOGS                                     = 0x1F,     //    tlv8        pr|ev
    HAP_CHARACTERISTIC_MANUFACTURER                             = 0x20,     //    string      pr
    HAP_CHARACTERISTIC_MODEL                                    = 0x21,     //    string      pr
    HAP_CHARACTERISTIC_MOTION_DETECTED                          = 0x22,     //    bool        pr|ev
    HAP_CHARACTERISTIC_MUTE                                     = 0x11A,    //    bool        pr|pw|ev
    HAP_CHARACTERISTIC_NAME                                     = 0x23,     //    string      pr
    HAP_CHARACTERISTIC_NIGHT_VISION                             = 0x11B,    //    bool        pr|pw|ev
    HAP_CHARACTERISTIC_NITROGEN_DIOXIDE_DENSITY                 = 0xC4,     //    float       pr|ev
    HAP_CHARACTERISTIC_OBSTRUCTION_DETECTED                     = 0x24,     //    bool        pr|ev
    HAP_CHARACTERISTIC_OCCUPANCY_DETECTED                       = 0x71,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_ON                                       = 0x25,     //    bool        pr|pw|ev
    HAP_CHARACTERISTIC_OPTICAL_ZOOM                             = 0x11C,    //    float       pr|pw|ev
    HAP_CHARACTERISTIC_OUTLET_IN_USE                            = 0x26,     //    bool        pr|ev
    HAP_CHARACTERISTIC_OZONE_DENSITY                            = 0xC3,     //    float       pr|ev
    HAP_CHARACTERISTIC_PAIRING_FEATURES                         = 0x4F,     //    uint8       pr
    HAP_CHARACTERISTIC_PAIRING_PAIRINGS                         = 0x50,     //    tlv8        pr|pw
    HAP_CHARACTERISTIC_PAIR_SETUP                               = 0x4C,     //    tlv8        pr|pw
    HAP_CHARACTERISTIC_PAIR_VERIFY                              = 0x4E,     //    tlv8        pr|pw
    HAP_CHARACTERISTIC_PM10_DENSITY                             = 0xC7,     //    float       pr|ev
    HAP_CHARACTERISTIC_PM2_5_DENSITY                            = 0xC6,     //    float       pr|ev
    HAP_CHARACTERISTIC_POSITION_STATE                           = 0x72,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_PROGRAMMABLE_SWITCH_EVENT                = 0x73,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_PROGRAM_MODE                             = 0xD1,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_RELATIVE_HUMIDITY_DEHUMIDIFIER_THRESHOLD = 0xC9,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_RELATIVE_HUMIDITY_HUMIDIFIER_THRESHOLD   = 0xCA,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_REMAINING_DURATION                       = 0xD4,     //    uint32      pr|ev
    HAP_CHARACTERISTIC_RESET_FILTER_INDICATION                  = 0xAD,     //    uint8       pw
    HAP_CHARACTERISTIC_ROTATION_DIRECTION                       = 0x28,     //    int         pr|pw|ev
    HAP_CHARACTERISTIC_ROTATION_SPEED                           = 0x29,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_SATURATION                               = 0x2F,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_SECURITY_SYSTEM_ALARM_TYPE               = 0x8E,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_SECURITY_SYSTEM_CURRENT_STATE            = 0x66,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_SECURITY_SYSTEM_TARGET_STATE             = 0x67,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_SELECTED_RTP_STREAM_CONFIGURATION        = 0x117,    //    tlv8        pr|pw
    HAP_CHARACTERISTIC_SERIAL_NUMBER                            = 0x30,     //    string      pr
    HAP_CHARACTERISTIC_SERVICE_LABEL_INDEX                      = 0xCB,     //    uint8       pr
    HAP_CHARACTERISTIC_SERVICE_LABEL_NAMESPACE                  = 0xCD,     //    uint8       pr
    HAP_CHARACTERISTIC_SETUP_ENDPOINTS                          = 0x118,    //    tlv8        pr|pw
    HAP_CHARACTERISTIC_SET_DURATION                             = 0xD3,     //    uint32      pr|pw|ev
    HAP_CHARACTERISTIC_SLAT_TYPE                                = 0xC0,     //    uint8       pr
    HAP_CHARACTERISTIC_SMOKE_DETECTED                           = 0x76,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_STATUS_ACTIVE                            = 0x75,     //    bool        pr|ev
    HAP_CHARACTERISTIC_STATUS_FAULT                             = 0x77,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_STATUS_JAMMED                            = 0x78,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_STATUS_LOW_BATTERY                       = 0x79,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_STATUS_TAMPERED                          = 0x7A,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_STREAMING_STATUS                         = 0x120,    //    tlv8        pr|ev
    HAP_CHARACTERISTIC_SULPHUR_DIOXIDE_DENSITY                  = 0xC5,     //    float       pr|ev
    HAP_CHARACTERISTIC_SUPPORTED_AUDIO_STREAM_CONFIGURATION     = 0x115,    //    tlv8        pr
    HAP_CHARACTERISTIC_SUPPORTED_RTP_CONFIGURATION              = 0x116,    //    tlv8        pr
    HAP_CHARACTERISTIC_SUPPORTED_VIDEO_STREAM_CONFIGURATION     = 0x114,    //    tlv8        pr
    HAP_CHARACTERISTIC_SWING_MODE                               = 0xB6,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_AIR_PURIFIER_STATE                = 0xA8,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_AIR_QUALITY                       = 0xAE,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_DOOR_STATE                        = 0x32,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_FAN_STATE                         = 0xBF,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_HEATER_COOLER_STATE               = 0xB2,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_HEATING_COOLING_STATE             = 0x33,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_HORIZONTAL_TILT_ANGLE             = 0x7B,     //    int         pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_HUMIDIFIER_DEHUMIDIFIER_STATE     = 0xB4,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_POSITION                          = 0x7C,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_RELATIVE_HUMIDITY                 = 0x34,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_SLAT_STATE                        = 0xBE,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_TEMPERATURE                       = 0x35,     //    float       pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_TILT_ANGLE                        = 0xC2,     //    int         pr|pw|ev
    HAP_CHARACTERISTIC_TARGET_VERTICAL_TILT_ANGLE               = 0x7D,     //    int         pr|pw|ev
    HAP_CHARACTERISTIC_TEMPERATURE_DISPLAY_UNITS                = 0x36,     //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_VALVE_TYPE                               = 0xD5,     //    uint8       pr|ev
    HAP_CHARACTERISTIC_VERSION                                  = 0x37,     //    string      pr|ev
    HAP_CHARACTERISTIC_VOC_DENSITY                              = 0xC8,     //    float       pr|ev
    HAP_CHARACTERISTIC_VOLUME                                   = 0x119,    //    uint8       pr|pw|ev
    HAP_CHARACTERISTIC_WATER_LEVEL                              = 0xB5,     //    float       pr|ev
} HAP_CHARACTERISTIC;

#endif /* HAPCHARACTERISTICS_HPP_ */
