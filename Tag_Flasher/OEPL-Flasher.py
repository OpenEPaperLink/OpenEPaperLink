import argparse
import serial.tools.list_ports
import serial
import time
from intelhex import IntelHex
import os.path
import socket
import sys

CMD_GET_VERSION = 1
CMD_RESET_ESP = 2
CMD_RESET = 11
CMD_SET_POWER = 13
CMD_SET_TESTP = 14

CMD_ERASE_FLASH = 26
CMD_ERASE_INFOPAGE = 27
CMD_SAVE_MAC_FROM_FW = 40
CMD_PASS_THROUGH = 50

CMD_SELECT_ZBS243 = 60
CMD_SELECT_NRF82511 = 61

CMD_SELECT_EEPROM_PT = 69

CMD_SELECT_PORT = 70

CMD_READ_EEPROM = 90
CMD_WRITE_EEPROM = 91

CMD_READ_INFOPAGE = 80
CMD_READ_FLASH = 81
CMD_WRITE_INFOPAGE = 82
CMD_WRITE_FLASH = 83
CMD_AUTOFLASH = 87
CMD_COMPLETE = 88

CMD_WRITE_ERROR = 99

TRANSPORT_SER = 0
TRANSPORT_TCP = 1

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
    if transport == TRANSPORT_TCP:
        tcp_socket.send(return_data)
    else:
        ser.write(return_data)

def wait_for_command():
    if transport == TRANSPORT_TCP:
        return wait_for_command_tcp()
    else:
        return wait_for_command_ser()

def wait_for_command_ser():
    start_time = time.time()
    ser.timeout = 3  # Set the timeout to 3 seconds
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

def wait_for_command_tcp():
    start_time = time.time()
    tcp_socket.settimeout(5)
    while True:
        try:
            if tcp_socket.recv(2) == b"AT":
                # Read the command byte
                cmd = int.from_bytes(tcp_socket.recv(1), byteorder='big')
                data_length = int.from_bytes(
                    tcp_socket.recv(4), byteorder='big')  # Read the data length
                data = tcp_socket.recv(data_length)  # Read the data
                # Read the CRC value
                crc = int.from_bytes(tcp_socket.recv(2), byteorder='big')
                # Verify CRC
                crc_val = 0xAB34
                for x in to_byte(cmd, 1) + to_byte(data_length, 4) + data:
                    crc_val += x
                if crc_val & 0xffff == crc:
                    return cmd, data
                else:
                    print("Invalid CRC. Discarding command. Got " +
                          str(crc_val) + " but was expecting " + str(crc))
                    print("data was:" + str(data))
        except socket.timeout:
            if time.time() - start_time > 1:
                print("timeout waiting for reply")
                return None, None

def list_available_com_ports():
    available_ports = [port.device for port in serial.tools.list_ports.comports()]
    print("Available COM ports:", ', '.join(available_ports))

def validate_arguments(args):
    if not (args.port or args.ip):
        print("Either --port or --ip option is required.")
        list_available_com_ports()
        return False
    if args.command:
        if not (args.nrf82511 or args.zbs243):
            print("Either -nrf82511 or -zbs243 option is required.")
            return False
    if not (args.internalap or args.external or args.altradio):
        print("Using external port")
        args.external = True
    if args.command in ["read", "write"] and not (args.flash or args.infopage or args.eeprom):
        print("One of --flash, --infopage or --eeprom arguments is required for read and write commands.")
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


def read_from_serial(filename, args):
    if args.flash:
        print(
            f"Reading flash data and saving to file: {filename}.")
    elif args.infopage:
        print(
            f"Reading info page and saving to file: {filename}.")
    elif args.eeprom:
        print(
            f"Reading EEPROM and saving to file: {filename}.")

    print("Now downloading...\n")
    file = open(filename, "wb")
    bytecount = 0
    while True:

        print(f'\r{bytecount} bytes', end='', flush=True)

        if (args.flash):
            send_cmd(CMD_READ_FLASH, bytearray([]))
        elif args.infopage:
            send_cmd(CMD_READ_INFOPAGE, bytearray([]))
        elif args.eeprom:
            send_cmd(CMD_READ_EEPROM, bytearray([]))

        cmd, answer = wait_for_command()
        if ((cmd == CMD_READ_FLASH) or (cmd == CMD_READ_INFOPAGE) or (cmd == CMD_READ_EEPROM)):
            file.write(bytearray(answer))
            bytecount += len(answer)
        elif (cmd == CMD_COMPLETE):
            file.close()
            print("\r All done!         \nFile saved\n", end='', flush=True)
            break
        else:
            print("Failed reading block, timeout?")


def write_to_serial(filename, args):
    if (args.flash):
        print(f"\nErasing flash... ")
        send_cmd(CMD_ERASE_FLASH, bytearray([]))
    elif (args.infopage):
        print(f"\nErasing infopage... ")
        send_cmd(CMD_ERASE_INFOPAGE, bytearray([]))

    if(args.flash or args.infopage):
        cmd, answer = wait_for_command()
        if ((cmd == CMD_ERASE_FLASH) or (cmd == CMD_ERASE_INFOPAGE)):
            print("DONE!\n")
        else:
            print("\nFailed to erase the flash?")
            exit(0)

    chunk_size = 256

    if (args.flash):
        print(f"Writing flash data from file: {filename}\n")
    elif (args.infopage):
        print(f"Writing info page data from file: {filename}\n")
    elif(args.eeprom):
        print(f"Writing eeprom from file {filename}\n")
        chunk_size = 4096



    if filename.endswith('.bin'):
        file_data = read_binary_file(filename)
    elif filename.endswith('.hex'):
        file_data = read_hex_file(filename)

    for i in range(0, len(file_data), chunk_size):
        chunk = file_data[i:i + chunk_size]
        if (args.flash):
            send_cmd(CMD_WRITE_FLASH, bytearray(chunk))
        elif args.infopage:
            send_cmd(CMD_WRITE_INFOPAGE, bytearray(chunk))
        elif args.eeprom:
            send_cmd(CMD_WRITE_EEPROM, bytearray(chunk))
        cmd, answer = wait_for_command()
        if ((cmd == CMD_WRITE_FLASH) or (cmd == CMD_WRITE_INFOPAGE) or (cmd==CMD_WRITE_EEPROM)):
            print(f'\rSent {i} bytes', end='', flush=True)
        elif (cmd == CMD_COMPLETE):
            print(
                '\nTried to write more bytes than we have room for!   \n', end='', flush=True)
            return
        elif (cmd == CMD_WRITE_ERROR):
            print(
                '\nError writing to the tag. Check your connection.   \n', end='', flush=True)
            return
        else:
            print("Some other error, dunno\n")
            return
    print('\rAll done writing! ', end='', flush=True)


def short_passthrough(period_time):
    start_time = time.time()
    while time.time() - start_time < period_time:
        if transport == TRANSPORT_TCP:
            try:
                data = tcp_socket.recv(1)
            except socket.timeout:
                pass
        else:
            try:
                data = ser.read(1)
            except UnicodeDecodeError:
                pass
        print(data, end='')
        if chr(0x04) in data:
            break

def main():
    try:
        parser = argparse.ArgumentParser(
            description="OpenEPaperLink Flasher for AP/Flasher board")
        parser.add_argument("-p", "--port", help="COM port to use")
        parser.add_argument("-t", "--ip", help="IP Address to use")
        parser.add_argument("command", nargs="?", choices=["read", "write", "autoflash", "debug"], help="Command to execute")
        parser.add_argument("filename", nargs="?",
                            help="Filename for read/write commands")
        parser.add_argument("-f", "--flash", action="store_true",
                            help="Write to the flash")
        parser.add_argument("-i", "--infopage", action="store_true",
                            help="Write to the infopage/UICR")
        parser.add_argument("-eep", "--eeprom", action="store_true",
                            help="EEPROM operations")
        parser.add_argument("-n", "--nrf82511", action="store_true",
                            help="nRF82511 programming")
        parser.add_argument("-z", "--zbs243", action="store_true",
                            help="ZBS243 programming")
        parser.add_argument("--internalap", action="store_true",
                            help="Selects the internal accesspoint port")
        parser.add_argument("-e", "--external", action="store_true",
                            help="Selects the external(side) port (default)")
        parser.add_argument("--altradio", action="store_true",
                            help="Selects the alternate radio port")
        parser.add_argument("--pt", "--passthrough", action="store_true",
                            help="Enters serial passthrough for debug output after flashing")
        parser.add_argument("--exit", action="store_true", help="Exit eeprom loader after sending data")

        args = parser.parse_args()

        if not validate_arguments(args):
            program_name = os.path.basename(sys.argv[0])
            print(f"Usage: {program_name} --help")
            return

        global transport
        if (args.ip):
            global tcp_socket
            ip_address = args.ip
            port = 243
            tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            transport = TRANSPORT_TCP

            try:
                tcp_socket.connect((ip_address, port))
                print(f"Connected to {ip_address}:{port}")
            except Exception as e:
                print(f"Error: {e}")
                return
        else:
            global ser
            try:
                ser = serial.Serial(args.port, baudrate=115200)
                print(f"Successfully opened serial port {args.port}")
            except serial.SerialException as e:
                print(f"Error: {e}")
                print("Please check if the specified COM port exists or is accessible.")
                list_available_com_ports()
                return
            transport = TRANSPORT_SER
            time.sleep(0.1)  # Flush serial data
            while (ser.inWaiting() > 0):
                data_str = ser.read(ser.inWaiting())
	
        send_cmd(CMD_GET_VERSION, bytearray([]))
        cmd, answer = wait_for_command()
        if (cmd == CMD_GET_VERSION):
            print("Connection with the flasher established. Version: " +
                  str(answer[0] << 24 | answer[1] << 16 | answer[2] << 8 | answer[3]))
        else:
            print(
                "Couldn't establish a connection with the flasher, did you select the correct serial port?")
            exit(0)

        if (args.eeprom):
            send_cmd(CMD_SET_TESTP, bytearray([0]))
            cmd, answer = wait_for_command()
            send_cmd(CMD_SET_POWER, bytearray([1]))
            cmd, answer = wait_for_command()
            send_cmd(CMD_RESET, bytearray([]))
            cmd, answer = wait_for_command()
            send_cmd(CMD_SELECT_EEPROM_PT, bytearray([])) ## selects eeprom serial loader mode
            cmd, answer = wait_for_command()

        elif (args.flash or args.infopage):
            if (args.command != "debug"):
                if args.internalap:
                    send_cmd(CMD_SELECT_PORT, bytearray([0]))
                if args.external:
                    send_cmd(CMD_SELECT_PORT, bytearray([1]))
                if args.altradio:
                    send_cmd(CMD_SELECT_PORT, bytearray([2]))

                send_cmd(CMD_SET_POWER, bytearray([1]))
                cmd, answer = wait_for_command()

                if args.nrf82511:
                    send_cmd(CMD_SELECT_NRF82511, bytearray([]))
                if args.zbs243:
                    send_cmd(CMD_SELECT_ZBS243, bytearray([]))
                cmd, answer = wait_for_command()

                if (answer[0] == 1):
                    print("Connection established to microcontroller")
                elif (answer[0] == 2):
                    print("Established connection to the microcontroller, but it is locked. Exiting.")
                    send_cmd(CMD_SET_POWER, bytearray([0]))
                    exit(0)
                else:
                    print("Failed to establish a connection to the microcontroller")
                    send_cmd(CMD_SET_POWER, bytearray([0]))
                    exit(0)
                    
        if args.command == "read":
            read_from_serial(args.filename, args)
        elif args.command == "write":
            write_to_serial(args.filename, args)
        elif args.command == "autoflash":
            print("Starting automatic tag flash")
            send_cmd(CMD_AUTOFLASH, bytearray([]))
            short_passthrough(30)
        else:
            print("No command")

        if(args.eeprom):
            send_cmd(CMD_SET_TESTP, bytearray([1]))
            cmd, answer = wait_for_command()

        if args.exit:
            print("Exiting after writing eeprom");

        send_cmd(CMD_RESET, bytearray([]))
        cmd, answer = wait_for_command()

        if (args.pt or args.command == "debug"):
            send_cmd(CMD_SET_POWER, bytearray([1]))
            cmd, answer = wait_for_command()
            # enter passthrough mode
            send_cmd(CMD_PASS_THROUGH, bytearray([]))
            print("Now showing debug output from the tag - CTRL+C to quit")
            print(
                "---------------------------------------------------------------------------------")
            while True:
                if transport == TRANSPORT_TCP:
                    try:
                        data = tcp_socket.recv(1)
                        data_str = data.decode('utf-8')
                    except socket.timeout:
                        data_str = ""
                        pass
                else:
                    try:
                        data = ser.read(1)
                        data_str = data.decode('utf-8')
                    except UnicodeDecodeError:
                        data_str = ""
                        pass
                print(data_str, end='')
                if chr(0x04) in data_str:
                    break

    except KeyboardInterrupt:
        print("\nBye!")
        if transport == TRANSPORT_TCP:
            tcp_socket.close()
        else:
            ser.close()
        exit(1)


if __name__ == "__main__":
    main()
