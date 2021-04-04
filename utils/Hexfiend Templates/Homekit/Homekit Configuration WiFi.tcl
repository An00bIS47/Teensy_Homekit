#
# Homekit Configuration WiFi
# Homekit
#
#  Created on: 19.01.2021
#      Author: michael
#

uint8 "Enabled"
uint8 "Mode"

while { ![end] } {
	section "Network" {		
		ascii 32 "SSID"
		ascii 64 "Password"
	}
}