#!/usr/bin/env python
 
### TlsrComProg.py ###
###  Autor: pvvx   ###
 
import sys
import signal
import struct
import serial
import platform
import time
import argparse
import os
import io

__progname__ = 'TLSR825x Floader'
__filename__ = 'TlsrComProg'
__version__ = "10.11.20"

CMD_VER	 = b'\x00'	# Get version 
CMD_RBF	 = b'\x01'	# Read Block Flash
CMD_WBF	 = b'\x02'	# Write Block Flash
CMD_EFS	 = b'\x03'	# Erase Flash Sectors
CMD_EAF  = b'\x04'	# Erase All Flash
CMD_JDC	 = b'\x05'	# Get Flash JEDEC ID

FLASH_SECTOR_SIZE = 4096

class FatalError(RuntimeError):
	def __init__(self, message):
		RuntimeError.__init__(self, message)

	@staticmethod
	def WithResult(message, result):
		message += " (result was %s)" % hexify(result)
		return FatalError(message)

def signal_handler(signal, frame):
	print()
	print('Keyboard Break!')
	sys.exit(0)

crctable = (
0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 )
def crc16(data: bytearray, length):
	crc = 0xFFFF
	for i in range(0, length):
		crc = (crc >> 8) ^ crctable[(crc ^ data[i]) & 0xFF]
	return bytearray([crc&0xFF,(crc>>8)&0xFF])
def crc_blk(data: bytearray):
	return data+crc16(data, len(data))
def crc_chk(data: bytearray):
	c = data[len(data)-2:];
	if c == crc16(data, len(data)-2):
		return 1
	return 0
def arg_auto_int(x):
	return int(x, 0)
def sws_code_blk(blk):
	pkt=[]
	d = bytearray([0xe8,0xef,0xef,0xef,0xef])
	for el in blk:
		if (el & 0x80) != 0:
			d[0] &= 0x0f
		if (el & 0x40) != 0:
			d[1] &= 0xe8
		if (el & 0x20) != 0:
			d[1] &= 0x0f
		if (el & 0x10) != 0:
			d[2] &= 0xe8
		if (el & 0x08) != 0:
			d[2] &= 0x0f
		if (el & 0x04) != 0:
			d[3] &= 0xe8
		if (el & 0x02) != 0:
			d[3] &= 0x0f
		if (el & 0x01) != 0:
			d[4] &= 0xe8
		pkt += d 
		d = bytearray([0xef,0xef,0xef,0xef,0xef])
	return pkt
def sws_rd_addr(addr):
	return sws_code_blk(bytearray([0x5a, (addr>>16)&0xff, (addr>>8)&0xff, addr & 0xff, 0x80]))
def sws_code_end():
	return sws_code_blk(b'\xff')
def sws_wr_addr(addr, data):
	return sws_code_blk(bytearray([0x5a, (addr>>16)&0xff, (addr>>8)&0xff, addr & 0xff, 0x00]) + bytearray(data)) + sws_code_end()

#--------------------
def EraseSectorsFlash(rs, offset = 0, count = 1):
	cnt_err = 2
	size = count * FLASH_SECTOR_SIZE
	offset = offset & (0xffffff^(FLASH_SECTOR_SIZE-1))
	ret = 1
	for i in range(count):
		print('\rErase sector at 0x%06x...' % offset, end = '')
		while 1:
			blk = struct.pack('<BBH', ord(CMD_EFS), offset & 0xff,	(offset>>8) & 0xffff)
			ret = rs.write(crc_blk(blk))
			data = rs.read(6)
			if len(data) != 6 or data[0] != ord(CMD_EFS) or not crc_chk(data):
				cnt_err -= 1
				if cnt_err == 0:
					print('\rError Erase sector at 0x%06x!' % offset)
					return 0
				print(' Error', cnt_err)
			else:
				cnt_err = 2
			break
		offset += FLASH_SECTOR_SIZE
	print('\r                               \r',  end = '')
	return ret
#--------------------
def EraseAllFlash(rs):
	blk = struct.pack('<BBH', ord(CMD_EAF), 0, 0)
	ret = rs.write(crc_blk(blk))
	data = rs.read(6)
	if len(data) != 6 or data[0] != ord(CMD_EAF) or not crc_chk(data):
		print('\rError Erase All Flash!')
		return 0
	return ret
#--------------------
def WriteBlockFlash(rs, stream, offset = 0, size = 0, erase = True):
	cnt_err = 2
	offset &= 0xffffff
	wrsize = 1024
	ret = 1
	if erase and (offset & (FLASH_SECTOR_SIZE-1)) != 0:
		erasec = offset & (0xffffff^(FLASH_SECTOR_SIZE-1))
	else:
		erasec = 0xffffffff
	while size > 0:
		if erase:
			wrsec = offset & (0xffffff^(FLASH_SECTOR_SIZE-1))
			if erasec != wrsec:
				print('\rErase at 0x%06x...' % offset, end = '')
				while 1:
					blk = struct.pack('<BBH', ord(CMD_EFS), offset & 0xff,	(offset>>8) & 0xffff)
					ret = rs.write(crc_blk(blk))
					data = rs.read(6)
					if len(data) != 6 or data[0] != ord(CMD_EFS) or not crc_chk(data):
						cnt_err -= 1
						if cnt_err == 0:
							print('\rError Erase sector at 0x%06x!' % offset)
							return 0
						print(' Error', cnt_err)
					else:
						cnt_err = 2
					break
				erasec = wrsec
		data = stream.read(wrsize)
		wrsize = len(data)
		if not data or wrsize == 0: # end of stream
			print(' Error Read file!')
			return 0
		for e in data:
			if e != 0xff:
				while 1:
					print('\rWrite to 0x%04x...' % offset, end = '')
					ret = rs.write(crc_blk(struct.pack('<BBH',ord(CMD_WBF),offset & 0xff,(offset>>8) & 0xffff)+data))
					rdata = rs.read(6);
					if len(rdata) != 6 or rdata[0] != ord(CMD_WBF) or not crc_chk(rdata):
						cnt_err -= 1
						if cnt_err == 0:
							print('\rError Write Flash data at 0x%06x!' % offset)
							return 0
						print(' Error', cnt_err)
					else:
						cnt_err = 2
					break
				break
		offset += wrsize
		size -= wrsize
	print('\r                               \r',  end = '')
	return ret
#--------------------
def ReadBlockFlash(rs, stream, offset = 0, size = 0x80000):
	cnt_err = 2
	offset &= 0xffffff
	rdsize = 1024
	ret = 1
	while size > 0:
		if rdsize > size:
			rdsize = size
		print('\rRead from 0x%06x...' % offset, end = '')
		while 1:
			blk = struct.pack('<BBHH', ord(CMD_RBF), offset & 0xff,	(offset>>8) & 0xffff, rdsize)
			ret = rs.write(crc_blk(blk))
			data = rs.read(rdsize+6)
			if len(data) != rdsize + 6 or data[0] != ord(CMD_RBF) or not crc_chk(data):
				cnt_err -= 1
				if cnt_err == 0:
					print('\rError Read Flash data at 0x%06x!' % offset)
					return 0
				print(' Error', cnt_err)
			else:
				cnt_err = 2
			break
		stream.write(data[4:rdsize+4]);
		size -= rdsize
		offset += rdsize
	print('\r                               \r',  end = '')
	return ret
#============================= main()
def main():
	signal.signal(signal.SIGINT, signal_handler)
	parser = argparse.ArgumentParser(description='%s version %s' % (__progname__, __version__), prog=__filename__)
	parser.add_argument(
		'--port', '-p',
		help='Serial port device (default: COM1)',
		default='COM1')
	parser.add_argument(
		'--tact','-t', 
		help='Time Activation ms (0-off, default: 600 ms)', 
		type=arg_auto_int, 
		default=600)
	parser.add_argument(
		'--fldr','-f', 
		help='Filename floader (default: floader.bin)', 
		default='floader.bin')
	parser.add_argument(
		'--baud','-b', 
		help='UART Baud Rate (default: 230400)', 
		type=arg_auto_int, 
		default=230400)
	subparsers = parser.add_subparsers(
			dest='operation',
			help='Run '+__filename__+' {command} -h for additional help')

	parser_read_flash = subparsers.add_parser(
			'rf',
			help='Read Flash to binary file')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash.add_argument('filename', help='Name of binary file')

	parser_burn_flash = subparsers.add_parser(
			'we',
			help='Write file to Flash with sectors erases')
	parser_burn_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_burn_flash.add_argument('filename', help='Name of binary file')

	parser_write_flash = subparsers.add_parser(
			'wf',
			help='Write file to Flash without sectors erases')
	parser_write_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_write_flash.add_argument('filename', help='Name of binary file')

	parser_erase_sec_flash = subparsers.add_parser(
			'es',
			help='Erase Region (sectors) of Flash')
	parser_erase_sec_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_erase_sec_flash.add_argument('size', help='Size of region', type=arg_auto_int)

	parser_erase_all_flash = subparsers.add_parser(
			'ea',
			help='Erase All Flash')

	args = parser.parse_args()
	print('================================================')
	print('%s version %s' % (__progname__, __version__))
	print('------------------------------------------------')
	print ('Open %s, %d baud...' % (args.port, args.baud))
	try:
		serialPort = serial.Serial(args.port, args.baud, \
								   serial.EIGHTBITS,\
								   serial.PARITY_NONE, \
								   serial.STOPBITS_ONE)
		serialPort.setRTS(False)
		serialPort.setDTR(False)
		# time.sleep(0.01)
		serialPort.flushInput()
		serialPort.flushOutput()
		serialPort.reset_input_buffer()
		serialPort.reset_output_buffer()
	except:
		print ('Error: Open %s, %d baud!' % (args.port, args.baud))
		sys.exit(1)
	warn = 0
	serialPort.timeout = 33*12/args.baud + 0.001
	#--------------------------------
	# Test Floader Already: Send BAD CMD
	# 1F 34 56 78 79 BC -> 9F 34 56 78 50 7C ?
	byteSent = serialPort.write(crc_blk(b'\x1f\x34\x56\x78'))
	read = serialPort.read(33)
	if read != crc_blk(b'\x9f\x34\x56\x78'):
		#--------------------------------
		# Open Floader
		try:
			stream = open(args.fldr, 'rb')
			size = os.path.getsize(args.fldr)
		except:
			serialPort.close
			print('Error: Not open input file <%s>!' % args.fldr)
			sys.exit(2)
		if size < 8:
			stream.close
			serialPort.close
			print('Error: File size = %d!'% size)
			sys.exit(3)
		#--------------------------------
   	    # Stop CPU|: [0x0602]=5
		serialPort.write(sws_code_end())
		blk = sws_wr_addr(0x0602, b'\x05')
		serialPort.write(blk)
		if args.tact != 0:
			#--------------------------------
			# issue reset-to-bootloader:
			# RTS = either RESET (active low = chip in reset)
			# DTR = active low
			print('Reset module (RTS low)...')
			serialPort.setDTR(True)
			serialPort.setRTS(True)
			time.sleep(0.05)
			print('Activate (%d ms)...' % args.tact)
			tact = args.tact/1000.0
			for _ in range(5):
				byteSent +=	serialPort.write(blk)
			serialPort.setRTS(False)
			for _ in range(5):
				byteSent +=	serialPort.write(blk)
			serialPort.setDTR(False)
			byteSent = serialPort.write(blk)
			t1 = time.time()
			while time.time()-t1 < tact:
				for _ in range(5):
					serialPort.write(blk)
		time.sleep(0.001)
		serialPort.flushOutput()
		serialPort.flushInput()
		serialPort.reset_input_buffer()
		#--------------------------------
		# Set SWS speed low: [0x00b2]=50
		byteSent = serialPort.write(sws_code_end())
		x = int(32000000/args.baud)
		if x > 127:
			x = 127
		byteSent += serialPort.write(sws_wr_addr(0x00b2, bytearray([x])))
		#--------------------------------
		# Test read bytes [0x00b2]
		byteSent += serialPort.write(sws_rd_addr(0x00b2))
		byteRead = len(serialPort.read(byteSent+33))
		# print(byteRead, byteSent)
		if byteRead != byteSent:
			byteSent = 0
			warn += 1
			print('Warning: Wrong RX-TX connection?')
		# Start read
		serialPort.write(b'\xff')
		read = serialPort.read(33)
		byteRead = len(read)
		if byteRead == 1 and read[0] == b'\xff':
			print('Warning: Pin RX no connection to the module?')		
			warn += 1
		else:
			print('Connection...')		
		# stop read
		byteSent = serialPort.write(sws_code_end())
		serialPort.read(33)
		#--------------------------------
		# Load floader.bin
		binWrite = 0
		rdsize = 0x100
		addr = 0x40000
		print('Load <%s> to 0x%04x...' % (args.fldr, addr))		
		while size > 0:
			print('\r0x%04x' % addr, end = '')		
			data = stream.read(rdsize)
			if not data: # end of stream
				print('send: at EOF')
				break
			byteSent += serialPort.write(sws_wr_addr(addr, data))
			serialPort.reset_input_buffer()
			binWrite += len(data)
			addr += len(data)
			size -= len(data)
		stream.close
		print('\rBin bytes writen:', binWrite)		
		print('CPU go Start...')
		byteSent += serialPort.write(sws_wr_addr(0x0602, b'\x88')) # cpu go Start
		time.sleep(0.07)
		serialPort.flushInput()
		serialPort.flushOutput()
		serialPort.reset_input_buffer()
		serialPort.reset_output_buffer()
		time.sleep(0.07)
	# print('COM bytes sent:', byteSent)
	print('------------------------------------------------')
	# print('Get version floader...')
	byteSent = serialPort.write(crc_blk(b'\x00\x00\x00\x00'))
	read = serialPort.read(10)
	if len(read)==6 and  read[0]==0  and crc_chk(read):
		cid = read[2] | (read[3]<<8)
		ver = read[1]
		if cid == 0 and ver == 0:
			print('Error: Check connection to the module!')
			serialPort.close
			sys.exit(10)
		if cid == 0x5562:
			chip = 'TLSR8253'
		else:	
			chip = '?'
		print('ChipID: 0x%04x (%s), Floader ver: %x.%x' % (cid, chip, (ver>>4)&0x0f, ver&0x0f))
	else:
		print('Error get version floader!')
		serialPort.close
		sys.exit(4)
	# print('Get Flash Info...')
	byteSent = serialPort.write(crc_blk(b'\x05\x00\x00\x00'))
	read = serialPort.read(10)
	if len(read)==6 and  read[0]==5 and crc_chk(read):
		jedecid = (read[1]<<16) | (read[2]<<8) | read[3]
		if jedecid == 0xffffff:
			print('Power Off in the module, Reset and Restart!')
			serialPort.close
			sys.exit(5)
		elif jedecid == 0:					
			print('Error: Check connection to the module!')
			serialPort.close
			sys.exit(10)
		else:			
			print('Flash JEDEC ID: %06x, Size: %d kbytes' % (jedecid, ((1<<read[3])>>10)))
	else:
		print('Error get version floader!')
		serialPort.close
		sys.exit(6)
	print('------------------------------------------------')
	if args.operation == 'rf':
		serialPort.timeout = 1024*12/args.baud + 0.004
		print('Read Flash from 0x%06x to 0x%06x...' % (args.address, args.address+args.size))
		print('Outfile: %s' % args.filename)
		try:
			stream = open(args.filename, 'wb')
		except:
			print('Error: Not open Outfile file <%s>!' % args.filename)
			sys.exit(2)
		if not ReadBlockFlash(serialPort, stream, args.address, args.size):
			stream.close
			serialPort.close
			sys.exit(5)
		stream.close
	elif args.operation == 'wf' or args.operation == 'we':
		serialPort.timeout = 0.15
		print('Inputfile: %s' % (args.filename))
		try:
			stream = open(args.filename, 'rb')
			size = os.path.getsize(args.filename)
		except:
			serialPort.close
			print('Error: Not open input file <%s>!' % args.fldr)
			sys.exit(2)
		if size < 1:
			stream.close
			serialPort.close
			print('Error: File size = %d!'% size)
			sys.exit(3)
		offset = args.address & 0x00ffffff
		print('Write Flash data 0x%08x to 0x%08x...' % (offset, offset + size))
		if not WriteBlockFlash(serialPort, stream, offset, size, bool(args.operation == 'we')):
			stream.close
			serialPort.close
			sys.exit(9)
		stream.close
	elif args.operation == 'es':
		serialPort.timeout = 0.15
		count = int((args.size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE)
		size = (count * FLASH_SECTOR_SIZE)
		offset = args.address & (0xffffff^(FLASH_SECTOR_SIZE-1))
		print('Erase Flash %d sectors,\r\ndata from 0x%06x to 0x%06x ...' % (count, offset, offset + size))
		if not EraseSectorsFlash(serialPort,offset,count):
			serialPort.close
			sys.exit(6)
	elif args.operation == 'ea':
		serialPort.timeout = 2.5
		print('Erase All Flash ...')
		if not EraseAllFlash(serialPort):
			serialPort.close
			sys.exit(7)
	serialPort.close
	print('------------------------------------------------')
	if warn == 0:
		print('Done!')
	else:
		print('(%d) Warning' % warn)
	sys.exit(0)

if __name__ == '__main__':
	main()
