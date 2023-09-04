Import("env")

# Custom HEX from ELF
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}.hex"
    ]), "Building $BUILD_DIR/${PROGNAME}.hex")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "python", "make_ota.py", "$BUILD_DIR/${PROGNAME}.hex", "${PIOENV}-ota.bin"
    ]), "Generating OTA binary ${PIOENV}-ota.bin")       
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "python", "mergehex.py", "OEPL_bootloader.hex", "$BUILD_DIR/${PROGNAME}.hex", "-o ${PIOENV}-full-flash.hex", "--overlap=replace"
    ]), "Generating full-flash including bootloader ${PIOENV}-full-flash.hex")       
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "python", "hextobin.py", "${PIOENV}-full-flash.hex", "${PIOENV}-full-flash.bin"
    ]), "Generating full-flash including bootloader ${PIOENV}-full-flash.bin")       
)
