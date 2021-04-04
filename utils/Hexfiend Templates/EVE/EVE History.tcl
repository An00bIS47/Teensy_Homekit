
while { ![end] } {
	section "Entry" {
		set size [uint8]
		move -1

		uint8 "size"
		uint32 "entry Counter"
		uint32 "seconds since ref Time"

		set bitmask [uint8]
		move -1
		hex 1 "bitmask"

		if {$bitmask == 0x81} {
		    # Ref  Time 
		    uint32 "reference timestamp"
		    hex 7 "padding"
		} elseif {$bitmask == 0x07} {
		    
		    uint16 "temperature"
		    uint16 "humidity"
		    uint16 "pressure"
		}  elseif {$bitmask == 0x1F} {

			uint16 "??"
			uint16 "??"
			uint16 "power"
			uint16 "??"
			uint16 "??"
		} elseif {$bitmask == 0x01} {
			uint16 "status"		
		} elseif {$bitmask == 0x02} {
			uint16 "humidity"
		}
		# else {
		#     puts "vbl is not one or two"
		# }
	}
}

