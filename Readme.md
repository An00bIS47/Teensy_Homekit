# Homekit

[![pipeline status](http://SynologyNAS:30000/michael/teensy_homekit/badges/master/pipeline.svg)](http://SynologyNAS:30000/michael/teensy_homekit/commits/master)


This project uses [platformio](https://platformio.org) as build tool. 

In the root directory of the project is a file named `platformio.ini` where you can define building environments, dependent libraries, add flags for compiling etc.

The `platformio.ini` consists of multiple environments (boards or build configurations) and uses additional scripts for modifying parameters for building depending on the users choices.

In short the command to build and upload this would be:

`pio run -e teensy_41 -t upload` 

or

`pio run -e teensy_41_debug -t upload` 

depending on the enviroment you want to build. Leave out `-t upload` for compiling only.

## plugins option:
```
[plugins]
bme280 = 1
led = 1
dht = 0
hygrometer = 0
knx = 0
```

With this option you can choose, which plugin should be available and enabled for your build. You can select multiple.
A `extra_script` will add the required source files to the `src_filter` and will also add the required defines and include paths.

## custom_options:

For each environment you will find the `custom_options`. There you can define, which feature options should be available. 

```
custom_options =
    teensy_debug = 0
    teensy_config_spiffs_ext = 0
    teensy_config_spiffs_int = 0
    config_eeprom = 1
    qrcode = 1
    ntp = 1
    webserver = 0
    ota_update = 0
    keystore = 0
    reset = 1
    wifi = 0
```

For the Teensy, only `teensy_debug`, `config_eeprom`, `ntp`, `qr_code` and `reset` will be working right now.

### teensy_debug
This will enable TeensyDebug

### config_eeprom
Use EEPROM to save the configuration and pairings

### ntp
Enables NTP as Time source

### reset
This will reset and delete the configuration from EEPROM.


### qr_code
This will enable a QR code generation and will print the QR code for pairing to Serial


## extra_scripts:

The scripts are located in the `scripts/platformio` folder and are referenced in the platformio.ini under 

```
[common]
extra_scripts =
	pre:scripts/platformio/pre_increment_version.py
	pre:scripts/platformio/pre_config_builder.py
	pre:scripts/platformio/pre_debug_macro.py
	;post:scripts/platformio/post_custom_debug_flags.py
	post:scripts/platformio/post_upload_delay.py
        scripts/platformio/custom_targets.py
```

The scripts do the following:
* `pre:scripts/platformio/pre_increment_version.py`:
 Increments the build number in the file `HAPBuildnumber` and adds infos about the git repository to `HAPVersionHomekit`
* `pre:scripts/platformio/pre_config_builder.py`: 
 Adds build flags (in particular: src_filter and build_flags) depending on the choices of the field `custom_options` for each environment. Additional it adds the same build flags for each "selected" plugin
* `pre:scripts/platformio/pre_debug_macro.py`: 
  Basically just adds the define `DEBUG=1` if the `release_type` is `debug`
* `post:scripts/platformio/post_custom_debug_flags.py`:
  This would change the `-0g`optimization parameter (which adds platformio as default if the `release_type` is set to `debug`) to `-0s`. Currently this is commented out
* `post:scripts/platformio/post_upload_delay.py`: 
  Adds a small delay of a few seconds after compiling so that the serial monitor can find the Teensy on the USB port.
* `scripts/platformio/custom_targets.py`: 
  Adds additional targets to the `pio run` command, like `teensy_size`, `tycmd reset` and so on.

I have not tested the scripts with Windows, but they should work on macOS and Linux.