""" 
    Create version header and tracker file if missing 
"""
from datetime import datetime
import os
import getpass
import os
import subprocess

#Import("env")

# Return the git revision as a string
def git_version():
    def _minimal_ext_cmd(cmd):
        # construct minimal environment
        env = {}
        for k in ['SYSTEMROOT', 'PATH']:
            v = os.environ.get(k)
            if v is not None:
                env[k] = v
        # LANGUAGE is used on win32
        env['LANGUAGE'] = 'C'
        env['LANG'] = 'C'
        env['LC_ALL'] = 'C'
        out = subprocess.Popen(cmd, stdout = subprocess.PIPE, env=env).communicate()[0]
        return out

    try:
        out = _minimal_ext_cmd(['git', 'rev-parse', '--short', 'HEAD'])
        GIT_REVISION = out.strip().decode('ascii')
    except OSError:
        GIT_REVISION = "Unknown"

    return GIT_REVISION

def incrementBuildNumber(buildnumber):
    return buildnumber + 1

#VERSION_FILE = 'version'
BUILDNUMBER_HEADER = "HAP/HAPBuildnumber.hpp"

BUILDNUMBER_CONTENT = """//
// HABuildnumber.hpp
// Homekit
//
//  Created on: {}
//      Author: {}
//

#ifndef HAPBUILDNUMBER_HPP_
#define HAPBUILDNUMBER_HPP_

#define HOMEKIT_VERSION_BUILD {}
#define HOMEKIT_VERSION_BUILD_STR "{}"
#define HOMEKIT_GIT_REV "{}"

#endif /* HAPBUILDNUMBER_HPP_ */"""



now = datetime.now()
buildNumber = 153


output = BUILDNUMBER_CONTENT.format(now.strftime("%d.%m.%Y"), getpass.getuser(), buildNumber, buildNumber, git_version())
lines = output.split('\n')

for line in lines:
    if line.startswith("#define HOMEKIT_VERSION_BUILD "): 
        buildNumber = int(line.split(" ")[-1:][0])

buildNumber = incrementBuildNumber(buildNumber)


output = BUILDNUMBER_CONTENT.format(now.strftime("%d.%m.%Y"), getpass.getuser(), buildNumber, buildNumber, git_version())
print(output)
#if not os.path.exists(".buildnumber_no_increment"):