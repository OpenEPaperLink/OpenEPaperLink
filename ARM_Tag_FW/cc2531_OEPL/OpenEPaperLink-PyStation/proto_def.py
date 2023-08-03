import ctypes

from c_util import c_pretty

"""
c-structures are copies from AP firmware.
used for serial communication.
"""


@c_pretty
class AvailableDataRequest(ctypes.Structure):
    _fields_ = (
        ('outerChecksum', ctypes.c_uint8),
        ('sourceMac', ctypes.c_uint8 * 8),
        ('innerChecksum', ctypes.c_uint8),
        ('lastPacketLQI', ctypes.c_uint8),
        ('lastPacketRSSI', ctypes.c_int8),
        ('temperature', ctypes.c_int8),
        ('batteryMv', ctypes.c_uint16),
        ('hwType', ctypes.c_uint8),
        ('wakeupReason', ctypes.c_uint8),
    )


@c_pretty
class AvailDataInfo(ctypes.Structure):
    _pack_ = 1
    _fields_ = (
        ('checksum', ctypes.c_uint8),
        ('dataVer', ctypes.c_uint8 * 8),  # MD5 (first half) of potential traffic
        ('dataSize', ctypes.c_uint32),
        ('dataType', ctypes.c_uint8),  # allows for 16 different datatypes
        # extra specification or instruction for the tag (LUT to be used for drawing image)
        ('dataTypeArgument', ctypes.c_uint8),
        ('nextCheckIn', ctypes.c_uint16),  # when should the tag check-in again? Measured in minutes
        ('attemptsLeft', ctypes.c_uint16),
        ('targetMac', ctypes.c_uint8 * 8),
    )


@c_pretty
class BlockRequest(ctypes.Structure):
    _pack_ = 1
    _fields_ = (
        ('checksum', ctypes.c_uint8),
        ('dataVer', ctypes.c_uint8 * 8),  # MD5 (first half) of potential traffic
        ('blockId', ctypes.c_uint8),
        ('srcMac', ctypes.c_uint8 * 8),
    )


@c_pretty
class BlockHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = (
        ('length', ctypes.c_uint16),
        ('checksum', ctypes.c_uint16),
    )


@c_pretty
class XferComplete(ctypes.Structure):
    _pack_ = 1
    _fields_ = (
        ('checksum', ctypes.c_uint8),
        ('srcMac', ctypes.c_uint8 * 8),
    )
