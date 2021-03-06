//
// HAPGlobals.hpp
// Homekit
//
//  Created on: 08.08.2017
//      Author: michael
//
#ifndef HAPGLOBALS_HPP_
#define HAPGLOBALS_HPP_


// ToDo: Use KConfig

// #ifndef DEPRECATED
// #define DEPRECATED __attribute__ ((deprecated))
// #endif

/**
 * General
 ********************************************************************/
#define HAP_LOGLEVEL				LogLevel::DEBUG

#ifndef HAP_PIN_CODE
#define HAP_PIN_CODE 				"031-45-712"
#endif

#ifndef HAP_SETUP_ID
#define HAP_SETUP_ID 				"UPFT"
#endif


#if defined(ARDUINO_ARCH_ESP32)
#define HAP_HOSTNAME_PREFIX			"esp32"
#elif defined (CORE_TEENSY)
#define HAP_HOSTNAME_PREFIX			"T41"
#endif

#define HAP_MANUFACTURER			"An00bIS47"

#if defined(ARDUINO_ARCH_ESP32)
#define HAP_MODELL_NAME				"ESP32 KNX Homekit"
#elif defined (CORE_TEENSY)
#define HAP_MODELL_NAME				"Teensy41 KNX Homekit"
#endif


#ifndef HAP_RESET_EEPROM
#define HAP_RESET_EEPROM 			0		// ToDo: Deprecated
#endif

#ifndef HAP_RESET_PAIRINGS
#define HAP_RESET_PAIRINGS			0
#endif

#define HAP_BUTTON_PIN 				0       // PIN of onboard button


#define HAP_ADD_DESC_TO_JSON		1		// Adds description for each chr to accessory.json

#ifndef HAP_ENABLE_PIXEL_INDICATOR
#define HAP_ENABLE_PIXEL_INDICATOR 	0
#endif


#define HAP_PIXEL_INDICATOR_PIN		A0		// PIN of the NeoPixel inidicator pin for wifi connection etc
#define HAP_PIXEL_INIDICATOR_BRIGHTNESS 75


/**
 * Homekit Accessory Protocol
 ********************************************************************/
#define HOMEKIT_PROTOKOL_VERSION			"1.0"
#define HOMEKIT_CONFIGURATION_NUMBER 		1		// Internal - keep value at 1
#define HAP_BATTERY_LEVEL_LOW_THRESHOLD		15		// Battery threshold for alert

/**
 * Debug
 ********************************************************************/
#ifndef HAP_DEBUG
#define HAP_DEBUG 					0
#endif

#ifndef HAP_DEBUG_HEAP
#define HAP_DEBUG_HEAP				0
#endif

#ifndef HAP_DEBUG_TLV8
#define HAP_DEBUG_TLV8				0
#endif

#ifndef HAP_DEBUG_HOMEKIT
#define HAP_DEBUG_HOMEKIT			0
#endif

#ifndef HAP_DEBUG_FAKEGATO
#define HAP_DEBUG_FAKEGATO			0
#endif

#ifndef HAP_DEBUG_FAKEGATO_DETAILED
#define HAP_DEBUG_FAKEGATO_DETAILED 0
#endif

#ifndef HAP_DEBUG_FAKEGATO_SCHEDULE
#define HAP_DEBUG_FAKEGATO_SCHEDULE 0
#endif

#ifndef HAP_DEBUG_PAIRINGS
#define HAP_DEBUG_PAIRINGS			0
#endif

#ifndef HAP_DEBUG_RESPONSES
#define HAP_DEBUG_RESPONSES			0
#endif

#ifndef HAP_DEBUG_ENCRYPTION
#define HAP_DEBUG_ENCRYPTION		0
#endif

#ifndef HAP_DEBUG_EVENTMANAGER
#define HAP_DEBUG_EVENTMANAGER		0
#endif

#ifndef HAP_DEBUG_KEYSTORE
#define HAP_DEBUG_KEYSTORE			0
#endif

#ifndef HAP_DEBUG_RF24
#define HAP_DEBUG_RF24				0
#endif

#ifndef HAP_DEBUG_WEBSERVER
#define HAP_DEBUG_WEBSERVER 		0
#endif

#ifndef HAP_DEBUG_MIFLORA
#define HAP_DEBUG_MIFLORA			0
#endif

#ifndef HAP_DEBUG_IR
#define HAP_DEBUG_IR				0
#endif

#ifndef HAP_DEBUG_CONFIGURATION
#define HAP_DEBUG_CONFIGURATION		0
#endif

/**
 * Homekit pairing
 ********************************************************************/
#ifndef HAP_ALLOW_PAIRING_WHILE_PAIRED
#define HAP_ALLOW_PAIRING_WHILE_PAIRED 1		// disable for release !!
#endif


/**
 * KNX
 ********************************************************************/
#ifndef HAP_SERVER_USE_INTEGRATED_KNX
#define HAP_SERVER_USE_INTEGRATED_KNX  0
#endif


/**
 * Platform: ESP32 / Teensy 4.1
 ********************************************************************/
#if defined(ARDUINO_ARCH_ESP32)
#ifndef HAP_ENABLE_WIFI
#define HAP_ENABLE_WIFI 			1
#endif

#ifndef HAP_ENABLE_KEYSTORE
#define HAP_ENABLE_KEYSTORE 		1
#endif

#ifndef HAP_ENABLE_WEBSERVER
#define HAP_ENABLE_WEBSERVER 		1
#endif

#ifndef HAP_PROVISIONING_ENABLE_BLE
#define HAP_PROVISIONING_ENABLE_BLE 1
#endif

#ifndef HAP_ENABLE_UPDATE_OTA
#define HAP_ENABLE_UPDATE_OTA 		1
#endif

//#define HAP_SPRINTF_UI32			"%zu"



#ifndef HAP_ENABLE_WIFI_BUTTON
#define HAP_ENABLE_WIFI_BUTTON		0
#endif

#ifndef HAP_USE_PREFERENCES
#define HAP_USE_PREFERENCES 		1
#endif

#elif defined ( CORE_TEENSY)
#ifndef HAP_ENABLE_WIFI
#define HAP_ENABLE_WIFI 			0
#endif

#ifndef HAP_ENABLE_KEYSTORE
#define HAP_ENABLE_KEYSTORE 		0
#endif

#ifndef HAP_ENABLE_WEBSERVER
#define HAP_ENABLE_WEBSERVER 		0
#endif

#ifndef HAP_PROVISIONING_ENABLE_BLE
#define HAP_PROVISIONING_ENABLE_BLE 0
#endif

#ifndef HAP_ENABLE_UPDATE_OTA
#define HAP_ENABLE_UPDATE_OTA 		0
#endif




#ifndef HAP_ENABLE_WIFI_BUTTON
#define HAP_ENABLE_WIFI_BUTTON		0
#endif

//#define HAP_SPRINTF_UI32			"%lu"

#ifndef HAP_USE_SPIFFS_CONFIGURATION
#define HAP_USE_SPIFFS_CONFIGURATION 0
#endif

#else	/* END CORE_TEENSY */
//#define HAP_SPRINTF_UI32			"%lu"
#endif

#define HAP_ETHERNET_TIMEOUT		10000




/**
 * WiFi
 ********************************************************************/

#ifndef HAP_ENABLE_WIFI
#define HAP_ENABLE_WIFI 	1
#endif


#if HAP_ENABLE_WIFI
#ifndef HAP_WIFI_MODE_DEFAULT
#define HAP_WIFI_MODE_DEFAULT			1	// 0 = HAPWiFiModeAccessPoint
#endif										// 1 = HAPWiFiModeMulti
											// 2 = HAPWiFiModeWPS			-> push button only
											// 3 = HAPWiFiModeSmartConfig	-> not working with ios13 ?
											// 4 = HAPWiFiModeBLEProv
											// 5 = HAPWiFiModeAPProv

#define HAP_WIFI_CONNECTION_MAX_RETRIES 5	// max retries for connection error
                                            // before switching back to default mode
											// default: 5
#define HAP_WIFI_CONNECTION_RETRY_DELAY	2000
#define ESP_WIFI_CONNECTION_TIMEOUT		20000

#ifndef HAP_PROVISIONING_ENABLE_BLE
#define HAP_PROVISIONING_ENABLE_BLE		1
#endif

#if HAP_PROVISIONING_ENABLE_BLE
#ifndef HAP_PROVISIONING_POP
#define HAP_PROVISIONING_POP			"abcd1234" 		// Proof of possesion
#endif
#ifndef HAP_PROVISIONING_PREFIX
#define HAP_PROVISIONING_PREFIX			"PROV_"			// Prefix for provisioning
#endif
#endif

#endif

/**
 * Include WiFi credentials if necessary
 ********************************************************************/
#if HAP_ENABLE_WIFI
#if HAP_WIFI_MODE_DEFAULT == 1

#include "../WiFiCredentials.hpp"

#ifndef WIFI_SSID
#error	No WiFi SSID defined!
#endif

#ifndef WIFI_PASSWORD
#error	No WiFi Password defined!
#endif

#endif
#endif

/**
 * Keystore
 ********************************************************************/
#ifndef HAP_ENABLE_KEYSTORE
#define HAP_ENABLE_KEYSTORE				0
#endif

#define HAP_KEYSTORE_PARTITION_LABEL	 	"keystore_0"
#define HAP_KEYSTORE_PARTITION_LABEL_ALT 	"keystore_1"
#define HAP_KEYSTORE_STORAGE_LABEL   		"keystore"


/**
 * WebServer
 ********************************************************************/
#ifndef HAP_ENABLE_WEBSERVER
#define HAP_ENABLE_WEBSERVER		0		// Enable Webinterface
#endif										// Default: 1

// #define HAP_ENABLE_WEBSERVER_CORE_0	0		// Run webserver on core 0 in a seperate task
// 											// default 0 - Still work-in-progress
// 											// currently could causes heap caps failures if used with BLE


#if HAP_ENABLE_WEBSERVER
#define HAP_WEBSERVER_USE_JWT 		0		// use JWT for access
											// TODO: Proper token signature verification
											// currently not implemented properly

#ifndef HAP_WEBSERVER_USE_SSL
#define HAP_WEBSERVER_USE_SSL		1		// use SSL for WebServer
#endif										// Default: 1


#ifndef HAP_WEBSERVER_ADMIN_USERNAME
#define HAP_WEBSERVER_ADMIN_USERNAME	"admin"
#endif


#ifndef HAP_WEBSERVER_ADMIN_PASSWORD
#define HAP_WEBSERVER_ADMIN_PASSWORD	"secret"
#endif


#ifndef HAP_WEBSERVER_API_USERNAME
#define HAP_WEBSERVER_API_USERNAME		"api"
#endif

#ifndef HAP_WEBSERVER_API_PASSWORD
#define HAP_WEBSERVER_API_PASSWORD		"test"
#endif


#define HAP_WEBSERVER_USE_SPIFFS	0
#define HTTPS_DISABLE_SELFSIGNING 	1		// Disable self signed certificate generation on the fly
#define DEBUG_MULTIPART_PARSER 		0		// Enable to debug multipart form parser


#if HAP_DEBUG_WEBSERVER == 1
#define HTTPS_LOGLEVEL 4					// Log Levels:
											// 0 : NONE
											// 1 : + ERROR
											// 2 : + WARNING
											// 3 : + INFO
											// 4 : + DEBUG
#define HTTPS_LOGTIMESTAMP					// Add Timestamp (uptime) to log
#endif
#endif


/**
 * Captive Portal
 ********************************************************************/
#define HAP_CAPTIVE_DNSSERVER_PORT	53
#define HAP_CAPTIVE_AP_IP			"192.168.0.1"
#define HAP_CAPTIVE_AP_SSID			"Homekit Captive Portal"	// ssid of the access point
#define HAP_CAPTIVE_TITLE			"Homekit Captive Portal"	// Title of the webpage for captive portal


/**
 * Fakegato
 ********************************************************************/

#ifndef HAP_FAKEGATO_BUFFER_SIZE
#define HAP_FAKEGATO_BUFFER_SIZE	768     // Number of history entries for each characteristic
#endif										// default: 768


#ifndef HAP_FAKEGATO_INTERVAL
#define HAP_FAKEGATO_INTERVAL       300000	// Interval to add entry to history in millis
#endif                                      // EVE app requires at least one entry every 10 mins
											// default: 300000

#ifndef HAP_FAKEGATO_CHUNK_SIZE
#define HAP_FAKEGATO_CHUNK_SIZE     16      // Number of entries sent at once from device to EVE app
#endif										// default: 16



/**
 * OTA + HAP Update Server
 ********************************************************************/

#ifndef HAP_ENABLE_UPDATE_OTA
#define HAP_ENABLE_UPDATE_OTA		0		// Enable ArduinoOTA
#endif										// Default: 1

#ifndef HAP_ENABLE_UPDATE_WEB
#define HAP_ENABLE_UPDATE_WEB 	0		// Use HAP update server to check
#endif										// if a update is available on the
											// provided webserver
											// Default: 0

#ifndef HAP_UPDATE_OTA_PASSWORD
#define HAP_UPDATE_OTA_PASSWORD		"secret"
#endif

#ifndef HAP_UPDATE_OTA_PORT
#define HAP_UPDATE_OTA_PORT			3232
#endif

#ifndef HAP_UPDATE_WEB_INTERVAL
#define HAP_UPDATE_WEB_INTERVAL		60000	// Interval for web update check in ms
#endif


#if HAP_ENABLE_UPDATE_WEB
//#define HAP_UPDATE_SERVER_URL 	"192.168.178.151"
#define HAP_UPDATE_SERVER_HOST 		"homebridge"		// HTTP Server url for updates
#define HAP_UPDATE_SERVER_PORT		3001				// Update Server port
#define HAP_UPDATE_ENABLE_SSL		1					// enable SSL for HAP Update
#endif


#define HAP_UPDATE_TIMEOUT 			2000





/**
 * EEPROM
 ********************************************************************/
#if defined(ARDUINO_TEENSY30) || defined(ARDUINO_TEENSY31) || defined(ARDUINO_TEENSY32)
#define HAP_EEPROM_SIZE 2048      // Teensy 3.0/3.1/3.2
#elif defined(ARDUINO_TEENSYLC)
#define HAP_EEPROM_SIZE 128       // Teensy LC
#elif defined(ARDUINO_TEENSY35) || defined(ARDUINO_TEENSY36)
#define HAP_EEPROM_SIZE 4096       // Teensy 3.5/3.6
#elif definedARDUINO_TEENSY40
#define HAP_EEPROM_SIZE 1080      // Teensy 4.0
#elif defined(ARDUINO_TEENSY41)
#define HAP_EEPROM_SIZE 4284      // Teensy 4.1
#else
#define HAP_EEPROM_SIZE 8192
#endif

#ifndef HAP_EEPROM_OFFSET
#define HAP_EEPROM_OFFSET 0
#endif /* HAP_EEPROM_OFFSET */


// ToDo: deprecated !
// #define HAP_EEPROM_OFFSET_PAIRINGS    	HAP_EEPROM_OFFSET
// #define HAP_EEPROM_PAIRINGS_SIZE      	(sizeof(HAPPairing) * HAP_PAIRINGS_MAX)
// #define HAP_EEPROM_PAIRINGS_KEYSIZE 	HAP_PAIRINGS_LTPK_LENGTH + HAP_PAIRINGS_LTSK_LENGTH
// #define HAP_EEPROM_CONFIG_OFFSET    	(HAP_EEPROM_OFFSET + HAP_EEPROM_PAIRINGS_KEYSIZE + HAP_EEPROM_PAIRINGS_SIZE)
// #define HAP_EEPROM_CONFIG_SIZE      	(HAP_EEPROM_SIZE - (HAP_EEPROM_OFFSET + HAP_EEPROM_PAIRINGS_KEYSIZE + HAP_EEPROM_PAIRINGS_SIZE))



/**
 * Event Manager
 ********************************************************************/
#ifndef HAP_EVENTMANAGER_LISTENER_SIZE
#define HAP_EVENTMANAGER_LISTENER_SIZE	32
#endif

#ifndef HAP_EVENTMANAGER_QUEUE_SIZE
#define HAP_EVENTMANAGER_QUEUE_SIZE		32	// ToDo: Increase for KNX
#endif


#define EVENTMANAGER_LISTENER_LIST_SIZE		HAP_EVENTMANAGER_LISTENER_SIZE
#define EVENTMANAGER_EVENT_QUEUE_SIZE 		HAP_EVENTMANAGER_QUEUE_SIZE






/**
 * QR Code
 ********************************************************************/
#ifndef HAP_PRINT_QRCODE
#define HAP_PRINT_QRCODE			0		// Print QR code on console
#endif


#ifndef HAP_PRINT_QRCODE_SVG
#define HAP_PRINT_QRCODE_SVG		0		// Print QR code as SVG on console
#endif

/**
 * Plugins
 * !!! Add new plugins on top here and
 *     add them as well on top of the define bellow !!!
 ********************************************************************/

#ifndef HAP_PLUGIN_USE_KNX
#define HAP_PLUGIN_USE_KNX				0
#endif

#ifndef HAP_PLUGIN_USE_NIMBLE_MIFLORA
#define HAP_PLUGIN_USE_NIMBLE_MIFLORA 	0		// https://github.com/h2zero/esp-nimble-cpp
#endif

#ifndef HAP_PLUGIN_USE_IR
#define HAP_PLUGIN_USE_IR				0
#endif

#ifndef HAP_PLUGIN_USE_FAN_HONEYWELL
#define HAP_PLUGIN_USE_FAN_HONEYWELL	0
#endif

#ifndef HAP_PLUGIN_USE_RF24
#define HAP_PLUGIN_USE_RF24				0
#endif

#ifndef HAP_PLUGIN_USE_SSD1306
#define HAP_PLUGIN_USE_SSD1306			0
#endif

#ifndef HAP_PLUGIN_USE_LED
#define HAP_PLUGIN_USE_LED				0
#endif

#ifndef HAP_PLUGIN_USE_SSD1331
#define HAP_PLUGIN_USE_SSD1331			0
#endif

#ifndef HAP_PLUGIN_USE_PCA301
#define HAP_PLUGIN_USE_PCA301			0
#endif

#ifndef HAP_PLUGIN_USE_NEOPIXEL
#define HAP_PLUGIN_USE_NEOPIXEL			0
#endif

#ifndef HAP_PLUGIN_USE_INFLUXDB
#define HAP_PLUGIN_USE_INFLUXDB			0
#endif

#ifndef HAP_PLUGIN_USE_HYGROMETER
#define HAP_PLUGIN_USE_HYGROMETER		0
#endif

#ifndef HAP_PLUGIN_USE_RCSWITCH
#define HAP_PLUGIN_USE_RCSWITCH			0
#endif

#ifndef HAP_PLUGIN_USE_DHT
#define HAP_PLUGIN_USE_DHT				0
#endif

#ifndef HAP_PLUGIN_USE_BME280
#define HAP_PLUGIN_USE_BME280			0	// < last digit of feature number
#endif

/**
 * Plugin Logic
 *
 ********************************************************************/
// Enable IR if Honeywell is enabled
#if HAP_PLUGIN_USE_FAN_HONEYWELL
#undef HAP_PLUGIN_USE_IR
#define HAP_PLUGIN_USE_IR 1
#endif

#if HAP_PLUGIN_USE_NIMBLE_MIFLORA
#define HAP_ENABLE_BLE 1
#endif

#if HAP_ENABLE_BLE
// disable HTTPS if BLE is enabled
#undef HAP_WEBSERVER_USE_SSL
#define HAP_WEBSERVER_USE_SSL 0

// reduce fakegato buffer
#undef HAP_FAKEGATO_BUFFER_SIZE
#define HAP_FAKEGATO_BUFFER_SIZE 512
#endif

// shut off pixel indicator if using neo pixel
#if HAP_PLUGIN_USE_NEOPIXEL
#undef HAP_ENABLE_PIXEL_INDICATOR
#define HAP_ENABLE_PIXEL_INDICATOR 0
#endif

/**
 * Plugins feature rev calculation
 * !!! Add new plugins on top here !!!
 ********************************************************************/
#define HAP_PLUGIN_FEATURE_NUMBER \
STR(HAP_PLUGIN_USE_NIMBLE_MIFLORA) \
STR(HAP_PLUGIN_USE_IR) \
STR(HAP_PLUGIN_USE_FAN_HONEYWELL) \
STR(HAP_PLUGIN_USE_RF24) \
STR(HAP_PLUGIN_USE_SSD1306) \
STR(HAP_PLUGIN_USE_LED) \
STR(HAP_PLUGIN_USE_SSD1331) \
STR(HAP_PLUGIN_USE_PCA301) \
STR(HAP_PLUGIN_USE_NEOPIXEL) \
STR(HAP_PLUGIN_USE_INFLUXDB) \
STR(HAP_PLUGIN_USE_HYGROMETER) \
STR(HAP_PLUGIN_USE_RCSWITCH) \
STR(HAP_PLUGIN_USE_DHT) \
STR(HAP_PLUGIN_USE_BME280)

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)




/**
 * Options - Do not edit !!!
 ********************************************************************/
#define HAP_LONG_UUID				0		// Use long uuid as type in accessory json
											// Default: 0


#ifndef HAP_MINIMAL_PLUGIN_INTERVAL
#define HAP_MINIMAL_PLUGIN_INTERVAL	1000	// Minimal plugin handle interval in ms
#endif										// Default: 1000
											// ToDo: Nedded ??




/**
 * Limits
 * !! Do not edit !!!
 ********************************************************************/
#ifndef HAP_BUFFER_CLIENT_SIZE
#define HAP_BUFFER_CLIENT_SIZE		HAP_PRINT_ENCRYPTED_BUFFER_SIZE	// StreamUtils bufferClient buffer size
										// default: 128
#endif

#define HAP_ARDUINOJSON_BUFFER_SIZE 3192	//(ESP.getMaxAllocHeap() - 4096)

#if defined( ARDUINO_ARCH_ESP32 )
#define HAP_ENCRYPTION_BUFFER_SIZE 	16384 // (ESP.getMaxAllocHeap() - 4096)	// 16384
#elif defined( CORE_TEENSY )
#define HAP_ENCRYPTION_BUFFER_SIZE 	3192
#endif


#define HAP_PAIRINGS_MAX			16		// Number of available pairings
											// Default: 16

#define HAP_HOMEKIT_DEFAULT_STRING_LENGTH	64


#define MAX_BRAND_LENGTH 			12
#define MAX_FIRMWARE_NAME_LENGTH 	8		 // Homekit == 7 + 1
#define MAX_FIRMWARE_VERSION_LENGTH	19		 // 1000.1000.1000.1000


/**
 * SRP
 * !! Do not edit !!!
 ********************************************************************/
#ifndef SRP_TEST
#define SRP_TEST 0		// Test SRP - keep disabled !
#endif




/**
 * Keysizes
 * Do not edit!
 ********************************************************************/
#ifndef CURVE25519_SECRET_LENGTH
#define CURVE25519_SECRET_LENGTH    	32
#endif

#ifndef ED25519_PUBLIC_KEY_LENGTH
#define ED25519_PUBLIC_KEY_LENGTH    	32
#endif

#ifndef ED25519_PRIVATE_KEY_LENGTH
#define ED25519_PRIVATE_KEY_LENGTH    	64
#endif

#ifndef ED25519_BYTES
#define ED25519_BYTES    				64
#endif

#ifndef ED25519_SIGN_LENGTH
#define ED25519_SIGN_LENGTH    			64
#endif

#ifndef HAP_ENCRYPTION_NONCE_SIZE
#define HAP_ENCRYPTION_NONCE_SIZE 		12		// Don't change!
#endif

#ifndef HAP_ENCRYPTION_HMAC_SIZE
#define HAP_ENCRYPTION_HMAC_SIZE		16		// Don't change!
#endif

#ifndef HAP_ENCRYPTION_KEY_SIZE
#define HAP_ENCRYPTION_KEY_SIZE			32		// Don't change!
#endif

#ifndef HAP_ENCRYPTION_AAD_SIZE
#define HAP_ENCRYPTION_AAD_SIZE 		2
#endif

#ifndef CHACHA20_POLY1305_AEAD_KEYSIZE
#define CHACHA20_POLY1305_AEAD_KEYSIZE      32
#endif

#ifndef CHACHA20_POLY1305_AUTH_TAG_LENGTH
#define CHACHA20_POLY1305_AUTH_TAG_LENGTH	16
#endif

#ifndef CHACHA20_POLY1305_NONCE_LENGTH
#define CHACHA20_POLY1305_NONCE_LENGTH      12
#endif

#ifndef HKDF_KEY_LEN
#define HKDF_KEY_LEN      CHACHA20_POLY1305_AEAD_KEYSIZE
#endif

#ifndef HAP_PAIRINGS_ID_LENGTH
#define HAP_PAIRINGS_ID_LENGTH 		36
#endif

#define HAP_PAIRINGS_LTPK_LENGTH 	ED25519_PUBLIC_KEY_LENGTH
#define HAP_PAIRINGS_LTSK_LENGTH 	ED25519_PRIVATE_KEY_LENGTH



/**
 * Plugin configuration
 ********************************************************************/

/**
 * MiFlora
 *****************************/
#ifndef HAP_PLUGIN_MIFLORA_MAX_DEVICES
#define HAP_PLUGIN_MIFLORA_MAX_DEVICES     			10
#endif

#ifndef HAP_PLUGIN_MIFLORA_SCAN_DURATION
#define HAP_PLUGIN_MIFLORA_SCAN_DURATION    		3       // Max duration of BLE scan (in seconds)
#endif

#ifndef HAP_PLUGIN_MIFLORA_SCAN_INTERVAL
#define HAP_PLUGIN_MIFLORA_SCAN_INTERVAL    		300000
#endif

#ifndef HAP_PLUGIN_MIFLORA_RETRY
#define HAP_PLUGIN_MIFLORA_RETRY       				5		// number of retries to connect to sensor
#endif

#ifndef HAP_PLUGIN_MIFLORA_ENABLE_SCANNER
#define HAP_PLUGIN_MIFLORA_ENABLE_SCANNER 			1		// enable ble scanner to detect mifloras
#endif

#ifndef HAP_PLUGIN_MIFLORA_ENABLE_HISTORY
#define HAP_PLUGIN_MIFLORA_ENABLE_HISTORY			0		// download history from sensor and provide it via fakegato
#endif

#ifndef HAP_PLUGIN_MIFLORA_HISTORY_BATCH_SIZE
#define HAP_PLUGIN_MIFLORA_HISTORY_BATCH_SIZE		7		// Number of history entries to catch in one try
#endif
															// keep this below 8
/**
 * Hygrometer
 *****************************/
#ifndef HAP_PLUGIN_HYGROMTER_REFERENCE
#define HAP_PLUGIN_HYGROMTER_REFERENCE      		2550    // value if put in a glass of water
#endif

#ifndef HAP_PLUGIN_HYGROMETER_PIN_VCC
#define HAP_PLUGIN_HYGROMETER_PIN_VCC       		A6	// 14
#endif

#ifndef HAP_PLUGIN_HYGROMETER_PIN_ADC
#define HAP_PLUGIN_HYGROMETER_PIN_ADC       		A7	// 32
#endif

#ifndef HAP_HYGROMETER_LEAK_SENSOR_ENABLED
#define HAP_HYGROMETER_LEAK_SENSOR_ENABLED  		0		// enable leak sensor for hygrometer
#endif

#ifndef HAP_HYGROMETER_LEAK_PERCENTAGE
#define HAP_HYGROMETER_LEAK_PERCENTAGE				35		// Level when a "leak" notification is triggered
#endif


/**
 * DHT
 *****************************/
#ifndef HAP_PLUGIN_DHT_USE_DUMMY
#define HAP_PLUGIN_DHT_USE_DUMMY 	0
#endif

#ifndef HAP_PLUGIN_DHT_USE_PRESSURE
#define HAP_PLUGIN_DHT_USE_PRESSURE 0
#endif

#ifndef DHTPIN
#define DHTPIN 		A8 	// 15
#endif

#ifndef DHTTYPE
#define DHTTYPE    	DHT22
#endif

/**
 * BME280
 *****************************/
#ifndef HAP_PLUGIN_BME280_USE_DUMMY
#define HAP_PLUGIN_BME280_USE_DUMMY		0	// if 0 then use real sensor,
#endif  									// if 1 then use random values without any real sensor connected

#ifndef SEALEVELPRESSURE_HPA
#define SEALEVELPRESSURE_HPA 			(1013.25)
#endif


/**
 * Fan Honeywell
 *****************************/
#ifndef HAP_PLUGIN_IR_DEVICE_FREQUENCY
#define HAP_PLUGIN_IR_DEVICE_FREQUENCY 		38000
#endif

#ifndef HAP_PLUGIN_HONEYWELL_DELAY_SEND
#define HAP_PLUGIN_HONEYWELL_DELAY_SEND 	300  // in m
#endif


/**
 * IR
 *****************************/
#ifndef HAP_PLUGIN_IR_SEND_PIN
#define HAP_PLUGIN_IR_SEND_PIN      		14
#endif

#ifndef HAP_PLUGIN_IR_SEND_PIN
#define HAP_PLUGIN_IR_ENABLE_RECV 			1
#endif

#ifndef HAP_PLUGIN_IR_ENABLE_RECV
#define HAP_PLUGIN_IR_ENABLE_RECV 			1
#endif


#if HAP_PLUGIN_IR_ENABLE_RECV

#ifndef HAP_PLUGIN_IR_RECV_PIN
#define HAP_PLUGIN_IR_RECV_PIN				A2
#endif

#ifndef HAP_PLUGIN_IR_RECEIVE_BUFFER_SIZE
#define HAP_PLUGIN_IR_RECEIVE_BUFFER_SIZE	1024
#endif

#ifndef HAP_PLUGIN_IR_RECEIVE_TIMEOUT
#define HAP_PLUGIN_IR_RECEIVE_TIMEOUT		150 // ms
#endif

#ifndef HAP_PLUGIN_IR_RECEIVE_TIMER
#define HAP_PLUGIN_IR_RECEIVE_TIMER			2
#endif

#endif /* HAP_PLUGIN_IR_ENABLE_RECV */


/**
 * NeoPixel
 *****************************/
#ifndef HAP_PLUGIN_NEOPIXEL_NUM_LEDS
#define HAP_PLUGIN_NEOPIXEL_NUM_LEDS 1
#endif

#ifndef HAP_PLUGIN_NEOPIXEL_DATA_PIN
#define HAP_PLUGIN_NEOPIXEL_DATA_PIN A5
#endif

/**
 * RCSwitch
 *****************************/
#ifndef HAP_PLUGIN_RCSWITCH_PIN
#define HAP_PLUGIN_RCSWITCH_PIN        27  // GPIO27
#endif

#endif /* HAPGLOBALS_HPP_ */