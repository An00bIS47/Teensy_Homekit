Import("env")

# Print System environment
env.AddCustomTarget("sysenv", None, 'python -c "import os; print(os.environ)"')

# Print pio environment
env.AddCustomTarget(
    name="pioenv",
    dependencies=None,
    actions=[
        "pio --version",
        "python --version"
    ],
    title="Core Env",
    description="Show PlatformIO Core and Python versions"
)

# Reset Teensy
env.AddCustomTarget(
    name="reset",
    dependencies=None,
    actions=[
        "tycmd reset"
    ],
    title="Reset",
    description="Reset Teensy"
)

# Print teensy_size details
env.AddCustomTarget(
    name="teensy_size",
    dependencies=None,
    actions=[
        "teensy_size $BUILD_DIR/${PROGNAME}.elf"
    ],
    title="Teensy Size",
    description="Print detailed teensy size information"
)

