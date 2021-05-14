import argparse
import sys
import logging
import re
import os
import tlv8
import base64
import json
import binascii
import uuid
import ed25519

from homekit.controller import Controller
from homekit.log_support import setup_logging, add_log_arguments
from homekit.model.characteristics import CharacteristicsTypes, CharacteristicsDecoderLoader
from homekit.model.services import ServicesTypes
from homekit.model.characteristics.characteristic_formats import CharacteristicFormats
from homekit.controller.tools import AbstractPairing
from homekit.exceptions import AlreadyPairedError, HomeKitException
from homekit.controller.additional_pairing import AdditionalPairing

from termcolor import colored, cprint
import struct

import time
import datetime as dt
import gitlab
import timeit
from tabulate import tabulate
from functools import wraps



class TestReport(object):

    """docstring for ClassName"""

    def __init__(self, desc=None, args=None, gitCommit=None):
        super(TestReport, self).__init__()

        self.args = args
        self.desc = desc
        self.gitCommit = gitCommit
        self.passed = True
        self.steps = []
        self.startTime = dt.datetime.now().strftime("%d-%m-%Y %H:%M:%S")

    def getMillis(self, timing):
        millis = 0
        parts = timing.split(":")

        millis += (int(parts[0]) * 86400 * 1000)

        millis += (int(parts[1]) *  3600 * 1000)

        seconds_parts = parts[2].split(".")
        millis += (int(seconds_parts[0]) * 1000)
        millis += int(seconds_parts[1]) / 1000
        return millis


    def addStep(self, name, result, timings, data=None):
        timingsArray = []
        for t in timings:
            tim = {
                "desc": t["desc"],
                "duration": self.getMillis(t["duration"])
            }
            timingsArray.append(tim)


        step = {
            "timestamp": dt.datetime.now().strftime("%d-%m-%Y %H:%M:%S.%f"),
            "name": name,
            "timings": timingsArray,
            "passed": result,
            "data": data
        }
        self.steps.append(step)


        if result == False:
            self.passed = False


    def stepsCount(self):
        return len(self.steps)


    def getTotalDuration(self):
        totalDuration = 0
        for step in self.steps:
            for t in step["timings"]:
                totalDuration += int(t["duration"])
        return totalDuration


    def summary(self):
        print("")
        print("")
        print("")
        print("Summary: ".format())
        table = [
            ["Description", self.desc],
            ["Command Line", self.args],
            ["Git Commit", self.gitCommit],
            ["Date", self.startTime],
            ["Passed", self.passed],
            ["Steps", self.stepsCount()],
            ["Duration", str(self.getTotalDuration()) + " ms"]
        ]
        print(tabulate(table))

        print("")
        print("Details: ".format())
        table = []

        for step in self.steps:
            passed = ""

            if step["passed"] == True:
                passed = "OK"
            else:
                passed = "FAILED"

            duration = 0
            for t in step["timings"]:
                duration += int(t["duration"])

            if step["passed"] == False:
                tableRow = [step["name"], passed, duration, "ms", str(step["data"])]
            else:
                tableRow = [step["name"], passed, duration, "ms", ""]

            table.append(tableRow)


        print(tabulate(table))


    def toJson(self):
        data = {}
        data["desc"] = self.desc
        data["cmd"] = self.args
        data["git"] = self.gitCommit
        data["date"] = self.startTime
        data["passed"] = self.passed
        data["steps"] = []


        for step in self.steps:
            stepInfo = {
                "timestamp": step["timestamp"],
                "name": step["name"],
                "timings": step["timings"],
                "passed": step["passed"],
                "data": str(step["data"])
            }
            data["steps"].append(stepInfo)
        return data

    def toMarkDown(self, accessoryJson=None):
        result = "# Test Report" + "\n"

        result += "## Summary: ".format() + "\n"
        table = [
                    ["Description", self.desc],
                    ["Command Line", self.args],
                    ["Git Commit", self.gitCommit],
                    ["Date", self.startTime],
                    ["Passed", ":white_check_mark:" if self.passed == True else ":red_circle:"  ],
                    ["Steps", self.stepsCount()],
                    ["Duration", str(self.getTotalDuration()) + " ms"]
                ]
        result += tabulate(table, headers=["Key","Info"], tablefmt="github")

        result += "\n"
        result += "## Details: ".format() + "\n"
        table = []

        for step in self.steps:
            passed = ""

            if step["passed"] == True:
                passed = ":white_check_mark:"
            else:
                passed = ":red_circle:"

            duration = 0
            for t in step["timings"]:
                duration += int(t["duration"])

            if step["passed"] == False:
                tableRow = [step["name"], passed, str(duration), str(step["data"])]
            else:
                tableRow = [step["name"], passed, str(duration), ":warning:" if duration > 1000 and step["name"] == "pair step 2" else ""]

            table.append(tableRow)


        result += tabulate(table, headers=["Name", "Passed", "Duration in ms", "Comment"], tablefmt="github")
        result += "\n"


        if accessoryJson != None:
            result += "\n"
            result += "## Accessory Details" + "\n"
            result += "<details>" + "\n"
            result += "<summary>Click this to collapse/fold.</summary>" + "\n"
            result += "<pre><code>" + "\n"
            result += json.dumps(accessoryJson, indent=4) + "\n"
            result += "</code></pre>" + "\n"
            result += "</details>" + "\n"


        result += "\n"
        result += "## Report Details" + "\n"
        result += "<details>" + "\n"
        result += "<summary>Click this to collapse/fold.</summary>" + "\n"
        result += "<pre><code>" + "\n"
        result += json.dumps(self.toJson(), indent=4) + "\n"
        result += "</code></pre>" + "\n"
        result += "</details>" + "\n"

        return result

    def __str__(self):
        return json.dumps(self.toJson(), indent=4)

    def __repr__(self):
        return self.__str__()


class TimingManager(object):
    """Context Manager used with the statement 'with' to time some execution.

    Example:

    with TimingManager() as t:
       # Code to time
    """
    def __init__(self, desc=None, loggingArray=None):
        self.clock = timeit.default_timer
        self.desc  = desc
        self.elapsed = 0
        self.loggingArray = loggingArray



    def __enter__(self):
        """
        """
        self.start = self.clock()

        print('{} '.format(self.desc), end="")
        self.log('{}')
        #self.log('=> Start Timing: {}')


        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        """
        self.endlog()

        return False


    def log(self, s, elapsed=None):
        """Log current time and elapsed time if present.
        :param s: Text to display, use '{}' to format the text with
            the current time.
        :param elapsed: Elapsed time to display. Dafault: None, no display.
        """
        # print(s.format(self._secondsToStr(self.clock())))

        if(elapsed is not None):
            self.elapsed = elapsed
            print('time elapsed {}'.format(self.elapsed))


    def endlog(self):
        """Log time for the end of execution with elapsed time.
        """
        self.log('{}', self.now())
        if self.loggingArray is not None:
            self.loggingArray.append({"desc": self.desc, "duration": self.elapsed})


    def now(self):
        """Return current elapsed time as hh:mm:ss string.
        :return: String.
        """
        return str(dt.timedelta(seconds = self.clock() - self.start))

    def _secondsToStr(self, sec):
        """Convert timestamp to h:mm:ss string.
        :param sec: Timestamp.
        """
        return str(dt.datetime.fromtimestamp(sec))



def measure(func):
    @wraps(func)
    def _time_it(*args, **kwargs):
        start = int(round(time.time() * 1000))
        try:
            return func(*args, **kwargs)
        finally:
            end_ = int(round(time.time() * 1000)) - start
            print(f"Total execution time: {end_ if end_ > 0 else 0} ms")
    return _time_it


class HomekitTester(object):
    """docstring for ClassName"""
    def __init__(self, args):
        super(HomekitTester, self).__init__()
        self.args = args

        setup_logging(self.args.loglevel)

        self.controller = Controller()

        self.createStorageFile()

        self.report = TestReport(args=' '.join(sys.argv[0:]), gitCommit=self.args.gitCommit)

        self.requestedEntry = 0

        self.fakegatoHistories = []

        self.eventCount = 0

        self.remoteId = None

        try:
            self.controller.load_data(self.args.file)
        except Exception as e:
            print(e)
            logging.debug(e, exc_info=True)
            sys.exit(-1)


    def printSummary(self):
        self.report.summary()

    def saveReport(self, filepath, format="json"):

        filename = filepath + "/" + dt.datetime.now().strftime("%d-%m-%Y_%H-%M") + "_report." + format

        result = ""
        if format == "json":
            result = self.report.__str__()
        elif format == "md" or format == "markdown":
            result = self.report.toMarkDown(self.accessoryData)

        with open(filename, 'w') as the_file:
            the_file.write(result)

    def getReport(self, format="json"):
        result = ""
        if format == "json":
            result = self.report.__str__()
        elif format == "md" or format == "markdown":
            result = self.report.toMarkDown(self.accessoryData)
        return result

    def uploadReportToGitlab(self, section = 'synologynas', configFile = './gitlab.ini', projectName = "Teensy_Homekit"):
        gl = gitlab.Gitlab.from_config(section, [configFile])

        # list all the projects
        projects = gl.projects.list()
        for project in projects:
            if project.name == projectName:
                #print(project.id)
                now = dt.datetime.now() # current date and time
                date_time = now.strftime("%Y-%m-%d")
                #print("date and time:",date_time)

                project.wikis.create({'title': 'Testreports/Testreport ' + date_time,
                                    'content': self.report.toMarkDown(self.accessoryData)})

                # Add to Testreport Overview
                page = project.wikis.get('Testreport-Overview')
                addLine = "| " + date_time
                addLine += " | :white_check_mark:" if self.report.passed == True else " | :red_circle:"
                addLine += " | " + self.report.gitCommit
                addLine += " | " + "[report](Testreports/Testreport " + date_time + ")"
                addLine += " |\n"

                page.content = page.content + addLine
                page.save()

    def validateAccessory(self, data):
        assertions = []
        success = True
        data = {"accessories": data}

        for accessory in data["accessories"]:
            aid = accessory['aid']
            for service in accessory['services']:
                s_type = service['type']
                s_iid = service['iid']
                if self.args.quiet == False:
                    print('{aid}.{iid}: >{stype}<'.format(aid=aid, iid=s_iid, stype=ServicesTypes.get_short(s_type)))

                for characteristic in service['characteristics']:
                    c_iid = characteristic['iid']
                    value = characteristic.get('value', '')

                    assertion = {}

                    c_type = characteristic['type']
                    perms = ','.join(characteristic['perms'])
                    desc = characteristic.get('description', '')
                    c_type = CharacteristicsTypes.get_short(c_type)
                    if 'maxLen' in characteristic:
                        maxLen = characteristic.get('maxLen')

                        if value is not None and len(value) <= maxLen:

                            if self.args.quiet == False:
                                cprint('  {aid}.{iid}: {value} [len: {length} <= {maxLen}] ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                              iid=c_iid,
                                                                                              value=value,
                                                                                              length=len(value),
                                                                                              maxLen=maxLen,
                                                                                              ctype=c_type,
                                                                                              perms=perms,
                                                                                              description=desc), "green")


                            assertion["text"] = "Check maxLen of {}.{}: Length: {}/{}".format(aid, c_iid, len(value), maxLen)
                            assertion["result"] = True
                            assertions.append(assertion)

                        elif value is None:
                            if self.args.quiet == False:
                                print('  {aid}.{iid}: {value} [{maxLen}] ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                              iid=c_iid,
                                                                                              value=value,
                                                                                              maxLen=maxLen,
                                                                                              ctype=c_type,
                                                                                              perms=perms,
                                                                                              description=desc))
                        else:

                            cprint('  {aid}.{iid}: {value} [len: {length} <= {maxLen}] ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                              iid=c_iid,
                                                                                              value=value,
                                                                                              length=len(value),
                                                                                              maxLen=maxLen,
                                                                                              ctype=c_type,
                                                                                              perms=perms,
                                                                                              description=desc), "red")

                            assertion["text"] = "Check maxLen of {}.{}: Length: {}/{}".format(aid, c_iid, len(value), maxLen)
                            assertion["result"] = False
                            assertions.append(assertion)
                            success = False


                    elif 'minValue' in characteristic and 'maxValue' in characteristic:

                        minValue = characteristic.get('minValue')
                        maxValue = characteristic.get('maxValue')


                        if isinstance(value, int):




                            if minValue <= value and value <= maxValue:
                                # cprint('  {minValue} < {value} < {maxValue}: '.format(minValue=minValue, value=value, maxValue=maxValue ), "green")
                                if self.args.quiet == False:
                                    cprint('  {aid}.{iid}: {minValue}:> {value} <:{maxValue} ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                                  iid=c_iid,
                                                                                                  value=value,
                                                                                                  ctype=c_type,
                                                                                                  perms=perms,
                                                                                                  description=desc,
                                                                                                  minValue=minValue,
                                                                                                  maxValue=maxValue), "green")

                                assertion["text"] = "Check value of {}.{}: {} < {} < {}".format(aid, c_iid, minValue, value, maxValue)
                                assertion["result"] = True
                                assertions.append(assertion)
                            else:
                                cprint('  {aid}.{iid}: {minValue}>: {value} <:{maxValue} ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                                  iid=c_iid,
                                                                                                  value=value,
                                                                                                  ctype=c_type,
                                                                                                  perms=perms,
                                                                                                  description=desc,
                                                                                                  minValue=minValue,
                                                                                                  maxValue=maxValue), "red")
                                assertion["text"] = "Check value of {}.{}: {} < {} < {}".format(aid, c_iid, minValue, value, maxValue)
                                assertion["result"] = False
                                assertions.append(assertion)
                                success = False

                        else:
                            if self.args.quiet == False:
                                print('  {aid}.{iid}: {value} ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                              iid=c_iid,
                                                                                              value=value,
                                                                                              ctype=c_type,
                                                                                              perms=perms,
                                                                                              description=desc))

                    else:
                        if self.args.quiet == False:
                            print('  {aid}.{iid}: {value} ({description}) >{ctype}< [{perms}]'.format(aid=aid,
                                                                                              iid=c_iid,
                                                                                              value=value,
                                                                                              ctype=c_type,
                                                                                              perms=perms,
                                                                                              description=desc))
        return success


    """
        Create Storage File
    """
    def createStorageFile(self):
        if not os.path.isfile(self.args.file):
            try:
                with open(self.args.file, 'w') as fp:
                    fp.write('{}')
            except PermissionError:
                print('Permission denied to create file "{f}".'.format(f=self.args.file))
        else:
            print('File "{f}" already exists.'.format(f=self.args.file))

    """
        PIN Functions used for pairing
    """
    def pin_from_parameter(self, number):
        def tmp():
            return number
        return tmp

    """
        PIN Functions used for pairing
    """
    def pin_from_keyboard(self):
        def tmp():
            read_pin = ''
            while re.match(r'^\d{3}-\d{2}-\d{3}$', read_pin) is None:
                read_pin = input('Enter device pin (XXX-YY-ZZZ): ')
            return read_pin
        return tmp


    def get_characteristic_decoders(self, pairing: AbstractPairing) -> dict:
        """
        This function filters characteristics of an accessory for decodable types (currently only TLV characteristics) and
        tries to load the decoder functions for said characteristic's uuid.
        :param pairing: an implementation of `AbstractPairing` (either for IP or BLE)
        :return: a dict of aid/cid to decoder functions
        """
        loaded_decoders = {}
        loader = CharacteristicsDecoderLoader()
        for a in pairing.list_accessories_and_characteristics():
            aid = a['aid']
            for s in a['services']:
                for c in s['characteristics']:
                    c_format = c['format']
                    # TODO what about CharacteristicFormats.data?
                    if c_format not in [CharacteristicFormats.tlv8]:
                        continue
                    c_id = c['iid']
                    key = '{}.{}'.format(aid, c_id)
                    c_type = c['type']
                    decoder = loader.load(c_type)
                    if decoder is not None:
                        loaded_decoders[key] = decoder

        return loaded_decoders


    def decode_values(self, input_data):
        """
        Try to decode the values from the given data structure. This depends on:
         - if the field is of type `CharacteristicFormats.tlv8`
         - a decoder was found for the characteristic
        :param input_data: data returned from `AbstractPairing.list_accessories_and_characteristics`
        :return: the input data with decoded value fields
        """
        loader = CharacteristicsDecoderLoader()
        for accessory in input_data:
            for service in accessory['services']:
                for characteristic in service['characteristics']:
                    c_format = characteristic['format']
                    c_type = characteristic['type']
                    # TODO what about CharacteristicFormats.data?
                    if c_format in [CharacteristicFormats.tlv8]:
                        decoder = loader.load(c_type)
                        if decoder:
                            try:
                                characteristic['value'] = decoder(base64.b64decode(characteristic['value']))
                            except Exception as e:
                                logging.error('error during decode %s: %s' % (c_type, characteristic['value']), e)
        return input_data


    def handle_file_values(self, characteristics):
        tmp = []
        for characteristic in characteristics:
            val = characteristic[2]
            if isinstance(val, str) and val.startswith('@'):
                filename = val[1:]
                with open(filename, 'r') as input_file:
                    val = input_file.read()
            tmp.append((characteristic[0], characteristic[1], val, ))
        return tmp



    def runTest(self, name, function, *args):
        if self.args.quiet == False:
            print("=========================== {} ===========================".format(name))

        data, result = None, None
        if args != None:
            data, result = function(name, *args)
        else:
            data, result = function(name)

        if self.args.quiet == False:
            print("========================= End {} =========================".format(name))
            print("")
            print("")
        return data, result

    @measure
    def putCharacteristic(self, testname, characteristics, convert=False):

        loggingArray = []
        if self.args.alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=self.args.alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=self.args.alias))
            return -1, None

        try:
            with TimingManager(testname, loggingArray) as tm:
                pairing = self.controller.get_pairings()[args.alias]


                characteristics = self.handle_file_values(characteristics)
                results = pairing.put_characteristics(characteristics, do_conversion=convert)


        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray, e)
            return -1, None

        passed = True
        strData = []
        for key, value in results.items():
            aid = key[0]
            iid = key[1]
            status = value['status']
            desc = value['description']

            strData.append({"aid": aid, "iid": iid, "status": status, "desc": desc})
            # used to be < 0 but bluetooth le errors are > 0 and only success (= 0) needs to be checked
            if status != 0:
                cprint('put_characteristics failed on {aid}.{iid} because: {reason} ({code})'.format(aid=aid, iid=iid,
                                                                                                    reason=desc,
                                                                                                    code=status), "red")
                passed = False


        self.report.addStep(testname, passed, loggingArray, strData)
        if passed == True:
            if self.args.quiet == False:
                cprint("OK", "green")
        else:
            if self.args.quiet == False:
                cprint("FAILED", "red")

        return passed, None


    @measure
    def getCharacteristic(self, testname, characteristics, includeMeta=True, includePerms=True, includeTypes=True, includeEvents=True):


        loggingArray = []
        if self.args.alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=self.args.alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=self.args.alias))
            return -1, None

        pairing = self.controller.get_pairings()[self.args.alias]

        passed = False
        # get the data
        try:

            with TimingManager(testname, loggingArray) as tm:
                data = pairing.get_characteristics(characteristics, include_meta=includeMeta, include_perms=includePerms,
                                               include_type=includeTypes, include_events=includeEvents)

            self.report.addStep(testname, True, loggingArray, data)
            passed = True
        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray, e)
            return -1, None

        if self.args.decode:
            decoders = self.get_characteristic_decoders(pairing)
        else:
            decoders = {}

        # print the data
        tmp = {}
        for k in data:
            nk = str(k[0]) + '.' + str(k[1])
            value = data[k]

            if decoders.get(nk):
                try:
                    value['value'] = decoders.get(nk)(base64.b64decode(value['value']))
                except Exception as e:
                    logging.ERROR('could not decode', e)

            tmp[nk] = value

        if self.args.quiet == False:
            cprint(json.dumps(tmp, indent=4, cls=tlv8.JsonEncoder), "green")
        return passed, data

    @measure
    def getAccessories(self, testname):

        loggingArray = []


        if self.args.alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=self.args.alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=self.args.alias))
            return -1, None

        try:
            with TimingManager(testname, loggingArray) as tm:
                pairing = self.controller.get_pairings()[self.args.alias]
                data = pairing.list_accessories_and_characteristics()

            self.accessoryData = data

            self.controller.save_data(self.args.file)


        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray, e)
            return -1, None

        if self.args.decode:
            data = self.decode_values(data)

        # prepare output
        if args.output == 'json':
            print(json.dumps(data, indent=4, cls=tlv8.JsonEncoder))


        result = self.validateAccessory(data)
        self.report.addStep(testname, result, loggingArray, data)

        return result, data


    def printEvents(self, events):
        for event in events:
            print('event for {aid}.{iid}: {event}'.format(aid=event[0], iid=event[1], event=event[2]))
            self.eventCount = self.eventCount + 1


    @measure
    def listenEvents(self, testname):
        loggingArray = []
        if self.args.alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=self.args.alias), "red")
            logging.debug('"{a}" is no known alias'.format(a=self.args.alias), exc_info=True)
            self.report.addStep(testname, False, loggingArray, '"{a}" is no known alias'.format(a=self.args.alias))
            return -1, None

        print("Waiting for {} events ...".format(self.args.eventCount))


        try:

            with TimingManager(testname, loggingArray) as tm:

                pairing = self.controller.get_pairings()[self.args.alias]
                characteristics = [(int(c.split('.')[0]), int(c.split('.')[1])) for c in self.args.characteristicsEvent]
                results = pairing.get_events(characteristics, self.printEvents, max_events=self.args.eventCount, max_seconds=self.args.secondsCount)

            self.report.addStep(testname, True, loggingArray)


        except KeyboardInterrupt:
            #sys.exit(-1)
            self.report.addStep(testname, False, loggingArray, "User canceled")
            return -1, None

        except Exception as e:
            print(e)
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray, e)
            #sys.exit(-1)
            return -1, None

        for key, value in results.items():
            aid = key[0]
            iid = key[1]
            status = value['status']
            desc = value['description']
            if status < 0:
                print('put_characteristics failed on {aid}.{iid} because: {reason} ({code})'.format(aid=aid, iid=iid,
                                                                                                    reason=desc,
                                                                                                  code=status))


        if self.eventCount == self.args.eventCount:
            cprint('Received {}/{} events'.format(self.eventCount, self.args.eventCount), "green")
            return True, None
        else:
            cprint('Received {}/{} events'.format(self.eventCount, self.args.eventCount), "red")
            return False, None


    @measure
    def removePairing(self, testname):

        loggingArray = []

        if self.args.alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=self.args.alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=self.args.alias))
            return 0, None


        try:
            with TimingManager(testname, loggingArray) as tm:
                self.controller.remove_pairing(self.args.alias, self.args.controllerPairingId)

            self.report.addStep(testname, True, loggingArray)

            self.controller.save_data(self.args.file)
            if self.args.quiet == False:
                cprint('Pairing for "{a}" was removed.'.format(a=self.args.alias), "green")


        except Exception as e:
            cprint("{a}".format(a=e), "red")
            self.report.addStep(testname, False, loggingArray,"{a}".format(a=e))
            logging.debug(e, exc_info=True)
            return 0, None
        
        return 1, None

    @measure
    def removeRemotePairing(self, testname):

        loggingArray = []
        alias = self.args.alias + "+add"
        if alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=alias))
            return 0, None


        try:
            with TimingManager(testname, loggingArray) as tm:
                self.controller.remove_pairing(alias, self.remoteId)

                self.report.addStep(testname, True, loggingArray)

                self.controller.save_data(self.args.file)
                if self.args.quiet == False:
                    cprint('Pairing for "{a}" was removed.'.format(a=alias), "green")


        except Exception as e:
            cprint("{a}".format(a=e), "red")
            self.report.addStep(testname, False, loggingArray,"{a}".format(a=e))
            logging.debug(e, exc_info=True)
            return 0, None
        
        return 1, None        

    @measure
    def listPairings(self, testname):
        loggingArray = []

        if self.args.alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=self.args.alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=self.args.alias))
            return 0, None

        pairing = self.controller.get_pairings()[self.args.alias]
        try:
            with TimingManager(testname, loggingArray) as tm:
                pairings = pairing.list_pairings()

                data = []
                counter = 1
                print("")
                for pairing in pairings:
                    data.append({"pairingId": pairing['pairingId'], "publicKey": pairing['publicKey'], "permissions": pairing['permissions'], "controllerType": pairing['controllerType']})
                    if self.args.quiet == False:
                        cprint('{c}. Pairing Id: {id}'.format(c=counter, id=pairing['pairingId']), "green")
                        cprint('   Public Key: 0x{key}'.format(key=pairing['publicKey']), "green")
                        cprint('   Permissions: {perm} ({type})'.format(perm=pairing['permissions'],
                                                                    type=pairing['controllerType']), "green")
                    counter = counter + 1

            self.report.addStep(testname, (len(pairings) > 0), loggingArray, data)

        except Exception as e:
            cprint("{a}".format(a=e), "red")
            self.report.addStep(testname, False, loggingArray,"{a}".format(a=e))
            logging.debug(e, exc_info=True)
            return 0, None

        return 1, None

    @measure
    def prepareAddAdditionalPairing(self, testname):
        loggingArray = []

        alias = self.args.alias + "+add"
        print("")
        try:
            
            with TimingManager(testname, loggingArray) as tm:
                pairings = self.controller.get_pairings()
                if alias in pairings:
                    pairing_data = pairings[alias]._get_pairing_data()
                    additional_controller_pairing_identifier = pairing_data['iOSPairingId']
                    ios_device_ltpk = pairing_data['iOSDeviceLTPK']
                    text = 'Alias "{a}" is already in state add additional pairing.\n'\
                        'Please add this to homekit.add_additional_pairing:\n'\
                        '    -i {id} -k {pk}'\
                        .format(a=alias,
                                id=additional_controller_pairing_identifier,
                                pk=ios_device_ltpk
                                )
                    cprint(text, "green")
                    a = {
                        'iOSPairingId': additional_controller_pairing_identifier,
                        'iOSDeviceLTPK': ios_device_ltpk,
                        'Connection': 'ADDITIONAL_PAIRING'
                    }
                    self.report.addStep(testname, True, loggingArray, text)
                    return 0, a


                additional_controller_pairing_identifier = str(uuid.uuid4())
                ios_device_ltsk, ios_device_ltpk = ed25519.create_keypair()

                public_key = binascii.hexlify(ios_device_ltpk.to_bytes()).decode()

                text = 'Please add this to homekit.add_additional_pairing:\n' \
                    '    -i {id} -k {pk}' \
                    .format(id=additional_controller_pairing_identifier,
                            pk=public_key
                            )
                cprint(text, "green")

                a = {
                    'iOSPairingId': additional_controller_pairing_identifier,
                    'iOSDeviceLTSK': ios_device_ltsk.to_ascii(encoding='hex').decode()[:64],
                    'iOSDeviceLTPK': public_key,
                    'Connection': 'ADDITIONAL_PAIRING'
                }
                pairings[alias] = AdditionalPairing(a)
                self.controller.save_data(self.args.file)

            self.report.addStep(testname, True, loggingArray, a)
            return 0, a

        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray, e)
            return -1, None

    @measure
    def addAdditionalPairing(self, testname, pairing_id, key, permission):
        loggingArray = []

        alias = self.args.alias + "+add"

        print("")
        if alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=alias))
            return -1, None

        try:
            with TimingManager(testname, loggingArray) as tm:
                pairing = self.controller.get_pairings()[self.args.alias]
                pairing.add_pairing(pairing_id, key, permission)
                pairing.pairing_data['Connection'] = 'IP'
                if pairing.pairing_data['Connection'] == 'IP':
                    text = 'Please add this to homekit.finish_add_remote_pairing:\n' \
                        '    -c {c} -i {id} -k {pk}' \
                        .format(c=pairing.pairing_data['Connection'],
                                id=pairing.pairing_data['AccessoryPairingID'],
                                pk=pairing.pairing_data['AccessoryLTPK']
                                )
                    cprint(text, "green")
                    self.report.addStep(testname, True, loggingArray, text)

                elif pairing.pairing_data['Connection'] == 'BLE':
                    text = 'Please add this to homekit.finish_add_remote_pairing:\n' \
                        '    -c {c} -i {id} -m {mac} -k {pk}' \
                        .format(c=pairing.pairing_data['Connection'],
                                id=pairing.pairing_data['AccessoryPairingID'],
                                mac=pairing.pairing_data['AccessoryMAC'],
                                pk=pairing.pairing_data['AccessoryLTPK']
                                )
                    cprint(text, "green")
                    self.report.addStep(testname, True, loggingArray, text)

                else:
                    print('Not known')
                    self.report.addStep(testname, False, loggingArray, "Not known")
                    return -1, None
            return 0, {"Connection": pairing.pairing_data['Connection'], "AccessoryPairingID": pairing.pairing_data['AccessoryPairingID'], "AccessoryLTPK": pairing.pairing_data['AccessoryLTPK']}
        except HomeKitException as exception:
            print(exception)
            logging.debug(exception, exc_info=True)
            self.report.addStep(testname, False, loggingArray, exception)
            return -1, None

    @measure
    def finishAddAdditionalPairing(self, testname, id, key, mac):
        loggingArray = []

        alias = self.args.alias + "+add"

        if alias not in self.controller.get_pairings():
            cprint('"{a}" is no known alias'.format(a=alias), "red")
            self.report.addStep(testname, False, loggingArray,'"{a}" is no known alias'.format(a=alias))
            return -1, None
        
        print("")
        try:
            with TimingManager(testname, loggingArray) as tm:
                pairings = self.controller.get_pairings()

                pairing_data = pairings[alias]._get_pairing_data()
                #pairing_data['Connection'] = self.args.connection
                pairing_data['Connection'] = 'IP'
                # if args.connection == 'IP':
                pairing_data['AccessoryPairingID'] = id
                pairing_data['AccessoryLTPK'] = key
                # elif args.connection == 'BLE':
                #     pairing_data['AccessoryPairingID'] = id
                #     pairing_data['AccessoryLTPK'] = key
                #     pairing_data['AccessoryMAC'] = mac

                self.controller.save_data(self.args.file)
                self.report.addStep(testname, True, loggingArray)
                return 0, id
        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray,"{a}".format(a=e))
            return -1, None

        try:
            pairing = self.controller.get_pairings()[self.args.alias]
            data = pairing.list_accessories_and_characteristics()
            self.controller.save_data(self.args.file)

            self.report.addStep(testname, True, loggingArray, data)

            return 0, None
        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname, False, loggingArray,"{a}".format(a=e))
            return -1, None


    @measure
    def addRemotePairing(self, testname):

        loggingArray = []
        with TimingManager(testname, loggingArray) as tm:
            result, data = self.prepareAddAdditionalPairing(testname + " step 1")
            if result != 0:
                return -1, None
            print(data)
            result, data = self.addAdditionalPairing(testname + " step 2", data["iOSPairingId"], data["iOSDeviceLTPK"], 0)
            if result != 0:
                return -1, None
            print(data)
            result, self.remoteId = self.finishAddAdditionalPairing(testname + " step 3", data["AccessoryPairingID"], data["AccessoryLTPK"], None)

        return 0, self.remoteId


    @measure
    def pair(self, testname):

        loggingArray = []
        if self.args.alias in self.controller.get_pairings():
            cprint('"{a}" is a already known alias'.format(a=self.args.alias), "green")
            self.report.addStep(testname + " step 1", False, loggingArray)
            self.report.addStep(testname + " step 2", False, loggingArray, "SKIPPED")
            return 0, None

        if self.args.pin:
            self.pin_function = self.pin_from_parameter(self.args.pin)
        else:
            self.pin_function = self.pin_from_keyboard()

        try:

            with TimingManager(testname + " step 1", loggingArray) as tm:
                finish_pairing = self.controller.start_pairing(self.args.alias, self.args.device)
                finish_pairing(self.pin_function())

            self.report.addStep(testname + " step 1", True, loggingArray)


        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname + " step 1", False, loggingArray, e)
            self.report.addStep(testname + " step 2", False, loggingArray, "SKIPPED")
            return -1, None

        loggingArray = []
        try:
            with TimingManager(testname + " step 2", loggingArray) as tm:
                pairing = self.controller.get_pairings()[self.args.alias]
                data = pairing.list_accessories_and_characteristics()
                self.accessoryData = {"accessories": data }

            self.report.addStep(testname + " step 2", True, loggingArray)


            self.controller.save_data(self.args.file)
            if self.args.quiet == False:
                cprint('Pairing for "{a}" was established.'.format(a=self.args.alias), "green")

            #print(self.loggingArray)
            return 1, None
        except Exception as e:
            cprint(e, "red")
            logging.debug(e, exc_info=True)
            self.report.addStep(testname + " step 2", False, loggingArray, e)
            return -1, None

    def toHexString(self, bytes):
        return "".join(map(lambda b: format(b, "02x"), bytes))


    def getHistoryEntry(self):
        passed, data = self.runTest("getHistoryEntry", self.getCharacteristic, [(2, 19)])

        if passed == True:
            #print(data)
            return passed, data
            #print(b)


        return passed, None


    def putHistoryAddress(self, address):
        data = struct.pack('<bbhhb', 0x01, 0x14, address, 0,0)
        #hexdata = "".join(map(lambda b: format(b, "02x"), data))
        #print(hexdata)

        base64_bytes = base64.b64encode(data)
        #print(base64_bytes)

        passed, data = self.runTest("putHistoryAddress", self.putCharacteristic, [(2, 20, base64_bytes.decode("utf-8"))], False)


    def getHistoryStatus(self):
        passed, data = self.runTest("getHistoryStatus", self.getCharacteristic, [(2, 18)])
        if passed == True:
            #print(data[(2, 18)]["value"])

            if data[(2, 18)]["value"] is None:
                return None

            base64_bytes = base64.b64decode(data[(2, 18)]["value"])

            hexdata = "".join(map(lambda b: format(b, "02x"), base64_bytes))
            b = bytearray.fromhex(hexdata)
            #print(b)

            self.openInHexFiend(b)


            s = struct.unpack('<iiib', b[:13])
            #print(s)

            fmt = "<iiib"
            fmt_len = 13
            for i in range(0, s[3]):
                fmt += "bb"
                fmt_len += 2
            fmt += "hhi"
            fmt_len += 8

            #print(fmt_len)

            s = struct.unpack(fmt, b[:fmt_len])
            #print(s)

            signature = []
            for i in range(4,len(s)-3):
                signature.append(s[i])

            fakegatoInfo = {
                "evetime": s[0],
                "negativeOffset": s[1],
                "refTime": s[2],
                "sigLength": s[3],

                "signature": signature,

                "used": s[len(s)-3],
                "size": s[len(s)-2],
                "rollover": s[len(s)-1]
            }

            #print(fakegatoInfo)
            return fakegatoInfo

    def openInHexFiend(self, byte):
        if self.args.openHexfiend:
            os.system("echo " + byte.hex() + " | xxd -r -p | hexf")


    def runFakegato(self):

        loggingArray = []
        with TimingManager("Fakegato", loggingArray) as tm:
            info = self.getHistoryStatus()

            if info is None:
                return False, None

            if self.args.quiet == False:
                print(info)

            if int(info["rollover"]) > 0:
                self.requestedEntry = info["rollover"]
            else:
                self.requestedEntry = 0




            historyData = bytearray()

            allPassed = True
            while self.requestedEntry < info["size"]:

                if self.args.quiet == False:
                    print("requestedEntry: " + str(self.requestedEntry))

                self.putHistoryAddress(self.requestedEntry + 1)
                passed, data = self.getHistoryEntry()
                if passed == True:
                    base64_bytes = base64.b64decode(data[(2, 19)]["value"])

                    hexdata = "".join(map(lambda b: format(b, "02x"), base64_bytes))
                    historyData.extend(bytearray.fromhex(hexdata))
                else:
                    allPassed = False

                self.requestedEntry += 16

                if self.requestedEntry > info["used"] or data[(2,19)]["value"] == "AA==":
                    break


            self.fakegatoHistories.append(historyData)

            self.report.addStep("Fakegato", allPassed, loggingArray)
        return allPassed, self.fakegatoHistories


def setup_args_parser():
    parser = argparse.ArgumentParser(description='HomeKit IP pairing app')
    parser.add_argument('-d', action='store', required=True, dest='device',
                        help='HomeKit Device ID (use discover to get it)')
    parser.add_argument('-p', action='store', required=False, dest='pin', help='HomeKit configuration code')
    parser.add_argument('-f', action='store', required=True, dest='file', help='HomeKit pairing data file')
    parser.add_argument('-a', action='store', required=True, dest='alias', help='alias for the pairing')
    parser.add_argument('-g', action='store', required=False, dest='gitCommit', default=None, help='git commit sha1')
    parser.add_argument('-H', action='store', required=False, dest='openHexfiend', default=False, help='open fakegato in hex fiend', type=bool)
    parser.add_argument('-i', action='store', required=False, dest='controllerPairingId',
                        help='this pairing ID identifies the controller who should be REMOVED from accessory')
    parser.add_argument('-o', action='store', dest='output', default='compact', choices=['json', 'compact'],
                        help='Specify output format')
    parser.add_argument('--adapter', action='store', dest='adapter', default='hci0',
                        help='the bluetooth adapter to be used (defaults to hci0)')
    parser.add_argument('-D', action='store_true', required=False, dest='decode',
                        help='If set, try to find a decoder for each characteristic and show the decoded value')
    parser.add_argument('-c', action='append', required=True, dest='characteristics',
                        help='READ characteristics, multiple characteristics can be given by repeating the option')
    parser.add_argument('-C', action='append', required=False, dest='characteristicsPut', nargs=2,
                        help='Use `aid.iid value` to CHANGE the value. Repeat to change multiple characteristics. '
                             'If the value starts with `@` it is interpreted as a file')
    parser.add_argument('-e', action='append', required=True, dest='characteristicsEvent',
                        help='Use aid.iid for subscribing to a characteristic. Repeat to subscribe to multiple characteristics.')
    parser.add_argument('-n', action='store', required=False, dest='eventCount', help='max number of events before end',
                        default=-1, type=int)
    parser.add_argument('-N', action='store', required=False, dest='iterations', help='iterations do execute',
                        default=-1, type=int)
    parser.add_argument('-s', action='store', required=False, dest='secondsCount', default=-1, type=int,
                        help='max number of seconds before end')
    parser.add_argument('-r', action='store_true', required=False, dest='report',
                        help='create report')
    parser.add_argument('-R', action='store', dest='reportFormat', default='md', choices=['md', 'json'],
                        help='Specify report format')
    parser.add_argument('-z', action='store_true', required=False, dest='summary',
                        help='print summary')
    parser.add_argument('-q', action='store_true', required=False, default=False, dest='quiet',
                        help='surpress output')
    parser.add_argument('-u', action='store_true', required=False, default=False, dest='uploadReport',
                        help='Upload report to gitlab')

    add_log_arguments(parser)
    args = parser.parse_args()

    if 'characteristics' not in args or not args.characteristics:
        parser.print_help()
        sys.exit(-1)
    if 'characteristicsPut' not in args or not args.characteristicsPut:
        parser.print_help()
        sys.exit(-1)
    return args




if __name__ == '__main__':
    args = setup_args_parser()


    #print(args.characteristics)
    #print(args.characteristicsPut)

    # convert the command line parameters to the required form
    characteristics = [(int(c.split('.')[0]), int(c.split('.')[1])) for c in args.characteristics]
    characteristicsPut = [(int(c[0].split('.')[0]),  # the first part is the aid, must be int
                                int(c[0].split('.')[1]),  # the second part is the iid, must be int
                                c[1]) for c in args.characteristicsPut]


    #print(characteristics)
    #print(characteristicsPut)


    tester = HomekitTester(args)

    for k in range(0, args.iterations):

        tester.runTest("pair", tester.pair)

        tester.runTest("listPairings", tester.listPairings)
        # tester.runTest("addRemotePairing", tester.addRemotePairing)
        # tester.runTest("removeRemotePairing", tester.removeRemotePairing)

        tester.runTest("getAccessories", tester.getAccessories)
        tester.runTest("getCharacteristic", tester.getCharacteristic, characteristics)
        #tester.runTest("putCharacteristic", tester.putCharacteristic, characteristicsPut)
        tester.runTest("getAccessories", tester.getAccessories)

        tester.runTest("listenEvents", tester.listenEvents)

        tester.runFakegato()

        for historyEntry in tester.fakegatoHistories:
            tester.openInHexFiend(historyEntry)

        tester.runTest("removePairing", tester.removePairing)

        tester.runTest("pair", tester.pair)
        for i in range(0, args.iterations):
            print(i)
            #time.sleep(0.1)
            tester.runTest("getAccessories", tester.getAccessories)
        tester.runTest("removePairing", tester.removePairing)


        for i in range(0, args.iterations):
            print(i)
            time.sleep(0.3)
            tester.runTest("pair", tester.pair)
            tester.runTest("getAccessories", tester.getAccessories)
            tester.runTest("removePairing", tester.removePairing)


    if args.summary == True:
        tester.printSummary()

    if args.uploadReport == True:
        tester.uploadReportToGitlab()

    if args.report == True:
        tester.saveReport("./reports/", args.reportFormat)
        #tester.saveReport("/Volumes/docker/markserv/data/Testreports", args.reportFormat)






