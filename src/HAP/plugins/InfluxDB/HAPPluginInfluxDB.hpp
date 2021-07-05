//
// HAPPluginInfluxDB.hpp
// Homekit
//
//  Created on: 29.04.2018
//      Author: michael
//

#ifndef HAPPLUGININFLUXDB_HPP_
#define HAPPLUGININFLUXDB_HPP_

#include <Arduino.h>

#include "HAPPlugins.hpp"
#include "HAPLogger.hpp"
#include "HAPAccessory.hpp"
#include "HAPGlobals.hpp"
#include <InfluxDb.h>
#include "HAPCharacteristics.hpp"

struct HAPPluginInfluxDBConfig {
	uint16_t port = 0;
	char username[32 + 1] 	= {0,};
	char password[32 + 1] 	= {0,};
	char database[32 + 1] 	= {0,};
	char url[255 + 1] 		= {0,};

	size_t getDataSize(){
		return sizeof(username) + sizeof(password) + sizeof(url) + 	sizeof(database) + sizeof(uint16_t);
	}

	HAPPluginInfluxDBConfig(uint16_t port_, const char* username_, const char* password_, const char* database_, const char* url_){
		port = port_;

		if (strlen(username_) > 32) {
			username[0] = '\0';
		} else {
			memcpy(username, username_, strlen(username_));
			username[strlen(username_) > 32 ? 32 : strlen(username_)] = '\0';
		}

		if (strlen(password_) > 32) {
			password[0] = '\0';
		} else {
			memcpy(password, password_, strlen(password_));
			password[strlen(password_) > 32 ? 32 : strlen(password_)] = '\0';
		}

		if (strlen(database_) > 32) {
			database[0] = '\0';
		} else {
			memcpy(database, database_, strlen(database_));
			database[strlen(database_) > 32 ? 32 : strlen(database_)] = '\0';
		}

		if (strlen(url_) > 32) {
			url[0] = '\0';
		} else {
			memcpy(url, url_, strlen(url_));
			url[strlen(url_) > 255 ? 255 : strlen(url_)] = '\0';
		}
	}


};

inline const char* characteristicsName(uint16_t type){
    switch(type) {
        case HAP_CHARACTERISTIC_ACCESSORY_FLAGS:                          // 0xA6     ==   166
            return "AccessoryFlags";
        case HAP_CHARACTERISTIC_ACTIVE:                                   // 0xB0     ==   176
            return "Active";
        case HAP_CHARACTERISTIC_ADMINISTRATOR_ONLY_ACCESS:                // 0x1      ==     1
            return "AdministratorOnlyAccess";
        case HAP_CHARACTERISTIC_AIR_PARTICULATE_DENSITY:                  // 0x64     ==   100
            return "AirParticulateDensity";
        case HAP_CHARACTERISTIC_AIR_PARTICULATE_SIZE:                     // 0x65     ==   101
            return "AirParticulateSize";
        case HAP_CHARACTERISTIC_AIR_QUALITY:                              // 0x95     ==   149
            return "AirQuality";
        case HAP_CHARACTERISTIC_AUDIO_FEEDBACK:                           // 0x5      ==     5
            return "AudioFeedback";
        case HAP_CHARACTERISTIC_BATTERY_LEVEL:                            // 0x68     ==   104
            return "BatteryLevel";
        case HAP_CHARACTERISTIC_BRIGHTNESS:                               // 0x8      ==     8
            return "Brightness";
        case HAP_CHARACTERISTIC_CARBON_DIOXIDE_DETECTED:                  // 0x92     ==   146
            return "CarbonDioxideDetected";
        case HAP_CHARACTERISTIC_CARBON_DIOXIDE_LEVEL:                     // 0x93     ==   147
            return "CarbonDioxideLevel";
        case HAP_CHARACTERISTIC_CARBON_DIOXIDE_PEAK_LEVEL:                // 0x94     ==   148
            return "CarbonDioxidePeakLevel";
        case HAP_CHARACTERISTIC_CARBON_MONOXIDE_DETECTED:                 // 0x69     ==   105
            return "CarbonMonoxideDetected";
        case HAP_CHARACTERISTIC_CARBON_MONOXIDE_LEVEL:                    // 0x90     ==   144
            return "CarbonMonoxideLevel";
        case HAP_CHARACTERISTIC_CARBON_MONOXIDE_PEAK_LEVEL:               // 0x91     ==   145
            return "CarbonMonoxidePeakLevel";
        case HAP_CHARACTERISTIC_CHARGING_STATE:                           // 0x8F     ==   143
            return "ChargingState";
        case HAP_CHARACTERISTIC_COLOR_TEMPERATURE:                        // 0xCE     ==   206
            return "ColorTemperature";
        case HAP_CHARACTERISTIC_CONTACT_SENSOR_STATE:                     // 0x6A     ==   106
            return "ContactSensorState";
        case HAP_CHARACTERISTIC_COOLING_THRESHOLD_TEMPERATURE:            // 0xD      ==    13
            return "CoolingThresholdTemperature";
        case HAP_CHARACTERISTIC_CURRENT_AIR_PURIFIER_STATE:               // 0xA9     ==   169
            return "CurrentAirPurifierState";
        case HAP_CHARACTERISTIC_CURRENT_AMBIENT_LIGHT_LEVEL:              // 0x6B     ==   107
            return "CurrentAmbientLightLevel";
        case HAP_CHARACTERISTIC_CURRENT_DOOR_STATE:                       // 0xE      ==    14
            return "CurrentDoorState";
        case HAP_CHARACTERISTIC_CURRENT_FAN_STATE:                        // 0xAF     ==   175
            return "CurrentFanState";
        case HAP_CHARACTERISTIC_CURRENT_HEATER_COOLER_STATE:              // 0xB1     ==   177
            return "CurrentHeaterCoolerState";
        case HAP_CHARACTERISTIC_CURRENT_HEATING_COOLING_STATE:            // 0xF      ==    15
            return "CurrentHeatingCoolingState";
        case HAP_CHARACTERISTIC_CURRENT_HORIZONTAL_TILT_ANGLE:            // 0x6C     ==   108
            return "CurrentHorizontalTiltAngle";
        case HAP_CHARACTERISTIC_CURRENT_HUMIDIFIER_DEHUMIDIFIER_STATE:    // 0xB3     ==   179
            return "CurrentHumidifierDehumidifierState";
        case HAP_CHARACTERISTIC_CURRENT_POSITION:                         // 0x6D     ==   109
            return "CurrentPosition";
        case HAP_CHARACTERISTIC_CURRENT_RELATIVE_HUMIDITY:                // 0x10     ==    16
            return "CurrentRelativeHumidity";
        case HAP_CHARACTERISTIC_CURRENT_SLAT_STATE:                       // 0xAA     ==   170
            return "CurrentSlatState";
        case HAP_CHARACTERISTIC_CURRENT_TEMPERATURE:                      // 0x11     ==    17
            return "CurrentTemperature";
        case HAP_CHARACTERISTIC_CURRENT_TILT_ANGLE:                       // 0xC1     ==   193
            return "CurrentTiltAngle";
        case HAP_CHARACTERISTIC_CURRENT_VERTICAL_TILT_ANGLE:              // 0x6E     ==   110
            return "CurrentVerticalTiltAngle";
        case HAP_CHARACTERISTIC_DIGITAL_ZOOM:                             // 0x11D    ==   285
            return "DigitalZoom";
        case HAP_CHARACTERISTIC_FILTER_CHANGE_INDICATION:                 // 0xAC     ==   172
            return "FilterChangeIndication";
        case HAP_CHARACTERISTIC_FILTER_LIFE_LEVEL:                        // 0xAB     ==   171
            return "FilterLifeLevel";
        case HAP_CHARACTERISTIC_FIRMWARE_REVISION:                        // 0x52     ==    82
            return "FirmwareRevision";
        case HAP_CHARACTERISTIC_HARDWARE_REVISION:                        // 0x53     ==    83
            return "HardwareRevision";
        case HAP_CHARACTERISTIC_HEATING_THRESHOLD_TEMPERATURE:            // 0x12     ==    18
            return "HeatingThresholdTemperature";
        case HAP_CHARACTERISTIC_HOLD_POSITION:                            // 0x6F     ==   111
            return "HoldPosition";
        case HAP_CHARACTERISTIC_HUE:                                      // 0x13     ==    19
            return "Hue";
        case HAP_CHARACTERISTIC_IDENTIFY:                                 // 0x14     ==    20
            return "Identify";
        case HAP_CHARACTERISTIC_IMAGE_MIRRORING:                          // 0x11F    ==   287
            return "ImageMirroring";
        case HAP_CHARACTERISTIC_IMAGE_ROTATION:                           // 0x11E    ==   286
            return "ImageRotation";
        case HAP_CHARACTERISTIC_IN_USE:                                   // 0xD2     ==   210
            return "InUse";
        case HAP_CHARACTERISTIC_IS_CONFIGURED:                            // 0xD6     ==   214
            return "IsConfigured";
        case HAP_CHARACTERISTIC_LEAK_DETECTED:                            // 0x70     ==   112
            return "LeakDetected";
        case HAP_CHARACTERISTIC_LOCK_CONTROL_POINT:                       // 0x19     ==    25
            return "LockControlPoint";
        case HAP_CHARACTERISTIC_LOCK_CURRENT_STATE:                       // 0x1D     ==    29
            return "LockCurrentState";
        case HAP_CHARACTERISTIC_LOCK_LAST_KNOWN_ACTION:                   // 0x1C     ==    28
            return "LockLastKnownAction";
        case HAP_CHARACTERISTIC_LOCK_MANAGEMENT_AUTO_SECURITY_TIMEOUT:    // 0x1A     ==    26
            return "LockManagementAutoSecurityTimeout";
        case HAP_CHARACTERISTIC_LOCK_PHYSICAL_CONTROLS:                   // 0xA7     ==   167
            return "LockPhysicalControls";
        case HAP_CHARACTERISTIC_LOCK_TARGET_STATE:                        // 0x1E     ==    30
            return "LockTargetState";
        case HAP_CHARACTERISTIC_LOGS:                                     // 0x1F     ==    31
            return "Logs";
        case HAP_CHARACTERISTIC_MANUFACTURER:                             // 0x20     ==    32
            return "Manufacturer";
        case HAP_CHARACTERISTIC_MODEL:                                    // 0x21     ==    33
            return "Model";
        case HAP_CHARACTERISTIC_MOTION_DETECTED:                          // 0x22     ==    34
            return "MotionDetected";
        case HAP_CHARACTERISTIC_MUTE:                                     // 0x11A    ==   282
            return "Mute";
        case HAP_CHARACTERISTIC_NAME:                                     // 0x23     ==    35
            return "Name";
        case HAP_CHARACTERISTIC_NIGHT_VISION:                             // 0x11B    ==   283
            return "NightVision";
        case HAP_CHARACTERISTIC_NITROGEN_DIOXIDE_DENSITY:                 // 0xC4     ==   196
            return "NitrogenDioxideDensity";
        case HAP_CHARACTERISTIC_OBSTRUCTION_DETECTED:                     // 0x24     ==    36
            return "ObstructionDetected";
        case HAP_CHARACTERISTIC_OCCUPANCY_DETECTED:                       // 0x71     ==   113
            return "OccupancyDetected";
        case HAP_CHARACTERISTIC_ON:                                       // 0x25     ==    37
            return "On";
        case HAP_CHARACTERISTIC_OPTICAL_ZOOM:                             // 0x11C    ==   284
            return "OpticalZoom";
        case HAP_CHARACTERISTIC_OUTLET_IN_USE:                            // 0x26     ==    38
            return "OutletInUse";
        case HAP_CHARACTERISTIC_OZONE_DENSITY:                            // 0xC3     ==   195
            return "OzoneDensity";
        case HAP_CHARACTERISTIC_PAIRING_FEATURES:                         // 0x4F     ==    79
            return "PairingFeatures";
        case HAP_CHARACTERISTIC_PAIRING_PAIRINGS:                         // 0x50     ==    80
            return "PairingPairings";
        case HAP_CHARACTERISTIC_PAIR_SETUP:                               // 0x4C     ==    76
            return "PairSetup";
        case HAP_CHARACTERISTIC_PAIR_VERIFY:                              // 0x4E     ==    78
            return "PairVerify";
        case HAP_CHARACTERISTIC_PM10_DENSITY:                             // 0xC7     ==   199
            return "PM10Density";
        case HAP_CHARACTERISTIC_PM2_5_DENSITY:                            // 0xC6     ==   198
            return "PM2.5Density";
        case HAP_CHARACTERISTIC_POSITION_STATE:                           // 0x72     ==   114
            return "PositionState";
        case HAP_CHARACTERISTIC_PROGRAMMABLE_SWITCH_EVENT:                // 0x73     ==   115
            return "ProgrammableSwitchEvent";
        case HAP_CHARACTERISTIC_PROGRAM_MODE:                             // 0xD1     ==   209
            return "ProgramMode";
        case HAP_CHARACTERISTIC_RELATIVE_HUMIDITY_DEHUMIDIFIER_THRESHOLD: // 0xC9     ==   201
            return "RelativeHumidityDehumidifierThreshold";
        case HAP_CHARACTERISTIC_RELATIVE_HUMIDITY_HUMIDIFIER_THRESHOLD:   // 0xCA     ==   202
            return "RelativeHumidityHumidifierThreshold";
        case HAP_CHARACTERISTIC_REMAINING_DURATION:                       // 0xD4     ==   212
            return "RemainingDuration";
        case HAP_CHARACTERISTIC_RESET_FILTER_INDICATION:                  // 0xAD     ==   173
            return "ResetFilterIndication";
        case HAP_CHARACTERISTIC_ROTATION_DIRECTION:                       // 0x28     ==    40
            return "RotationDirection";
        case HAP_CHARACTERISTIC_ROTATION_SPEED:                           // 0x29     ==    41
            return "RotationSpeed";
        case HAP_CHARACTERISTIC_SATURATION:                               // 0x2F     ==    47
            return "Saturation";
        case HAP_CHARACTERISTIC_SECURITY_SYSTEM_ALARM_TYPE:               // 0x8E     ==   142
            return "SecuritySystemAlarmType";
        case HAP_CHARACTERISTIC_SECURITY_SYSTEM_CURRENT_STATE:            // 0x66     ==   102
            return "SecuritySystemCurrentState";
        case HAP_CHARACTERISTIC_SECURITY_SYSTEM_TARGET_STATE:             // 0x67     ==   103
            return "SecuritySystemTargetState";
        case HAP_CHARACTERISTIC_SELECTED_RTP_STREAM_CONFIGURATION:        // 0x117    ==   279
            return "SelectedRTPStreamConfiguration";
        case HAP_CHARACTERISTIC_SERIAL_NUMBER:                            // 0x30     ==    48
            return "SerialNumber";
        case HAP_CHARACTERISTIC_SERVICE_LABEL_INDEX:                      // 0xCB     ==   203
            return "ServiceLabelIndex";
        case HAP_CHARACTERISTIC_SERVICE_LABEL_NAMESPACE:                  // 0xCD     ==   205
            return "ServiceLabelNamespace";
        case HAP_CHARACTERISTIC_SETUP_ENDPOINTS:                          // 0x118    ==   280
            return "SetupEndpoints";
        case HAP_CHARACTERISTIC_SET_DURATION:                             // 0xD3     ==   211
            return "SetDuration";
        case HAP_CHARACTERISTIC_SLAT_TYPE:                                // 0xC0     ==   192
            return "SlatType";
        case HAP_CHARACTERISTIC_SMOKE_DETECTED:                           // 0x76     ==   118
            return "SmokeDetected";
        case HAP_CHARACTERISTIC_STATUS_ACTIVE:                            // 0x75     ==   117
            return "StatusActive";
        case HAP_CHARACTERISTIC_STATUS_FAULT:                             // 0x77     ==   119
            return "StatusFault";
        case HAP_CHARACTERISTIC_STATUS_JAMMED:                            // 0x78     ==   120
            return "StatusJammed";
        case HAP_CHARACTERISTIC_STATUS_LOW_BATTERY:                       // 0x79     ==   121
            return "StatusLowBattery";
        case HAP_CHARACTERISTIC_STATUS_TAMPERED:                          // 0x7A     ==   122
            return "StatusTampered";
        case HAP_CHARACTERISTIC_STREAMING_STATUS:                         // 0x120    ==   288
            return "StreamingStatus";
        case HAP_CHARACTERISTIC_SULPHUR_DIOXIDE_DENSITY:                  // 0xC5     ==   197
            return "SulphurDioxideDensity";
        case HAP_CHARACTERISTIC_SUPPORTED_AUDIO_STREAM_CONFIGURATION:     // 0x115    ==   277
            return "SupportedAudioStreamConfiguration";
        case HAP_CHARACTERISTIC_SUPPORTED_RTP_CONFIGURATION:              // 0x116    ==   278
            return "SupportedRTPConfiguration";
        case HAP_CHARACTERISTIC_SUPPORTED_VIDEO_STREAM_CONFIGURATION:     // 0x114    ==   276
            return "SupportedVideoStreamConfiguration";
        case HAP_CHARACTERISTIC_SWING_MODE:                               // 0xB6     ==   182
            return "SwingMode";
        case HAP_CHARACTERISTIC_TARGET_AIR_PURIFIER_STATE:                // 0xA8     ==   168
            return "TargetAirPurifierState";
        case HAP_CHARACTERISTIC_TARGET_AIR_QUALITY:                       // 0xAE     ==   174
            return "TargetAirQuality";
        case HAP_CHARACTERISTIC_TARGET_DOOR_STATE:                        // 0x32     ==    50
            return "TargetDoorState";
        case HAP_CHARACTERISTIC_TARGET_FAN_STATE:                         // 0xBF     ==   191
            return "TargetFanState";
        case HAP_CHARACTERISTIC_TARGET_HEATER_COOLER_STATE:               // 0xB2     ==   178
            return "TargetHeaterCoolerState";
        case HAP_CHARACTERISTIC_TARGET_HEATING_COOLING_STATE:             // 0x33     ==    51
            return "TargetHeatingCoolingState";
        case HAP_CHARACTERISTIC_TARGET_HORIZONTAL_TILT_ANGLE:             // 0x7B     ==   123
            return "TargetHorizontalTiltAngle";
        case HAP_CHARACTERISTIC_TARGET_HUMIDIFIER_DEHUMIDIFIER_STATE:     // 0xB4     ==   180
            return "TargetHumidifierDehumidifierState";
        case HAP_CHARACTERISTIC_TARGET_POSITION:                          // 0x7C     ==   124
            return "TargetPosition";
        case HAP_CHARACTERISTIC_TARGET_RELATIVE_HUMIDITY:                 // 0x34     ==    52
            return "TargetRelativeHumidity";
        case HAP_CHARACTERISTIC_TARGET_SLAT_STATE:                        // 0xBE     ==   190
            return "TargetSlatState";
        case HAP_CHARACTERISTIC_TARGET_TEMPERATURE:                       // 0x35     ==    53
            return "TargetTemperature";
        case HAP_CHARACTERISTIC_TARGET_TILT_ANGLE:                        // 0xC2     ==   194
            return "TargetTiltAngle";
        case HAP_CHARACTERISTIC_TARGET_VERTICAL_TILT_ANGLE:               // 0x7D     ==   125
            return "TargetVerticalTiltAngle";
        case HAP_CHARACTERISTIC_TEMPERATURE_DISPLAY_UNITS:                // 0x36     ==    54
            return "TemperatureDisplayUnits";
        case HAP_CHARACTERISTIC_VALVE_TYPE:                               // 0xD5     ==   213
            return "ValveType";
        case HAP_CHARACTERISTIC_VERSION:                                  // 0x37     ==    55
            return "Version";
        case HAP_CHARACTERISTIC_VOC_DENSITY:                              // 0xC8     ==   200
            return "VOCDensity";
        case HAP_CHARACTERISTIC_VOLUME:                                   // 0x119    ==   281
            return "Volume";
        case HAP_CHARACTERISTIC_WATER_LEVEL:                              // 0xB5     ==   181
            return "WaterLevel";
        default:
            return "";
    }
}


class HAPPluginInfluxDB: public HAPPlugin {
public:

	HAPPluginInfluxDB();
	~HAPPluginInfluxDB() {
		delete _configInternal;
		delete _influxdb;
	}

	HAPAccessory* initAccessory() override;

	bool begin();


    void setValue(String oldValue, String newValue){};
    String getValue(int iid) { return ""; };

    void handleImpl(bool forced = false);
	void handleEvents(int eventCode, struct HAPEvent eventParam);

#if HAP_ENABLE_WEBSERVER
	HAPConfigurationValidationResult validateConfig(JsonObject object);

	JsonObject getConfigImpl();
	void setConfigImpl(JsonObject root);
#endif

	void addEventListener(EventManager* eventManager);

	HAPConfigurationPlugin* setDefaults() override;
	void internalConfigToJson(Print& prt);
	void setConfiguration(HAPConfigurationPlugin* cfg) override;


protected:

	Influxdb* _influxdb;

	HAPPluginInfluxDBConfig* _configInternal;

	uint16_t _usedSize;

	// bool shouldHandle();
};

REGISTER_PLUGIN(HAPPluginInfluxDB)

#endif /* HAPPLUGININFLUXDB_HPP_ */