

# LibSodium
 replace with mbedtls or other solutions - done

# Preferences and Config 
rewrite to struct and save to eeprom - done
config shouldn't be a json ?

# Plugins
BME280 - done

# Time errors for KNX for Teensy
- done: remove Time.h from Teensy package

# Add HAL and CAL
Hardware Abstraction Layer like in KNX
Crypto Abstraction Layer - only mbedtls is supported (adk version or 2.18.0)

# KXX Time libaray für Teensy anpassen - required EEPROM using StreamUtils.h

# Place all String constants to FLASH using F macro!!
String constants may be placed only in the flash using F("string") syntax.