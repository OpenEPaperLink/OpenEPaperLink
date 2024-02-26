import struct
import argparse

def parse_mac_address(mac_str):
    # Remove any non-hex characters from the MAC address
    mac_str = ''.join(c for c in mac_str if c.isalnum())
    
    # Convert the MAC address to bytes
    return bytes.fromhex(mac_str)

def edit_binary_file(file_path, mac=None, oepltype=None, bpp=None, xres=None, yres=None, controllertype=None):
    if not any([mac, oepltype, bpp, xres, yres, controllertype]):
        print("Error: No arguments provided. At least one argument (mac, oepltype, bpp, xres, yres, controllertype) is required.")
        return
    struct_format = '8s H H B B B I I 8s'
    struct_size = struct.calcsize(struct_format)

    with open(file_path, 'rb+') as file:
        # Find the position of 'MAC_BEGINS_HERE>' in the binary file
        start_marker = b'MAC_BEGINS_HERE>'
        start_position = file.read().find(start_marker)
        
        if start_position == -1:
            print("Error: 'MAC_BEGINS_HERE>' not found in the file.")
            return
        
        # Move to the beginning of the struct
        file.seek(start_position + len(start_marker))

        # Read the existing struct
        current_values = struct.unpack(struct_format, file.read(struct_size))

        # Update values based on command line arguments
        mac = parse_mac_address(mac) if mac else current_values[0]
        oepltype = int(oepltype, 0) if oepltype else current_values[5]
        bpp = int(bpp) if bpp else current_values[3]
        xres = int(xres) if xres else current_values[1]
        yres = int(yres) if yres else current_values[2]
        controllertype = int(controllertype) if controllertype else current_values[4]

        # Pack the updated values into binary data
        updated_values = struct.pack(struct_format, mac, xres, yres, bpp, controllertype, oepltype, 0, 0, b'\x00' * 8)

        # Move back to the beginning of the struct and write the updated values
        file.seek(start_position + len(start_marker))
        file.write(updated_values)

        print("File successfully updated.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Edit binary file with specified parameters.")
    parser.add_argument("file", help="Path to the binary file")
    parser.add_argument("-mac", help="MAC address in the format 01:12:23:34:45:56:67:78 or 0112233445566778")
    parser.add_argument("-oepltype", help="Hexadecimal starting with 0x or decimal value")
    parser.add_argument("-bpp", type=int, choices=[1, 2], help="Bits per pixel, either 1 or 2")
    parser.add_argument("-xres", type=int, help="X resolution (number)")
    parser.add_argument("-yres", type=int, help="Y resolution (number)")
    parser.add_argument("-controllertype", type=int, help="Controller type (number)")

    args = parser.parse_args()
    edit_binary_file(args.file, args.mac, args.oepltype, args.bpp, args.xres, args.yres, args.controllertype)
