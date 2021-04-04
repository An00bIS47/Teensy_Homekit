
import traceback
import binascii
from termcolor import cprint

def decode_chunked(data):
    offset = 0
    encdata = ''
    newdata = ''
    offset = data.index("\r\n\r\n") + 4 # get the offset 
    # of the data payload. you can also parse content-length header as well.
    encdata = data[offset:]
    try:
        while (encdata != ''):
            off = int(encdata[:encdata.index("\r\n")],16)
            if off == 0:
                break
            encdata = encdata[encdata.index("\r\n") + 2:]
            newdata = "%s%s" % (newdata, encdata[:off])
            encdata = encdata[off+2:]
                             
    except:
       line = traceback.format_exc()
       print("Exception! {}".format(line)) # probably indexes are wrong
    return newdata



with open("Response_Accessory.dat", mode='rb') as file: # b is important -> binary
    bytesRead = file.read()


hex_bytes = binascii.hexlify(bytesRead)
hex_string = hex_bytes.decode("utf-8")

print(len(hex_string))

i = 0
newLine = True
while (i < len(hex_string)):
		
	if newLine:
		#print(i, end=" ")
		newLine = False
		#print("{}{}".format(hex_string[i], hex_string[i+1]), end="")
	#if i == 6548 or i == 6549:
	if i == 13096 or i == 13096:	
		cprint("{}".format(hex_string[i]), "red", end="")

	else:
		print("{}".format(hex_string[i]), end="")

	i += 1

	#if i == 6548 or i == 6549:
	if i == 13096 or i == 13097:
		cprint("{}".format(hex_string[i]), "red", end="")
	else:
		print("{}".format(hex_string[i]), end="")		

	i += 1

	if i % 8 == 0:
		print(" ", end="")

	if i % 64 == 0:
		#print(" | ", end="")			
		print("")
		newLine = True



print(decode_chunked(bytesRead.decode("utf-8")))


