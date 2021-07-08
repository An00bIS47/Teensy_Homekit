Import("env")


if env.GetBuildType() == "debug":
    print("Applying debug build flags", end="")
    env.Append(
        BUILD_FLAGS=["-D DEBUG=1"]
    )

#print(env["BUILD_FLAGS"])
