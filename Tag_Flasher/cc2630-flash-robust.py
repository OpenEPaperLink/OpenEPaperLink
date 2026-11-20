#!/usr/bin/python
# Robust flashing of a TI CC2630 tag (e.g. Solum TG-GR7500C) via the TI ROM serial bootloader.
# Uses the command implementation of cc2538-bsl (JelmerT, https://github.com/JelmerT/cc2538-bsl) and adds:
#  - repeated synch attempts while you switch the power on, so bouncing contacts don't matter
#  - writes sector by sector with a CRC check each; after a communication error it waits for
#    the bootloader again and redoes only the sector that failed
#  - refuses images whose CCFG would lock out the serial bootloader
#
# Order: bank erase, then every non-empty 4 KB sector, the CCFG sector (last one) at the very end.
# An erased CC2630 has no valid image and always starts the ROM bootloader, so after the
# bank erase an interrupted run can simply be continued; TI_DN is only needed the first time.
#
# Wiring: adapter TXD -> tag RXD, adapter RXD -> tag TXD, GND -> GND, 3V3 -> BAT (connect last),
#         TI_DN held on GND while the power comes on (bootloader backdoor, DIO11 on the TG-GR7500C).
#
# cc2538-bsl is looked up as an installed package first
#   (pip install git+https://github.com/JelmerT/cc2538-bsl),
# then in the directory given by the CC2538_BSL_PATH environment variable,
# then in a "cc2538-bsl" checkout next to this script.
#
# Usage:  python cc2630-flash-robust.py COM4 Tag_FW_CC2630_TG-GR7500C.bin

import sys
import os
import time
import binascii


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
SECTOR = 0x1000
CHUNK = 248            # bytes per Send Data packet (bootloader max is 252)
BAUD = 115200

if len(sys.argv) != 3:
    print("Usage: python cc2630-flash-robust.py COMx firmware.bin")
    sys.exit(1)
port, fname = sys.argv[1], sys.argv[2]

image = bytearray(open(fname, "rb").read())
if len(image) != FLASH_SIZE:
    print("%s has %d bytes, expected exactly %d (full image including CCFG)." % (fname, len(image), FLASH_SIZE))
    sys.exit(1)

# CCFG BL_CONFIG lives 0x30 bytes into the 88-byte CCFG at the end of flash:
# [0] BOOTLOADER_ENABLE (0xC5), [1] BL_PIN_NUMBER, [2] bit0 = BL_LEVEL, [3] BL_ENABLE (0xC5)
bl = image[FLASH_SIZE - 88 + 0x30: FLASH_SIZE - 88 + 0x34]
if bl[0] != 0xC5 or bl[3] != 0xC5:
    print("ABORTED: this firmware disables the serial bootloader (BL_CONFIG %s)." % bl.hex(' '))
    print("After flashing it, the tag could only be reprogrammed via JTAG.")
    sys.exit(1)
print("Firmware: %s, bootloader backdoor stays enabled (DIO%d, active %s)."
      % (os.path.basename(fname), bl[1], "high" if bl[2] & 1 else "low"))

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


def wait_for_bootloader(need_tidn):
    if need_tidn:
        print("\nWaiting for the bootloader ... hold TI_DN on GND and connect power (3V3) now.")
    else:
        print("\nWaiting for the bootloader ... connect power (3V3) now (TI_DN is no longer needed).")
    seen = 0
    last_note = time.time()
    while True:
        ok, junk = try_synch()
        seen += len(junk)
        if ok:
            print("Bootloader answers.")
            return
        if time.time() - last_note > 5:
            print("  ... no answer (%s)" % ("%d unexpected bytes" % seen if seen else "nothing received yet"))
            last_note = time.time()


def check(ok, what):
    if not ok:
        raise bsl.CmdException(what + " failed")


def write_sector(addr, data):
    check(cmd.cmdDownload(addr, len(data)), "Download 0x%05X" % addr)
    for off in range(0, len(data), CHUNK):
        check(cmd.cmdSendData(data[off:off + CHUNK]), "Send data 0x%05X" % (addr + off))
    crc = cmd.cmdCRC32CC26xx(addr, len(data))
    want = binascii.crc32(bytes(data)) & 0xffffffff
    if crc != want:
        raise bsl.CmdException("CRC mismatch at 0x%05X (%s instead of %08X)"
                               % (addr, crc if crc is None else "%08X" % crc, want))


# sectors that contain anything but 0xFF; the CCFG sector always, and always last
empty = b"\xff" * SECTOR
last = FLASH_SIZE - SECTOR
todo = [a for a in range(0, last, SECTOR) if image[a:a + SECTOR] != empty] + [last]

erased = False
dirty = None           # sector that was being written when the connection broke
idx = 0
while True:
    try:
        wait_for_bootloader(need_tidn=not erased)
        chip_id = cmd.cmdGetChipId()
        print("Chip ID: 0x%04X" % chip_id)
        if not erased:
            print("Erasing the whole flash ...")
            check(cmd.cmdBankErase(), "Bank erase")
            erased = True
        if dirty is not None:
            check(cmd.cmdSectorEraseCC26xx(dirty), "Sector erase 0x%05X" % dirty)
            dirty = None
        while idx < len(todo):
            addr = todo[idx]
            dirty = addr
            write_sector(addr, image[addr:addr + SECTOR])
            dirty = None
            idx += 1
            sys.stdout.write("\rWriting: sector %2d / %2d  (0x%05X)" % (idx, len(todo), addr))
            sys.stdout.flush()
        print()
        crc = cmd.cmdCRC32CC26xx(0, FLASH_SIZE)
        want = binascii.crc32(bytes(image)) & 0xffffffff
        if crc != want:
            raise bsl.CmdException("Full-flash CRC mismatch (%s instead of %08X)" % (crc, want))
        print("Full-flash CRC OK: %08X" % want)
        break
    except (bsl.CmdException, AssertionError, IndexError, TypeError) as e:
        print("\nInterrupted: %s" % e)
        print("Disconnect 3V3, wait 10 s, connect it again.")
        print("The script continues with the same sector. (Ctrl+C to quit)")
        time.sleep(1)

print("Flashing done. Release TI_DN and disconnect power; the new firmware starts")
print("on the next power-up (ideally with batteries).")
