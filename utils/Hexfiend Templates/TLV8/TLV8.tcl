
while { ![end] } {
	section "Entry" {
		hex 1 "Type"

		set length [uint8]
		move -1

		uint8 "Length"

		if {$length > 0} {
			hex $length "Value"
		}
		
	}
}

