


# def bin2header(data, var_name='var'):
#     out = []
#     out.append('uint8_t {var_name}[] = {{'.format(var_name=var_name))
#     l = [ data[i:i+12] for i in range(0, len(data), 12) ]
#     for i, x in enumerate(l):
#         line = ', '.join([ '0x{val:02x}'.format(val=ord(c)) for c in x ])
#         out.append('  {line}{end_comma}'.format(line=line, end_comma=',' if i<len(l)-1 else ''))
#     out.append('};')
#     out.append('uint8_t {var_name}_len = {data_len};'.format(var_name=var_name, data_len=len(data)))
#     return '\n'.join(out)


def getSizeType(length):
	if length < 256:
		return "uint8_t"
	elif length < 65536:
		return "uint16_t"
	else: 
		return "size_t"


def hex2header(hex_string, var_name):
	hex_bytes = bytearray.fromhex(hex_string)
	print("const {} length = {};".format(getSizeType(len(hex_bytes)), len(hex_bytes)))
	print("uint8_t " + var_name + "[length] = {", end=" ")
	counter=0
	for h in hex_bytes:
		print("0x{0:0{1}X}".format(h,2), end="")
		
		if counter == len(hex_bytes) - 1:
			print("", end=" ")
		else:
			print(",", end=" ")
		counter = counter + 1
	print("};")

hex_string="6F3A8407A0000000041010A52F500A4D6173746572436172649F38069F5C089F4005BF0C179F5E095413339000001513019F5D030101009F4D020B0A"
hex2header(hex_string, "data")

hex_string2="8407A0000000041010A52F500A4D6173746572436172649F38069F5C089F4005BF0C179F5E095413339000001513019F5D030101009F4D020B0A"
hex2header(hex_string2, "expected")

hex_string3="770E8202580094080801010010010301"
hex2header(hex_string3, "data")

hex_string4="8202580094080801010010010301"
hex2header(hex_string4, "expected")








