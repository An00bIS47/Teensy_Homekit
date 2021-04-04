#
# Homekit Configuration WebServer
# Homekit
#
#  Created on: 19.01.2021
#      Author: michael
#

uint8  "Enabled"
uint16 "Port"

while { ![end] } {
	section "Credentials" {		
		uint8 "Permission"
		ascii 32 "Username"
		ascii 32 "Password"
	}
}