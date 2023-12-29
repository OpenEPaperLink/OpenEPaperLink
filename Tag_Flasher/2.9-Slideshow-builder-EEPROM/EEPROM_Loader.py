import argparse
import serial.tools.list_ports
import serial
import time
from intelhex import IntelHex
import os.path

CMD_GET_VERSION = 1
CMD_RESET_ESP = 2
CMD_RESET = 11
CMD_SET_POWER = 13

CMD_ERASE_FLASH = 26
CMD_ERASE_INFOPAGE = 27
CMD_SAVE_MAC_FROM_FW = 40
CMD_PASS_THROUGH = 50

CMD_SELECT_ZBS243 = 60
CMD_SELECT_NRF82511 = 61

CMD_SELECT_PORT = 70

CMD_READ_INFOPAGE = 80
CMD_READ_FLASH = 81
CMD_WRITE_INFOPAGE = 82
CMD_WRITE_FLASH = 83
CMD_AUTOFLASH = 87
CMD_COMPLETE = 88


def read_binary_file(file_path):
    with open(file_path, 'rb') as file:
        binary_data = file.read()
    return binary_data


def read_hex_file(file_path):
    ih = IntelHex(file_path)
    binary_data = ih.tobinarray()
    return bytes(binary_data)


def to_byte(input, number=4):
    return input.to_bytes(number, byteorder='big')


def send_cmd(cmd, data):
    return_data = to_byte(cmd, 1) + to_byte(len(data), 4) + data
    crc_val = 0xAB34
    for x in return_data:
        crc_val += x
    return_data = b"AT" + return_data + to_byte(crc_val & 0xffff, 2)
    ser.write(return_data)


def wait_for_command():
    start_time = time.time()
    ser.timeout = 5  # Set the timeout to 1 second
    while True:
        if ser.in_waiting > 0:
            command = ser.read(2)  # Read the "AT" prefix
            if command == b"AT":
                # Read the command byte
                cmd = int.from_bytes(ser.read(1), byteorder='big')
                data_length = int.from_bytes(
                    ser.read(4), byteorder='big')  # Read the data length
                data = ser.read(data_length)  # Read the data
                # Read the CRC value
                crc = int.from_bytes(ser.read(2), byteorder='big')
                # Verify CRC
                crc_val = 0xAB34
                for x in to_byte(cmd, 1) + to_byte(data_length, 4) + data:
                    crc_val += x
                if crc_val & 0xffff == crc:
                    return cmd, data
                else:
                    print("Invalid CRC. Discarding command. Got " +
                          str(crc_val) + " but was expecting "+str(crc))
                    print("data was:"+str(data))
        if time.time() - start_time > ser.timeout:
            print("timeout waiting for reply")
            return None, None


def list_available_com_ports():
    ports = serial.tools.list_ports.comports()
    available_ports = [port.device for port in ports]
    print("Specify a serial port to use with -p <PORT>")
    print("available COM ports:")
    for port in available_ports:
        print(port)


def validate_arguments(args):
    if not (args.nrf82511 or args.zbs243):
        print("Either -nrf82511 or -zbs243 option is required.")
        return False
    if not (args.internalap or args.external or args.altradio):
        print("One of -internalap, -external, or -altradio options is required.")
        return False
    if args.command in ["read", "write"] and not (args.flash or args.infopage):
        print("One of --flash or --infopage arguments is required for read and write commands.")
        return False
    if ((args.command == "debug" or args.pt) and not (args.external)):
        print("Debugging/Passthrough is only available on the external port!")
        return False
    if args.command == "write" and not os.path.isfile(args.filename):
        print("Couldn't find the specified file!")
        return False
    if args.command in ["read", "write"] and not args.filename:
        print("Please specify a file to save read data")
        return False
    if args.command == "read" and len(args.filename) < 2:
        print("Please specify a file to save read data")
        return False
    return True


def read_from_serial(port, filename, flash):
    if flash:
        print(
            f"Reading flash data and saving to file: {filename}.")
    else:
        print(
            f"Reading info page and saving to file: {filename}.")

    print("Now downloading...\n")
    file = open(filename, "wb")
    bytecount = 0
    while True:

        print(f'\r{bytecount} bytes', end='', flush=True)
        if (flash):
            send_cmd(CMD_READ_FLASH, bytearray([]))
        else:
            send_cmd(CMD_READ_INFOPAGE, bytearray([]))

        cmd, answer = wait_for_command()
        if ((cmd == CMD_READ_FLASH) or (cmd == CMD_READ_INFOPAGE)):
            file.write(bytearray(answer))
            bytecount += len(answer)
        elif (cmd == CMD_COMPLETE):
            file.close()
            print("\r All done!         \nFile saved\n", end='', flush=True)
            break
        else:
            print("Failed reading block, timeout?")


def write_to_serial(port, filename, flash):

    if flash:
        print(f"\nErasing flash... ")
        send_cmd(CMD_ERASE_FLASH, bytearray([]))
    else:
        print(f"\nErasing infopage... ")
        send_cmd(CMD_ERASE_INFOPAGE, bytearray([]))

    cmd, answer = wait_for_command()
    if ((cmd == CMD_ERASE_FLASH) or (cmd == CMD_ERASE_INFOPAGE)):
        print("DONE!\n")
    else:
        print("\nFailed to erase the flash?")
        exit(0)

    # TODO: Implement logic to write data from the file to the serial port with flash option
    if flash:
        print(f"Writing flash data from file: {filename}\n")
    else:
        print(f"Writing info page data from file: {filename}\n")

    chunk_size = 256

    if filename.endswith('.bin'):
        file_data = read_binary_file(filename)
    elif filename.endswith('.hex'):
        file_data = read_hex_file(filename)

    for i in range(0, len(file_data), chunk_size):
        chunk = file_data[i:i + chunk_size]
        if (flash):
            send_cmd(CMD_WRITE_FLASH, bytearray(chunk))
        else:
            send_cmd(CMD_WRITE_INFOPAGE, bytearray(chunk))
        cmd, answer = wait_for_command()
        if ((cmd == CMD_WRITE_FLASH) or (cmd == CMD_WRITE_INFOPAGE)):
            print(f'\rSent {i} bytes', end='', flush=True)
        elif (cmd == CMD_COMPLETE):
            print(
                '\Tried to write more bytes than we have room for!   \n', end='', flush=True)
            return
        else:
            print("Some other error, dunno\n")
            return
    print('\rAll done writing! ', end='', flush=True)


def short_passthough(period_time):
    start_time = time.time()
    while time.time() - start_time < period_time:
        data = ser.read()
        if data:
            print(data.decode(), end='')
            if chr(0x04) in data.decode():
                break


def main():
    try:
        parser = argparse.ArgumentParser(
            description="OpenEPaperLink Flasher for AP/Flasher board")
        parser.add_argument("-p", "--port", help="COM port to use")
        parser.add_argument("command", choices=[
                            "read", "write", "autoflash", "debug"], help="Command to execute")
        parser.add_argument("filename", nargs="?",
                            help="Filename for read/write commands")
        parser.add_argument("-f", "--flash", action="store_true",
                            help="Write to the flash")
        parser.add_argument("-i", "--infopage", action="store_true",
                            help="Write to the infopage/UICR")
        parser.add_argument("-n", "--nrf82511", action="store_true",
                            help="nRF82511 programming")
        parser.add_argument("-z", "--zbs243", action="store_true",
                            help="ZBS243 programming")
        parser.add_argument("--internalap", action="store_true",
                            help="Selects the internal accesspoint port")
        parser.add_argument("-e", "--external", action="store_true",
                            help="Selects the external(side) port")
        parser.add_argument("--altradio", action="store_true",
                            help="Selects the alternate radio port")
        parser.add_argument("--pt", "--passthrough", action="store_true",
                            help="Enters serial passthrough for debug output after flashing")

        args = parser.parse_args()

        if not validate_arguments(args):
            return

        if not args.port:
            list_available_com_ports()
            return

        global ser
        ser = serial.Serial(args.port, baudrate=115200)
        time.sleep(1)  # Flush serial data
        while (ser.inWaiting() > 0):
            data_str = ser.read(ser.inWaiting())

        send_cmd(CMD_GET_VERSION, bytearray([]))
        cmd, answer = wait_for_command()
        if (cmd == CMD_GET_VERSION):
            print("AP/Flasher version: " +
                  str(answer[0] << 24 | answer[1] << 16 | answer[2] << 8 | answer[3]))
        else:
            print(
                "Couldn't establish a connection with the flasher, did you select the correct serial port?")
            exit(0)
        send_cmd(CMD_SET_POWER, bytearray([1]))
        cmd, answer = wait_for_command()
        if (args.command != "debug"):
            if args.internalap:
                send_cmd(CMD_SELECT_PORT, bytearray([0]))
            if args.external:
                send_cmd(CMD_SELECT_PORT, bytearray([1]))
            if args.altradio:
                send_cmd(CMD_SELECT_PORT, bytearray([2]))

            if args.nrf82511:
                send_cmd(CMD_SELECT_NRF82511, bytearray([]))
            if args.zbs243:
                send_cmd(CMD_SELECT_ZBS243, bytearray([]))
            cmd, answer = wait_for_command()
#            send_cmd(CMD_SET_POWER, bytearray([1]))
#            cmd, answer = wait_for_command()
            if (answer[0] == 1):
                print("Connection established to microcontroller")
            else:
                print("Failed to establish a connection to the microcontroller")
                exit(0)

            if args.command == "read":
                read_from_serial(ser, args.filename, args.flash)
            elif args.command == "write":
                write_to_serial(ser, args.filename, args.flash)
            elif args.command == "autoflash":
                print("Starting automatic tag flash")
                send_cmd(CMD_AUTOFLASH, bytearray([]))
                short_passthough(30)
            else:
                print("Invalid command!")

            send_cmd(CMD_RESET, bytearray([]))
            cmd, answer = wait_for_command()

        if (args.pt or args.command == "debug"):
            # enter passthrough mode
            send_cmd(CMD_PASS_THROUGH, bytearray([]))
            print("Now showing debug output from the tag - CTRL+C to quit")
            print(
                "---------------------------------------------------------------------------------")
            while True:
                try:
                    data = ser.read()
                    if data:
                        print(data.decode('utf-8', errors='replace'), end='')
                except UnicodeDecodeError:
                    print(" ")
                data = ser.read()
                if data:
                    print(data.decode(), end='')
                    if chr(0x04) in data.decode():
                        break

    except KeyboardInterrupt:
        print("\nBye!")
        ser.close()
        exit(1)


if __name__ == "__main__":
    main()
