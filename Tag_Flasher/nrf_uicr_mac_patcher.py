#!/usr/bin/env python3
# -*- coding: utf-8 -*-
##
# It's not 2002 anymore and we have wide screens.
# pylint: disable=C0301
"""
This is a very simple script to read / patch the MAC address in uicr partition dump from a nrf based tag.
The script was built for / tested against solum newton M3/2.9 inch tags but should be easy to adapt to other tags.

What the uicr partition contains has been reverse engineered and details are here:
https://github.com/jjwbruijn/OpenEPaperLink/blob/master/ARM_Tag_FW/Newton_M3_nRF52811/hal/Newton_M3_nRF52811/tagtype_db.cpp


If you have a tag with mac: 00:00:7E:7E:79:1F:B1:93, then only the bytes (1F 79 7E 7E) will be present in the uicr partition dump.
This is because the first two bytes (00:00) are padding / not currently used and the last two octets are a checksum.
The checksum is calculated by the tag firmware and is not relevant to us.

If you're curious, the checksum calc is here:
https://github.com/jjwbruijn/OpenEPaperLink/blob/d2c2963b3b5b24346bff0d7966eeac28cf888377/ARM_Tag_FW/Newton_M3_nRF52811/hal/Newton_M3_nRF52811/HAL_Newton_M3.cpp#L140

Starting at offset 0x80, you should see this in your dump file:

    0000:0080 | 1F 79 7E 7E

So, we're really only interested in patching 4 bytes from 0x80 through 0x84.
Note that the order of the bytes is reversed in the uicr partition dump.

This script is meant to be used with the `OEPL-Flasher.py` tool to pull a known good uicr partition
and to flash back a modified uicr partition dump to the tag.

## Usage

# Read a known good tag
❯ python3 OEPL-Flasher.py -e -n --passthrough -p /dev/ttyACM0 -i read working_infopage.bin

# Confirm that the dump has the MAC you expect
❯ python3 nrf_uicr_mac_patcher.py working_infopage.bin 
MAC Address: 00:00:7E:7E:79:1F:00:00

# Create a copy of the known good dump and patch it with the new MAC
❯ python3 nrf_uicr_mac_patcher.py working_infopage.bin 7e:7e:4c:9b recovery1.bin
Patching working_infopage.bin with new MAC address 7e:7e:4c:9b
File patched and saved to recovery1.bin

# Check your work ;)
❯ python3 nrf_uicr_mac_patcher.py recovery1.bin 
MAC Address: 00:00:7E:7E:4C:9B:00:00

# Write the modified uicr partition dump to the tag and then flash the tag with the new firmware
❯ py OEPL-Flasher.py -e -n --passthrough -p /dev/ttyACM0 -i write recovery6.bin
❯ py OEPL-Flasher.py -e -n --passthrough -p /dev/ttyACM0 --flash write ../binaries/Tag/SOL_M3_Uni_full_0027.bin


"""

import sys


def read_mac_from_file(filepath: str) -> None:
    """Reads the MAC address from a file

    Args:
        filepath (str): Path to bin file
    """
    with open(filepath, "rb") as file:

        # Seek to offset 0x80, read 4 bytes
        file.seek(0x80)
        mac_bytes = file.read(4)
        # Reverse the bytes
        mac_bytes = mac_bytes[::-1]
    # two left most bytes are 0x00, and the two right most bytes are a checksum that's
    # calculated by the tag firmware.
    mac = [0, 0] + list(mac_bytes) + [0, 0]
    mac_str = ":".join(f"{byte:02X}" for byte in mac)
    print("MAC Address:", mac_str)


def patch_file(input_path: str, new_mac_str: str, output_path: str) -> None:
    """Patch file

    Args:
        input_path (str): Path to input file; this should be a known good uicr partition dump
        new_mac_str (str): colon delineated string containing the 4 bytes of new MAC address
        output_path (str): Path to the output file
    """
    print(f"Patching {input_path} with new MAC address {new_mac_str}")
    new_mac = [int(x, 16) for x in new_mac_str.split(":")[0:4]]

    with open(input_path, "rb") as file:
        data = bytearray(file.read())

    # Update MAC address bytes, write them in reverse order
    new_mac = new_mac[::-1]
    data[0x80:0x84] = new_mac[0:4]

    with open(output_path, "wb") as file:
        file.write(data)
    print(f"File patched and saved to {output_path}")


if __name__ == "__main__":
    if len(sys.argv) == 2:
        read_mac_from_file(sys.argv[1])
    elif len(sys.argv) == 4:
        patch_file(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print(f"Usage: {sys.argv[0]} <file_path> [new_mac] [output_file_path]")
