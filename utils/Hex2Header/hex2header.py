


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



hex_string5="9B5E061701EA7AEB39CF6E3519655A853CF94C75CAF2555EF1FAF759BB79CB477014E04A88D68FFC05323891D4C205B8DE81C2F203D8FAD1B24D2C109737F1BEBBD71F912447C4A03C26B9FAD8EDB3E780778E302529ED1EE138CCFC36D4BA313CC48B14EA8C22A0186B222E655F2DF5603FD75DF76B3B08FF8950069ADD03A754EE4AE88587CCE1BFDE36794DBAE4592B7B904F442B041CB17AEBAD1E3AEBE3CBE99DE65F4BB1FA00B0E7AF06863DB53B02254EC66E781E3B62A8212C86BEB0D50B5BA6D0B478D8C4E9BBCEC21765326FBD14058D2BBDE2C33045F03873E53948D78B794F0790E48C36AED6E880F557427B2FC06DB5E1E2E1D7E661AC482D18E528D7295EF7437295FF1A72D402771713F16876DD050AE5B7AD53CCB90855C93956648358ADFD966422F52498732D68D1D7FBEF10D78034AB8DCB6F0FCF885CC2B2EA2C3E6AC86609EA058A9DA8CC63531DC915414DF568B09482DDAC1954DEC7EB714F6FF7D44CD5B86F6BD115810930637C01D0F6013BC9740FA2C633BA89"
hex2header(hex_string5, "verifier")





