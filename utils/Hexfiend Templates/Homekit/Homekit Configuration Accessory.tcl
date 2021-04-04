#
# Homekit Configuration Accessory
# Homekit
#
#  Created on: 19.01.2021
#      Author: michael
#

uint16 "Port"
ascii 32 "ModelName"
ascii 10 "PinCode"
ascii 4  "SetupId"
uint32 "Init. ConfigNumber"

hex 32 "LTPK"
hex 64 "LTSK"

while { ![end] } {
	section "Pairing" {		
		hex 36 "Id"
		hex 32 "Key"
		uint8  "isAdmin"
	}
}