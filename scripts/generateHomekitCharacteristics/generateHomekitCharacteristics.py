import plistlib
import os
from gitlab.v4.objects import services
from tabulate import tabulate
from datetime import datetime
import getpass
import gitlab



printAsCode = [
    "Short UUID",
    "UUID",
    "Enum",
    "Format",
    "Unit"
]

mappingTable = {
    "read": "Paired Read",
    "write": "Paired Write",
    "cnotify": "Notify",
    "uncnotify": ""
}


displayTable = {
    "Name",
    "Enum",
    "Short UUID",
    "Format",
    "Read",
    "Write",
    "Notify",
    "Value"
}

HEADER_STR="""//
// {0}.hpp
// Homekit
//
//  Created on: {1}
//      Author: {2}
//

#ifndef {3}_HPP_
#define {3}_HPP_

{4}

#endif /* {3}_HPP_ */
"""

def readPlist(fileName):
    if os.path.exists(fileName):
        with open(fileName, 'rb') as fp:
            pl = plistlib.load(fp)
            return pl

def shortUUID(uuidstr):
    return "{:04X}".format(int(uuidstr.split("-")[0], base=16))


def toCamelCase(word, spacer=""):
        return ''.join(x.capitalize() or ' ' for x in word.split(' '))

def toUpperCase(word, spacer=""):
    if spacer != "":
        word = word.replace(" ", spacer).upper()
    return word.upper()

def codify(str):
    return "`{}`".format(str)

def prettyName(str, prefix="", suffix="", spacer="", prettyfier=toCamelCase):
    str = str.replace(".","_")

    result = prefix + spacer
    result += prettyfier(str, spacer)
    if suffix != "":
        result += spacer + suffix
    return result

def parseCategories(array):
    categories = []
    for category in array:
        d = {
            'Name': category["Name"],
            'Enum': prettyName(category["Name"]),
            'Value': category["Category"]
        }
        categories.append(d)
    return categories

def remap(array):
    newArray = []
    for key in array:
        if key in mappingTable.keys():
            key = mappingTable[key]

        if len(key) > 0:
            newArray.append(key)
    return newArray

def parseCharacteristics(array):
    chars = []
    for category in array:
        #print(category)
        d = {
            'Name': category["Name"],
            'Enum': prettyName(category["Name"], prefix="", spacer="", prettyfier=toCamelCase),
            'Short UUID': shortUUID(category["UUID"]),
            'UUID': category["UUID"],
            'Format': category["Format"],
            'Permissions': category['Permissions'],
            'Properties': remap(category['Properties']),
        }

        if 'Unit' in category.keys():
            d['Unit'] = category['Unit']

        if 'Constraints' in category.keys():
            if 'MinimumValue' in category['Constraints'].keys():
                d['MinVal'] = category['Constraints']['MinimumValue']
            if 'MaximumValue' in category['Constraints'].keys():
                d['MaxVal'] = category['Constraints']['MaximumValue']
            if 'StepValue' in category['Constraints'].keys():
                d['Step'] = category['Constraints']['StepValue']
            if 'ValidValues' in category['Constraints'].keys():
                d['ValidValues'] = category['Constraints']['ValidValues']
            if 'ValidBits' in category['Constraints'].keys():
                d['ValidBits'] = category['Constraints']['ValidBits']

        chars.append(d)
    return chars

def categoriesToEnum(array, enumName, typeStr = None):
    result = ""

    if typeStr == None:
        result += "enum class {} {} \n".format(enumName, "{")
    else:
        result += "enum class {} : {} {} \n".format(enumName, typeStr, "{")
    for e in array:
        result += "    {:20} = 0x{:02X},\n".format(e['Enum'], e['Value'])

    result = result[:-2] + "\n"
    result += "};\n"

    return HEADER_STR.format(enumName, now.strftime("%d.%m.%Y"), getpass.getuser(), enumName.upper(), result)


def characteristicsToEnum(array, enumName, typeStr = None):
    result = ""

    if typeStr == None:
        result += "enum class {} {} \n".format(enumName, "{")
    else:
        result += "enum class {} : {} {} \n".format(enumName, typeStr, "{")


    sortedArray = []
    for e in array:
        sortedArray.append({"name": e['Enum'], "value": int(e['Short UUID'], base=16)})
    
    #print(sortedArray)
    sortedArray = sorted(sortedArray, key = lambda i: i['value'])
    #print(sortedArray)

    for e in sortedArray:
        result += "    {:40} = 0x{:04X},\n".format(e['name'], e['value'])

    result = result[:-2] + "\n"
    result += "};\n"

    return HEADER_STR.format(enumName, now.strftime("%d.%m.%Y"), getpass.getuser(), enumName.upper(), result)

def servicesToEnum(array, enumName, typeStr = None):
    result = ""

    if typeStr == None:
        result += "enum class {} {} \n".format(enumName, "{")
    else:
        result += "enum class {} : {} {} \n".format(enumName, typeStr, "{")


    sortedArray = []
    for e in array:
        sortedArray.append({"name": prettyName(e['Name']), "value": int(shortUUID(e['UUID']), base=16)})
    
    #print(sortedArray)
    sortedArray = sorted(sortedArray, key = lambda i: i['value'])
    #print(sortedArray)

    for e in sortedArray:
        result += "    {:40} = 0x{:04X},\n".format(e['name'], e['value'])

    result = result[:-2] + "\n"
    result += "};\n"

    return HEADER_STR.format(enumName, now.strftime("%d.%m.%Y"), getpass.getuser(), enumName.upper(), result)



def codifyArray(array):
    for d in array:
        for key, value in d.items():
            if key in printAsCode:
                d[key] = codify(value)
    return array

def toMarkdown(array, displayTable):
    newarray = prepareForMarkdown(array, displayTable)
    newarray = codifyArray(newarray)
    return tabulate(newarray, headers="keys", tablefmt="github")


def parseProperties(array):
    newArray = []
    for d in array:
        newD = {}
        for key in d.keys():
            if key == "Properties":
                if "Paired Read" in d[key]:
                    newD["Read"] = ":white_check_mark:"
                if "Paired Write" in d[key]:
                    newD["Write"] = ":white_check_mark:"
                if "Notify" in d[key]:
                    newD["Notify"] = ":white_check_mark:"
            newD[key] = d[key]
        newArray.append(newD)
    return newArray

def prepareForMarkdown(array, displayTable, link = True):
    newArray = []
    for d in array:
        newD = {}
        for key in d.keys():
            if key == "Name" and link:
                newD["Name"] = "[{0:}](Homekit Accessory Protocol/Characteristics/{0:})".format(d[key])

            elif key == "Properties":
                if "Paired Read" in d[key]:
                    newD["Read"] = ":white_check_mark:"
                if "Paired Write" in d[key]:
                    newD["Write"] = ":white_check_mark:"
                if "Notify" in d[key]:
                    newD["Notify"] = ":white_check_mark:"

            elif key in displayTable:
                newD[key] = d[key]
        newArray.append(newD)
    return newArray



def deleteCharacteristicPage(name, section = 'synologynas', configFile = './gitlab.ini', projectName = "Teensy_Homekit"):
    gl = gitlab.Gitlab.from_config(section, [configFile])

    print('"Deleting gitlab page on {0:} for {1:}"'.format(gl.url, name))

    # list all the projects
    projects = gl.projects.list()
    for project in projects:
        if project.name == projectName:
            pages = project.wikis.list()
            for p in pages:
                if name == p.title:
                    p.delete()




def getGitlabProject(section = 'synologynas', configFile = './gitlab.ini', projectName = "Teensy_Homekit"):
    gl = gitlab.Gitlab.from_config(section, [configFile])

    print('"Get gitlab project on {0:} for {1:}"'.format(gl.url, projectName))

    # list all the projects
    projects = gl.projects.list()
    for project in projects:
        if project.name == projectName:
            return project
    return None

def uploadGitlabPage(content, name, path, project):
    print("Uploading {} to {}".format(name, path + name))
    pages = project.wikis.list()
    found = False
    for p in pages:
        if name == p.title:
            found = True
            # page = project.wikis.get(p.slug)
            # page.content = content
            # page.save()
    if found == False:
        project.wikis.create({'title': path + name, 'content': content})

def deleteGitlabPages(names, project):
    pages = project.wikis.list()
    for p in pages:
        #print(p)
        if p.title in names:
            print("Deleting {}".format(p.slug))
            page = project.wikis.get(p.slug)
            page.delete()


def createCharacteristicPages(array, project):

    array = parseProperties(array)

    print("Upload {} characteristics".format(len(array)))
    for d in array:

        content = ""
        print("   " + d["Name"])
        #for k,v in d.items():
        #    print(k,v)

        desc = """
# {}

* Short UUID: {}
* UUID: {}
* Format: {}
* Unit: {}
* Required iOS Version: {}

""".format(codify("HAPCharacteristicType::" + d["Enum"]), codify(d["Short UUID"]), codify(d["UUID"]),  codify(d["Format"]), codify(d["Unit"]) if 'Unit' in d else "`None`", d["iOS"] if 'iOS' in d else "",)
        content += desc


        permissions = """
## Permissions

| (Paired) Read  | Notify | (Paired) Write |
| ------------ | ------ |------------- |
| {} | {} | {} |

""".format(":white_check_mark:" if "Read" in d else "", ":white_check_mark:" if "Notify" in d else "", ":white_check_mark:" if "Write" in d else "")
        content += permissions

        if "MinVal" in d.keys() or "MaxVal" in d.keys() or "Step" in d.keys():
            constraints = """
## Constraints

| Min Value  | Max Value | Step |
| ---------- | --------- |----- |
| {} | {} | {} |

""".format(codify(d["MinVal"]) if 'MinVal' in d else "", codify(d["MaxVal"]) if 'MaxVal' in d else "", codify(d["Step"]) if 'Step' in d else "")
            content += constraints

        lineformat="| {} | {} |\n"
        validFormat = """
## Valid {}

| Value  | Description |
| ------ | ----------- |
"""
        if "ValidBits" in d.keys():
            validBits = validFormat.format("Bits")
            for k,v in d["ValidBits"].items():
                validBits += lineformat.format(codify(k), v)
            content += "\n" + validBits

        if "ValidValues" in d.keys():
            validBits = validFormat.format("Values")
            for k, v in d["ValidValues"].items():
                validBits += lineformat.format(codify(k), v)
            content += "\n" + validBits

        #print(content)
        uploadGitlabPage(content, d["Name"], 'Homekit Accessory Protocol/Characteristics/', project)


def characteristicForUUID(uuidStr, chars):
    for chr in chars:
        if chr["UUID"] == uuidStr:
             return chr
    return None


def createServicePages(services, project):
    
    for s in services:
        print(s["Name"], s["UUID"])
        content = ""

        desc = """
# {}

* Short UUID: {}
* UUID: {}
* Required iOS Version: {}

""".format(codify(prettyName(s["Name"],"HAPCharacteristicType::")), codify(shortUUID(s["UUID"])), codify(s["UUID"]),  s["iOS"] if 'iOS' in s else "",)
        content += desc + "\n"

        content += "## Required Characteristics\n\n"
        for c in s["RequiredCharacteristics"]:
            chr = characteristicForUUID(c, chars)
            content += "* [{0:}](Homekit Accessory Protocol/Characteristics/{0:}): {1:}\n".format(chr["Name"], codify(prettyName(chr["Name"],"HAPCharacteristicType::")))
        content += "\n\n"

        content += "## Optional Characteristics\n\n"
        for c in s["OptionalCharacteristics"]:
            chr = characteristicForUUID(c, chars)
            content += "* [{0:}](Homekit Accessory Protocol/Characteristics/{0:}): {1:}\n".format(chr["Name"], codify(prettyName(chr["Name"],"HAPCharacteristicType::")))
        content += "\n\n"

        #print(content)
        uploadGitlabPage(content, s["Name"], 'Homekit Accessory Protocol/Services/', project)

now = datetime.now()

pList = readPlist("/Applications/HomeKit Accessory Simulator.app/Contents/Frameworks/HAPAccessoryKit.framework/Versions/A/Resources/default.metadata.plist")


# for c in pList["Characteristics"]:
#     print(c)


project = getGitlabProject()

# categories = parseCategories(pList["Categories"])
# catEnumContent = categoriesToEnum(categories, "HAPAccessoryType", "uint8_t")
# print(catEnumContent)
# print("\n\n")

# content = toMarkdown(categories, displayTable)
# deleteGitlabPages(["Accessory Types"], project)
# uploadGitlabPage(content, "Accessory Types", 'Homekit Accessory Protocol/', project)


chars = parseCharacteristics(pList["Characteristics"])


# deleteGitlabPages(["Characteristic Types"], project)
# content = toMarkdown(chars, displayTable)
# uploadGitlabPage(content, "Characteristic Types", 'Homekit Accessory Protocol/', project)


# #names = [a_dict["Name"] for a_dict in chars]
# deleteGitlabPages([a_dict["Name"] for a_dict in chars], project)
# createCharacteristicPages(chars, project)

# chrEnumContent = characteristicsToEnum(chars, "HAPCharacteristicType", "uint16_t")
# print(chrEnumContent)
# print("\n\n")

services = pList["Services"]
catEnumContent = servicesToEnum(services, "HAPServiceType", "uint16_t")
print(catEnumContent)

#serviceNames = [a_dict["Name"] for a_dict in services]
#deleteGitlabPages(serviceNames, project)
#createServicePages(services, project)

