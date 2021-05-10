


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



hex_string5="6072C6F41CFB16B6CD2204C761EA726EB4260318A236119DAEA44CB09A5F34E91B3298970C8C85754D4BBF6BEB727797E225898511BF8656692EE509BFDC84B694D94614B806C59102EA403202D2978FE1B45A29799CDEF735E1ABA7C9C0AD366C926C61F0683FADF4FDFCA31C4D7D1457F74F836F598322A4F674A61B3C15A404A81C74347556D58E85C3515327D7D2042ACD9631219336DE5CAC57E7B4921CF2ABEB17989B0062462BF846B58D37531FB95510148CB4535F1D86FB5300AB89D34D9BCAB56A5783A6691A4CE129EDE972CC560D7F018D55BAA965454CBED76E7C5756B6E54B83DEA3A7154AEAC2E7F022CA53DDEE263161B26C66F112404DC930C902CFB5DD6549A3C7D26F86A3B8D3D42EA609FE92BC59604E40006FE6F213AF695E4EDEB775E59F50476FE2FAA540A8704F1A83D050F8AB8B2637A7704E831621FF71A9CD1271195D7FEDCAF422D3DB63A904B99EE21C3FDAEF4C222125DF77E5DF07B90D7973BC3E1224EF90A33E8F81056359EEEDE4D8ED308738CF47CC"
hex2header(hex_string5, "three")





