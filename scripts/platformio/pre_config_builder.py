#Import("env", "projenv")
Import("env")


config = {
}

def splitByNewLine(string):
    array = []
    lines = string.split('\n')
    for line in lines:
        if len(line) > 0:
            array.append(line)
    return array


def splitIncludesToTuples(string):
    array = []
    lines = string.split('\n')

    #print(lines)
    for line in lines:
        if line.startswith("-I"):
            line = line[2:].strip()      

            if len(line) > 0:
                if not line.startswith(env["PROJECT_DIR"]):
                    line = env["PROJECT_DIR"] + "/" + line
                array.append(line)
    return array


def splitDefinesToTuples(string):
    array = []
    lines = string.split('\n')

    #print(lines)
    for line in lines:
        if line.startswith("-D"):
            line = line[2:].strip()
        if "=" in line:
            #print(line.split("="))
            parts = line.split("=")
            for p in parts:
                p = p.strip()

            if (parts[1].isnumeric()):
                parts[1] = int(parts[1])
            
            array.append(tuple(parts))
        else:        

            if len(line) > 0:
                array.append(line)
    return array

def optionsToDict(string):
    d = {}
    lines = string.split('\n')

    for line in lines:
        #print(line)
        if "=" in line:
            #print(line.split("="))
            d[line.split("=")[0].strip()] = line.split("=")[1].strip()
        else:
            if len(line) > 0:
                d[line] = None
    return d

def getOptions(config):
    # config["options"] = env.GetProjectConfig().items("options", as_dict=True)
    
    #config["options"]["config_eeprom"] = env.GetProjectOption("custom_option_config_eeprom");
    #config["options"]["qrcode"] = env.GetProjectOption("custom_option_qrcode");

    config["options"] = optionsToDict(env.GetProjectOption("custom_options"));



def processOption(optionKey):
    #print(optionKey)
    details = env.GetProjectConfig().items("option_" + optionKey, as_dict=True)
    #print(details)
    for key, value in details.items():
        key = key.strip()

        
        if len(value) > 0:            
            if (key == optionKey + "_build_flags"):
                buildflags = splitByNewLine(value)
                #print(buildflags)
                env.Append(
                   BUILD_FLAGS=buildflags
                )
                includes = splitIncludesToTuples(value)
                #print(includes)
                env.Append(
                   CPPPATH=includes
                )
            elif (key == optionKey + "_cpp_defines"):
                defines = splitDefinesToTuples(value)
                #print(defines)
                env.Append(
                   CPPDEFINES=defines
                )                            
            elif (key == optionKey + "_src_filter"):
                files = splitByNewLine(value)
                #print(files)
                env.Append(
                   SRC_FILTER=files
                )
            elif (key == optionKey + "_cpp_path"):
                includes = splitIncludesToTuples(value)
                #print(includes)
                env.Append(
                   CPPPATH=includes
                )

def processPlugin(pluginKey):
    #print(pluginKey)
    details = env.GetProjectConfig().items("plugin_" + pluginKey, as_dict=True)
    #print(details)
    for key, value in details.items():
        key = key.strip()

        if len(value) > 0:     
            #print(key)
            if (key == pluginKey + "_build_flags"):
                buildflags = splitByNewLine(value)
                #print(buildflags)
                env.Append(
                   BUILD_FLAGS=buildflags
                )
            elif (key == pluginKey + "_cpp_defines"):
                defines = splitDefinesToTuples(value)
                #print(defines)
                env.Append(
                   CPPDEFINES=defines
                )
            elif (key == pluginKey + "_cpp_path"):
                includes = splitIncludesToTuples(value)
                #print(includes)
                env.Append(
                   CPPPATH=includes
                )
            elif (key == pluginKey + "_src_filter"):
                files = splitByNewLine(value)
                #print(files)
                env.Append(
                   SRC_FILTER=files
                )
            elif (key == pluginKey + "_cpp_path"):
                includes = splitIncludesToTuples(value)
                #print(includes)
                env.Append(
                   CPPPATH=includes
                )



def processOptions(options):    
    for key, value in options.items():    
        #print(key, '->', value)
        if int(value) == 1:
            processOption(key)
            
def processPlugins(plugins):    
    for key, value in plugins.items():    
        #print(key, '->', value)
        if int(value) == 1:
            processPlugin(key)


def getPlugins(config):
    config["plugins"] = env.GetProjectConfig().items("plugins", as_dict=True)



print("Applying build configuration ...", end="")
getOptions(config)
getPlugins(config)

processOptions(config["options"])
processPlugins(config["plugins"])
print("OK")
#print("config: {}".format(config))
#print(projenv["CPPPATH"])
#print(env.Dump())


