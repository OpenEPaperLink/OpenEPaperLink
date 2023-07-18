# Copyright 2014-present PlatformIO <contact@platformio.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
from platform import system
from os import makedirs
from os.path import isdir, join, basename

from SCons.Script import (ARGUMENTS, COMMAND_LINE_TARGETS, AlwaysBuild,
                          Builder, Default, DefaultEnvironment)

from platformio.public import list_serial_ports


def BeforeUpload(target, source, env):  # pylint: disable=W0613,W0621
    env.AutodetectUploadPort()

    upload_options = {}
    if "BOARD" in env:
        upload_options = env.BoardConfig().get("upload", {})

    if not bool(upload_options.get("disable_flushing", False)):
        env.FlushSerialBuffer("$UPLOAD_PORT")

    before_ports = list_serial_ports()

    if bool(upload_options.get("use_1200bps_touch", False)):
        env.TouchSerialPort("$UPLOAD_PORT", 1200)

    if bool(upload_options.get("wait_for_upload_port", False)):
        env.Replace(UPLOAD_PORT=env.WaitForNewSerialPort(before_ports))

    # use only port name for BOSSA
    if ("/" in env.subst("$UPLOAD_PORT") and
            env.subst("$UPLOAD_PROTOCOL") == "sam-ba"):
        env.Replace(UPLOAD_PORT=basename(env.subst("$UPLOAD_PORT")))


env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()
variant = board.get("build.variant", "")

env.Replace(
    AR="arm-none-eabi-ar",
    AS="arm-none-eabi-as",
    CC="arm-none-eabi-gcc",
    CXX="arm-none-eabi-g++",
    GDB="arm-none-eabi-gdb",
    OBJCOPY="arm-none-eabi-objcopy",
    RANLIB="arm-none-eabi-ranlib",
    SIZETOOL="arm-none-eabi-size",

    ARFLAGS=["rc"],

    SIZEPROGREGEXP=r"^(?:\.text|\.data|\.rodata|\.text.align|\.ARM.exidx)\s+(\d+).*",
    SIZEDATAREGEXP=r"^(?:\.data|\.bss|\.noinit)\s+(\d+).*",
    SIZECHECKCMD="$SIZETOOL -A -d $SOURCES",
    SIZEPRINTCMD='$SIZETOOL -B -d $SOURCES',

    ERASEFLAGS=["--eraseall", "-f", "nrf52"],
    ERASECMD="nrfjprog $ERASEFLAGS",

    PROGSUFFIX=".elf"
)

# Allow user to override via pre:script
if env.get("PROGNAME", "program") == "program":
    env.Replace(PROGNAME="firmware")

env.Append(
    BUILDERS=dict(
        ElfToBin=Builder(
            action=env.VerboseAction(" ".join([
                "$OBJCOPY",
                "-O",
                "binary",
                "$SOURCES",
                "$TARGET"
            ]), "Building $TARGET"),
            suffix=".bin"
        ),
        ElfToHex=Builder(
            action=env.VerboseAction(" ".join([
                "$OBJCOPY",
                "-O",
                "ihex",
                "-R",
                ".eeprom",
                "$SOURCES",
                "$TARGET"
            ]), "Building $TARGET"),
            suffix=".hex"
        ),
        MergeHex=Builder(
            action=env.VerboseAction(" ".join([
                '"%s"' % join(platform.get_package_dir("tool-sreccat") or "",
                     "srec_cat"),
                "$SOFTDEVICEHEX",
                "-intel",
                "$SOURCES",
                "-intel",
                "-o",
                "$TARGET",
                "-intel",
                "--line-length=44"
            ]), "Building $TARGET"),
            suffix=".hex"
        )
    )
)

upload_protocol = env.subst("$UPLOAD_PROTOCOL")

if "nrfutil" == upload_protocol or (
    board.get("build.bsp.name", "nrf5") == "adafruit"
    and "arduino" in env.get("PIOFRAMEWORK", [])
):
    env.Append(
        BUILDERS=dict(
            PackageDfu=Builder(
                action=env.VerboseAction(" ".join([
                    '"$PYTHONEXE"',
                    '"%s"' % join(platform.get_package_dir(
                        "tool-adafruit-nrfutil") or "", "adafruit-nrfutil.py"),
                    "dfu",
                    "genpkg",
                    "--dev-type",
                    "0x0052",
                    "--sd-req",
                    board.get("build.softdevice.sd_fwid"),
                    "--application",
                    "$SOURCES",
                    "$TARGET"
                ]), "Building $TARGET"),
                suffix=".zip"
            ),
            SignBin=Builder(
                action=env.VerboseAction(
                    " ".join(
                        [
                            '"$PYTHONEXE"',
                            '"%s"' % join(
                                platform.get_package_dir(
                                    "framework-arduinoadafruitnrf52"
                                )
                                or "",
                                "tools",
                                "pynrfbintool",
                                "pynrfbintool.py",
                            ),
                            "--signature",
                            "$TARGET",
                            "$SOURCES",
                        ]
                    ),
                    "Signing $SOURCES",
                ),
                suffix="_signature.bin",
            ),
        )
    )


if not env.get("PIOFRAMEWORK"):
    env.SConscript("frameworks/_bare.py")

#
# Target: Build executable and linkable firmware
#

if "zephyr" in env.get("PIOFRAMEWORK", []):
    env.SConscript(
        join(platform.get_package_dir(
            "framework-zephyr"), "scripts", "platformio", "platformio-build-pre.py"),
        exports={"env": env}
    )

target_elf = None
if "nobuild" in COMMAND_LINE_TARGETS:
    target_elf = join("$BUILD_DIR", "${PROGNAME}.elf")
    target_firm = join("$BUILD_DIR", "${PROGNAME}.hex")
else:
    target_elf = env.BuildProgram()

    if "SOFTDEVICEHEX" in env:
        target_firm = env.MergeHex(
            join("$BUILD_DIR", "${PROGNAME}"),
            env.ElfToHex(join("$BUILD_DIR", "userfirmware"), target_elf))
    elif "nrfutil" == upload_protocol:
        target_firm = env.PackageDfu(
            join("$BUILD_DIR", "${PROGNAME}"),
            env.ElfToHex(join("$BUILD_DIR", "${PROGNAME}"), target_elf))
    elif "nrfjprog" == upload_protocol:
        target_firm = env.ElfToHex(
            join("$BUILD_DIR", "${PROGNAME}"), target_elf)
    elif "sam-ba" == upload_protocol:
        target_firm = env.ElfToBin(join("$BUILD_DIR", "${PROGNAME}"), target_elf)
    else:
        if "DFUBOOTHEX" in env:
            target_firm = env.SignBin(
                join("$BUILD_DIR", "${PROGNAME}"),
                env.ElfToBin(join("$BUILD_DIR", "${PROGNAME}"), target_elf))
        else:
            target_firm = env.ElfToHex(
                join("$BUILD_DIR", "${PROGNAME}"), target_elf)
        env.Depends(target_firm, "checkprogsize")

AlwaysBuild(env.Alias("nobuild", target_firm))
target_buildprog = env.Alias("buildprog", target_firm, target_firm)

if "DFUBOOTHEX" in env:
    env.Append(
        # Check the linker script for the correct location
        BOOT_SETTING_ADDR=board.get("build.bootloader.settings_addr", "0x7F000")
    )

    env.AddPlatformTarget(
        "dfu",
        env.PackageDfu(
            join("$BUILD_DIR", "${PROGNAME}"),
            env.ElfToHex(join("$BUILD_DIR", "${PROGNAME}"), target_elf),
        ),
        target_firm,
        "Generate DFU Image",
    )

    env.AddPlatformTarget(
        "bootloader",
        None,
        [
            env.VerboseAction(
                "nrfjprog --program $DFUBOOTHEX -f nrf52 --chiperase",
                "Uploading $DFUBOOTHEX",
            ),
            env.VerboseAction(
                "nrfjprog --erasepage $BOOT_SETTING_ADDR -f nrf52",
                "Erasing bootloader config",
            ),
            env.VerboseAction(
                "nrfjprog --memwr $BOOT_SETTING_ADDR --val 0x00000001 -f nrf52",
                "Disable CRC check",
            ),
            env.VerboseAction("nrfjprog --reset -f nrf52", "Reset nRF52"),
        ],
        "Burn Bootloader",
    )

if "bootloader" in COMMAND_LINE_TARGETS and "DFUBOOTHEX" not in env:
    sys.stderr.write("Error. The board is missing the bootloader binary.\n")
    env.Exit(1)

#
# Target: Print binary size
#

target_size = env.AddPlatformTarget(
    "size",
    target_elf,
    env.VerboseAction("$SIZEPRINTCMD", "Calculating size $SOURCE"),
    "Program Size",
    "Calculate program size",
)

#
# Target: Upload by default .bin file
#

debug_tools = env.BoardConfig().get("debug.tools", {})
upload_actions = []

if upload_protocol == "mbed":
    upload_actions = [
        env.VerboseAction(env.AutodetectUploadPort, "Looking for upload disk..."),
        env.VerboseAction(env.UploadToDisk, "Uploading $SOURCE")
    ]

elif upload_protocol.startswith("blackmagic"):
    env.Replace(
        UPLOADER="$GDB",
        UPLOADERFLAGS=[
            "-nx",
            "--batch",
            "-ex", "target extended-remote $UPLOAD_PORT",
            "-ex", "monitor %s_scan" %
            ("jtag" if upload_protocol == "blackmagic-jtag" else "swdp"),
            "-ex", "attach 1",
            "-ex", "load",
            "-ex", "compare-sections",
            "-ex", "kill"
        ],
        UPLOADCMD="$UPLOADER $UPLOADERFLAGS $BUILD_DIR/${PROGNAME}.elf"
    )
    upload_actions = [
        env.VerboseAction(env.AutodetectUploadPort, "Looking for BlackMagic port..."),
        env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")
    ]

elif upload_protocol == "nrfjprog":
    env.Replace(
        UPLOADER="nrfjprog",
        UPLOADERFLAGS=[
            "--sectorerase" if "DFUBOOTHEX" in env else "--chiperase",
            "--reset"
        ],
        UPLOADCMD="$UPLOADER $UPLOADERFLAGS --program $SOURCE"
    )
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

elif upload_protocol == "nrfutil":
    env.Replace(
        UPLOADER=join(platform.get_package_dir(
            "tool-adafruit-nrfutil") or "", "adafruit-nrfutil.py"),
        UPLOADERFLAGS=[
            "dfu",
            "serial",
            "-p",
            "$UPLOAD_PORT",
            "-b",
            "$UPLOAD_SPEED",
            "--singlebank",
        ],
        UPLOADCMD='"$PYTHONEXE" "$UPLOADER" $UPLOADERFLAGS -pkg $SOURCE'
    )
    upload_actions = [
        env.VerboseAction(BeforeUpload, "Looking for upload port..."),
        env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")
    ]

elif upload_protocol == "sam-ba":
    env.Replace(
        UPLOADER="bossac",
        UPLOADERFLAGS=[
            "--port", '"$UPLOAD_PORT"', "--write", "--erase", "-U", "--reset"
        ],
        UPLOADCMD="$UPLOADER $UPLOADERFLAGS $SOURCES"
    )
    if int(ARGUMENTS.get("PIOVERBOSE", 0)):
        env.Prepend(UPLOADERFLAGS=["--info", "--debug"])

    upload_actions = [
        env.VerboseAction(BeforeUpload, "Looking for upload port..."),
        env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")
    ]

elif upload_protocol.startswith("jlink"):

    def _jlink_cmd_script(env, source):
        build_dir = env.subst("$BUILD_DIR")
        if not isdir(build_dir):
            makedirs(build_dir)
        script_path = join(build_dir, "upload.jlink")
        commands = ["h"]
        if "DFUBOOTHEX" in env:
            commands.append("loadbin %s,%s" % (str(source).replace("_signature", ""),
                env.BoardConfig().get("upload.offset_address", "0x26000")))
            commands.append("loadbin %s,%s" % (source, env.get("BOOT_SETTING_ADDR")))
        else:
            commands.append("loadbin %s,%s" % (source, env.BoardConfig().get(
                "upload.offset_address", "0x0")))

        commands.append("r")
        commands.append("q")

        with open(script_path, "w") as fp:
            fp.write("\n".join(commands))
        return script_path

    env.Replace(
        __jlink_cmd_script=_jlink_cmd_script,
        UPLOADER="JLink.exe" if system() == "Windows" else "JLinkExe",
        UPLOADERFLAGS=[
            "-device", env.BoardConfig().get("debug", {}).get("jlink_device"),
            "-speed", env.GetProjectOption("debug_speed", "4000"),
            "-if", ("jtag" if upload_protocol == "jlink-jtag" else "swd"),
            "-autoconnect", "1",
            "-NoGui", "1"
        ],
        UPLOADCMD='$UPLOADER $UPLOADERFLAGS -CommanderScript "${__jlink_cmd_script(__env__, SOURCE)}"'
    )
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

elif upload_protocol in debug_tools:
    openocd_args = [
        "-d%d" % (2 if int(ARGUMENTS.get("PIOVERBOSE", 0)) else 1)
    ]
    openocd_args.extend(
        debug_tools.get(upload_protocol).get("server").get("arguments", []))
    if env.GetProjectOption("debug_speed"):
        openocd_args.extend(
            ["-c", "adapter speed %s" % env.GetProjectOption("debug_speed")]
        )
    openocd_args.extend([
        "-c", "program {$SOURCE} %s verify reset; shutdown;" %
        board.get("upload.offset_address", "")
    ])
    openocd_args = [
        f.replace("$PACKAGE_DIR",
                  platform.get_package_dir("tool-openocd") or "")
        for f in openocd_args
    ]
    env.Replace(
        UPLOADER="openocd",
        UPLOADERFLAGS=openocd_args,
        UPLOADCMD="$UPLOADER $UPLOADERFLAGS")
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

# custom upload tool
elif upload_protocol == "custom":
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

else:
    sys.stderr.write("Warning! Unknown upload protocol %s\n" % upload_protocol)

env.AddPlatformTarget("upload", target_firm, upload_actions, "Upload")


#
# Target: Erase Flash
#

env.AddPlatformTarget(
    "erase", None, env.VerboseAction("$ERASECMD", "Erasing..."), "Erase Flash")

#
# Information about obsolete method of specifying linker scripts
#

if any("-Wl,-T" in f for f in env.get("LINKFLAGS", [])):
    print("Warning! '-Wl,-T' option for specifying linker scripts is deprecated. "
          "Please use 'board_build.ldscript' option in your 'platformio.ini' file.")

#
# Default targets
#

Default([target_buildprog, target_size])
