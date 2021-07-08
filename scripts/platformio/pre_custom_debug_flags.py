Import("env")



if env.GetBuildType() == "debug":
   print("Applying custom debug flags ...", end="")
   for scope in ("ASFLAGS", "CCFLAGS", "LINKFLAGS"):
      for i, flag in enumerate(env[scope]):
         if flag == "-Og":
            env[scope][i] = "-O0"
   print("OK")