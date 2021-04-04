import json
import os
import xml.etree.ElementTree as ET
from xml.dom import minidom
import argparse


ns = {'knx': 'http://knx.org/xml/project/14'}




# parameter = 
# 	{
# 		"Id": ,
# 		"Name": "NEEDED" + X,
# 		"ParameterType": "NEEDED",
# 		"Text": "NEEDED",
# 		"Value": "NEEDED",
# 		"Memory": {
# 			"Offset": "NEEDED" + X,
# 			"BitOffset": "NEEDED"
# 		}
# 		"Size": "NEEDED"
# 	}



# xml_data = root[0][0]
# application_programs = 0

# count = 0

# for child in xml_data.findall():
# 	if child.tag == "ApplicationPrograms":
# 		application_programs = count
# 		break
# 	count += 1

# for el in root[0][0][application_programs][0][0][2]:
# 	print(el.tag, el.attrib)



def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    # reparsed = minidom.parseString(rough_string)
    # return reparsed.toprettyxml(indent="  ")
    return rough_string.decode("utf-8") 


def getDPTSize(dpt):
	if dpt == "DPST-1-1":
		return "1 Bit"
	elif dpt == "DPST-1-16":
		return "1 Byte"
	elif dpt == "DPST-5-1":
		return "1 Byte"
	elif dpt == "DPST-9-1":
		return "2 Bytes"
	elif dpt == "DPST-9-7":
		return "2 Bytes"	
	elif dpt == "DPST-13-13":
		return "4 Bytes"		
	elif dpt == "DPST-14-19":
		return "4 Bytes"

	else:
		print("NOT SIZE FOR DPT: " + str(dpt))
		
def getNextChannelNumber(channelNos):
	cNo = max(channelNos) + 1
	channelNos.append(cNo)
	return cNo

def getNextChannelId(channelIds):
	cNo = max(channelIds) + 1
	channelIds.append(cNo)
	return "M-00FA_A-0000-00-0000_CH-" + str(cNo)

def getNextPBRefId(pbRefIds):
	cNo = max(pbRefIds) + 1
	pbRefIds.append(cNo)
	return "M-00FA_A-0000-00-0000_P-" + str(cNo) + "_R-" + str(cNo)

def getNextPBId(pbIds):
	cNo = max(pbIds) + 1
	pbIds.append(cNo)
	return "M-00FA_A-0000-00-0000_PB-" + str(cNo)



def getNextComObjectNumber(comNumbers):
	cNo = max(comNumbers) + 1
	comNumbers.append(cNo)
	return cNo

def getNextComObjectId(comIds):
	cId = max(comIds) + 1
	comIds.append(cId)
	return cId

def getNextComObjectRefId(comRefIds):
	cId = max(comRefIds) + 1
	comRefIds.append(cId)
	return cId

def getLastOffset(pOffsets):
	
	return max(pOffsets)


def getNextOffset(pOffsets):
	return max(pOffsets) + 1

def addToOffsets(pOffsets, size, dictOffsetNo, name):
	for i in range(0, size):
		off = getNextOffset(pOffsets)
		pOffsets.append(off)
		dictOffsetNo[name] = off

def getNextParameterId(pIds):
	pId = max(pIds) + 1
	pIds.append(pId)
	# return "M-00FA_A-0000-00-0000_P-" + str(pId)
	return pId

def printParameterElement(p):
	pId = p.attrib["Id"]
	pName = p.attrib["Name"]
	pText = p.attrib["Text"]
	pParameterType = p.attrib["ParameterType"]
	pValue = p.attrib["Value"]
	pMemoryOffset = -1

	for m in p.findall('knx:Memory', ns):
		pMemoryOffset = m.attrib["Offset"]
	
		
	print( "{} {} {} {} {} {}".format(pId, pName, pText, pParameterType, pValue, pMemoryOffset ))


def fillChannelNumberToText(text, curChannel):
	return text.replace("<C>", str(curChannel))


def newParameterElement(jsonData, pIds, pOffsets, channel, dictOffsetNo):

	pId = getNextParameterId(pIds)

	parameterId = "M-00FA_A-0000-00-0000_P-" + str(pId)
	p = ET.Element('Parameter')
	p.set("Id", parameterId)
	p.set("Text", fillChannelNumberToText(jsonData["Text"], channel))
	p.set("Name", fillChannelNumberToText(jsonData["Name"], channel))
	p.set("ParameterType", jsonData["ParameterType"])
	p.set("Value", fillChannelNumberToText(str(jsonData["Value"]), channel))

	# print(fillChannelNumberToText(jsonData["Name"], channel))
	if "Memory" in jsonData:
		m = ET.SubElement(p, 'Memory')
		m.set("CodeSegment", "M-00FA_A-0000-00-0000_RS-04-00000")		
		if "BitOffset" in jsonData["Memory"]:
			# print("HAS BitOffset SET !!!! ")
			if int(jsonData["Memory"]["BitOffset"]) > 0:
				offset = getLastOffset(pOffsets)
				# print("HAS BitOffset SET >>>>>>>>>> 0 ")
				m.set("BitOffset", str(jsonData["Memory"]["BitOffset"]))
				m.set("Offset", str(offset))
				# print("getLastOffset: " +  str(offset))
				# print("BitOffset" + str(jsonData["Memory"]["BitOffset"]))
			else:

				offset = getNextOffset(pOffsets)

				# print("HAS BitOffset SET <<<<<<<<<<< 0 ")
				m.set("BitOffset", str(jsonData["Memory"]["BitOffset"]))
				m.set("Offset", str(offset))

				# print("getNextOffset: " +  str(offset))
				# print("BitOffset" + str(jsonData["Memory"]["BitOffset"]))

				addToOffsets(pOffsets, jsonData["Memory"]["Size"], dictOffsetNo, fillChannelNumberToText(jsonData["Name"], channel))

		else:
			# print("HAS NOOOOOOOOOOOOOOOOO BitOffset SET !!!! ")
			m.set("BitOffset", str(0))
			m.set("Offset", str(getNextOffset(pOffsets)))

			addToOffsets(pOffsets, jsonData["Memory"]["Size"], dictOffsetNo, fillChannelNumberToText(jsonData["Name"], channel))

	prId, pr = newParameterRef(pId)
	return pId, p, prId, pr


def newParameterRef(pid):
	pr = ET.Element('ParameterRef')
	pr.set("Id", "M-00FA_A-0000-00-0000_P-" + str(pid) + "_R-" + str(pid))
	pr.set("RefId", "M-00FA_A-0000-00-0000_P-"+ str(pid))

	return "M-00FA_A-0000-00-0000_P-"+ str(pid) + "_R-" + str(pid), pr


def parseParameterIds(root):	
	parameterIds=[]

	for aprgm in root[0][0].findall('knx:ApplicationPrograms', ns):
		for ap in aprgm.findall('knx:ApplicationProgram', ns):
			for s in ap.findall('knx:Static', ns):
				for ps in s.findall('knx:Parameters', ns):
					for p in ps.findall('knx:Parameter', ns):
						_id = p.attrib["Id"]
						# M-00FA_A-0000-00-0000_P-XXX
						
						parameterIds.append(int(_id[24:]))
	return parameterIds					


def newComObjectRef(refIds, cId, dpt = None):
	refId = getNextComObjectRefId(refIds)
	cr = ET.Element('ComObjectRef')
	cr.set("Id", "M-00FA_A-0000-00-0000_O-" + str(cId) + "_R-" + str(refId))
	cr.set("RefId", "M-00FA_A-0000-00-0000_O-" + str(cId))

	if dpt is not None:
		cr.set("ObjectSize", getDPTSize(dpt) )
		cr.set("DatapointType", dpt)		

	return "M-00FA_A-0000-00-0000_O-" + str(cId) + "_R-" + str(refId), cr

def newComObject(jsonData, comIds, comNumbers, refIds, channel):
	
	cId = getNextComObjectId(comIds)
	cNo = getNextComObjectNumber(comNumbers)


	co = ET.Element('ComObject')	
	co.set("Id", "M-00FA_A-0000-00-0000_O-" + str(cId))
	co.set("Name", fillChannelNumberToText(str(jsonData["Name"]), channel))
	co.set("Text", fillChannelNumberToText(str(jsonData["Text"]), channel))
	co.set("FunctionText", jsonData["FunctionText"])
	co.set("Number", str(cNo))
	co.set("ReadFlag", jsonData["ReadFlag"])
	co.set("WriteFlag", jsonData["WriteFlag"])
	co.set("CommunicationFlag", jsonData["CommunicationFlag"])
	co.set("TransmitFlag", jsonData["TransmitFlag"])
	co.set("UpdateFlag", jsonData["UpdateFlag"])
	co.set("ReadOnInitFlag", jsonData["ReadOnInitFlag"])
	co.set("UpdateFlag", jsonData["UpdateFlag"])


	rIds = []
	crs = []

	if len(jsonData["DatapointType"]) == 1:
		co.set("DatapointType", jsonData["DatapointType"][0])
		co.set("ObjectSize", getDPTSize(jsonData["DatapointType"][0]))	

		rId, cr = newComObjectRef(refIds, cId)
		rIds.append(rId)
		crs.append(cr)
	else:
		for dpt in jsonData["DatapointType"]:
			rId, cr = newComObjectRef(refIds, cId, dpt)
			rIds.append(rId)
			crs.append(cr)
	return cId, co, rIds, crs, cNo


def newChannelService(channelRefId, channelIds, channelNos, pbIds, channel, ParameterRefRefs, ComObjectRefRefs):
	xml = ET.Element('choose')

	xml.set("ParamRefId", channelRefId)

	# 1: Weather 
	newChannelServiceTypeWeather(xml, channelIds, channelNos,pbIds, channel, ParameterRefRefs, ComObjectRefRefs)	

	# 2: Outlet
	newChannelServiceTypeOutlet(xml, channelIds, channelNos,pbIds, channel, ParameterRefRefs, ComObjectRefRefs)
	
	# 3: Switch
	newChannelServiceTypeSwitch(xml, channelIds, channelNos,pbIds, channel, ParameterRefRefs, ComObjectRefRefs)	
	

	print(prettify(xml))

	return xml

def newChannelServiceTypeWeather(xml, channelIds, channelNos, pbIds, curChannel, ParameterRefRefs, ComObjectRefRefs):
	
	# when test==1
	when = ET.SubElement(xml, 'when')
	when.set("test", "1")	

	comment = ET.Comment(' ========================= ServiceType: Weather ========================= ')
	when.append(comment)


	# <Channel Id="M-00FA_A-0000-00-0000_CH-11" Name="ServiceTypeWeather" Number="1" Text="1: Wettersensor">
	channel = ET.SubElement(when, 'Channel')
	channel.set("Id", str(getNextChannelId(channelIds)))
	channel.set("Name", "ServiceTypeWeather" + str(curChannel))
	channel.set("Number", str(getNextChannelNumber(channelNos)))
	channel.set("Text", str(curChannel) + ": Wettersensor" )

	# <ParameterBlock Id="M-00FA_A-0000-00-0000_PB-12" Name="ServiceTypeWeatherSettings" Text="Einstellungen">
	pb = ET.SubElement(channel, "ParameterBlock")
	pb.set("Id", getNextPBId(pbIds))
	pb.set("Name", "ServiceTypeWeatherSettings" + str(curChannel))
	pb.set("Text", "Einstellungen")

	comment = ET.Comment(' Display Weather Settings here')
	pb.append(comment)
	# 
	# Service Name
	# 
	pbrr = ET.SubElement(pb, "ParameterRefRef")
	pbrr.set("RefId", ParameterRefRefs["ServiceName" + str(curChannel)])
	
	comment = ET.Comment(' ServiceName')
	pb.append(comment)


	#
	# Line
	#
	pbsep1 = ET.SubElement(pb, "ParameterSeparator")
	pbsep1.set("Id", "M-00FA_A-0000-00-0000_PS-1")
	pbsep1.set("Text", " ")

	pbsep1 = ET.SubElement(pb, "ParameterSeparator")
	pbsep1.set("Id", "M-00FA_A-0000-00-0000_PS-2")
	pbsep1.set("Text", "Angeschlossene Sensoren")


	# 
	# Temperature Sensor
	# 
	pbTempSensor = ET.SubElement(pb, "ParameterRefRef")
	pbTempSensor.set("RefId", ParameterRefRefs["TemperatureSensor" + str(curChannel)])

	comment = ET.Comment(" ========= TemperatureSensor ========= ")
	pb.append(comment)



	pbchooseTempSensor = ET.SubElement(pb, "choose")
	pbchooseTempSensor.set("ParamRefId", ParameterRefRefs["TemperatureSensor" + str(curChannel)])

	pbchooseTempSensorWhen = ET.SubElement(pbchooseTempSensor, "when")
	pbchooseTempSensorWhen.set("test", "1")

	comment = ET.Comment(" ========= Parameter ========= ")
	pbchooseTempSensorWhen.append(comment)


	comment = ET.Comment(" ========= ComObjects ========= ")
	pbchooseTempSensorWhen.append(comment)


	comment = ET.Comment(" ###### " + str("??") + " RemoteTemperature ")
	pbchooseTempSensorWhen.append(comment)

	pbchooseTempSensorWhenComObjectRefRef = ET.SubElement(pbchooseTempSensorWhen, "ComObjectRefRef")
	pbchooseTempSensorWhenComObjectRefRef.set("RefId", ComObjectRefRefs["RemoteTemperature" + str(curChannel)])


	# 
	# Humidity Sensor
	# 

	comment = ET.Comment(" ========= HumiditySensor ========= ")
	pb.append(comment)

	pbHumSensor = ET.SubElement(pb, "ParameterRefRef")
	pbHumSensor.set("RefId", ParameterRefRefs["HumiditySensor" + str(curChannel)])

	pbchooseHumSensor = ET.SubElement(pb, "choose")
	pbchooseHumSensor.set("ParamRefId", ParameterRefRefs["HumiditySensor" + str(curChannel)])

	pbchooseHumSensorWhen = ET.SubElement(pbchooseHumSensor, "when")
	pbchooseHumSensorWhen.set("test", "1")

	pbchooseHumSensorWhenDPT = ET.SubElement(pbchooseHumSensorWhen, "ParameterRefRef")
	pbchooseHumSensorWhenDPT.set("RefId", ParameterRefRefs["HumidityDPT" + str(curChannel)])

	pbchooseHumSensorWhenDPTChoose = ET.SubElement(pbchooseHumSensorWhen, "choose")
	pbchooseHumSensorWhenDPTChoose.set("ParamRefId", ParameterRefRefs["HumidityDPT" + str(curChannel)])

	pbchooseHumSensorWhenDPTChooseWhen5 = ET.SubElement(pbchooseHumSensorWhenDPTChoose, "when")
	pbchooseHumSensorWhenDPTChooseWhen5.set("test", "5")

	comment = ET.Comment(" ========= Parameter ========= ")
	pbchooseHumSensorWhenDPTChooseWhen5.append(comment)


	comment = ET.Comment(" ========= ComObjects ========= ")
	pbchooseHumSensorWhenDPTChooseWhen5.append(comment)


	comment = ET.Comment(" ###### " + str("??") + " RemoteHumidity DPST-5-1 ")
	pbchooseHumSensorWhenDPTChooseWhen5.append(comment)

	pbchooseHumSensorWhenDPTChooseWhenComObjectRefRef5 = ET.SubElement(pbchooseHumSensorWhenDPTChooseWhen5, "ComObjectRefRef")
	pbchooseHumSensorWhenDPTChooseWhenComObjectRefRef5.set("RefId", ComObjectRefRefs["RemoteHumidity" + str(curChannel) + "DPST-5-1"])


	pbchooseHumSensorWhenDPTChooseWhen9 = ET.SubElement(pbchooseHumSensorWhenDPTChoose, "when")
	pbchooseHumSensorWhenDPTChooseWhen9.set("test", "9")

	comment = ET.Comment(" ========= Parameter ========= ")
	pbchooseHumSensorWhenDPTChooseWhen9.append(comment)


	comment = ET.Comment(" ========= ComObjects ========= ")
	pbchooseHumSensorWhenDPTChooseWhen9.append(comment)


	comment = ET.Comment(" ###### " + str("??") + " RemoteHumidity DPST-9-7 ")
	pbchooseHumSensorWhenDPTChooseWhen9.append(comment)

	pbchooseHumSensorWhenDPTChooseWhenComObjectRefRef9 = ET.SubElement(pbchooseHumSensorWhenDPTChooseWhen9, "ComObjectRefRef")
	pbchooseHumSensorWhenDPTChooseWhenComObjectRefRef9.set("RefId", ComObjectRefRefs["RemoteHumidity" + str(curChannel) + "DPST-9-7"])


	# 
	# Air Pressure Sensor
	# 
	comment = ET.Comment(" ========= AirPressureSensor ========= ")
	pb.append(comment)

	pbAirPressureSensor = ET.SubElement(pb, "ParameterRefRef")
	pbAirPressureSensor.set("RefId", ParameterRefRefs["AirPressureSensor" + str(curChannel)])

	pbchooseAPSensor = ET.SubElement(pb, "choose")
	pbchooseAPSensor.set("ParamRefId", ParameterRefRefs["AirPressureSensor" + str(curChannel)])

	pbchooseAPSensorWhen = ET.SubElement(pbchooseAPSensor, "when")
	pbchooseAPSensorWhen.set("test", "1")


	comment = ET.Comment(" ###### " + str("??") + " RemoteAirPressure ")
	pbchooseAPSensorWhen.append(comment)

	pbchooseAPSensorWhenComObjectRefRef = ET.SubElement(pbchooseAPSensorWhen, "ComObjectRefRef")
	pbchooseAPSensorWhenComObjectRefRef.set("RefId", ComObjectRefRefs["RemoteAirPressure" + str(curChannel)])


	#
	# Line
	#
	pbsepE = ET.SubElement(pb, "ParameterSeparator")
	pbsepE.set("Id", "M-00FA_A-0000-00-0000_PS-1")
	pbsepE.set("Text", " ")


	#
	# Fakegato
	#
	comment = ET.Comment(" ========= Fakegato ========= ")
	pb.append(comment)

	pbEnableFakegato = ET.SubElement(pb, "ParameterRefRef")
	pbEnableFakegato.set("RefId", ParameterRefRefs["EnableFakegato" + str(curChannel)])



def newChannelServiceTypeOutlet(xml, channelIds, channelNos, pbIds, curChannel, ParameterRefRefs, ComObjectRefRefs):
	
	# when test==1
	when = ET.SubElement(xml, 'when')
	when.set("test", "2")	

	comment = ET.Comment(' ========================= ServiceType: Outlet ========================= ')
	when.append(comment)


	# <Channel Id="M-00FA_A-0000-00-0000_CH-11" Name="ServiceTypeWeather" Number="1" Text="1: Wettersensor">
	channel = ET.SubElement(when, 'Channel')
	channel.set("Id", str(getNextChannelId(channelIds)))
	channel.set("Name", "ServiceTypeOutlet" + str(curChannel))
	channel.set("Number", str(getNextChannelNumber(channelNos)))
	channel.set("Text", str(curChannel) + ": Steckdose" )

	# <ParameterBlock Id="M-00FA_A-0000-00-0000_PB-12" Name="ServiceTypeWeatherSettings" Text="Einstellungen">
	pb = ET.SubElement(channel, "ParameterBlock")
	pb.set("Id", getNextPBId(pbIds))
	pb.set("Name", "ServiceTypeOutletSettings" + str(curChannel))
	pb.set("Text", "Einstellungen")

	comment = ET.Comment(' Display Outlet Settings here')
	pb.append(comment)
	# 
	# Service Name
	# 
	pbrr = ET.SubElement(pb, "ParameterRefRef")
	pbrr.set("RefId", ParameterRefRefs["ServiceName" + str(curChannel)])
	
	comment = ET.Comment(' ServiceName')
	pb.append(comment)


	#
	# Line
	#
	pbsep1 = ET.SubElement(pb, "ParameterSeparator")
	pbsep1.set("Id", "M-00FA_A-0000-00-0000_PS-1")
	pbsep1.set("Text", " ")


	#
	# Fakegato
	#
	comment = ET.Comment(" ========= Fakegato ========= ")
	pb.append(comment)

	pbEnableFakegato = ET.SubElement(pb, "ParameterRefRef")
	pbEnableFakegato.set("RefId", ParameterRefRefs["EnableFakegato" + str(curChannel)])

	#
	# Schedule
	#
	comment = ET.Comment(" ========= Schedule ========= ")
	pb.append(comment)
	pbEnableSchedule = ET.SubElement(pb, "ParameterRefRef")
	pbEnableSchedule.set("RefId", ParameterRefRefs["EnableSchedule" + str(curChannel)])


	#
	# Com Objects
	#
	comment = ET.Comment(" ========= ComObjects ========= ")
	pb.append(comment)

	comment = ET.Comment(" ###### " + str("??") + " RemoteSwitchWrite ")
	pb.append(comment)
	pbComObjectRefRefW = ET.SubElement(pb, "ComObjectRefRef")
	pbComObjectRefRefW.set("RefId", ComObjectRefRefs["RemoteSwitchWrite" + str(curChannel)])

	comment = ET.Comment(" ###### " + str("??") + " RemoteSwitchRead ")
	pb.append(comment)
	pbComObjectRefRefR = ET.SubElement(pb, "ComObjectRefRef")
	pbComObjectRefRefR.set("RefId", ComObjectRefRefs["RemoteSwitchRead" + str(curChannel)])


	comment = ET.Comment(" ###### " + str("??") + " RemoteOutletCurrent ")
	pb.append(comment)
	pbComObjectRefRefC = ET.SubElement(pb, "ComObjectRefRef")
	pbComObjectRefRefC.set("RefId", ComObjectRefRefs["RemoteOutletCurrent" + str(curChannel)])


	comment = ET.Comment(" ###### " + str("??") + " RemoteOutletActEnergy ")
	pb.append(comment)
	pbComObjectRefRefAE = ET.SubElement(pb, "ComObjectRefRef")
	pbComObjectRefRefAE.set("RefId", ComObjectRefRefs["RemoteOutletActEnergy" + str(curChannel)])




def newChannelServiceTypeSwitch(xml, channelIds, channelNos, pbIds, curChannel, ParameterRefRefs, ComObjectRefRefs):
	
	# when test==1
	when = ET.SubElement(xml, 'when')
	when.set("test", "3")	

	comment = ET.Comment(' ========================= ServiceType: Switch ========================= ')
	when.append(comment)


	# <Channel Id="M-00FA_A-0000-00-0000_CH-11" Name="ServiceTypeWeather" Number="1" Text="1: Wettersensor">
	channel = ET.SubElement(when, 'Channel')
	channel.set("Id", str(getNextChannelId(channelIds)))
	channel.set("Name", "ServiceTypeSwitch" + str(curChannel))
	channel.set("Number", str(getNextChannelNumber(channelNos)))
	channel.set("Text", str(curChannel) + ": Schalter" )

	# <ParameterBlock Id="M-00FA_A-0000-00-0000_PB-12" Name="ServiceTypeWeatherSettings" Text="Einstellungen">
	pb = ET.SubElement(channel, "ParameterBlock")
	pb.set("Id", getNextPBId(pbIds))
	pb.set("Name", "ServiceTypeSwitchSettings" + str(curChannel))
	pb.set("Text", "Einstellungen")

	comment = ET.Comment(' Display Switch Settings here')
	pb.append(comment)
	# 
	# Service Name
	# 
	pbrr = ET.SubElement(pb, "ParameterRefRef")
	pbrr.set("RefId", ParameterRefRefs["ServiceName" + str(curChannel)])
	
	comment = ET.Comment(' ServiceName')
	pb.append(comment)


	#
	# Line
	#
	pbsep1 = ET.SubElement(pb, "ParameterSeparator")
	pbsep1.set("Id", "M-00FA_A-0000-00-0000_PS-1")
	pbsep1.set("Text", " ")


	#
	# Fakegato
	#
	comment = ET.Comment(" ========= Fakegato ========= ")
	pb.append(comment)

	pbEnableFakegato = ET.SubElement(pb, "ParameterRefRef")
	pbEnableFakegato.set("RefId", ParameterRefRefs["EnableFakegato" + str(curChannel)])

	#
	# Com Objects
	#
	comment = ET.Comment(" ========= ComObjects ========= ")
	pb.append(comment)


	comment = ET.Comment(" ###### " + str("??") + " RemoteSwitchWrite ")
	pb.append(comment)
	pbComObjectRefRefW = ET.SubElement(pb, "ComObjectRefRef")
	pbComObjectRefRefW.set("RefId", ComObjectRefRefs["RemoteSwitchWrite" + str(curChannel)])


	comment = ET.Comment(" ###### " + str("??") + " RemoteSwitchRead ")
	pb.append(comment)
	pbComObjectRefRefR = ET.SubElement(pb, "ComObjectRefRef")
	pbComObjectRefRefR.set("RefId", ComObjectRefRefs["RemoteSwitchRead" + str(curChannel)])




def newChannelIdentipendent(pbIds, maxChannels, ParameterRefRefs):
	pb = ET.Element('ParameterBlock')
	pb.set("Id", getNextPBId(pbIds))
	pb.set("Name", "ChannelSelection")
	pb.set("Text", "Kanaleinstellungen")

	refIds = []

	for i in range(1, maxChannels+1):
		pbRef = ET.SubElement(pb, "ParameterRefRef")
		pbRef.set("RefId", ParameterRefRefs["ServiceType" + str(i)])
		refIds.append(ParameterRefRefs["ServiceType" + str(i)])
	return refIds, pb

def printXML(parameters, name):
	print('\n')
	print("<!-- ======================================== " + name + " ======================================== -->")
	for el in parameters:
		print(prettify(el))
	print('\n')
	print('\n')

def pretty_print_xml_given_root(root, output_xml):
    """
    Useful for when you are editing xml data on the fly
    """
    xml_string = minidom.parseString(ET.tostring(root)).toprettyxml()
    xml_string = os.linesep.join([s for s in xml_string.splitlines() if s.strip()]) # remove the weird newline issue
    with open(output_xml, "w") as file_out:
        file_out.write(xml_string)

def pretty_print_xml_given_file(input_xml, output_xml):
    """
    Useful for when you want to reformat an already existing xml file
    """
    tree = ET.parse(input_xml)
    root = tree.getroot()
    pretty_print_xml_given_root(root, output_xml)


def mulitplyChannels(numChannels=10, outputFile="Homekit_gen.xml"):

	maxChannels = numChannels
	hkOffset	= 0

	tree = ET.parse('Homekit_Blanko.xml')
	root = tree.getroot()


	pIds = [hkOffset]
	pOffsets = [hkOffset]

	comIds = [hkOffset]
	comNumbers = [hkOffset]
	refIds = [hkOffset]

	XMLparameters = []
	XMLparameterRefs = []

	XMLcomObjects = []
	XMLcomObjectRefs = []

	pbIds = [hkOffset]
	pbRefIds = [hkOffset]
	channelIds = [hkOffset]
	channelNos = [hkOffset]


	dictOffsetNo = {}

	# pIds  = parseParameterIds(root)
	# print(pIds)


	ParameterRefRefs = {}	
	ComObjectRefRefs = {}
	# print(getNextParameterId(pIds))
	

	xmlParameterRoot = root.findall(".//{http://knx.org/xml/project/14}ManufacturerData/{http://knx.org/xml/project/14}Manufacturer/{http://knx.org/xml/project/14}ApplicationPrograms/{http://knx.org/xml/project/14}ApplicationProgram/{http://knx.org/xml/project/14}Static/{http://knx.org/xml/project/14}Parameters")[0]
	xmlParameterRefsRoot = root.findall(".//{http://knx.org/xml/project/14}ManufacturerData/{http://knx.org/xml/project/14}Manufacturer/{http://knx.org/xml/project/14}ApplicationPrograms/{http://knx.org/xml/project/14}ApplicationProgram/{http://knx.org/xml/project/14}Static/{http://knx.org/xml/project/14}ParameterRefs")[0]
	xmlComObjectsRoot = root.findall(".//{http://knx.org/xml/project/14}ManufacturerData/{http://knx.org/xml/project/14}Manufacturer/{http://knx.org/xml/project/14}ApplicationPrograms/{http://knx.org/xml/project/14}ApplicationProgram/{http://knx.org/xml/project/14}Static/{http://knx.org/xml/project/14}ComObjectTable")[0]
	xmlComObjectsRefsRoot = root.findall(".//{http://knx.org/xml/project/14}ManufacturerData/{http://knx.org/xml/project/14}Manufacturer/{http://knx.org/xml/project/14}ApplicationPrograms/{http://knx.org/xml/project/14}ApplicationProgram/{http://knx.org/xml/project/14}Static/{http://knx.org/xml/project/14}ComObjectRefs")[0]

	xmlChannelIndenpendentRoot = root.findall(".//{http://knx.org/xml/project/14}ManufacturerData/{http://knx.org/xml/project/14}Manufacturer/{http://knx.org/xml/project/14}ApplicationPrograms/{http://knx.org/xml/project/14}ApplicationProgram/{http://knx.org/xml/project/14}Dynamic/{http://knx.org/xml/project/14}ChannelIndependentBlock")[0]
	xmlChannelDynamicRoot = root.findall(".//{http://knx.org/xml/project/14}ManufacturerData/{http://knx.org/xml/project/14}Manufacturer/{http://knx.org/xml/project/14}ApplicationPrograms/{http://knx.org/xml/project/14}ApplicationProgram/{http://knx.org/xml/project/14}Dynamic")[0]

	channelCount = 1
	with open('channel.json') as json_file:
		data = json.load(json_file)	

		for channelCount in range(1, maxChannels+1):	

			comment = ET.Comment(" ======================== channel " + str(channelCount) + " ======================== ")
			xmlParameterRoot.append(comment)
			xmlParameterRefsRoot.append(comment)

			for parameter in data["parameters"]:
				pId, p, prId, pRef = newParameterElement(parameter, pIds, pOffsets, channelCount, dictOffsetNo)

				comment = ET.Comment(" === "  + fillChannelNumberToText(parameter["Name"], channelCount) + " ===  ")
				xmlParameterRoot.append(comment)
				xmlParameterRefsRoot.append(comment)

				XMLparameters.append(p)
				XMLparameterRefs.append(pRef)
			
				ParameterRefRefs[fillChannelNumberToText(parameter["Name"], channelCount)] = prId
				
				xmlParameterRoot.append(p)						
				xmlParameterRefsRoot.append(pRef)


			comment = ET.Comment(" ======================== channel " + str(channelCount) + " ======================== ")
			xmlComObjectsRoot.append(comment)
			xmlComObjectsRefsRoot.append(comment)

			for coData in data["comObjects"]:	
				cId, co, rIds, crs, cNo = newComObject(coData, comIds, comNumbers, refIds, channelCount)


				comment = ET.Comment(" === "  + fillChannelNumberToText(coData["Name"], channelCount) + " ===  ")
				xmlComObjectsRoot.append(comment)
				xmlComObjectsRefsRoot.append(comment)


				if len(rIds) == 1:
					ComObjectRefRefs[fillChannelNumberToText(coData["Name"], channelCount)] = rIds[0]
				else:
					count = 0
					for r in rIds:
						tName = coData["Name"] + coData["DatapointType"][count]
						ComObjectRefRefs[fillChannelNumberToText(tName, channelCount)] = rIds[count]
						count += 1

				XMLcomObjects.append(co)

				xmlComObjectsRoot.append(co)
				
				for c in crs:					
					xmlComObjectsRefsRoot.append(c)	
					XMLcomObjectRefs.append(c)	

			


	print('\n')
	print('\n')
	print("<!-- ====================================== channel settings ====================================== -->")
	refId, pbb = newChannelIdentipendent(pbIds, maxChannels, ParameterRefRefs)		
	xml_str = ET.tostring(pbb).decode()
	print(xml_str)
	xmlChannelIndenpendentRoot.append(pbb)

	for channelCount in range(1, maxChannels+1):
		# print(refId[channelCount-1])

		print("<!-- ====================================== service type selection ====================================== -->")
		xmlChannelService = newChannelService(refId[channelCount-1], channelIds, channelNos, pbIds, channelCount, ParameterRefRefs, ComObjectRefRefs)
		xmlChannelDynamicRoot.append(xmlChannelService)



	print('\n')
	print('\n')
	print(ParameterRefRefs)
	print(ComObjectRefRefs)
	print('\n')
	print('\n')

	printXML(XMLparameters, "parameters")
	printXML(XMLparameterRefs, "parameterRefs")
	printXML(XMLcomObjects, "comObjects")
	printXML(XMLcomObjectRefs, "comObjectRefs")


	print(max(pOffsets))
	print(dictOffsetNo)

	tree.write(open(outputFile, 'w'), encoding='unicode')


	# clean up xml
	with open(outputFile) as f:
		lines = f.readlines()

	# Remove namespace
	for i in range(0, len(lines)):
			lines[i] = lines[i].replace("ns0:", "")			

	# Replace first two lines 
	lines[0] = '<?xml version="1.0" encoding="utf-8" ?>\n<KNX xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" CreatedBy="KNX MT" ToolVersion="5.1.255.16695" xmlns="http://knx.org/xml/project/14">\n'
	with open("Homekit_gen.xml", "w") as f:
		f.writelines(lines)


	# pretty_print_xml_given_file('Homekit_gen.xml', 'Homekit_gen.xml')

	# with open('Homekit_gen.xml') as xmldata:
	# 	xml = minidom.parseString(xmldata.read())  # or xml.dom.minidom.parseString(xml_string)
	# 	xml_pretty_str = xml.toprettyxml()

	# with open("Homekit_gen.xml", "w") as f:
	# 	f.write(xml_pretty_str)		

	# for aprgm in root[0][0].findall('knx:ApplicationPrograms', ns):
	# 	for ap in aprgm.findall('knx:ApplicationProgram', ns):
	# 		for s in ap.findall('knx:Static', ns):
	# 			for ps in s.findall('knx:Parameters', ns):
	# 				for p in ps.findall('knx:Parameter', ns):
	# 					printParameterElement(p)

						# pId = p.attrib["Id"]
						# pName = p.attrib["Name"]
						# pText = p.attrib["Text"]
						# pParameterType = p.attrib["ParameterType"]
						# pValue = p.attrib["Value"]
						# pMemoryOffset = -1

						# for m in p.findall('knx:Memory', ns):
						# 	pMemoryOffset = m.attrib["Offset"]


						# print( "{} {} {} {} {} {}".format(pId, pName, pText, pParameterType, pValue, pMemoryOffset ))


def main():
	parser = argparse.ArgumentParser(description='ETS multiply channels for HOMEKIT')
	parser.add_argument('numChannels', type=int, action='store',
                    help='an integer for the accumulator')
	parser.add_argument('--output', dest='outputFile', type=str, action='store',
                    default="Homekit_gen.xml",
                    help='sum the integers (default: find the max)')

	args = parser.parse_args()
	print(args.numChannels, args.outputFile)
	mulitplyChannels(args.numChannels, args.outputFile)



main()

