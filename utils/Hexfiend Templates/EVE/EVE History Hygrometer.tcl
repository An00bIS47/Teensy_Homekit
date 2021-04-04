

section "History Weather" {

	uint8 "size"
	uint32 "entry Counter"
	uint32 "seconds since ref Time"

	set bitmask [uint8]
	move -1
	hex 1 "bitmask"
	if {$bitmask == 0x81} {
		uint32 "ref time"	
		hex 7 "padding"
	} else {
		#uint16 "temperature"
		uint16 "humidity"
		# uint16 "pressure"	
	}		
}

