#!/usr/bin/python

###    Autor: Aaron Christophel ATCnetz.de    ###

# arg1 = COMPORT  arg2 = read or write arg3 = filename to read or write

import sys
import os
import time
import serial.tools.list_ports


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


def byteconversion(input, number=4):
    return input.to_bytes(number, byteorder='big')


def cmdsend(cmd, data):
    return_data = byteconversion(cmd, 1) + byteconversion(len(data), 4) + data
    crc_val = 0xAB34
    for x in return_data:
        crc_val += x
    return_data = b"AT" + return_data + byteconversion(crc_val & 0xffff, 2)
    ser.write(return_data)


def commandwait():
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
                for x in byteconversion(cmd, 1) + byteconversion(data_length, 4) + data:
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


def bytes_to_int(bytes):
    result = 0
    for b in range(3, -1, -1):
        result = result * 256 + int(bytes[b])
    return result


def to_byte(input, number=4):
    return input.to_bytes(number, byteorder='little')


def create_full_fw(filename, out_filename):
    print("Opening file " + filename)
    in_file = open(filename, "rb")
    data = bytearray(in_file.read())
    filesize = len(data)
    in_file.close()
    print("file size: "+str(filesize))
    entry_point = bytes_to_int(data[4:8])
    print("entry point: " + hex(entry_point))

    # adding firmware header here
    header = to_byte(0x00000000)
    header += to_byte(0xFFFFFFFF)
    header += to_byte(filesize)
    header += to_byte(0xFFEEFFFF)
    header += to_byte(0xFFFFFFFF)
    header += to_byte(0x100000)
    header += to_byte(entry_point)
    header += to_byte(0x4D52564C)
    data = header+data
    file = open(out_filename, "wb")
    file.write(data)
    file.close()


# doing this testing here so we dont need a UART
if (len(sys.argv) == 4 and sys.argv[1] == 'img'):
    create_full_fw(sys.argv[2], sys.argv[3])
    exit()


if (len(sys.argv) != 4):
    print("Example: COM1 read file.bin, or COM1 write file.bin, or COM1 write_flash file.bin")
    print("To create an OTA file use: img file.bin")
    print("Not the right arguments but here are the... please wait...")
    ports_list = "possible UART ports: "
    for port in serial.tools.list_ports.comports():
        ports_list += port.device + " "
    print(ports_list)
    exit()

usedCom = sys.argv[1]  # "COM14"
read_or_write = sys.argv[2]
file = sys.argv[3]
usedBaud = 500000

global ser
ser = serial.Serial(usedCom, usedBaud, serial.EIGHTBITS,
                    serial.PARITY_NONE, serial.STOPBITS_ONE, timeout=5)

#ser = serial.Serial(usedCom, baudrate=500000)
time.sleep(1)  # Flush serial data
while (ser.inWaiting() > 0):
    data_str = ser.read(ser.inWaiting())

cmdsend(CMD_GET_VERSION, bytearray([]))
cmd, answer = commandwait()
if (cmd == CMD_GET_VERSION):
    print("AP/Flasher version: " +
          str(answer[0] << 24 | answer[1] << 16 | answer[2] << 8 | answer[3]))
else:
    print(
        "Couldn't establish a connection with the flasher, did you select the correct serial port?")
    exit(0)

cmdsend(CMD_SELECT_PORT, bytearray([1]))
cmd, answer = commandwait()
cmdsend(CMD_SET_POWER, bytearray([0]))

time.sleep(0.1)
cmdsend(CMD_SET_POWER, bytearray([1]))

cmdsend(CMD_PASS_THROUGH, bytearray([]))

serialPort = ser
print('Using port: {}'.format(usedCom))

bootloader_password = 0xffffff01
uart_checksum = 0x00

def get_cmd(cmd,length=0,address=0,plus_size=0):
    return_data = to_byte(cmd,1) + to_byte(0x00,1) + to_byte(length,1) + to_byte(bootloader_password) + to_byte(address)
    return bytearray([len(return_data)+plus_size]) + return_data
    
def send_cmd(cmd):
    global uart_checksum
    uart_checksum ^= 0x40
    cmd[2] = uart_checksum
    #print(' '.join(format(x, '02x') for x in cmd))
    serialPort.write(cmd)

def wait_for_prompt():
    while (True):
        if (serialPort.inWaiting()>0):
            data_str = serialPort.read(serialPort.inWaiting())
            print(' '.join(format(x, '02x') for x in data_str))
            for x in data_str:
                if x == 0x3A:
                    serialPort.write(to_byte(0x3A,1))
                    print("Halting boot")
                    return
                if x == 0x23:
                    print("Got it")
                    return
        time.sleep(0.01)

def uart_flush(): 
    time.sleep(1)# Flush serial data 
    while(serialPort.inWaiting()>0):
        data_str = serialPort.read(serialPort.inWaiting())
        print(' '.join(format(x, '02x') for x in data_str))

def uart_receive_handler(expected_len):
    data_str = serialPort.read(expected_len)   
    #print("Answer: " + ' '.join(format(x, '02x') for x in data_str))
    if(data_str[0]!=len(data_str)-1):
        print("Error while receiving")
        exit()
    return data_str
    
def enable_swd(state=1):
    return get_cmd(1,0,not state)
    
def read_memory(address,length):#max 128bytes
    return get_cmd(2,int(length/4),address)

#Example: write_memory(0x00,bytearray([0x02, 0x07, 0x40, 0x34]))
def write_memory(address,data):#max 128bytes
    return get_cmd(3,int(len(data)/4),0x100000+address,len(data))+data
    
def read_flash(address,length):
    return get_cmd(4,int(length/4),address)
    
def run_address(address):
    return get_cmd(5,0,address)
    
def reset_chip():
    return get_cmd(7)
    
def erase_flash():
    return run_address(0x666)
    
def erase_flash_custom():
    return get_cmd(8)

def flash_file(filename):
    print("Opening file " + filename)
    in_file = open(filename, "rb")
    data = bytearray(in_file.read())
    filesize = len(data)
    in_file.close()
    print("file size: "+str(filesize))
    entry_point = bytes_to_int(data[4:8])
    print("entry point: "+ hex(entry_point))
    
    block_size = 80 # bytes per sending
    cur_address = 0
    while(filesize-cur_address>0):
        if(filesize-cur_address>block_size):
            cur_size = block_size
        else:
            cur_size = filesize-cur_address
        cur_data = data[cur_address:cur_address+cur_size]
        print("Current address: " + hex(cur_address))
        send_cmd(write_memory(cur_address,cur_data))
        cur_address+=cur_size
        uart_receive_handler(4)
    print("Running fimrware from: " + hex(entry_point))
    send_cmd(run_address(entry_point))
        
def flash_file_flash(filename):
    print("Opening file " + filename)
    in_file = open(filename, "rb")
    data = bytearray(in_file.read())
    filesize = len(data)
    in_file.close()
    print("file size: "+str(filesize))
    entry_point = bytes_to_int(data[4:8])
    print("entry point: "+ hex(entry_point))
    
    #adding firmware header here
    
    header =  to_byte(0x00000000)
    header += to_byte(0xFFFFFFFF)
    header += to_byte(filesize)
    header += to_byte(0xFFEEFFFF)
    header += to_byte(0xFFFFFFFF)
    header += to_byte(0x100000)
    header += to_byte(entry_point)
    header += to_byte(0x4D52564C)
    filesize+=32
    data = header+data
    
    block_size = 80 # bytes per sending
    cur_address = 0
    while(filesize-cur_address>0):
        if(filesize-cur_address>block_size):
            cur_size = block_size
        else:
            cur_size = filesize-cur_address
        cur_data = data[cur_address:cur_address+cur_size]
        print("Current address: " + hex(cur_address))
        send_cmd(write_memory(cur_address,cur_data))
        cur_address+=cur_size
        uart_receive_handler(4)
    print("Rebooting to new firmware")
    send_cmd(reset_chip())
    
def flash_erase():
    print("Erasing flash now")
    send_cmd(erase_flash_custom())
    uart_receive_handler(4)

def dump_flash(filename):
    block_size = 128 # bytes per request
    for x in range(int(0x80000/block_size)):
        print("Current address: " + hex(x*block_size))
        send_cmd(read_flash(x*block_size,block_size)) 
        current_block_dump = uart_receive_handler(4+block_size)
        file = open(filename,"ab+") 
        file.write((''.join(chr(i) for i in current_block_dump[4:])).encode('charmap')) 
        file.close() 
    print("Done downloading flash!")
    exit(0)

print("Waiting for boot prompt, please reset the chip") 
wait_for_prompt()
uart_flush()
        
if(read_or_write=='read'):
    dump_flash('read_' + file)
    
elif(read_or_write=='write'):
    flash_file(file)
    
elif(read_or_write=='write_flash'):
    dir_path = os.path.dirname(os.path.realpath(__file__))
    flash_file(dir_path+"/88mz100_second_stage_bootloader.bin")
    if(uart_receive_handler(8)[1:].decode('utf-8') == "ATCboot"):
        print("Second stage bootloader started");
        uart_checksum = 0x00
        flash_erase()
        flash_file_flash(file)
    
data_str = ""
while(1):
    while(serialPort.inWaiting()>0):
        try:
            data_str += serialPort.read(serialPort.inWaiting()).decode('utf-8')
            if(data_str.endswith("\n")):
                print(data_str, end = '')
                data_str = ""
        except:
            print("Error")
            
        
serialPort.close()