#!/usr/bin/python
# Robust read-out of a TI CC2630 tag (e.g. Solum TG-GR7500C) via the TI ROM serial bootloader.
# Uses the command implementation of cc2538-bsl (JelmerT, https://github.com/JelmerT/cc2538-bsl) and adds:
#  - repeated synch attempts while you switch the power on, so bouncing contacts don't matter
#  - resume after a communication error instead of aborting
#  - never overwrites an existing output file
#
# Wiring: adapter TXD -> tag RXD, adapter RXD -> tag TXD, GND -> GND, 3V3 -> BAT (connect last),
#         TI_DN held on GND while the power comes on (bootloader backdoor, DIO11 on the TG-GR7500C).
#
# cc2538-bsl is looked up as an installed package first
#   (pip install git+https://github.com/JelmerT/cc2538-bsl),
# then in the directory given by the CC2538_BSL_PATH environment variable,
# then in a "cc2538-bsl" checkout next to this script.
#
# Usage:  python cc2630-read-robust.py COM4 tg-gr7500c-original.bin

import sys
import os
import time


def load_bsl():
    try:
        from cc2538_bsl import cc2538_bsl
        return cc2538_bsl
    except ImportError:
        pass
    here = os.path.dirname(os.path.realpath(__file__))
    for path in (os.environ.get("CC2538_BSL_PATH"), os.path.join(here, "cc2538-bsl")):
        if path and os.path.isdir(os.path.join(path, "cc2538_bsl")):
            sys.path.insert(0, path)
            from cc2538_bsl import cc2538_bsl
            return cc2538_bsl
    print("cc2538-bsl not found. Install it with")
    print("  pip install git+https://github.com/JelmerT/cc2538-bsl")
    print("or point CC2538_BSL_PATH to a checkout of https://github.com/JelmerT/cc2538-bsl")
    sys.exit(1)


bsl = load_bsl()
bsl.QUIET = 0          # silence cc2538-bsl debug output; we print our own
FLASH_SIZE = 0x20000   # CC2630F128
BAUD = 115200

if len(sys.argv) != 3:
    print("Usage: python cc2630-read-robust.py COMx output.bin")
    sys.exit(1)
port, outname = sys.argv[1], sys.argv[2]
if os.path.exists(outname):
    print("File %s already exists, please choose another name (it will not be overwritten)." % outname)
    sys.exit(1)

cmd = bsl.CommandInterface()
cmd.open(port, BAUD)
cmd.sp.dtr = False
cmd.sp.rts = False


def try_synch():
    """One synch attempt; True when the ROM bootloader answered with ACK."""
    cmd.sp.reset_input_buffer()
    cmd.sp.timeout = 0.05
    cmd.sp.write(b"\x55\x55")
    stop = time.time() + 0.3
    got = bytearray()
    try:
        while time.time() < stop:
            got += cmd.sp.read(cmd.sp.in_waiting or 1)
            if b"\x00\xcc" in got:
                return True, got
        return False, got
    finally:
        cmd.sp.timeout = 1.0


def wait_for_bootloader():
    print("\nWaiting for the bootloader ... hold TI_DN on GND and connect power (3V3) now.")
    seen = 0
    recent = bytearray()
    last_note = time.time()
    while True:
        ok, junk = try_synch()
        seen += len(junk)
        recent += junk
        if ok:
            print("Bootloader answers.")
            return
        if time.time() - last_note > 5:
            note = "%d unexpected bytes so far" % seen if seen else "nothing received yet"
            print("  ... no answer (%s)" % note)
            if recent:
                chunk = bytes(recent[:48])
                print("      hex:   " + chunk.hex(' '))
                print("      text:  " + ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk))
                recent = bytearray()
            last_note = time.time()


def identify():
    chip_id = cmd.cmdGetChipId()
    print("Chip ID: 0x%04X" % chip_id)
    dev = bsl.CC26xx(cmd)
    print("Flash: %d KB" % (dev.size // 1024))
    return dev


data = bytearray()
dev = None
while True:
    try:
        wait_for_bootloader()
        if dev is None:
            dev = identify()
        size = dev.size or FLASH_SIZE
        while len(data) < size:
            word = cmd.cmdMemReadCC26xx(len(data))
            if word is None or len(word) != 4:
                raise bsl.CmdException("no valid answer at 0x%05X" % len(data))
            data += word
            if len(data) % 1024 == 0:
                sys.stdout.write("\rReading: %6d / %6d bytes" % (len(data), size))
                sys.stdout.flush()
        print()
        break
    except (bsl.CmdException, AssertionError, IndexError, TypeError) as e:
        print("\nInterrupted at 0x%05X: %s" % (len(data), e))
        print("Disconnect 3V3, wait 20 s, hold TI_DN on GND, connect it again.")
        print("The script continues where it stopped. (Ctrl+C to quit)")
        time.sleep(1)

open(outname, "wb").write(data)
print("Saved: %s (%d bytes)" % (outname, len(data)))
ccfg = data[-88:]
print("CCFG BL_CONFIG: %s  (C5 xx xx C5 = bootloader enabled, BL pin DIO%d)" % (ccfg[48:52].hex(' '), ccfg[49]))
