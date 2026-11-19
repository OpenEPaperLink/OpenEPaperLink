#!/usr/bin/python
# Robust variant of 88MZ100-Serial-Flasher.py (Aaron Christophel / ATCnetz.de).
# Same wire protocol and the same second stage bootloader; adds:
#  - debounce: answers every boot prompt and only starts once the line has stayed quiet for a while,
#    so bouncing contacts on a hand-held pogo/Dupont jig no longer break the transfer
#  - automatic restart after a communication error (no need to restart the script)
#  - "read" never appends to an existing file (it refuses to start instead)
#
# Wiring (plain USB-TTL adapter, 3.3 V): adapter TXD -> tag RXD, adapter RXD -> tag TXD, GND -> GND,
# 3V3 -> tag VCC. Connect 3V3 last, when the script asks for it.
#
# Usage:  python 88MZ100-Flasher-robust.py COM4 write_flash SOL_M2_75_full_0028.bin
#         python 88MZ100-Flasher-robust.py COM4 read backup.bin

import sys
import os
import time
import serial

QUIET_S = 1.5          # line must stay quiet this long after the halt before we start
BLOCK = 80             # bytes per write packet (as in the original)


def to_byte(value, number=4):
    return value.to_bytes(number, byteorder='little')


def bytes_to_int(b):
    return int.from_bytes(bytes(b[0:4]), 'little')


def find_offset(data, sub):
    try:
        return data.index(sub)
    except ValueError:
        return -1


if len(sys.argv) != 4 or sys.argv[2] not in ('read', 'write_flash'):
    print("Usage: python 88MZ100-Flasher-robust.py COMx write_flash firmware.bin")
    print("       python 88MZ100-Flasher-robust.py COMx read backup.bin")
    sys.exit(1)

port_name, mode, fname = sys.argv[1], sys.argv[2], sys.argv[3]
if mode == 'read' and os.path.exists(fname):
    print("File %s already exists, please choose another name (it will not be overwritten)." % fname)
    sys.exit(1)

ser = serial.Serial(port_name, 115200, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE, timeout=5)
ser.dtr = 0
ser.rts = 0
print('Using port: %s' % port_name)

PASSWORD = 0xffffff01
toggle = 0x00


class CommError(Exception):
    pass


def get_cmd(cmd, length=0, address=0, plus_size=0):
    d = to_byte(cmd, 1) + to_byte(0x00, 1) + to_byte(length, 1) + to_byte(PASSWORD) + to_byte(address)
    return bytearray([len(d) + plus_size]) + d


def send_cmd(cmd):
    global toggle
    toggle ^= 0x40
    cmd[2] = toggle
    ser.write(cmd)


def receive(expected_len):
    d = ser.read(expected_len)
    if len(d) == 0:
        raise CommError("no answer")
    if d[0] != len(d) - 1:
        raise CommError("unexpected answer: " + d.hex(' '))
    return d


def write_memory(address, data):
    return get_cmd(3, int(len(data) / 4), 0x100000 + address, len(data)) + data


def read_flash(address, length):
    return get_cmd(4, int(length / 4), address)


def run_address(address):
    return get_cmd(5, 0, address)


def reset_chip():
    return get_cmd(7)


def erase_flash_custom():
    return get_cmd(8)


def progress(label, done, total):
    sys.stdout.write("\r%s: %6d / %6d bytes" % (label, done, total))
    sys.stdout.flush()


def halt_chip():
    """Wait for the ROM boot prompt, halt it, and only return once the line stayed quiet."""
    print("\nWaiting for the chip ... connect power (3V3) now.")
    halted_at = None
    last_rx = time.time()
    after_halt = b""
    while True:
        n = ser.in_waiting
        if n:
            data = ser.read(n)
            last_rx = time.time()
            for x in data:
                if x == 0x3A:                         # ':' boot prompt
                    ser.write(b':')
                    halted_at = time.time()
                    after_halt = b""
                elif halted_at is not None:
                    after_halt += bytes([x])
            if halted_at is not None:
                if not b"OK0".startswith(after_halt[:3]) or len(after_halt) > 3:
                    print("  Noise after halting (%s) - waiting for a new boot." % after_halt.hex(' '))
                    print("  If nothing happens: disconnect 3V3, wait 10 s, connect it again.")
                    halted_at = None
                    after_halt = b""
        if halted_at is not None and after_halt == b"OK0" and time.time() - last_rx > QUIET_S:
            ser.reset_input_buffer()
            print("Chip halted and stable.")
            return
        time.sleep(0.01)


def load_to_ram(path):
    data = bytearray(open(path, "rb").read())
    entry = bytes_to_int(data[4:8])
    for addr in range(0, len(data), BLOCK):
        send_cmd(write_memory(addr, data[addr:addr + BLOCK]))
        receive(4)
        progress("Second stage bootloader", min(addr + BLOCK, len(data)), len(data))
    print()
    send_cmd(run_address(entry))


def write_flash(path):
    data = bytearray(open(path, "rb").read())
    fwlen = find_offset(data, b'---OEPL_FS!')
    if fwlen == -1:
        fwlen = len(data)
    entry = bytes_to_int(data[4:8])
    header = (to_byte(0x00000000) + to_byte(0xFFFFFFFF) + to_byte(fwlen) + to_byte(0xFFEEFFFF) +
              to_byte(0xFFFFFFFF) + to_byte(0x100000) + to_byte(entry) + to_byte(0x4D52564C))
    data = bytearray(header) + data
    for addr in range(0, len(data), BLOCK):
        send_cmd(write_memory(addr, data[addr:addr + BLOCK]))
        receive(4)
        progress("Firmware", min(addr + BLOCK, len(data)), len(data))
    print()


def do_write_flash():
    global toggle
    here = os.path.dirname(os.path.realpath(__file__))
    load_to_ram(os.path.join(here, "88mz100_second_stage_bootloader.bin"))
    banner = ser.read(8)
    if banner[1:] != b"ATCboot":
        raise CommError("second stage bootloader did not start: " + banner.hex(' '))
    print("Second stage bootloader running.")
    toggle = 0x00
    print("Erasing flash ...")
    ser.timeout = 30
    send_cmd(erase_flash_custom())
    receive(4)
    ser.timeout = 5
    write_flash(fname)
    print("Done, rebooting ...")
    send_cmd(reset_chip())


def do_read():
    out = bytearray()
    for addr in range(0, 0x80000, 128):
        send_cmd(read_flash(addr, 128))
        d = receive(4 + 128)
        out += d[4:]
        progress("Reading", addr + 128, 0x80000)
    print()
    open(fname, "wb").write(out)
    print("Saved: %s (%d bytes)" % (fname, len(out)))


attempt = 0
while True:
    attempt += 1
    toggle = 0x00
    try:
        halt_chip()
        if mode == 'write_flash':
            do_write_flash()
        else:
            do_read()
        break
    except CommError as e:
        print("\nAttempt %d aborted: %s" % (attempt, e))
        print("Disconnect 3V3, wait 10 s, connect it again. The script retries automatically.")
        print("(Ctrl+C to quit)")
        time.sleep(1)
        ser.reset_input_buffer()

print("All done. Remove the jig and insert the batteries.")
