import argparse
import os
import struct
import glob
import hashlib

# Define the TagSettings format
tag_settings_format = '<B B B B B B B B B H H B'

# Define the EepromImageHeader format
eeprom_header_format = '<Q I I B I B'

# Define the TagSettings struct


class TagSettings:
    def __init__(self, settingsVer, enableFastBoot, enableRFWake, enableTagRoaming,
                 enableScanForAPAfterTimeout, enableLowBatSymbol, enableNoRFSymbol,
                 fastBootCapabilities, customMode, batLowVoltage, minimumCheckInTime, fixedChannel):
        self.settingsVer = settingsVer
        self.enableFastBoot = enableFastBoot
        self.enableRFWake = enableRFWake
        self.enableTagRoaming = enableTagRoaming
        self.enableScanForAPAfterTimeout = enableScanForAPAfterTimeout
        self.enableLowBatSymbol = enableLowBatSymbol
        self.enableNoRFSymbol = enableNoRFSymbol
        self.fastBootCapabilities = fastBootCapabilities
        self.customMode = customMode
        self.batLowVoltage = batLowVoltage
        self.minimumCheckInTime = minimumCheckInTime
        self.fixedChannel = fixedChannel

    def pack(self):
        return struct.pack(tag_settings_format, self.settingsVer, self.enableFastBoot, self.enableRFWake,
                           self.enableTagRoaming, self.enableScanForAPAfterTimeout, self.enableLowBatSymbol,
                           self.enableNoRFSymbol, self.fastBootCapabilities, self.customMode, self.batLowVoltage,
                           self.minimumCheckInTime, self.fixedChannel)

# Function to calculate MD5 checksum


def calculate_md5_checksum(file_path):
    md5 = hashlib.md5()
    with open(file_path, 'rb') as file:
        for chunk in iter(lambda: file.read(4096), b""):
            md5.update(chunk)
    return int(md5.hexdigest()[:16], 16)  # Get the first 8 bytes as uint64_t


# Define the EepromImageHeader struct
class EepromImageHeader:
    def __init__(self, ver, validMarker, size, dataType, id, argument):
        self.ver = ver
        self.validMarker = validMarker
        self.size = size
        self.dataType = dataType
        self.id = id
        self.argument = argument

    def pack(self):
        return struct.pack(eeprom_header_format, self.ver, self.validMarker, self.size, self.dataType, self.id, self.argument)

# Function to write TagSettings at offset 0x00


def write_tagsettings(output_file, mode):
    tag_settings = TagSettings(1, 0, 0, 0, 1, 1, 1, 4, mode, 2450, 1, 0)
    output_file.write(tag_settings.pack())

# Function to process and write input files


def write_input_files(input_files, output_file):
    for file_id, file_path in enumerate(input_files, start=1):
        with open(file_path, 'rb') as input_file:
            offset = 0x1000 + (0x3000 * file_id)

            md5_checksum = calculate_md5_checksum(file_path)

            # determine if we have a color image or not
            if (os.path.getsize(file_path) > 5000):
                file_type = 0x21
            else:
                file_type = 0x20

            eeprom_header = EepromImageHeader(
                md5_checksum, 0x474d4721, 0x3000-14, file_type, file_id, 0x79)
            output_file.seek(offset)
            output_file.write(eeprom_header.pack())

            offset += 22  # The size of the EepromImageHeader struct

            input_data = input_file.read()
            output_file.seek(offset)
            output_file.write(input_data)


def addsplash(output_file):
    with open('splashscreen.raw', 'rb') as input_file:
        offset = 0x1000
        md5_checksum = calculate_md5_checksum('splashscreen.raw')
        # determine if we have a color image or not
        if (os.path.getsize('splashscreen.raw') > 5000):
            file_type = 0x21
        else:
            file_type = 0x20
        eeprom_header = EepromImageHeader(
            md5_checksum, 0x474d4721, 0x3000-14, file_type, 0, 0x09)
        output_file.seek(offset)
        output_file.write(eeprom_header.pack())
        offset += 22  # The size of the EepromImageHeader struct
        input_data = input_file.read()
        output_file.seek(offset)
        output_file.write(input_data)


# Main function
def main():
    parser = argparse.ArgumentParser(
        description="Creates a slideshow for 2.9\" OpenEPaperLink tags")
    parser.add_argument("output_file", help="Output binary file for eeprom")
    parser.add_argument("input_files", nargs='+',
                        help="Input image files to be included in the output")
    parser.add_argument("--splash", "-s", action="store_true",
                        help="Adds a splash-screen to the slideshow that will be shown first (must be named 'splashscreen.raw')")
    parser.add_argument("--waitrfwake", "-w", action="store_true",
                        help="Forces the tag to wait for RF wake signal to start the slideshow. Tag will display the first slideshow image while waiting for this signal")
    args = parser.parse_args()

    input_files = []
    for input_file in args.input_files:
        input_files.extend(glob.glob(input_file))

    with open(args.output_file, 'wb') as output_file:
        if (args.waitrfwake):
            write_tagsettings(output_file, 0x20)  # wait for wake first
        else:
            write_tagsettings(output_file, 0x08)  # slow slideshow

        write_input_files(input_files, output_file)
        if (args.splash):
            addsplash(output_file)


if __name__ == '__main__':
    main()
