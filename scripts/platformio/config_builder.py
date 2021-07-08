Import("env", "projenv")


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
                if not line.startswith(projenv["PROJECT_DIR"]):
                    line = projenv["PROJECT_DIR"] + "/" + line
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
                projenv.Append(
                   BUILD_FLAGS=buildflags
                )
                includes = splitIncludesToTuples(value)
                #print(includes)
                projenv.Append(
                   CPPPATH=includes
                )
            elif (key == optionKey + "_cpp_defines"):
                defines = splitDefinesToTuples(value)
                #print(defines)
                projenv.Append(
                   CPPDEFINES=defines
                )                            
            elif (key == optionKey + "_src_filter"):
                files = splitByNewLine(value)
                #print(files)
                projenv.Append(
                   SRC_FILTER=files
                )
            elif (key == optionKey + "_cpp_path"):
                includes = splitIncludesToTuples(value)
                #print(includes)
                projenv.Append(
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
                projenv.Append(
                   BUILD_FLAGS=buildflags
                )
            elif (key == pluginKey + "_cpp_defines"):
                defines = splitDefinesToTuples(value)
                #print(defines)
                projenv.Append(
                   CPPDEFINES=defines
                )
            elif (key == pluginKey + "_cpp_path"):
                includes = splitIncludesToTuples(value)
                #print(includes)
                projenv.Append(
                   CPPPATH=includes
                )
            elif (key == pluginKey + "_src_filter"):
                files = splitByNewLine(value)
                #print(files)
                projenv.Append(
                   SRC_FILTER=files
                )
            elif (key == pluginKey + "_cpp_path"):
                includes = splitIncludesToTuples(value)
                #print(includes)
                projenv.Append(
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


def mytarget_callback(*args, **kwargs):
    #print("Hello PlatformIO!")
    #env.Execute("ping " + host)
    getOptions(config)
    getPlugins(config)

    processOptions(config["options"])
    processPlugins(config["plugins"])

    print("config: {}".format(config))
    print(projenv["CPPPATH"])
    #print(env.Dump())


env.AddCustomTarget(
    "config_builder", 
    None, 
    mytarget_callback,
    always_build=True
)