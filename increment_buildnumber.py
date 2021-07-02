#Import("env")

""" 
    Create version header and tracker file if missing 
"""
from datetime import datetime
import os
import getpass
import os
import subprocess


BUILDNUMBER_HEADER = "./src/HAP/HAPBuildnumber.hpp"
BUILDNUMBER_CONTENT = """//
// HAPBuildnumber.hpp
// Homekit
//
//  Created on: {}
//      Author: {}
//

#ifndef HAPBUILDNUMBER_HPP_
#define HAPBUILDNUMBER_HPP_

#define HOMEKIT_VERSION_BUILD {}
#define HOMEKIT_VERSION_BUILD_STR "{}"

#define HOMEKIT_COMPILE_TIME "{}"

#endif /* HAPBUILDNUMBER_HPP_ */"""


VERSION_HEADER = "./src/HAP/HAPVersionHomekit.hpp"
VERSION_CONTENT = """//
// HAPVersionHomekit.hpp
// Teensy_Homekit
//
//  Created on: {}
//      Author: {}
//

#ifndef HAPVERSIONHOMEKIT_HPP_
#define HAPVERSIONHOMEKIT_HPP_

#define HOMEKIT_PROJECT             "{}"

#define HOMEKIT_GIT_BRANCH          "{}"
#define HOMEKIT_GIT_REV             "{}"
#define HOMEKIT_GIT_TAG             "{}"

#define HOMEKIT_VERSION_MAJOR 		{}
#define HOMEKIT_VERSION_MINOR 		{}
#define HOMEKIT_VERSION_REVISION 	{}

#endif /* HAPVERSIONHOMEKIT_HPP_ */"""



# construct minimal environment
def _minimal_ext_cmd(cmd):
    ext_env = {}
    for k in ['SYSTEMROOT', 'PATH']:
        v = os.environ.get(k)
        if v is not None:
            ext_env[k] = v
    # LANGUAGE is used on win32
    ext_env['LANGUAGE'] = 'C'
    ext_env['LANG'] = 'C'
    ext_env['LC_ALL'] = 'C'
    out = subprocess.Popen(cmd, stdout = subprocess.PIPE, env=ext_env).communicate()[0]

    return out

# Get Git project name
def gitProjectName():
    try:
        out = _minimal_ext_cmd(['git', 'rev-parse', '--show-toplevel'])
        GIT_PROJECT = out.strip().decode('ascii').strip()
        GIT_PROJECT = GIT_PROJECT.split("/")
        GIT_PROJECT = GIT_PROJECT[len(GIT_PROJECT)-1]
    except OSError:
        GIT_PROJECT = "Unknown"
    return GIT_PROJECT


# Get 0.0.0 version from latest Git tag
def gitTag():
    try:
        out = _minimal_ext_cmd(['git', 'describe', '--tags', '--abbrev=0'])
        GIT_TAG = out.strip().decode('ascii').strip()
    except OSError:
        GIT_TAG = "Unknown"
    return GIT_TAG


# Get branch name from Git
def gitBranch():
    try:
        out = _minimal_ext_cmd(['git', 'rev-parse', '--abbrev-ref', 'HEAD'])
        GIT_BRANCH = out.strip().decode('ascii').strip()
    except OSError:
        GIT_BRANCH = "Unknown"
    return GIT_BRANCH

# Get latest commit short from Git
def gitCommit():
    try:
        out = _minimal_ext_cmd(['git', 'rev-parse', '--short', 'HEAD'])
        GIT_REVISION = out.strip().decode('ascii').strip()
    except OSError:
        GIT_REVISION = "Unknown"

    return GIT_REVISION

def incrementBuildNumber(buildnumber):
    return (int(buildnumber) + 1)


def parseForBuildnumber(lines):
    buildNumber = 0
    for line in lines:
        if line.startswith("#define HOMEKIT_VERSION_BUILD "):
            buildNumber = int(line.split(" ")[-1:][0])
    return buildNumber


def splitTag(tag):
    mayor = int(tag.split(".")[0])
    minor = int(tag.split(".")[1])
    rev = int(tag.split(".")[2])
    return mayor, minor, rev


def initBuildNumber(fileName = ".buildnumber"):
    if not os.path.exists(fileName):
        f = open(fileName, "a")
        f.close()
        createBuildnumber(1)
        return 1
    else:
        return readBuildnumber(fileName)

def createBuildnumber(buildnumber, fileName = ".buildnumber"):
    with open(fileName, "w") as f:
        f.seek(0)
        f.write("{}".format(buildnumber))
        f.truncate()

def createBuildnumberHeader(buildnumberContent, fileName = BUILDNUMBER_HEADER):
    with open(fileName, "w") as f:
        f.seek(0)
        f.write("{}".format(buildnumberContent))
        f.truncate()


def readBuildnumber(fileName = ".buildnumber"):
    buildnumber = 1
    with open(fileName, "r") as f:
        buildnumber = f.read()
    return int(buildnumber)

def readBuildnumberHeader(fileName = BUILDNUMBER_HEADER):
    with open(fileName, "r") as f:
        lines = f.readlines()
        return int(parseForBuildnumber(lines))
    return 1

def createVersionHeader(versionContent, fileName = VERSION_HEADER):
    with open(fileName, "w") as f:
        f.seek(0)
        f.write("{}".format(versionContent))
        f.truncate()

now = datetime.now()


###
### Buildnummber
###
if not os.path.exists(".buildnumber_no_increment"):
    buildNumber = initBuildNumber()
    buildNumber = incrementBuildNumber(buildNumber)
    createBuildnumber(buildNumber)

    print("BUILDNUMBER: {}".format(buildNumber))

    # fileContent = BUILDNUMBER_CONTENT.format(now.strftime("%d.%m.%Y"), getpass.getuser(), buildNumber, buildNumber, now.strftime("%d.%m.%Y %H:%M"))

    # buildNumber = parseForBuildnumber(fileContent)
    # buildNumber = incrementBuildNumber(buildNumber)

    # env.Append(CPPDEFINES=[
    #     ("HOMEKIT_VERSION_BUILD", buildNumber),
    #     ("HOMEKIT_VERSION_BUILD_STR", str(buildNumber)),
    #     ("HOMEKIT_COMPILE_TIME", now.strftime("%d.%m.%Y %H:%M")),
    # ])

    output = BUILDNUMBER_CONTENT.format(now.strftime("%d.%m.%Y"), getpass.getuser(), buildNumber, buildNumber, now.strftime("%d.%m.%Y %H:%M"))
    # write output to src/HAP/HAPBuildnumber.hpp
    createBuildnumberHeader(output)

###
### Version
###
if not os.path.exists(".version_no_increment"):
    mayor, minor, rev = splitTag(gitTag())

    # append extra flags to only project build environment
    # env.Append(CPPDEFINES=[
    #     ("HOMEKIT_PROJECT", gitProjectName()),
    #     ("HOMEKIT_GIT_BRANCH", gitBranch()),
    #     ("HOMEKIT_GIT_REV", gitCommit()),
    #     ("HOMEKIT_GIT_TAG", gitTag()),
    #     ("HOMEKIT_VERSION_MAJOR", mayor),
    #     ("HOMEKIT_VERSION_MINOR", minor),
    #     ("HOMEKIT_VERSION_REVISION", rev),
    # ])
    print("GIT:")
    print(" - project: {}".format(gitProjectName()))
    print(" - branch: {}".format(gitBranch()))
    print(" - commit: {}".format(gitCommit()))
    print(" - tag: {}".format(gitTag()))

    output = VERSION_CONTENT.format(now.strftime("%d.%m.%Y"), getpass.getuser(), gitProjectName(), gitBranch(), gitCommit(), gitTag(), mayor, minor, rev)
    #print(output)
    # write output to src/HAP/HAPVersionHomekit.hpp
    createVersionHeader(output)
