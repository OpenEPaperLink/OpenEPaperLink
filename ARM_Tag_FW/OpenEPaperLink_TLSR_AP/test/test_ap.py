#!/usr/bin/env python3
"""Tests of the TLSR AP logic (src/ap.c, v2link.c, blockcache.c) on a PC.

The firmware is built as a shared library with test/host_hal.c and driven through ctypes. This file
plays the ESP32-S3 (protocol v2, written from docs/design/radio-protocol-v2.md and oepl-radio-v2.h,
independent of the C framing code) and the tags (802.15.4 frames as in proto.h).

    make -C test        # builds and runs
"""
import ctypes
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
lib = ctypes.CDLL(os.path.join(HERE, "build", "libap.so"))
lib.apLoop.restype = ctypes.c_uint32
lib.simTime.restype = ctypes.c_uint32
lib.simBaud.restype = ctypes.c_uint32
lib.simChannel.restype = ctypes.c_uint8
lib.simPower.restype = ctypes.c_int8
lib.simLed.restype = ctypes.c_uint8
lib.simSerialOut.restype = ctypes.c_size_t
lib.cacheBlocks.restype = ctypes.c_uint8
lib.cacheRoomBytes.restype = ctypes.c_uint32
lib.cacheHeldBytes.restype = ctypes.c_uint32
lib.cacheUsedKb.restype = ctypes.c_uint16
lib.cacheComplete.restype = ctypes.c_bool
lib.cacheKb.restype = ctypes.c_uint16
lib.cacheListBytesMax.restype = ctypes.c_uint16
lib.cacheEvictions.restype = ctypes.c_uint16

# --- protocol v2 (from the spec) ---------------------------------------------------------------
HELLO, HEARTBEAT, SET_CONFIG, SET_PENDING, CANCEL_PENDING, DATA_CHUNK, DATA_DROP, GET_STATS = 1, 2, 3, 4, 5, 6, 7, 8
ALLOW_CLEAR, ALLOW_ADD, ALLOW_REMOVE = 9, 10, 11
CHECKIN, DATA_NEEDED, BLOCK_SERVED, XFER_COMPLETE, XFER_TIMEOUT, TAG_RETURN_DATA, LOG = 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46
ACK, NAK, HELLO_REPLY, STATUS = 0x7E, 0x7F, 0x81, 0x82
F_RESPONSE, F_RETRANSMIT = 1, 2
V2_LOG_GAP_MS = 60  # as in v2link.c: the radio spaces its LOG frames
RING_BYTES = 1024  # V2_OUTQ_BYTES in v2link.c: events ([type][len][payload]) and LOG lines (payload + 1)


def crc16(data):
    crc = 0xFFFF
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF if crc & 0x8000 else (crc << 1) & 0xFFFF
    return crc


def cobs_encode(data):
    out, block = bytearray(), bytearray()
    for b in data:
        if b == 0:
            out += bytes([len(block) + 1]) + block
            block = bytearray()
        else:
            block.append(b)
            if len(block) == 254:
                out += b"\xff" + block
                block = bytearray()
    out += bytes([len(block) + 1]) + block
    return bytes(out)


def cobs_decode(data):
    out, i = bytearray(), 0
    while i < len(data):
        code = data[i]
        assert code != 0
        out += data[i + 1:i + code]
        i += code
        if code != 0xFF and i < len(data):
            out.append(0)
    return bytes(out)


def frame(ftype, seq, flags, payload=b""):
    raw = bytes([ftype, seq, flags]) + payload
    c = crc16(raw)
    return cobs_encode(raw + bytes([c & 0xFF, c >> 8])) + b"\x00"


# --- tags (proto.h) ------------------------------------------------------------------------------
PAN = 0x4447
PKT_AVAIL_DATA_SHORTREQ, PKT_AVAIL_DATA_REQ, PKT_AVAIL_DATA_INFO = 0xE3, 0xE5, 0xE6
PKT_BLOCK_REQUEST, PKT_BLOCK_PARTIAL_REQUEST, PKT_BLOCK_REQUEST_ACK, PKT_BLOCK_PART = 0xE4, 0xE7, 0xE9, 0xE8
PKT_XFER_COMPLETE, PKT_XFER_COMPLETE_ACK, PKT_CANCEL_XFER, PKT_PING, PKT_PONG = 0xEA, 0xEB, 0xEC, 0xED, 0xEE
PKT_TAG_RETURN_DATA, PKT_TAG_RETURN_DATA_ACK = 0xE1, 0xE2
AP_MAC = bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88])


def csum(b):
    return bytes([sum(b) & 0xFF]) + b


def bcast(src, ptype, payload=b"", seq=1):
    return struct.pack("<BBBHHH", 0x01, 0xC8, seq, PAN, 0xFFFF, PAN) + src + bytes([ptype]) + payload


def normal(src, dst, ptype, payload=b"", seq=1):
    return struct.pack("<BBBH", 0x41, 0xCC, seq, PAN) + dst + src + bytes([ptype]) + payload


def avail_data_req(src, wakeup=0, hw=0x33):
    adr = struct.pack("<BbbHBBBHBB8s", 200, -60, 21, 2900, hw, wakeup, 0, 0x0027, 11, 0, b"\0" * 8)
    return bcast(src, PKT_AVAIL_DATA_REQ, csum(adr) + b"\0")  # tags send one byte more: 40 bytes


def block_request(src, ver, block_id, parts=None, partial=False):
    mask = bytearray(6)
    for p in (range(42) if parts is None else parts):
        mask[p // 8] |= 1 << (p % 8)
    br = struct.pack("<QBB", ver, block_id, 0) + bytes(mask)
    return normal(src, AP_MAC, PKT_BLOCK_PARTIAL_REQUEST if partial else PKT_BLOCK_REQUEST, csum(br))


def parse_normal(f):
    fcs0, fcs1, seq, pan = struct.unpack_from("<BBBH", f)
    return {"fcs": (fcs0, fcs1), "pan": pan, "dst": f[5:13], "src": f[13:21], "type": f[21], "payload": f[22:]}


# --- fleets ------------------------------------------------------------------------------------
# What a fleet's MACs look like decides what the radio's tag store costs: a record is mac[0], mac[2], mac[3] and a profile index, and the
# profile is the other five bytes. These are the fleets the tests that count bytes use.
# This installation's tags, printed mac[7] first as the S3 does (tag_db.cpp):
SITE_MACS = ["0000021C5A6B341D", "0000032CA1B23E17", "00000335C3D43E1A", "0000BC9FE5F66580"]


def printed(mac):
    """a MAC as the S3 prints it, mac[7] first, as the 8 bytes a frame carries"""
    return bytes.fromhex(mac)[::-1]


# Seven families as this site has (mac[1] and mac[4..7]: 021C, 021D, 032C, 0335 and 0336, BC9F, 032D),
# the serial in mac[2..3] and mac[0] standing in for the check byte: what a site of these tags grows
# into. Twelve bytes a tag in the store, and seven profiles however many tags there are.
SITE_FAMILIES = [(0x34, 0x0000021C), (0x34, 0x0000021D), (0x3E, 0x0000032C), (0x3E, 0x00000335),
                 (0x3E, 0x00000336), (0x65, 0x0000BC9F), (0x3F, 0x0000032D)]


def site_mac(i):
    b1, hi = SITE_FAMILIES[i % len(SITE_FAMILIES)]
    serial = 0x0100 + i
    return bytes([(serial * 37 + 11) & 0xFF, b1, serial & 0xFF, (serial >> 8) & 0xFF]) + struct.pack("<I", hi)


def stranger(i):
    """a tag sharing nothing with any other but its three key bytes, which are the SAME for all of
    them: a profile each - 20 bytes a tag, the widest the store has - and past 256 of them two tags
    whose key words are equal, told apart only by the profile index's high byte (profHi)"""
    return bytes([0xC1, 0xA0, 0xB0, 0x00]) + struct.pack("<I", 0x5000 + i)


# --- the firmware -----------------------------------------------------------------------------
class Sim:
    def __init__(self):
        lib.simSetTime(1000)
        lib.apInit()
        self.seq = 0x40
        self.events = []
        self.out = bytearray()

    def time(self):
        return lib.simTime()

    def advance(self, ms, step=1):
        end = self.time() + ms
        while self.time() < end:
            self.loop()
            lib.simSetTime(min(self.time() + step, end))
        self.loop()

    def idle(self, ms):
        """let time pass as with a real S3: a HEARTBEAT every second (events stay queued)"""
        end = self.time() + ms
        while self.time() < end:
            lib.simSetTime(min(self.time() + 1000, end))
            self.loop()
            self.request(HEARTBEAT, expect=STATUS)

    def drain(self):
        """run the loop until a block burst is complete (one part per pass)"""
        self.loop(120)

    def loop(self, n=3):
        for _ in range(n):
            lib.apLoop()
        self._collect()
        # Frames are built in the radio's TX buffer: nothing may write a frame that is on its way out
        # (host_hal.c compares it with what was sent). Checked after every pass of every test.
        assert lib.simTxCorruptions() == 0, "a frame on the air was written into"

    def _collect(self):
        buf = ctypes.create_string_buffer(65536)
        n = lib.simSerialOut(buf, 65536)
        self.out += buf.raw[:n]

    def serial_in(self, data):
        lib.simSerialIn(data, len(data))
        self.loop()

    def ascii(self):
        self._collect()
        s = bytes(self.out).decode("latin1")
        self.out.clear()
        return s

    def frames(self):
        """the complete v2 frames sent by the radio, decoded (type, seq, flags, payload)"""
        self._collect()
        res = []
        while b"\x00" in self.out:
            i = self.out.index(b"\x00")
            enc, self.out = bytes(self.out[:i]), self.out[i + 1:]
            if not enc:
                continue
            raw = cobs_decode(enc)
            body, crc = raw[:-2], raw[-2] | raw[-1] << 8
            assert crc16(body) == crc, "CRC of a frame from the radio"
            res.append((body[0], body[1], body[2], body[3:]))
        return res

    def request(self, ftype, payload=b"", seq=None, flags=0, expect=None):
        """send a request, return its response; queued events are stored and acknowledged"""
        if seq is None:
            self.seq = (self.seq + 1) & 0xFF
            seq = self.seq
        self.serial_in(frame(ftype, seq, flags, payload))
        resp = None
        for f in self.frames():
            if f[2] & F_RESPONSE and f[1] == seq and resp is None:
                resp = f
            elif not f[2] & F_RESPONSE:
                self.events.append(f)
        assert resp is not None, "no response to %02X" % ftype
        if expect is not None:
            assert resp[0] == expect, "response %02X to %02X, expected %02X (%s)" % (resp[0], ftype, expect, resp[3].hex())
        return resp

    def pump_events(self, ack=True, ms=0):
        """collect events from the radio, acknowledging each (stop-and-wait)"""
        got = []
        for _ in range(200):
            self.loop()
            new = [f for f in self.frames() if not f[2] & F_RESPONSE] + self.events
            self.events = []
            if not new:
                if ms <= 0:
                    break
                lib.simSetTime(self.time() + 1)
                ms -= 1
                continue
            for f in new:
                # a retransmission of an event we have (same seq): the S3 acks it again, uses it once
                if not any(g[1] == f[1] and g[0] == f[0] for g in got[-1:]):
                    got.append(f)
                if ack and f[0] != LOG:
                    ack_frame = frame(ACK, f[1], F_RESPONSE)
                    lib.simSerialIn(ack_frame, len(ack_frame))
        return got

    def radio_in(self, f, age_us=500, rssi=-50):
        lib.simRadioIn(f, len(f), rssi, age_us)
        self.loop()

    def radio_out(self):
        res = []
        buf = ctypes.create_string_buffer(256)
        while lib.simRadioOut(buf):
            n = buf.raw[0]
            res.append(buf.raw[1:1 + n])
        return res


def start_v2(sim):
    sim.ascii()  # the boot banner (RES>RDY>), or what came before
    sim.serial_in(b"RDY?")
    assert sim.ascii() == "ACK>"
    sim.serial_in(b"FRM1")
    assert sim.ascii() == "ACK>"
    assert lib.simBaud() == 2000000
    r = sim.request(HELLO, struct.pack("<BBH", 2, 1, 0x4B), expect=HELLO_REPLY)
    return r


# --- tests --------------------------------------------------------------------------------------
TESTS = []


def test(fn):
    TESTS.append(fn)
    return fn


def test_built_with(so):
    """a test that needs a build other than the default one the suite runs against"""
    def deco(fn):
        fn.lib_so = so
        TESTS.append(fn)
        return fn
    return deco


@test
def boot_and_ascii():
    sim = Sim()
    assert sim.ascii() == "RES>RDY>"
    sim.serial_in(b"RDY?")
    assert sim.ascii() == "ACK>"
    sim.serial_in(b"NFO?")
    s = sim.ascii()
    assert s == "ACK>TYP>F8VER>0080MAC>1122334455667788ZCH>0BZPW>0APEN>00NOP>00CAP>006B", s
    assert lib.simBaud() == 115200


@test
def hello_and_status():
    sim = Sim()
    sim.ascii()
    r = start_v2(sim)
    major, minor, fw, hw, mac, caps, maxp, cache_kb, slots = struct.unpack("<BBHB8sHHHH", r[3])
    assert (major, minor, fw, hw) == (2, 1, 0x0080, 0xF8)
    assert mac == AP_MAC and caps == 0x6B and maxp == 256 and slots == 2048  # pendMax, derived
    # six full blocks (6 * 4120 = 24720) and, in the 4040 beside them, one of 4016 bytes: 28592
    assert cache_kb == 27, cache_kb
    r = sim.request(HEARTBEAT, expect=STATUS)
    st = struct.unpack("<IBBbBHHHIIHHHHB", r[3])
    assert len(r[3]) == 31
    assert st[1] == 11 and st[2] == 0xFF and st[3] == 10
    # a wrong major version is refused
    r = sim.request(HELLO, struct.pack("<BBH", 3, 0, 0), expect=NAK)
    assert r[3] == bytes([3])
    # unknown type
    r = sim.request(0x33, expect=NAK)
    assert r[3] == bytes([1])


@test
def set_config():
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 15, 0xFF, 10, 0, 128), expect=ACK)
    assert lib.simChannel() == 15 and lib.simPower() == 10 and lib.simLed() == 128
    r = sim.request(SET_CONFIG, struct.pack("<BBbBB", 12, 0xFF, 0x7F, 0, 0), expect=NAK)
    assert r[3] == bytes([3]) and lib.simChannel() == 15
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 0, 0), expect=ACK)
    assert lib.simChannel() == 15 and lib.simPower() == 10
    r = sim.request(SET_CONFIG, b"\x0b", expect=NAK)
    assert r[3] == bytes([2])
    # one line when a setting really moves, none when the S3 re-sends the same one
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 20, 0xFF, 8, 1, 64), expect=ACK)
    logs = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
    # and the arena's maxima with it, all derived at boot from the arena this build has: six blocks;
    # the list's cap guaranteed at 20 bytes a tag in the 24640 above the floor; a pending entry for
    # as many records as that holds, (24640 - 32) / 12 in whole granules of 16; the offers beside the
    # list at its guarantee, (24640 - 1028 * 8 - 1040 * 12) / 16 in fours; and no profiles yet
    assert logs == ["cfg ch 20 pwr 8 lock 0 led 64 ckb 27 tags 1024 pend 2048 ofr 244 prof 0 "], logs
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 20, 0xFF, 8, 1, 64), expect=ACK)
    assert [l for l in sim.pump_events() if l[0] == LOG] == []
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 20, 0xFF, 8, 5, 64), expect=ACK)  # lock on
    sim.advance(V2_LOG_GAP_MS)  # LOG frames are spaced, so let the previous one's gap pass
    logs = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
    assert logs == ["cfg ch 20 pwr 8 lock 2 led 64 ckb 27 tags 1024 pend 2048 ofr 244 prof 0 "], logs


@test
def duplicate_and_retransmit():
    sim = Sim()
    start_v2(sim)
    mac = bytes(range(1, 9))
    sp = mac + struct.pack("<QIBBHHB", 0x1234, 100, 0, 0, 5, 3, 0)
    r1 = sim.request(SET_PENDING, sp, seq=0x90, expect=ACK)
    # the same seq again (our ACK got lost): the stored reply, not handled twice
    r2 = sim.request(SET_PENDING, mac + struct.pack("<QIBBHHB", 0x9999, 100, 0, 0, 5, 3, 0), seq=0x90, flags=F_RETRANSMIT, expect=ACK)
    sim.request(GET_STATS, expect=STATUS)
    tag = bytes(range(1, 9))
    sim.radio_in(avail_data_req(tag))
    reply = parse_normal(sim.radio_out()[0])
    ver = struct.unpack_from("<Q", reply["payload"], 1)[0]
    assert ver == 0x1234, hex(ver)


@test
def older_duplicate_is_not_applied_again():
    sim = Sim()
    start_v2(sim)
    mac = bytes([0x30, 1, 2, 3, 4, 5, 6, 7])

    def pending(ver):
        return mac + struct.pack("<QIBBHHB", ver, 4096, 0x20, 0, 0, 10, 0)

    def offered():
        """the dataVer the radio offers this tag at a check-in, None when it has nothing"""
        sim.radio_in(avail_data_req(mac))
        adi = parse_normal(sim.radio_out()[0])["payload"]
        sim.pump_events()
        return None if adi[13] == 0 else struct.unpack_from("<Q", adi, 1)[0]

    # The S3 keeps up to V2_CHUNK_WINDOW requests unanswered, so the ack to one of them can be lost
    # while later ones get through, and the retransmission is then no longer the newest seq we saw.
    sim.request(SET_PENDING, pending(0xAAA), seq=0x10, expect=ACK)
    sim.request(CANCEL_PENDING, mac, seq=0x11, expect=ACK)
    sim.request(SET_PENDING, pending(0xAAA), seq=0x10, flags=F_RETRANSMIT, expect=ACK)
    assert offered() is None, "a retransmitted SET_PENDING brought cancelled data back"

    # the same race with the entry replaced rather than cancelled
    sim.request(SET_PENDING, pending(0xBBB), seq=0x12, expect=ACK)
    sim.request(SET_PENDING, pending(0xCCC), seq=0x13, expect=ACK)
    sim.request(SET_PENDING, pending(0xBBB), seq=0x12, flags=F_RETRANSMIT, expect=ACK)
    assert offered() == 0xCCC, "an older SET_PENDING overwrote the newer one"

    # Sequence numbers are a byte. Walk the whole space, so that 0x12 comes round again as a request
    # the S3 really means, and check it is handled rather than taken for the duplicate it is not.
    for s in list(range(0x14, 0x100)) + list(range(0x00, 0x12)):
        sim.request(HEARTBEAT, seq=s, expect=STATUS)
    sim.request(SET_PENDING, pending(0xDDD), seq=0x12, expect=ACK)
    assert offered() == 0xDDD, "a new request whose seq had wrapped was mistaken for a duplicate"


@test
def hello_starts_a_fresh_duplicate_window():
    sim = Sim()
    start_v2(sim)
    mac = bytes([0x31, 1, 2, 3, 4, 5, 6, 7])

    def pending(ver):
        return mac + struct.pack("<QIBBHHB", ver, 4096, 0x20, 0, 0, 10, 0)

    def offered():
        sim.radio_in(avail_data_req(mac))
        adi = parse_normal(sim.radio_out()[0])["payload"]
        sim.pump_events()
        return None if adi[13] == 0 else struct.unpack_from("<Q", adi, 1)[0]

    # A HELLO restarts the S3's sequence numbers, so a request after it may carry a seq the radio
    # handled on the old link. It is a new request, not a duplicate of that one.
    sim.request(SET_PENDING, pending(0xAAA), seq=0x01, expect=ACK)
    sim.request(HELLO, struct.pack("<BBH", 2, 1, 0x4B), seq=0x00, expect=HELLO_REPLY)
    sim.request(SET_PENDING, pending(0xBBB), seq=0x01, expect=ACK)
    assert offered() == 0xBBB, "a request after HELLO was taken for a duplicate from the old link"


@test
def checkin_without_data():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0xA0, 1, 2, 3, 4, 5, 6, 7])
    sim.radio_in(avail_data_req(tag))
    out = sim.radio_out()
    assert len(out) == 1
    f = parse_normal(out[0])
    assert f["type"] == PKT_AVAIL_DATA_INFO and f["dst"] == tag and f["src"] == AP_MAC and f["pan"] == PAN
    assert f["fcs"] == (0x41, 0xCC)
    adi = f["payload"]
    assert len(adi) == 17 and sum(adi[1:]) & 0xFF == adi[0]
    assert adi[13] == 0  # DATATYPE_NOUPDATE
    ev = sim.pump_events()
    assert len(ev) == 1 and ev[0][0] == CHECKIN
    ci = ev[0][3]
    assert len(ci) == 49 and ci[:8] == tag and ci[18] == 0 and ci[27] == 0 and ci[16:18] == b"\x47\x44"
    assert struct.unpack_from("<b", ci, 12)[0] == -50
    # the S3 ACKed, nothing more comes
    sim.advance(100)
    assert sim.pump_events() == []
    # short request
    sim.radio_in(bcast(tag, PKT_AVAIL_DATA_SHORTREQ))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_AVAIL_DATA_INFO
    ev = sim.pump_events()
    assert ev[0][0] == CHECKIN and ev[0][3][18] == 1  # kind SHORT
    # old 28-byte request
    adr_old = struct.pack("<BbbHBBB", 200, -60, 21, 2900, 0x33, 0, 0)
    sim.radio_in(bcast(tag, PKT_AVAIL_DATA_REQ, csum(adr_old) + b"\0"))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_AVAIL_DATA_INFO


def full_blocks():
    """Full 4096-byte blocks the cache has room for beside the tag store: its arena now, at the 4120
    bytes a full block costs (data, the tag's 4-byte header and the cache's 20). The blocks themselves
    are stored at their own length, so this is the cache's capacity in the unit the fixed regions had,
    which is what the arena tests are about - not what it holds (lib.cacheBlocks())."""
    return lib.cacheRoomBytes() // 4120


def make_file(size, seed=7):
    return bytes((i * 31 + seed + (i >> 8)) & 0xFF for i in range(size))


def send_block(sim, ver, data, block, expect_nak=None):
    total = len(data)
    start = block * 4096
    blk = data[start:start + 4096]
    for off in range(0, len(blk), 240):
        r = sim.request(DATA_CHUNK, struct.pack("<QII", ver, start + off, total) + blk[off:off + 240])
        if expect_nak is not None:
            assert r[0] == NAK and r[3][0] == expect_nak
            return
        assert r[0] == ACK, (r, off)


# Mirrors the burst budget in ap.c: a block part is 126 bytes on air plus 6 bytes of preamble, SFD
# and length byte at 32 us per byte, and a burst may fill BURST_WINDOW_MS of the tag's window.
BURST_COPIES = 2
BURST_FRAME_US = (126 + 6) * 32 + 120  # + the radio's TX settling time before each frame
RADIO_BURST_GAP_US = 600
BURST_WINDOW_MS = 260
BURST_MAX_FRAMES = (BURST_WINDOW_MS * 1000) // (BURST_FRAME_US + RADIO_BURST_GAP_US)


def check_schedule(seen, parts):
    """Every requested part goes out at least once and at most BURST_COPIES times, the burst is as long
    as the window allows, and the copies of a part are an odd number of frames apart whenever the
    number of requested parts is even. That last one matters because a tag that is still copying one
    part when the next arrives misses every second frame; with the copies an even distance apart it
    would miss the same parts twice."""
    counts = {p: seen.count(p) for p in parts}
    assert len(seen) == min(len(parts) * BURST_COPIES, BURST_MAX_FRAMES), (len(seen), len(parts))
    assert min(counts.values()) >= 1, counts
    assert max(counts.values()) <= BURST_COPIES, counts
    if len(parts) % 2 == 0:
        for part in parts:
            at = [i for i, p in enumerate(seen) if p == part]
            for a, b in zip(at, at[1:]):
                assert (b - a) % 2 == 1, ("part %d goes out at %s" % (part, at), len(parts))


def check_burst(frames, tag, data, block, parts):
    blk = data[block * 4096:block * 4096 + 4096]
    hdr = struct.pack("<HH", len(blk), sum(blk) & 0xFFFF)
    padded = hdr + blk + b"\xff" * (42 * 99 - 4 - len(blk))
    seen = []
    for raw in frames:
        f = parse_normal(raw)
        assert f["type"] == PKT_BLOCK_PART and f["dst"] == tag
        bp = f["payload"]
        assert len(bp) == 102 and sum(bp[1:]) & 0xFF == bp[0]
        assert bp[1] == block
        part = bp[2]
        assert bp[3:] == padded[part * 99:part * 99 + 99], "part %d data" % part
        seen.append(part)
    assert set(seen) == set(parts), (sorted(set(seen)), parts)
    return seen


@test
def transfer_preloaded():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0xB0, 1, 2, 3, 4, 5, 6, 7])
    data = make_file(6000)
    ver = 0x0102030405060708
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 1, 0, 10, 1), expect=ACK)
    send_block(sim, ver, data, 0)
    send_block(sim, ver, data, 1)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    # one pending entry, and 6 KB in the cache: 6000 bytes of blocks at their own length (4120 + 1928
    # of arena), where two fixed regions were 8
    assert st[5] == 1 and st[7] == 6, st
    # check-in: the radio offers the data
    sim.radio_in(avail_data_req(tag))
    adi = parse_normal(sim.radio_out()[0])["payload"]
    assert struct.unpack_from("<QIBBH", adi, 1) == (ver, 6000, 0x20, 1, 0)
    ev = sim.pump_events()
    assert ev[0][0] == CHECKIN and struct.unpack_from("<Q", ev[0][3], 19)[0] == ver and ev[0][3][27] == 0x20
    # block 0, all parts: cached, so a wait of 0, the burst 3 ms after that
    t0 = sim.time()
    sim.radio_in(block_request(tag, ver, 0))
    ack = parse_normal(sim.radio_out()[0])
    assert ack["type"] == PKT_BLOCK_REQUEST_ACK and struct.unpack_from("<H", ack["payload"], 1)[0] == 0
    sim.advance(2)
    assert sim.radio_out() == [], "no burst before the hold is over"
    sim.advance(2)
    sim.drain()
    parts = sim.radio_out()
    # All 42 parts, and repeats of as many as the listening window still has room for: not the single
    # copy each that the old cap of 42 frames per burst gave a full block.
    assert len(parts) == BURST_MAX_FRAMES, len(parts)
    seen = check_burst(parts, tag, data, 0, range(42))
    check_schedule(seen, list(range(42)))
    ev = sim.pump_events()
    assert [e[0] for e in ev] == [BLOCK_SERVED], ev
    mac, bver, bid, preq, fsent, late = struct.unpack("<8sQBBBh", ev[0][3])
    # late_ms is how long after the tag's wait the burst began: the 3 ms hold
    assert (mac, bver, bid, preq, fsent) == (tag, ver, 0, 42, BURST_MAX_FRAMES) and 3 <= late <= 5, late
    # block 1, a partial request of 3 parts: each twice, back to back with no tag's answer to wait for
    sim.radio_in(block_request(tag, ver, 1, parts=[0, 5, 19], partial=True))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
    sim.advance(60)
    sim.drain()
    seen = check_burst(sim.radio_out(), tag, data, 1, [0, 5, 19])
    assert sorted(seen) == [0, 0, 5, 5, 19, 19]
    check_schedule(seen, [0, 5, 19])
    sim.pump_events()
    # an even number of requested parts: the copies still land an odd number of frames apart
    sim.radio_in(block_request(tag, ver, 1, parts=[0, 1, 2, 3], partial=True))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
    sim.advance(300)
    sim.drain()
    seen = check_burst(sim.radio_out(), tag, data, 1, [0, 1, 2, 3])
    check_schedule(seen, [0, 1, 2, 3])
    sim.pump_events()
    # done: XFER_COMPLETE once, ack on air each time
    sim.radio_in(normal(tag, AP_MAC, PKT_XFER_COMPLETE))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_XFER_COMPLETE_ACK
    ev = sim.pump_events()
    assert [e[0] for e in ev] == [XFER_COMPLETE] and ev[0][3] == tag + struct.pack("<Q", ver)
    sim.radio_in(normal(tag, AP_MAC, PKT_XFER_COMPLETE))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_XFER_COMPLETE_ACK
    assert sim.pump_events() == []
    # the entry and its cached blocks are gone
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[5] == 0 and st[7] == 0, st
    sim.radio_in(avail_data_req(tag))
    assert parse_normal(sim.radio_out()[0])["payload"][13] == 0


@test
def transfer_data_needed():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0xC0, 1, 2, 3, 4, 5, 6, 7])
    data = make_file(5000, seed=3)
    ver = 0xAABBCCDD00112233
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x21, 0, 0, 10, 0), expect=ACK)
    sim.radio_in(block_request(tag, ver, 1))
    ack = parse_normal(sim.radio_out()[0])
    assert struct.unpack_from("<H", ack["payload"], 1)[0] == 140
    ev = sim.pump_events()
    assert ev[0][0] == DATA_NEEDED
    mac, dver, off, length, wait = struct.unpack("<8sQIIH", ev[0][3])
    assert (mac, dver, off, length, wait) == (tag, ver, 4096, 5000 - 4096, 140)
    # the same block again before the data is there: "coming", 30 ms, no second DATA_NEEDED
    sim.advance(10)
    sim.radio_in(block_request(tag, ver, 1))
    assert struct.unpack_from("<H", parse_normal(sim.radio_out()[0])["payload"], 1)[0] == 30
    assert sim.pump_events() == []
    send_block(sim, ver, data, 1)
    sim.advance(40)
    sim.drain()
    check_burst(sim.radio_out(), tag, data, 1, range(42))
    assert [e[0] for e in sim.pump_events()] == [BLOCK_SERVED]


@test
def every_tag_gets_the_same_burst_timing():
    """How a tag spends a wait depends on its chip and on its firmware version, and hwType says nothing
    about the version, so the radio does not ask. Whatever a tag reports: from the cache a wait of 0 and
    the burst 3 ms after the ack, and from the S3 the learned wait and the burst 5 ms after it."""
    sim = Sim()
    start_v2(sim)
    data = make_file(3000)
    want_wait, want_burst_ms, want_margin = 0, 3, 3
    for mac0, hw in ((0xA4, 0x04), (0xA9, 0x27), (0xA7, 0x07), (0xA5, 0x33), (0xAA, 0x28), (0xAB, 0x2D),
                     (0xA8, 0x05), (0xA6, None)):
        tag = bytes([mac0] + [1] * 7)
        ver = 0x700 + tag[0]
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, ver, data, 0)
        if hw is not None:
            sim.radio_in(avail_data_req(tag, hw=hw))
            sim.radio_out()
        sim.pump_events()
        sim.advance(1300)  # past the "busy with another tag" window of the previous transfer
        sim.radio_out()
        sim.radio_in(block_request(tag, ver, 0))
        ack = parse_normal(sim.radio_out()[0])
        assert struct.unpack_from("<H", ack["payload"], 1)[0] == want_wait, (hw, ack["payload"].hex())
        sim.advance(want_burst_ms - 1)
        assert sim.radio_out() == [], hw
        sim.advance(1)
        sim.drain()
        check_burst(sim.radio_out(), tag, data, 0, range(42))
        ev = [e for e in sim.pump_events() if e[0] == BLOCK_SERVED]
        # late_ms: how long after the tag's wait the burst began: the hold
        assert ev and want_margin <= struct.unpack_from("<h", ev[0][3], 19)[0] <= want_margin + 1, ev
    # and the block the S3 has to send: the same for a tag that once wrote a pw - 25 and one that sleeps it
    for mac0, hw in ((0xAC, 0x04), (0xAD, 0x33)):
        tag = bytes([mac0] + [1] * 7)
        ver = 0x700 + mac0
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
        sim.radio_in(avail_data_req(tag, hw=hw))
        sim.radio_out()
        sim.pump_events()
        sim.advance(1300)
        sim.radio_out()
        sim.radio_in(block_request(tag, ver, 0))
        ack = parse_normal(sim.radio_out()[0])
        assert struct.unpack_from("<H", ack["payload"], 1)[0] == 140, hw
        send_block(sim, ver, data, 0)
        sim.advance(140)
        assert sim.radio_out() == [], hw
        sim.advance(8)
        sim.drain()
        check_burst(sim.radio_out(), tag, data, 0, range(42))
        ev = [e for e in sim.pump_events() if e[0] == BLOCK_SERVED]
        assert ev and 5 <= struct.unpack_from("<h", ev[0][3], 19)[0] <= 6, ev


@test
def download_wait_only_ever_grows():
    """The ack has to name a wait before the radio knows how long the S3 will take. It starts at the
    old constant and rises when this S3 proves slower, and never falls: the tag sleeps through the
    wait, so there is nothing to win by shortening it, while underestimating costs it a whole extra
    request round with its radio on."""
    sim = Sim()
    start_v2(sim)
    data = make_file(3000)
    ver = [0x900]

    def uncached_wait(took_ms, deliver=True):
        """one uncached block request; returns the pleaseWaitMs offered, then optionally delivers"""
        ver[0] += 1
        tag = bytes([0xE0, 1, 2, 3, 4, 5, 6, ver[0] & 0xFF])
        sim.advance(1300)  # past the busy-with-another-tag window
        sim.radio_out()
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver[0], len(data), 0x21, 0, 0, 10, 0), expect=ACK)
        sim.radio_in(block_request(tag, ver[0], 0))
        ack = parse_normal(sim.radio_out()[0])
        assert ack["type"] == PKT_BLOCK_REQUEST_ACK, ack["type"]
        offered = struct.unpack_from("<H", ack["payload"], 1)[0]
        sim.advance(took_ms)
        if deliver:
            send_block(sim, ver[0], data, 0)
        sim.advance(500)
        sim.drain()
        sim.radio_out()
        sim.pump_events()
        return offered

    # nothing measured yet: the constant the C6 used
    assert uncached_wait(5) == 140
    # a quick answer must NOT shorten it
    assert uncached_wait(5) == 140
    # an answer slower than promised raises it above what that block took
    assert uncached_wait(200) == 140
    raised = uncached_wait(5)
    # by the margin over the slowest answer seen
    assert 220 <= raised <= 221, raised
    # a block that never arrives backs it off further, and it stays in range
    before = raised
    after = uncached_wait(5, deliver=False)
    assert after == before, (before, after)   # the miss is seen after this ack went out
    grown = uncached_wait(5)
    assert grown > before and grown <= 550, (before, grown)


@test
def burst_repeats_follow_what_the_fleet_can_afford():
    """One copy of each requested part is not discretionary; the repeats are, and they are paid for by
    every other tag whose request goes out while the radio transmits. So the count weighs the two
    costs the radio can measure, in milliseconds of a tag's radio: other tags' attempts the bursts are
    estimated to have gone over (their frames heard, scaled by how much of the window the radio spent
    transmitting) or heard too late, at a check-in window each, against tags coming back for parts, at
    a whole receive window each. A window that shows neither changes nothing - on a quiet installation
    a loop that read silence as permission would grow without bound exactly where it is least wanted.
    And nothing but the window caps it: three was a guess about someone else's installation."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)  # LOG frames on
    data = make_file(3000)
    ver = [0x700]

    def window():
        """close a reporting window and return its repeat count and its `air` line"""
        sim.pump_events()  # a LOG frame waits behind any event that has not been acked yet
        sim.idle(16000)
        logs = [l[3][1:].decode("utf-8", "replace") for l in sim.pump_events(ms=500) if l[0] == LOG]
        xfer = [l for l in logs if l.startswith("xfer brq ")]
        assert xfer, logs
        fields = xfer[-1].split()
        return int(fields[fields.index("cpy") + 1])

    def window_copies():
        return window()

    def quiet_checkin(i=0):
        sim.radio_in(avail_data_req(bytes([0xF0, i & 0xFF] + [3] * 6)))
        sim.radio_out()

    def late_checkin():
        # older than the limit for a check-in, so it is dropped instead of answered
        sim.radio_in(avail_data_req(bytes([0xF0] + [3] * 7)), age_us=9000)
        sim.radio_out()

    def transfer(rerequest=True):
        """a burst, then (or not) the tag asking again for part of the same block"""
        ver[0] += 1
        tag = bytes([0xF2, 3, 3, 3, 3, 3, 3, ver[0] & 0xFF])
        sim.advance(1300)
        sim.radio_out()
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver[0], len(data), 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, ver[0], data, 0)
        sim.radio_in(block_request(tag, ver[0], 0))
        sim.radio_out()
        sim.advance(400)
        sim.drain()
        sim.radio_out()
        if rerequest:
            # the same block again, missing two parts: that is a re-request
            sim.radio_in(block_request(tag, ver[0], 0, parts=[1, 4], partial=True))
            sim.radio_out()
            sim.advance(400)
            sim.drain()
            sim.radio_out()
        sim.pump_events()

    # activity, but nothing dropped, nobody asking again and no burst to go over anyone: unchanged
    quiet_checkin()
    assert window_copies() == 2
    quiet_checkin()
    assert window_copies() == 2, "a quiet window must not grow the budget"

    # a request we had to drop is an attempt we cost a tag, and nothing was bought with it
    late_checkin()
    assert window_copies() == 1, "a dropped request must shrink the budget"
    # ... and it does not keep falling below the one copy the tag actually asked for
    late_checkin()
    assert window_copies() == 1

    # a tag asking again says the repeats are worth their airtime
    transfer()
    assert window_copies() == 2
    transfer()
    assert window_copies() == 3
    transfer()
    assert window_copies() == 4, "the window is the ceiling, not three"

    # a tag coming back for parts costs it a whole receive window; one late attempt costs another tag
    # a check-in window. The re-request outweighs it.
    transfer()
    late_checkin()
    assert window_copies() == 5, "one late attempt must not outweigh a re-request"

    # a burst on air while other tags are plainly busy - heard once every few milliseconds of
    # listening - has gone over some of their attempts, and nobody came back for parts: it shrinks
    transfer(rerequest=False)
    for i in range(80):
        quiet_checkin(i)
    cpy = window()
    assert cpy == 4, cpy
    # ... and the air line says what it was weighed on (it may wait a report for the boot facts or a
    # high-water line, and counts from one air line to the next, so it covers this window too)
    air = air_line(sim)
    assert air["btx"] > 0 and air["oth"] >= 80 and air["hrm"] >= 1, air


@test
def data_never_comes():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0xC1, 1, 2, 3, 4, 5, 6, 7])
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 77, 3000, 0x21, 0, 0, 10, 0), expect=ACK)
    sim.radio_in(block_request(tag, 77, 0))
    sim.radio_out()
    sim.pump_events()
    sim.advance(400)
    assert sim.radio_out() == [], "nothing sent without data"


@test
def busy_with_other_tag_and_unknown_tag():
    sim = Sim()
    start_v2(sim)
    a, b = bytes([0xD0] + [1] * 7), bytes([0xD1] + [1] * 7)
    data = make_file(2000)
    sim.request(SET_PENDING, a + struct.pack("<QIBBHHB", 5, 2000, 0x20, 0, 0, 10, 0), expect=ACK)
    send_block(sim, 5, data, 0)
    sim.radio_in(block_request(a, 5, 0))
    sim.radio_out()
    sim.advance(5)
    sim.radio_out()  # a's burst has started
    sim.radio_in(block_request(b, 6, 0))
    out = [parse_normal(f) for f in sim.radio_out()]
    assert out and out[0]["type"] == PKT_CANCEL_XFER and out[0]["dst"] == b
    sim.advance(1300)
    sim.radio_out()
    sim.radio_in(block_request(b, 6, 0))  # no pending data for b
    out = [parse_normal(f) for f in sim.radio_out()]
    assert out[0]["type"] == PKT_CANCEL_XFER


@test
def late_requests_and_ping():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0xE0] + [2] * 7)
    sim.radio_in(avail_data_req(tag), age_us=6000)
    assert sim.radio_out() == []
    sim.radio_in(bcast(tag, PKT_PING))
    pong = parse_normal(sim.radio_out()[0])
    assert pong["type"] == PKT_PONG and pong["payload"] == bytes([11])
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[10] == 1, st  # late requests skipped
    # and the once-a-minute warning says so in full, not cut off mid-word
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)  # LOG frames on
    sim.idle(61000)
    logs = [l[3][1:].decode() for l in sim.pump_events(ms=500) if l[0] == LOG]
    assert "requests skipped in the last minute, too late to answer: 1" in logs, logs


@test
def inventory_lock():
    sim = Sim()
    start_v2(sim)
    known, other = bytes([0xF0] + [3] * 7), bytes([0xF1] + [3] * 7)
    sim.request(ALLOW_CLEAR, expect=ACK)
    sim.request(ALLOW_ADD, known, expect=ACK)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1 << 1, 0), expect=ACK)
    sim.radio_in(avail_data_req(other))
    sim.radio_in(bcast(other, PKT_PING))
    assert sim.radio_out() == []
    sim.radio_in(avail_data_req(known))
    assert len(sim.radio_out()) == 1
    # learning: a booting unknown tag gets an answer, a normal check-in does not
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 2 << 1, 0), expect=ACK)
    sim.radio_in(avail_data_req(other))
    assert sim.radio_out() == []
    sim.radio_in(avail_data_req(other, wakeup=0xFC))
    assert len(sim.radio_out()) == 1
    sim.request(ALLOW_REMOVE, known, expect=ACK)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1 << 1, 0), expect=ACK)
    sim.radio_in(avail_data_req(known))
    assert sim.radio_out() == []
    sim.request(ALLOW_ADD, b"\x01" * 7, expect=NAK)
    # the list holds 1024
    sim.request(ALLOW_CLEAR, expect=ACK)
    for chunk in range(0, 1024, 30):
        macs = b"".join(struct.pack("<Q", 0x1000 + i) for i in range(chunk, min(chunk + 30, 1024)))
        sim.request(ALLOW_ADD, macs, expect=ACK)
    r = sim.request(ALLOW_ADD, struct.pack("<Q", 0x9999), expect=NAK)
    assert r[3] == bytes([4])


@test
def event_retransmit_and_queue():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x99] + [4] * 7)
    sim.radio_in(avail_data_req(tag))
    sim.radio_out()
    first = [f for f in sim.frames() if not f[2] & F_RESPONSE]
    assert len(first) == 1 and first[0][0] == CHECKIN and not first[0][2] & F_RETRANSMIT
    sim.advance(31)
    again = sim.frames()
    assert len(again) == 1 and again[0][1] == first[0][1] and again[0][2] & F_RETRANSMIT
    sim.advance(200)  # 3 retries, then the link is down: nothing more
    assert len(sim.frames()) == 2
    sim.advance(100)
    assert sim.frames() == []
    # a frame from the S3 brings it back; the event is still there
    sim.request(GET_STATS, expect=STATUS)
    ev = sim.pump_events()
    assert ev and ev[0][0] == CHECKIN
    # 40 check-ins while the S3 does not answer: the ring keeps what fits in its 1024 bytes, a CHECKIN
    # costing 51 of them, so 20 (V2_OUTQ_BYTES in v2link.c; LOG lines are off here, so nothing else)
    for i in range(40):
        sim.radio_in(avail_data_req(bytes([0x50, i] + [0] * 6)))
        sim.radio_out()
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(GET_STATS, expect=STATUS)[3])
    assert st[14] == RING_BYTES // 51, st
    ev = sim.pump_events()
    assert len(ev) == RING_BYTES // 51
    # the one that was on the line, then the newest: the drops were the oldest of those waiting
    assert [e[3][1] for e in ev] == [0] + list(range(40 - len(ev) + 1, 40)), [e[3][1] for e in ev]


@test
def housekeeping_timeout():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x77] + [5] * 7)
    idle = bytes([0x78] + [5] * 7)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 42, 100, 0x20, 0, 30, 2, 0), expect=ACK)
    sim.request(SET_PENDING, idle + struct.pack("<QIBBHHB", 0, 0, 0, 0, 0x8005, 1, 0), expect=ACK)
    sim.idle(61000)  # one housekeeping round: ttl 2 -> 1, the idle entry (ttl 1) goes quietly
    assert [e for e in sim.pump_events() if e[0] == XFER_TIMEOUT] == []
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert (st[5], st[6]) == (1, 0), st
    sim.idle(60000)
    ev = [e for e in sim.pump_events() if e[0] in (XFER_TIMEOUT, XFER_COMPLETE)]
    assert [e[0] for e in ev] == [XFER_TIMEOUT] and ev[0][3] == tag + struct.pack("<Q", 42), ev


@test
def the_pending_table_fills_up_and_frees_up():
    """A full store refuses content with NO_SLOT, a freed entry is taken again, and every MAC still
    resolves. Full is not a table size any more: these tags are not on the allow-list, so each costs a
    record and an offer out of the room the list does not need - and the list may still grow to its
    1024 at the widest a tag can cost (a record and a profile each). Records for 144 of these tags and
    the 1024 promised, at 12 bytes in granules of 16 (14208), profiles for their seven families and
    the 1024 (8256) and 144 offers (2304) are 24768 of the 24640 above the floor at the 145th."""
    sim = Sim()
    start_v2(sim)
    N = 144

    def pend(i):
        return site_mac(i) + struct.pack("<QIBBHHB", 0x5000 + i, 100, 0x20, 0, 0, 250, 0)

    def offered(i):
        """the dataVer the radio offers this tag at a check-in, or None for DATATYPE_NOUPDATE"""
        sim.radio_in(avail_data_req(site_mac(i)))
        adi = parse_normal(sim.radio_out()[0])["payload"]
        sim.pump_events()
        return None if adi[13] == 0 else struct.unpack_from("<Q", adi, 1)[0]

    for i in range(N):
        sim.request(SET_PENDING, pend(i), expect=ACK)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(GET_STATS, expect=STATUS)[3])
    assert (st[5], st[6]) == (N, 0), st

    # full: the S3 is told so and takes it as "try again at this tag's next check-in"
    r = sim.request(SET_PENDING, pend(N), expect=NAK)
    assert r[3] == bytes([4]), r[3]  # V2_NAK_NO_SLOT

    # the entries are still all there and all reachable - the first, the last and the middle
    for i in (0, 1, 123, N - 2, N - 1):
        assert offered(i) == 0x5000 + i, i
    assert offered(N) is None  # the one that was refused was never stored

    # a hole in the middle is taken by the next entry, and nothing else moves
    sim.request(CANCEL_PENDING, site_mac(123), expect=ACK)
    assert offered(123) is None
    sim.request(SET_PENDING, pend(N), expect=ACK)
    assert offered(N) == 0x5000 + N
    assert offered(122) == 0x5000 + 122 and offered(124) == 0x5000 + 124

    # and the store is full again, which is the point: the hole was reused, not appended past the end
    assert sim.request(SET_PENDING, pend(N + 1), expect=NAK)[3] == bytes([4])


@test
def pending_entries_time_out_from_the_end_of_the_table():
    """Entries that expire shorten the scans; the ones that remain are still found"""
    sim = Sim()
    start_v2(sim)

    def mac(i):
        return bytes([0x90, i] + [0] * 6)

    def pend(i, ttl):
        return mac(i) + struct.pack("<QIBBHHB", 0x6000 + i, 100, 0x20, 0, 0, ttl, 0)

    def offered(i):
        sim.radio_in(avail_data_req(mac(i)))
        adi = parse_normal(sim.radio_out()[0])["payload"]
        return None if adi[13] == 0 else struct.unpack_from("<Q", adi, 1)[0]

    # the long-lived entry is at the bottom of the table and the short-lived ones above it, so
    # housekeeping has to pull the high-water mark back down past them without losing the first
    sim.request(SET_PENDING, pend(0, 5), expect=ACK)
    for i in range(1, 8):
        sim.request(SET_PENDING, pend(i, 1), expect=ACK)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(GET_STATS, expect=STATUS)[3])
    assert st[5] == 8, st

    sim.idle(61000)  # one housekeeping round: the seven ttl-1 entries go, the first counts down
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(GET_STATS, expect=STATUS)[3])
    assert st[5] == 1, st
    assert offered(0) == 0x6000, "the surviving entry is still found after the bound came down"
    for i in range(1, 8):
        assert offered(i) is None, i

    # and a new entry goes in above it and is found, so the bound went back up with it
    sim.request(SET_PENDING, pend(9, 5), expect=ACK)
    assert offered(9) == 0x6000 + 9 and offered(0) == 0x6000


@test
def link_lost_back_to_ascii():
    sim = Sim()
    start_v2(sim)
    sim.advance(5100, step=100)
    assert lib.simBaud() == 115200
    sim.frames()
    sim.serial_in(b"NFO?")
    assert sim.ascii().startswith("ACK>TYP>F8")
    start_v2(sim)


@test
def cache_eviction_and_pin():
    sim = Sim()
    start_v2(sim)
    # 7 blocks into 6 slots, none of them offered to a tag: the oldest goes
    data = make_file(7 * 4096)
    for b in range(7):
        send_block(sim, 0x55, data, b)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[7] == 24
    tag = bytes([0x66] + [6] * 7)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 0x55, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    sim.radio_in(block_request(tag, 0x55, 0))
    assert struct.unpack_from("<H", parse_normal(sim.radio_out()[0])["payload"], 1)[0] == 140  # evicted
    sim.pump_events()
    sim.radio_in(block_request(tag, 0x55, 6))
    assert struct.unpack_from("<H", parse_normal(sim.radio_out()[0])["payload"], 1)[0] == 0
    # chunks that do not fit a block are refused
    r = sim.request(DATA_CHUNK, struct.pack("<QII", 9, 100, 5000) + b"x" * 10, expect=NAK)
    assert r[3] == bytes([3])

    # A block of data no pending entry offers goes before any block a tag may still ask for, whatever
    # their ages: chunks of a preload that were on the wire when the transfer ended land like this.
    sim.request(DATA_DROP, struct.pack("<Q", 0x55), expect=ACK)
    sim.advance(1300)  # CONCURRENT_REQUEST_DELAY, or the next tag is turned away as a second transfer
    other = bytes([0x67] + [6] * 7)
    data = make_file(6 * 4096, seed=11)
    sim.request(SET_PENDING, other + struct.pack("<QIBBHHB", 0x77, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for b in range(5):
        send_block(sim, 0x77, data, b)
    send_block(sim, 0x99, data, 0)  # nothing offers 0x99; the sixth slot
    send_block(sim, 0x77, data, 5)  # full, so one of the six has to go
    for b in range(6):
        assert ask_block(sim, other, 0x77, b) == 0, b
        sim.pump_events()
    # and the block nothing offered is the one that went, so it has to come from the S3 again
    assert ask_block(sim, other, 0x99, 0) >= 140
    sim.pump_events()


def ask_block(sim, tag, ver, block):
    """A whole-block request; returns the pleaseWaitMs the radio promised (0 cached, 140 from the S3)"""
    sim.radio_in(block_request(tag, ver, block))
    out = [parse_normal(f) for f in sim.radio_out()]
    assert out and out[0]["type"] == PKT_BLOCK_REQUEST_ACK, out
    return struct.unpack_from("<H", out[0]["payload"], 1)[0]


def run_block(sim, tag, ver, data, block):
    """One block of a transfer the whole way: the request, the block from the S3 when the cache does
    not have it, and the burst that takes it to the tag. Returns the promised pleaseWaitMs."""
    wait = ask_block(sim, tag, ver, block)
    if wait != 0:
        ev = sim.pump_events()
        assert [e[0] for e in ev] == [DATA_NEEDED], ev
        send_block(sim, ver, data, block)
    sim.advance(wait + 10)
    sim.drain()
    frames = sim.radio_out()
    sim.advance(20)  # the pause after the first pass, once a tag has been seen to ask for its next block
    sim.drain()
    assert frames + sim.radio_out(), "no burst for block %d" % block
    assert [e[0] for e in sim.pump_events() if e[0] != LOG] == [BLOCK_SERVED]
    return wait


@test
def two_transfers_keep_their_own_blocks():
    """Two tags downloading at once do not both fit in the cache, so it has to evict. The block a tag
    has not asked for yet is the oldest of its image, so by age it was the first to go and the other
    tag's download took exactly the blocks that were about to be needed - each of them 140 ms of
    waiting for the tag instead of none. Blocks still to come now outlive blocks already
    delivered, whichever transfer they belong to."""
    sim = Sim()
    start_v2(sim)
    a, b = bytes([0xA0] + [1] * 7), bytes([0xB0] + [2] * 7)
    va, vb = 0x1111111111111111, 0x2222222222222222
    da, db = make_file(4 * 4096, seed=1), make_file(4 * 4096, seed=2)
    sim.request(SET_PENDING, a + struct.pack("<QIBBHHB", va, len(da), 0x20, 0, 0, 10, 0), expect=ACK)
    sim.request(SET_PENDING, b + struct.pack("<QIBBHHB", vb, len(db), 0x20, 0, 0, 10, 0), expect=ACK)

    # the S3 preloads A's image: four of the six slots, two still free
    for blk in range(4):
        send_block(sim, va, da, blk)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[7] == 16, st
    assert run_block(sim, a, va, da, 0) == 0

    # B downloads its whole image in between, a block at a time from the S3. Its last two blocks have
    # to take a slot from something: A's delivered block 0 first, then B's own delivered block 0.
    sim.advance(1300)  # CONCURRENT_REQUEST_DELAY: the radio serves one tag at a time
    for blk in range(4):
        assert run_block(sim, b, vb, db, blk) == 140

    # A comes back to a cache that still holds everything it has not had yet
    sim.advance(1300)
    for blk in (1, 2, 3):
        assert run_block(sim, a, va, da, blk) == 0, blk
    # and it was the blocks both tags were finished with that paid for B's download
    sim.advance(1300)
    assert ask_block(sim, a, va, 0) == 140
    sim.pump_events()

    # the cache is still full, of both tags' blocks
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[7] == 24, st
    # two slots were taken from a block that was in them, and neither block had ever gone out to a
    # tag: the figure a site reads to tell whether this is costing its tags anything
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    sim.idle(16000)
    logs = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
    blk = [l for l in logs if l.startswith("blk ")]
    assert len(blk) == 1 and "cev 2 cevu 0 " in blk[0], blk


@test
def evictions_and_cancel_reasons_are_reported():
    """The three things the C6 logged and this port did not: a silently evicted cache block, and the
    two cancels, which mean different things - a third tag while one is served and one waits its
    turn, or nothing to send. The tag that waits is counted apart (bq): it is served, not cancelled."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    a, b, c, d = (bytes([m] + [1] * 7) for m in (0xC0, 0xC1, 0xC2, 0xC3))
    data = make_file(8 * 4096)  # 8 blocks into 6 slots: two evictions
    sim.request(SET_PENDING, a + struct.pack("<QIBBHHB", 0x55, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for blk in range(8):
        send_block(sim, 0x55, data, blk)
    for tag, ver in ((b, 0x66), (c, 0x67)):
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
    sim.radio_in(block_request(a, 0x55, 7))
    sim.radio_out()
    sim.advance(5)
    sim.radio_out()  # a's burst has started
    sim.radio_in(block_request(b, 0x66, 0))  # a is being served: b waits its turn
    out = [parse_normal(f) for f in sim.radio_out()]
    assert out and out[0]["type"] == PKT_BLOCK_REQUEST_ACK and out[0]["dst"] == b, out
    sim.radio_in(block_request(c, 0x67, 0))  # ... and the slot is taken: c is turned away
    out = [parse_normal(f) for f in sim.radio_out()]
    assert out and out[0]["type"] == PKT_CANCEL_XFER and out[0]["dst"] == c, out
    sim.advance(1300)
    sim.radio_out()
    sim.radio_in(block_request(d, 0x77, 0))  # no pending data for d
    out = [parse_normal(f) for f in sim.radio_out()]
    assert out and out[0]["type"] == PKT_CANCEL_XFER, out
    sim.pump_events()
    sim.idle(16000)
    logs = [l[3][1:].decode("utf-8", "replace") for l in sim.pump_events() if l[0] == LOG]
    blk = [l for l in logs if l.startswith("blk ")]
    assert blk, logs
    f = blk[-1].split()
    assert f[f.index("cxlb") + 1] == "1", blk
    assert f[f.index("bq") + 1] == "1", blk
    assert f[f.index("cxln") + 1] == "1", blk
    assert f[f.index("cev") + 1] == "2", blk


@test
def tag_return_data():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x44] + [7] * 7)
    trd = struct.pack("<BQB", 0, 0xDEAD, 3) + bytes(range(90))
    sim.radio_in(bcast(tag, PKT_TAG_RETURN_DATA, csum(trd)))
    f = parse_normal(sim.radio_out()[0])
    assert f["type"] == PKT_TAG_RETURN_DATA_ACK and f["dst"] == tag
    ev = sim.pump_events()
    assert ev[0][0] == TAG_RETURN_DATA and ev[0][3][:8] == tag and ev[0][3][14:] == csum(trd)
    sim.radio_in(bcast(tag, PKT_TAG_RETURN_DATA, csum(trd)))  # same dataVer: acked, not reported again
    assert len(sim.radio_out()) == 1 and sim.pump_events() == []
    # another tag with the same dataVer is another report, not a repeat of this one
    other = bytes([0x45] + [7] * 7)
    sim.radio_in(bcast(other, PKT_TAG_RETURN_DATA, csum(trd)))
    assert parse_normal(sim.radio_out()[0])["dst"] == other
    ev = sim.pump_events()
    assert ev and ev[0][0] == TAG_RETURN_DATA and ev[0][3][:8] == other, ev
    sim.radio_in(bcast(other, PKT_TAG_RETURN_DATA, csum(trd)))  # now it is a repeat
    assert len(sim.radio_out()) == 1 and sim.pump_events() == []


@test
def tag_reports_take_check_ins_room_and_not_each_others():
    """A TAG_RETURN_DATA is 117 bytes of the ring, and while the S3 is not reading, reports and the
    check-ins between them queue until the ring is full; then it is the check-ins that make room, never
    a report, and a report there is no room for is lost itself. (There used to be three side buffers
    for reports, and a fourth report was lost however empty the queue was.)"""
    sim = Sim()
    start_v2(sim)
    dropped = ctypes.c_uint16.in_dll(lib, "v2EventsDropped")
    n = 10
    tags = [bytes([0x60 + i] + [3] * 7) for i in range(n)]
    # 101 bytes, the longest report a tag can send, so the event is the 115 bytes ap.c sends at most
    reports = [csum(struct.pack("<BQB", 1, 0x1000 + i, 3) + bytes([0x40 + i] * 90)) for i in range(n)]
    assert len(reports[0]) == 101

    def report(i):
        sim.radio_in(bcast(tags[i], PKT_TAG_RETURN_DATA, reports[i]))
        assert parse_normal(sim.radio_out()[0])["dst"] == tags[i]

    def checkin(i):
        sim.radio_in(avail_data_req(bytes([0x6F, i] + [3] * 6)))
        sim.radio_out()

    # one report on the line, then three retries, and the link is down: what follows only queues
    report(0)
    sim.advance(200)
    before = dropped.value
    checkin(0)
    checkin(1)
    for i in range(1, 7):  # 7 reports and 2 check-ins: 921 bytes, 103 free
        report(i)
    assert dropped.value == before
    report(7)  # 117 bytes: the older check-in makes the room
    assert dropped.value - before == 1, dropped.value - before
    # 37 bytes free and one check-in of 51 to give: not enough for a report, so this one is lost,
    # and the check-in is not given up for nothing
    report(8)
    assert dropped.value - before == 2, dropped.value - before

    sim.request(GET_STATS, expect=STATUS)  # the S3 is answering again
    ev = sim.pump_events()
    assert [e[0] for e in ev] == [TAG_RETURN_DATA, CHECKIN] + [TAG_RETURN_DATA] * 7, [hex(e[0]) for e in ev]
    assert ev[1][3][:2] == bytes([0x6F, 1]), ev[1][3][:8].hex()
    rep = [ev[0]] + ev[2:]
    for i in range(8):
        assert rep[i][3][:8] == tags[i], (i, rep[i][3][:8].hex())
        assert rep[i][3][14:] == reports[i], (i, rep[i][3][14:].hex())

    # the room came back with the events, so a report now goes through again (a fresh tag:
    # espNotifyTagReturnData suppresses a repeat of the last report it queued, even one the ring lost)
    report(n - 1)
    ev = [e for e in sim.pump_events() if e[0] == TAG_RETURN_DATA]
    assert len(ev) == 1 and ev[0][3][:8] == tags[n - 1] and ev[0][3][14:] == reports[n - 1], ev


@test
def the_event_window_slides_and_is_compacted():
    """Popping the front moves the head; the queue itself is only moved when it reaches the end"""
    sim = Sim()
    start_v2(sim)

    def tag(i):
        return bytes([0x70, i] + [0] * 6)

    def events():
        return [f for f in sim.frames() if not f[2] & F_RESPONSE]

    # Three events deep for the whole run, and never empty: the head walks the array and past its
    # end, and when it does the two events still waiting have to come out in order and intact. One
    # event deep would not prove that - the one being moved is the one already on the line, which is
    # never read again, so losing it in the move is invisible.
    for i in range(3):
        sim.radio_in(avail_data_req(tag(i)))
        sim.radio_out()
    cur = events()
    assert len(cur) == 1 and cur[0][0] == CHECKIN, cur  # stop-and-wait: only the head is on the line
    cur = cur[0]
    seen = [cur[3][:8]]
    for i in range(3, 60):
        sim.radio_in(avail_data_req(tag(i)))
        sim.radio_out()
        assert events() == [], i  # nothing goes out until the one on the line is acked
        sim.serial_in(frame(ACK, cur[1], F_RESPONSE))
        got = events()
        assert len(got) == 1 and got[0][0] == CHECKIN, (i, got)
        cur = got[0]
        seen.append(cur[3][:8])
    assert seen == [tag(i) for i in range(58)], [b.hex() for b in seen[:6]]


# --- the outbound ring and the frame encoder, through v2link.c's own interface --------------------
# These put records into the ring with v2QueueEvent() and v2Log() and run it with v2Poll(), without
# the rest of the AP, so a case can hold exactly the records it is about; the S3's side is still
# played byte for byte, from the spec.

class Link:
    """The v2 link alone: started by a real HELLO, then driven through v2link.c's own functions"""

    def __init__(self):
        self.sim = Sim()
        start_v2(self.sim)
        self.sim.frames()
        lib.v2SetLogEnabled(True)
        lib.v2LogsDropped.restype = ctypes.c_uint16
        lib.v2EventsQueued.restype = ctypes.c_uint8
        lib.v2OutHigh.restype = ctypes.c_uint16
        lib.v2MsUntilWork.restype = ctypes.c_uint32
        lib.simSerialTxFrames.restype = ctypes.c_uint32
        self.dropped = ctypes.c_uint16.in_dll(lib, "v2EventsDropped")

    def event(self, etype, payload, urgent=False):
        lib.v2QueueEvent(etype, payload, ctypes.c_size_t(len(payload)), bool(urgent))

    def log(self, text, level=3):
        lib.v2Log(level, text.encode())

    def poll(self):
        lib.v2Poll()
        return self.sim.frames()

    def wire(self):
        """v2Poll() once, and the bytes it put on the line, undecoded"""
        lib.v2Poll()
        self.sim._collect()
        w = bytes(self.sim.out)
        self.sim.out.clear()
        return w

    def ack(self, seq):
        f = frame(ACK, seq, F_RESPONSE)
        lib.v2RxBytes(f, ctypes.c_size_t(len(f)))

    def step(self, ms=1):
        lib.simSetTime(lib.simTime() + ms)

    def evd(self):
        return self.dropped.value

    def lgd(self):
        return lib.v2LogsDropped()

    def drain(self, check=None):
        """Acks each event as it comes and lets the LOG lines go at their pace, until the ring is
        empty; returns (events, LOG payloads) in the order they were sent. check(frame) sees each."""
        evs, logs = [], []
        for _ in range(100000):
            got = self.poll()
            for f in got:
                if check:
                    check(f)
                if f[0] == LOG:
                    logs.append(f[3])
                else:
                    assert not f[2] & F_RETRANSMIT, f  # acked within the pass: never resent
                    evs.append((f[0], f[3]))
                    self.ack(f[1])
            if not got and lib.v2EventsQueued() == 0 and lib.v2MsUntilWork() == 0xFFFFFFFF:
                return evs, logs
            self.step()
        raise AssertionError("the ring never emptied")


def rec(tag, n):
    """a payload of n bytes whose first byte tells it apart from the others"""
    return bytes([tag]) + bytes((tag * 7 + i) & 0xFF or 1 for i in range(n - 1))


EVENT_LEN = {CHECKIN: 49, DATA_NEEDED: 26, BLOCK_SERVED: 21, XFER_COMPLETE: 16, XFER_TIMEOUT: 16}


class RingModel:
    """The drop policy of v2link.c, written from its comments: LOG lines go first, oldest first, then
    the oldest CHECKIN or BLOCK_SERVED that is not on the line; an urgent event goes behind the one on
    the line; a record the policy cannot make room for is lost itself."""

    def __init__(self):
        self.ev, self.logs = [], []
        self.on_line = False  # the front event has been sent and waits for its ACK
        self.evd = self.lgd = 0

    def free(self):
        return RING_BYTES - sum(2 + len(p) for _, p in self.ev) - sum(len(p) + 1 for p in self.logs)

    def victims(self):
        return [i for i in range(1 if self.on_line else 0, len(self.ev)) if self.ev[i][0] in (CHECKIN, BLOCK_SERVED)]

    def event(self, t, p, urgent):
        need = 2 + len(p)
        if self.free() < need:
            could = sum(len(l) + 1 for l in self.logs) + sum(2 + len(self.ev[i][1]) for i in self.victims())
            if self.free() + could < need:
                self.evd += 1
                return
            while self.logs and self.free() < need:
                self.logs.pop(0)
                self.lgd += 1
            while self.free() < need:
                del self.ev[self.victims()[0]]
                self.evd += 1
        pos = (1 if self.on_line else 0) if urgent else len(self.ev)
        self.ev.insert(pos, (t, p))

    def log(self, payload):
        need = len(payload) + 1
        if self.free() + sum(len(l) + 1 for l in self.logs) < need:
            self.lgd += 1
            return
        while self.free() < need:
            self.logs.pop(0)
            self.lgd += 1
        self.logs.append(payload)


@test
def the_ring_sends_in_order_and_urgent_events_go_behind_the_one_on_the_line():
    L = Link()
    # Nothing on the line yet: an urgent event goes to the very front
    L.event(BLOCK_SERVED, rec(1, 21))
    L.event(DATA_NEEDED, rec(2, 26), urgent=True)
    first = L.poll()
    assert [(f[0], f[3][0]) for f in first] == [(DATA_NEEDED, 2)], first
    # With one on the line, each urgent event goes straight behind it: before every other event
    # waiting, the urgent ones queued earlier included, and never before the one the S3 may have
    L.event(XFER_COMPLETE, rec(3, 16))
    L.event(CHECKIN, rec(4, 49))
    L.event(DATA_NEEDED, rec(5, 26), urgent=True)
    L.event(DATA_NEEDED, rec(6, 26), urgent=True)
    assert L.poll() == [], "stop-and-wait: nothing until the one on the line is acked"
    L.ack(first[0][1])
    evs, _ = L.drain()
    assert [p[0] for _, p in evs] == [6, 5, 1, 3, 4], evs
    assert evs[0] == (DATA_NEEDED, rec(6, 26)) and evs[3] == (XFER_COMPLETE, rec(3, 16)), evs
    assert L.evd() == 0


@test
def a_stall_costs_log_lines_first_then_the_oldest_check_ins_and_never_a_transfer():
    """The S3 stops reading with an event on the line, a full diagnostics report waiting and events
    of every kind behind it, and check-ins keep coming. The ring gives up every LOG line, oldest
    first, before any event; then the oldest check-ins; never the one on the line, a transfer
    result, a DATA_NEEDED or a tag report; and when nothing it may give up is left, the new event is
    the one lost. Every loss is counted."""
    L = Link()
    m = RingModel()  # only to tell, below, a result that cost a check-in from one that was lost
    L.event(CHECKIN, rec(0, 49))
    m.event(CHECKIN, rec(0, 49), False)
    first = L.poll()
    assert [f[0] for f in first] == [CHECKIN]  # on the line, and the S3 has stalled: no ack
    m.on_line = True
    report = ["r%d " % i + "x" * 97 for i in range(6)]  # six lines of 100 characters: 612 bytes
    for line in report:
        L.log(line)
        m.log(b"\x03" + line.encode())

    def event(t, p, urgent=False):
        L.event(t, p, urgent)
        m.event(t, p, urgent)
        assert (L.evd(), L.lgd()) == (m.evd, m.lgd)

    event(XFER_COMPLETE, rec(1, 16))
    event(TAG_RETURN_DATA, rec(2, 115))
    event(DATA_NEEDED, rec(3, 26), urgent=True)
    assert L.lgd() == 0 and L.evd() == 0
    # 51 + 612 + 18 + 117 + 28 = 826 bytes; check-ins after it until the ring has to lose an event
    tag = 0x10
    while L.evd() == 0:
        lgd = L.lgd()
        event(CHECKIN, rec(tag, 49))
        tag += 1
        assert L.lgd() >= lgd
        if L.evd():
            assert L.lgd() == 6, "a check-in was dropped while a LOG line was still waiting"
    queued = list(range(0x10, tag))
    # results until one is lost: the check-ins go for them first, and only then a result itself
    xfers = []
    while True:
        p = rec(0x80 + len(xfers), 16)
        event(XFER_COMPLETE, p)
        if m.ev[-1][1] != p:
            break  # nothing left to give up for it: it was the one lost
        xfers.append(p[0])
    assert lib.v2EventsQueued() == len(m.ev)
    # with the ring full of what it may not give up, a LOG line is lost and nothing else
    n, before = lib.v2EventsQueued(), L.evd()
    L.log("a line longer than the room any ring full of results can have left")
    assert L.lgd() == 7 and lib.v2EventsQueued() == n and L.evd() == before
    assert lib.v2OutHigh() <= RING_BYTES

    L.ack(first[0][1])  # the S3 reads again
    evs, logs = L.drain()
    assert logs == [], "every LOG line was given up"
    kinds = [(t, p[0]) for t, p in evs]
    assert kinds[:3] == [(DATA_NEEDED, 3), (XFER_COMPLETE, 1), (TAG_RETURN_DATA, 2)], kinds
    rest = kinds[3:]
    cis = [p for t, p in rest if t == CHECKIN]
    assert cis == queued[len(queued) - len(cis):], "the check-ins kept are the newest, in order"
    assert [p for t, p in rest if t == XFER_COMPLETE] == xfers, "every result queued was delivered"
    assert L.evd() == len(queued) - len(cis) + 1, (L.evd(), len(queued), len(cis))
    assert len(xfers) > 20, "the results took the check-ins' room"


@test
def log_lines_are_spaced_and_never_hold_an_event_up():
    """LOG lines leave V2_LOG_GAP_MS apart, and only while no event waits for its ACK; an event never
    waits for a LOG line's gap, though the two are in the same ring"""
    L = Link()
    for i in range(5):
        L.log("line %d" % i)
    t0 = lib.simTime()
    sent, ack_at, seq = [], None, None
    for _ in range(400):
        t = lib.simTime() - t0
        if t == 110:
            L.event(BLOCK_SERVED, rec(1, 21))
        if t == ack_at:
            L.ack(seq)
        for f in L.poll():
            sent.append((t, f[0]))
            if f[0] != LOG:
                ack_at, seq = t + 25, f[1]  # the S3 takes 25 ms to answer
        L.step()
    logs = [t for t, kind in sent if kind == LOG]
    assert [t for t, kind in sent if kind == BLOCK_SERVED] == [110], sent  # at once, not at 120
    # 0 and 60; the third is due at 120 but an event waits for its ACK until 135; then 60 apart
    assert logs == [0, 60, 135, 195, 255], logs
    assert L.lgd() == 0


@test
def the_ring_matches_its_model_through_stalls_and_partial_drains():
    """Random traffic of every kind into the ring while the S3 is not reading, then part or all of it
    read, many times over: every event and LOG line comes out in the order the model says, with the
    model's losses, and the two queues sharing the ring wander through it, so it is compacted."""
    import random
    rng = random.Random(0x0EAD)
    L = Link()
    m = RingModel()
    seq_on_line = None
    kinds = [CHECKIN] * 6 + [BLOCK_SERVED] * 3 + [XFER_COMPLETE, XFER_TIMEOUT, DATA_NEEDED, TAG_RETURN_DATA, LOG, LOG, LOG]
    sent = [0, 0]

    def check(f, acked=True):
        """a frame the radio sent is the model's next; an event leaves the model when it is acked"""
        if f[0] == LOG:
            assert m.logs and f[3] == m.logs[0], (f, m.logs[:1])
            m.logs.pop(0)
        else:
            assert m.ev and (f[0], f[3]) == m.ev[0], (f, m.ev[:1])
            if acked:
                m.ev.pop(0)
        sent[f[0] == LOG] += 1

    for rnd in range(60):
        # the S3 is not reading: whatever is on the line stays there
        for _ in range(rng.randrange(1, 40)):
            k = rng.choice(kinds)
            if k == LOG:
                text = "".join(rng.choice("abcdefghij ") for _ in range(rng.randrange(0, 101)))
                level = rng.randrange(0, 5)
                L.log(text, level)
                m.log(bytes([level]) + text.encode())
            else:
                n = rng.randrange(14, 116) if k == TAG_RETURN_DATA else EVENT_LEN[k]
                p = bytes(rng.randrange(256) for _ in range(n))
                L.event(k, p, urgent=k == DATA_NEEDED)
                m.event(k, p, k == DATA_NEEDED)
            assert (L.evd(), L.lgd()) == (m.evd, m.lgd), (rnd, L.evd(), L.lgd(), m.evd, m.lgd)
            assert lib.v2EventsQueued() == len(m.ev)
        # the S3 reads again: answer the one on the line, then all of it or some of it
        if seq_on_line is not None:
            L.ack(seq_on_line)
            m.ev.pop(0)
            m.on_line, seq_on_line = False, None
        if rng.random() < 0.3:
            L.drain(check)
            assert m.ev == [] and m.logs == []
            continue
        for _ in range(rng.randrange(0, 12)):  # a few events, and the lines that fit between them
            for f in L.poll():
                check(f)
                if f[0] != LOG:
                    L.ack(f[1])
            L.step(rng.choice([1, 20, 61]))
        # and maybe one more sent and left on the line, for the next stall to queue behind
        if rng.random() < 0.5:
            for f in L.poll():
                check(f, acked=False)
                if f[0] != LOG:
                    m.on_line, seq_on_line = True, f[1]
    if seq_on_line is not None:
        L.ack(seq_on_line)
        m.ev.pop(0)
        m.on_line = False
    L.drain(check)
    assert m.ev == [] and m.logs == []
    assert sent[0] > 300 and sent[1] > 40 and m.evd > 20 and m.lgd > 20, (sent, m.evd, m.lgd)
    assert lib.v2OutHigh() <= RING_BYTES


@test
def frames_to_the_s3_are_encoded_byte_for_byte():
    """Frames are COBS-encoded straight into the UART's buffer, the CRC computed on the way. Compared
    byte for byte with the spec's encoding here, on what the streaming encoder can get wrong: runs of
    254 non-zero bytes and more (code 0xFF, with the run crossing the payload's end and the CRC at
    every offset), zeros in every position, frames of nothing but zeros, and a retransmission, whose
    flags make the header non-zero. And the reply resent for a lost ACK is the same bytes again."""
    import random
    rng = random.Random(254)
    L = Link()
    cases = [bytes([1]) * 255, bytes(255), bytes([0xFF]) * 255, b"", bytes(1)]
    cases += [bytes(rng.randrange(1, 256) for _ in range(n)) for n in range(245, 256)]
    for z in range(245, 255):  # a zero just before, at and after the 254th byte of the run
        p = bytearray(rng.randrange(1, 256) for _ in range(255))
        p[z] = 0
        cases.append(bytes(p))
    cases += [bytes(rng.choice([0, 0, 0, rng.randrange(256)]) for _ in range(rng.randrange(0, 256)))
              for _ in range(60)]
    cases += [bytes(rng.randrange(256) for _ in range(rng.randrange(0, 256))) for _ in range(100)]
    last = None
    for p in cases:
        frames = lib.simSerialTxFrames()
        L.event(TAG_RETURN_DATA, p)
        w = L.wire()
        seq = cobs_decode(w[:-1])[1]
        assert w == frame(TAG_RETURN_DATA, seq, 0, p), (len(p), w.hex())
        assert lib.simSerialTxFrames() == frames + 1, "not encoded in place"
        assert last is None or seq == (last + 1) & 0xFF
        last = seq
        L.ack(seq)
    # a retransmission: type, seq and flags all non-zero, so the first run starts in the header
    p = bytes(rng.randrange(1, 256) for _ in range(251))
    L.event(TAG_RETURN_DATA, p)
    w = L.wire()
    seq = cobs_decode(w[:-1])[1]
    L.step(V2_LOG_GAP_MS)  # past V2_ACK_TIMEOUT_MS
    w = L.wire()
    assert w == frame(TAG_RETURN_DATA, seq, F_RETRANSMIT, p), w.hex()
    L.ack(seq)
    # LOG frames: the longest line, and a level of 0 with no text
    L.step(V2_LOG_GAP_MS)
    L.log("y" * 120, level=0x7F)  # cut at 100 characters
    w = L.wire()
    assert w == frame(LOG, cobs_decode(w[:-1])[1], 0, b"\x7f" + b"y" * 100), w.hex()
    L.step(V2_LOG_GAP_MS)
    L.log("", level=0)
    w = L.wire()
    assert w == frame(LOG, cobs_decode(w[:-1])[1], 0, b"\x00"), w.hex()
    # a reply, and the same request again as if our reply had been lost: the same bytes, from the
    # copy kept of them (a STATUS has counters in it that move, so rebuilding it would differ)
    def reply_to(req):
        """the wire bytes of the radio's reply to req, whatever else the AP sends meanwhile"""
        L.sim.serial_in(req)
        L.sim._collect()
        out = bytes(L.sim.out)
        L.sim.out.clear()
        replies = [w + b"\x00" for w in out.split(b"\x00") if w and cobs_decode(w)[2] & F_RESPONSE]
        assert len(replies) == 1, out.hex()
        return replies[0]

    first = reply_to(frame(HEARTBEAT, 0x33, 0))
    L.step(5)
    again = reply_to(frame(HEARTBEAT, 0x33, F_RETRANSMIT))
    assert again == first, (first.hex(), again.hex())
    assert cobs_decode(first[:-1])[:2] == bytes([STATUS, 0x33])


@test
def burst_with_airtime_interleaves_serial():
    """With real airtime the burst is one frame per pass: the S3 is answered in between"""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        tag = bytes([0x33] + [8] * 7)
        data = make_file(4096)
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 3, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, 3, data, 0)
        sim.radio_in(block_request(tag, 3, 0))
        sim.radio_out()
        sim.advance(30)
        t = sim.time()
        sim.request(HEARTBEAT, expect=STATUS)
        assert sim.time() - t < 10, "STATUS waited for the whole burst"
        sim.advance(300)
        check_burst(sim.radio_out(), tag, data, 0, range(42))
    finally:
        lib.simSetAirtime(0)


@test
def burst_stops_when_the_tag_has_the_block():
    """A tag that reports the transfer complete, or checks in again, is no longer listening for block
    parts: the rest of the burst is airtime nobody receives, and other tags' windows are passing."""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        for finisher in ("xfc", "checkin"):
            tag = bytes([0x35 if finisher == "xfc" else 0x36] + [8] * 7)
            data = make_file(4096)
            ver = 0x50 + tag[0]
            sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
            send_block(sim, ver, data, 0)
            sim.advance(1300)  # past the busy-with-another-tag window
            sim.radio_out()
            sim.radio_in(block_request(tag, ver, 0))
            assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
            sim.advance(60)
            sent = len([f for f in sim.radio_out() if parse_normal(f)["type"] == PKT_BLOCK_PART])
            assert 0 < sent < BURST_MAX_FRAMES, sent
            # the tag says it is done with this block
            if finisher == "xfc":
                sim.radio_in(normal(tag, AP_MAC, PKT_XFER_COMPLETE))
            else:
                sim.radio_in(avail_data_req(tag))
            sim.advance(400)
            out = [parse_normal(f) for f in sim.radio_out()]
            more = [f for f in out if f["type"] == PKT_BLOCK_PART]
            assert len(more) <= 1, "burst kept going: %d more parts" % len(more)
            # ... and the burst is reported with the frames that really went out
            bs = [e for e in sim.pump_events() if e[0] == BLOCK_SERVED]
            assert len(bs) == 1, bs
            frames = struct.unpack_from("<B", bs[0][3], 18)[0]
            assert frames == sent + len(more), (frames, sent, len(more))
            assert frames < BURST_MAX_FRAMES, frames
    finally:
        lib.simSetAirtime(0)


@test
def repeated_request_during_burst_keeps_it_running():
    """A tag that missed the ack asks again for exactly what is being sent. It gets the ack once more
    with pleaseWaitMs 0 ("carry on receiving") and the burst runs on; asking for something else - the
    parts it is still missing - starts a new burst."""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        tag = bytes([0x34] + [8] * 7)
        data = make_file(4096)
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 4, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, 4, data, 0)
        sim.radio_in(block_request(tag, 4, 0))
        assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
        sim.advance(40)
        frames = sim.radio_out()
        assert frames, "burst has started"
        # the same request again, mid-burst
        sim.radio_in(block_request(tag, 4, 0))
        sim.advance(8)
        mid = sim.radio_out()
        acks = [f for f in mid if parse_normal(f)["type"] == PKT_BLOCK_REQUEST_ACK]
        assert len(acks) == 1, [parse_normal(f)["type"] for f in mid]
        assert struct.unpack_from("<H", parse_normal(acks[0])["payload"], 1)[0] == 0, "pleaseWaitMs 0"
        # the burst carried on where it was: every part still goes out, as one burst
        sim.advance(400)
        frames += [f for f in mid if parse_normal(f)["type"] == PKT_BLOCK_PART] + sim.radio_out()
        seen = check_burst(frames, tag, data, 0, range(42))
        check_schedule(seen, list(range(42)))
        assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED]
        # now a partial request for other parts: that is a new burst
        sim.advance(1300)
        sim.radio_in(block_request(tag, 4, 0, parts=[7, 9], partial=True))
        ack = parse_normal(sim.radio_out()[0])
        assert ack["type"] == PKT_BLOCK_REQUEST_ACK
        sim.advance(400)
        check_burst(sim.radio_out(), tag, data, 0, [7, 9])
    finally:
        lib.simSetAirtime(0)


@test
def mid_burst_reack_holds_the_burst_for_every_tag():
    """The re-ack carries pleaseWaitMs 0, and the burst holds its frames for 3 ms after it. An nRF52811
    before firmware 0x002D sleeps through a zero wait regardless and restarts its radio, so the frames
    it would miss are the ones it would come back for, at the cost of another receive window. Every
    other tag carries straight on receiving, which costs it those 3 ms of silence.

    Naming a non-zero wait instead is not the answer: it would have to be at least 10 ms to clear the
    "pleaseWaitMs - 5" and "- 10" underflows in tag firmware, and many tags spend a wait that short
    busy-waiting with the receiver on."""
    sim = Sim()
    start_v2(sim)
    data = make_file(4096)
    lib.simSetAirtime(4)  # so a burst not held sends a part as soon as the ack is off the air
    try:
        for mac0, hw in ((0x30, 0x04), (0x31, 0x33), (0x32, 0x28), (0x33, None)):
            tag = bytes([mac0] + [8] * 7)
            ver = 0x800 + mac0
            sim.advance(1300)  # past the busy-with-another-tag window of the previous round
            sim.radio_out()
            sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
            send_block(sim, ver, data, 0)
            if hw is not None:
                sim.radio_in(avail_data_req(tag, hw=hw))
                sim.radio_out()
                sim.pump_events()
                sim.advance(1300)
                sim.radio_out()
            sim.radio_in(block_request(tag, ver, 0))
            assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK, hw
            sim.advance(40)
            frames = sim.radio_out()
            assert frames, ("burst has started", hw)
            # exactly the same request again, mid-burst
            sim.radio_in(block_request(tag, ver, 0))
            mid = sim.radio_out()
            acks = [f for f in mid if parse_normal(f)["type"] == PKT_BLOCK_REQUEST_ACK]
            assert len(acks) == 1, (hw, [parse_normal(f)["type"] for f in mid])
            assert struct.unpack_from("<H", parse_normal(acks[0])["payload"], 1)[0] == 0, ("pleaseWaitMs 0", hw)
            sim.advance(5)
            soon = sim.radio_out()
            got = [f for f in soon if parse_normal(f)["type"] == PKT_BLOCK_PART]
            assert not got, (hw, len(got))
            # the burst still finishes and every part still goes out as one burst
            sim.advance(400)
            sim.drain()
            frames += mid + soon + sim.radio_out()
            check_burst([f for f in frames if parse_normal(f)["type"] == PKT_BLOCK_PART], tag, data, 0, range(42))
            assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED], hw
    finally:
        lib.simSetAirtime(0)


# --- frames read and built where the radio's DMA has them -------------------------------------------
# host_hal.c keeps received frames in four receive slots as the TLSR does - the armed one and three
# frames, the one the AP is handling among them - and poisons a slot for AddressSanitizer once the AP
# has released it, so a read of a frame after its release fails every test that reaches it. These
# are about the ring's edges, which the other tests, feeding one frame per pass, never reach.

def checkins(sim):
    """the CHECKIN events, as (mac, rssi, adr)"""
    return [(e[3][:8], struct.unpack_from("<b", e[3], 12)[0], e[3][28:49])
            for e in sim.pump_events() if e[0] == CHECKIN]


@test
def frames_wait_in_their_slots_and_a_full_ring_drops_the_newest():
    """Four check-ins arrive before the main loop comes round: three slots hold them and the fourth
    has nowhere to go - moving on would arm the slot the AP reads next. The three are answered in the
    order they came, each from its own slot, and the fourth is the `drop` the `rf` line counts."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)

    def report():
        lines = []
        for _ in range(3):
            sim.idle(16000)
            lines += [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
        return lines

    # A first window with something in it, for the boot facts, which the high-water line is never
    # sent beside: one frame, so rxq is 1 when the ring is filled below.
    sim.radio_in(avail_data_req(bytes([0x5F] + [5] * 7)))
    sim.radio_out()
    sim.pump_events()
    report()
    tags = [bytes([0x60 + i] + [5] * 7) for i in range(4)]
    kept = []
    for i, t in enumerate(tags):
        f = avail_data_req(t)
        kept.append(lib.simRadioIn(f, len(f), -40 - i, 500))
    assert kept == [1, 1, 1, 0], kept
    sim.loop(6)
    out = [parse_normal(f) for f in sim.radio_out()]
    assert [f["dst"] for f in out] == tags[:3], [f["dst"].hex() for f in out]
    assert all(f["type"] == PKT_AVAIL_DATA_INFO and sum(f["payload"][1:]) & 0xFF == f["payload"][0] for f in out)
    assert [(m, r) for m, r, _ in checkins(sim)] == [(t, -40 - i) for i, t in enumerate(tags[:3])]
    assert lib.simRadioDrops() == 1
    # the ring is whole again: the next frame goes into the slot the dropped one could not have
    sim.radio_in(avail_data_req(tags[3]))
    assert [parse_normal(f)["dst"] for f in sim.radio_out()] == [tags[3]]
    assert [m for m, _, _ in checkins(sim)] == [tags[3]]
    # ... and the counters say what happened: three waiting at most, one lost
    lines = report()
    rf = [l for l in lines if l.startswith("rf ")]
    assert rf and " drop 1 " in rf[0], lines
    hw = [l.split() for l in lines if l.startswith("hw ")]
    assert hw and hw[0][hw[0].index("rxq") + 1] == "3", lines


@test
def frames_arriving_while_one_is_answered_never_land_in_its_slot():
    """The AP holds a frame's slot from halRadioRx() until it has finished with it, and the radio must
    not arm that slot meanwhile. Four frames complete while the AP is still answering an old 28-byte
    check-in - which it pads out to the current length in its slot, so the slot is being written as
    well as read. Two of them find free slots; the other two would need the held one and are dropped.
    Had the slot been given back when the frame was taken rather than when it was done with, the
    fourth would have been DMA'd over the check-in being answered, and its event would carry another
    tag's MAC."""
    sim = Sim()
    start_v2(sim)
    held = bytes([0x70] + [6] * 7)
    late = [bytes([0x71 + i] + [6] * 7) for i in range(4)]
    for t in late:
        f = avail_data_req(t)
        lib.simRadioInDuringReply(f, len(f))
    adr_old = struct.pack("<BbbHBBB", 200, -60, 21, 2900, 0x33, 0, 0)
    sim.radio_in(bcast(held, PKT_AVAIL_DATA_REQ, csum(adr_old) + b"\0"), rssi=-44)
    sim.loop(6)
    out = [parse_normal(f) for f in sim.radio_out()]
    assert [f["dst"] for f in out] == [held] + late[:2], [f["dst"].hex() for f in out]
    ci = checkins(sim)
    assert [m for m, _, _ in ci] == [held] + late[:2], [m.hex() for m, _, _ in ci]
    mac, rssi, adr = ci[0]
    assert rssi == -44
    # the request as the tag sent it, and zeroes where an old request has no fields
    assert adr == csum(adr_old) + bytes(12), adr.hex()
    assert lib.simRadioDrops() == 2


@test
def a_reply_waits_for_the_burst_part_on_the_air_before_it_is_built():
    """Frames are built in the radio's TX buffer, which the DMA reads until the frame has left the
    antenna. A check-in heard just before a burst part went out is answered while that part is still
    on the air: the reply has to wait for it before it touches the buffer, or the part would go out
    with the reply's bytes in it. host_hal.c compares every frame with what was sent, after every pass
    of every test; this makes sure the case it guards really happens at least once."""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        tag, other = bytes([0x37] + [8] * 7), bytes([0x38] + [8] * 7)
        data = make_file(4096)
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 9, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, 9, data, 0)
        sim.radio_in(block_request(tag, 9, 0))
        assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
        sim.advance(40)
        for _ in range(8):
            if lib.halRadioTxBusy():
                break
            sim.advance(1)
        assert lib.halRadioTxBusy(), "a burst part is on the air"
        t = sim.time()
        f = avail_data_req(other)
        lib.simRadioIn(f, len(f), -50, 500)
        lib.apLoop()
        assert sim.time() > t, "the reply was built while the part was still going out"
        out = sim.radio_out()
        reply = parse_normal(out[-1])
        assert reply["type"] == PKT_AVAIL_DATA_INFO and reply["dst"] == other
        assert sum(reply["payload"][1:]) & 0xFF == reply["payload"][0]
        sim.advance(400)
        sim.drain()
        frames = out[:-1] + sim.radio_out()
        check_burst([f for f in frames if parse_normal(f)["type"] == PKT_BLOCK_PART], tag, data, 0, range(42))
        assert lib.simTxCorruptions() == 0
    finally:
        lib.simSetAirtime(0)


def air_line(sim):
    """close reporting windows until an `air` line comes, and return its fields. The line takes the
    report's one extra slot only when the boot facts and the high-water lines leave it free, and its
    counters run from one air line to the next, so a window it yields loses nothing; a window with a
    check-in in it is reported. LOG frames go out spaced, so each window is drained after it closes."""
    sim.pump_events()
    for i in range(6):
        sim.idle(16000)
        logs = [l[3][1:].decode("utf-8", "replace") for l in sim.pump_events(ms=500) if l[0] == LOG]
        air = [l for l in logs if l.startswith("air yld ")]
        sim.logs = logs  # the whole report, for a test that wants another line of it too
        if air:
            # The reporting rule: four lines and one more - never beside the boot facts or a high-water
            # line. A report starts with its `ci` line; what a drain begins with before one is the tail
            # of a report sent earlier, which events queued ahead of it delayed.
            lines = [l for l in logs if l.startswith(("ci ", "xfer ", "air ", "blk cxlb ", "rf ", "boot ", "hw ", "stk "))]
            starts = [k for k, l in enumerate(lines) if l.startswith("ci ")]
            for a, b in zip(starts, starts[1:] + [len(lines)]):
                report = lines[a:b]
                if any(l.startswith("air ") for l in report):
                    assert len(report) <= 5 and not any(l.startswith(("boot ", "hw ", "stk ")) for l in report), report
            a = air[-1].split()
            return {a[i]: int(a[i + 1]) for i in range(1, len(a) - 1, 2)}
        sim.radio_in(avail_data_req(bytes([0xED, i] + [1] * 6)))  # activity, so the next one is reported
        sim.radio_out()
    raise AssertionError("no air line")


def burst_setup(sim, mac0, blocks=1):
    """a tag with `blocks` blocks of data in the cache, ready to ask"""
    tag = bytes([mac0] + [8] * 7)
    ver = 0xA00 + mac0
    data = make_file(blocks * 4096)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for b in range(blocks):
        send_block(sim, ver, data, b)
    sim.advance(1300)  # past the busy-with-another-tag window
    sim.radio_out()
    return tag, ver, data


def parts_of(frames):
    return [f for f in frames if parse_normal(f)["type"] == PKT_BLOCK_PART]


@test
def a_burst_gives_way_to_a_frame_arriving_in_its_gap():
    """The receiver is on in the gap between two block parts, but a tag's frame is longer than that
    gap, so it is only ever caught starting in one - and sending the next part then cuts it off. That
    is what made a burst deaf from end to end: every other tag's request, and the served tag's own
    "I have it", went unheard for up to 260 ms. A part that is due while a frame is arriving now waits
    for it, the frame is answered, and the burst goes on with nothing lost."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)  # LOG frames on
    lib.simSetAirtime(4)
    try:
        tag, ver, data = burst_setup(sim, 0x37)
        other = bytes([0x38] + [8] * 7)
        sim.radio_in(block_request(tag, ver, 0))
        assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
        sim.advance(23)
        frames = sim.radio_out()
        assert frames, "burst has started"
        lib.simSetRxArriving(1)  # another tag starts a frame in the gap
        sim.advance(4)  # the part on the air when it began has ended: the next one is held
        held = sim.radio_out()
        assert len(held) <= 1, "at most the part already on the air when the frame began"
        lib.simSetRxArriving(0)
        sim.radio_in(avail_data_req(other))  # ... and it has arrived
        out = [parse_normal(f) for f in sim.radio_out()]
        assert out and out[0]["type"] == PKT_AVAIL_DATA_INFO and out[0]["dst"] == other, [f["type"] for f in out]
        sim.advance(400)
        frames += held + sim.radio_out()
        check_burst(parts_of(frames), tag, data, 0, range(42))
        assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED]
        air = air_line(sim)
        # one hold, and it was a frame: the evidence that a `yld` means a tag rather than a flag
        assert air["yld"] == 1 and air["ylf"] == 1 and air["yms"] == 0 and air["oth"] >= 1, air
    finally:
        lib.simSetRxArriving(0)
        lib.simSetAirtime(0)


@test
def a_receiving_flag_that_never_drops_does_not_stop_a_burst():
    """A flag up for longer than the longest frame lasts, 4.3 ms, is not a tag transmitting, so the part
    goes anyway - and the flag is not trusted again for the rest of that burst, which would otherwise
    pay that silence before every part. It has done this on hardware: holding for the whole 30 ms budget
    cost a tag seven times what the flag could possibly have been worth. The next burst trusts it again."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    lib.simSetAirtime(4)
    try:
        tag, ver, data = burst_setup(sim, 0x3A)
        sim.radio_in(block_request(tag, ver, 0))
        sim.radio_out()
        lib.simSetRxArriving(1)
        sim.advance(400)
        check_burst(parts_of(sim.radio_out()), tag, data, 0, range(42))
        air = air_line(sim)
        # no frame came of the hold, and the flag was seen up in the idle samples after the burst too:
        # a flag that is up while nothing is received is the signature of one that does not work
        assert air["yld"] == 1 and air["ylf"] == 0 and 4 <= air["yms"] <= 6, air
        # ... and in its 1 ms looks it was up far more often than any frame received accounts for
        assert air["fh"] >= 100 and air["fh"] > 10 * (air["fe"] + 1), air
        # the next burst starts out trusting it
        lib.simSetRxArriving(0)
        sim.pump_events()
        sim.advance(1300)
        sim.radio_out()
        sim.radio_in(block_request(tag, ver, 0))
        sim.radio_out()
        lib.simSetRxArriving(1)
        sim.advance(10)  # the ack's 4 ms on air and the 3 ms after it, then 3 ms of holding
        assert parts_of(sim.radio_out()) == [], "a flag up for 3 ms could still be a frame"
        lib.simSetRxArriving(0)
        sim.advance(400)
        assert parts_of(sim.radio_out()), "and the burst was there to hold"
    finally:
        lib.simSetRxArriving(0)
        lib.simSetAirtime(0)


@test
def a_stuck_flag_hides_the_channel_from_the_energy_check():
    """The radio gives no RSSI reading while the flag is up, so energy can't be seen past a stuck flag:
    the part goes after the flag's 4.3 ms, whatever is on the channel, and the budget plays no part."""
    sim = Sim()
    start_v2(sim)
    try:
        tag, ver, data = energy_burst(sim, 0x44)
        lib.simSetRxArriving(1)
        lib.simSetIdleRssi(-60)
        sim.advance(15)  # the ack's 4 ms on air, the 3 ms after it, and the flag's 4.3 ms
        assert parts_of(sim.radio_out()), "past the flag's limit the burst goes"
        sim.advance(400)
        sim.radio_out()
        air = air_line(sim)
        assert air["yld"] == 1 and 4 <= air["yms"] <= 6, air
    finally:
        lib.simSetRxArriving(0)
        lib.simSetIdleRssi(-95)
        lib.simSetAirtime(0)


@test
def a_frame_late_in_an_energy_hold_is_not_taken_for_a_stuck_flag():
    """The flag's limit runs from when the flag came up, not from when the hold began: the frame that
    follows noise on the channel is exactly the one the hold is there for."""
    sim = Sim()
    start_v2(sim)
    try:
        tag, ver, data = energy_burst(sim, 0x45)
        lib.simSetIdleRssi(-60)
        sim.advance(11)  # the burst started 3 ms in: well past a frame's length of holding by now
        assert parts_of(sim.radio_out()) == []
        lib.simSetIdleRssi(-95)
        lib.simSetRxArriving(1)
        sim.advance(4)
        assert parts_of(sim.radio_out()) == [], "the frame is still arriving"
        lib.simSetRxArriving(0)
        sim.radio_in(avail_data_req(bytes([0x45, 1] + [8] * 6)))
        frames = [f for f in sim.radio_out() if parse_normal(f)["type"] == PKT_BLOCK_PART]
        sim.advance(400)
        frames += sim.radio_out()
        check_burst(parts_of(frames), tag, data, 0, range(42))
        air = air_line(sim)
        assert air["yld"] == 1 and air["ylf"] == 1 and air["yms"] == 0, air
    finally:
        lib.simSetRxArriving(0)
        lib.simSetIdleRssi(-95)
        lib.simSetAirtime(0)


def energy_burst(sim, mac0):
    """a tag ready to ask, on a channel that has been quiet long enough to read as quiet"""
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    lib.simSetAirtime(4)
    tag, ver, data = burst_setup(sim, mac0)
    sim.pump_events()
    sim.radio_in(block_request(tag, ver, 0))
    sim.radio_out()
    return tag, ver, data


@test
def a_clean_channel_holds_nothing():
    """The whole point of listening rather than leaving a gap: with nothing on the channel the burst goes
    exactly when it would have, and a tag pays nothing for the protection."""
    sim = Sim()
    start_v2(sim)
    try:
        tag, ver, data = energy_burst(sim, 0x40)
        sim.advance(8)
        frames = parts_of(sim.radio_out())
        assert frames, "the burst has started, 3 ms after the ack"
        sim.advance(400)
        check_burst(frames + parts_of(sim.radio_out()), tag, data, 0, range(42))
        air = air_line(sim)
        assert air["yld"] == 0 and air["yms"] == 0, air
    finally:
        lib.simSetAirtime(0)


@test
def a_burst_waits_out_energy_it_cannot_decode():
    """WiFi holds no frame the radio could lock onto, so the receive flag says nothing, but a part sent
    into it is lost, and a tag that loses both copies of a part spends a whole further receive window.
    The burst waits until the channel clears, and carries on with nothing lost."""
    sim = Sim()
    start_v2(sim)
    try:
        tag, ver, data = energy_burst(sim, 0x41)
        lib.simSetIdleRssi(-60)
        sim.advance(15)
        assert parts_of(sim.radio_out()) == [], "nothing goes while the channel is in use"
        lib.simSetIdleRssi(-95)
        sim.advance(400)
        check_burst(parts_of(sim.radio_out()), tag, data, 0, range(42))
        assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED]
        air = air_line(sim)
        # from the moment the part fell due, a few ms after the request, until the channel cleared
        assert air["yld"] == 1 and air["ylf"] == 0 and 6 <= air["yms"] <= 13, air
    finally:
        lib.simSetIdleRssi(-95)
        lib.simSetAirtime(0)


@test
def energy_that_does_not_clear_costs_the_budget_once():
    """A noise that stays is not going away for waiting, so a burst waits at most 30 ms for it and then
    sends everything back to back, with no further holds: the tag is listening meanwhile, and that is
    the most it can pay. A wrong threshold costs no more than this either."""
    sim = Sim()
    start_v2(sim)
    try:
        tag, ver, data = energy_burst(sim, 0x42)
        lib.simSetIdleRssi(-60)
        sim.advance(28)
        assert parts_of(sim.radio_out()) == [], "still inside the budget"
        sim.advance(12)
        assert parts_of(sim.radio_out()), "the budget is gone, so the burst goes"
        sim.advance(400)
        sim.radio_out()
        assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED]
        air = air_line(sim)
        assert air["yld"] == 1 and air["yms"] == 30, air
    finally:
        lib.simSetIdleRssi(-95)
        lib.simSetAirtime(0)


@test
def real_frames_do_not_use_up_the_budget():
    """A tag's frame is why the hold exists and is over within 4.3 ms, so a hold that a good frame came
    of is free. If it were charged, a busy site would spend the budget on the frames it most wants to
    hear, and go deaf to the rest of them for the rest of the burst."""
    sim = Sim()
    start_v2(sim)
    try:
        tag, ver, data = energy_burst(sim, 0x43)
        sim.advance(8)
        frames = sim.radio_out()
        for i in range(12):  # 12 frames of 4 ms: more than the budget if any of it were charged
            lib.simSetRxArriving(1)
            sim.advance(4)
            frames += sim.radio_out()
            lib.simSetRxArriving(0)
            sim.radio_in(avail_data_req(bytes([0x44, i] + [8] * 6)))
            frames += [f for f in sim.radio_out() if parse_normal(f)["type"] == PKT_BLOCK_PART]
            sim.advance(5)
            frames += sim.radio_out()
        sim.advance(400)
        frames += sim.radio_out()
        check_burst(parts_of(frames), tag, data, 0, range(42))
        air = air_line(sim)
        assert air["yld"] >= 6 and air["ylf"] == air["yld"] and air["yms"] == 0, air
    finally:
        lib.simSetRxArriving(0)
        lib.simSetAirtime(0)


@test
def a_saturated_channel_is_not_waited_for():
    """On a channel in use a quarter of the time or more, waiting will not clear it, and the burst would
    only spend its budget every time. The radio already knows that (busy), so it does not try. It still
    gives way to a frame arriving, which is over soon whatever the channel is doing."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    lib.simSetAirtime(4)
    try:
        tag, ver, data = burst_setup(sim, 0x45)
        lib.simSetIdleRssi(-60)
        sim.advance(1000)  # a sample every 10 ms, against the 1300 ms of quiet burst_setup left: 43 % busy
        sim.pump_events()
        sim.radio_in(block_request(tag, ver, 0))
        sim.radio_out()
        sim.advance(8)
        assert parts_of(sim.radio_out()), "no wait"
        sim.advance(400)
        sim.radio_out()
        air = air_line(sim)
        assert air["yld"] == 0 and air["yms"] == 0, air
    finally:
        lib.simSetIdleRssi(-95)
        lib.simSetAirtime(0)


@test
def a_tag_that_leaves_when_it_has_the_block_is_listened_for():
    """A ZBS243 leaves its receive loop the moment it has the whole block and asks for the next one, so
    every repeat after that is airtime nobody reads - and the request it makes meanwhile goes out over
    those repeats and has to be retried. After the first pass the radio therefore listens for as long
    as such a tag has been seen to take, and only then sends repeats. That wait is measured, not set:
    0 until a tag has been seen (which is the old behaviour), then the shortest time a tag took."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    lib.simSetAirtime(4)
    try:
        tag, ver, data = burst_setup(sim, 0x3B, blocks=3)

        def first_pass(block):
            """ask for a block and run until all 42 parts have gone out once"""
            sim.radio_in(block_request(tag, ver, block))
            assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
            got = []
            for _ in range(400):
                sim.advance(1)
                got += parts_of(sim.radio_out())
                if len(got) >= 42:
                    break
            assert len(got) == 42, len(got)
            return got

        # block 0: nothing learned yet, so the repeats follow at once, as before
        first_pass(0)
        sim.advance(10)
        assert parts_of(sim.radio_out()), "no wait before anything has been seen"
        sim.advance(60)
        sim.radio_out()
        sim.pump_events()
        # block 1 asked for after the repeats: the first reading, long, because the tag is heard late
        first_pass(1)
        sim.advance(10)
        assert parts_of(sim.radio_out()) == [], "the radio listens after the first pass"
        # the tag has the block and asks for the next: heard on its first attempt, nothing repeated
        sim.radio_in(block_request(tag, ver, 2))
        assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
        served = [e for e in sim.pump_events() if e[0] == BLOCK_SERVED]
        assert [struct.unpack_from("<B", e[3], 16)[0] for e in served] == [1], served
        assert struct.unpack_from("<B", served[0][3], 18)[0] == 42, "block 1 went out once, no repeats"
        sim.advance(400)
        sim.radio_out()
        # the last block is answered with XFC, which the radio times separately
        sim.radio_in(normal(tag, AP_MAC, PKT_XFER_COMPLETE))
        sim.radio_out()
        air = air_line(sim)
        # 10 ms after the last part was handed to the radio, less the 4 it spent on the air: the time the
        # tag took, and not the long first reading
        assert 5 <= air["dn"] <= 7, air
        assert air["dx"] > 0, air
    finally:
        lib.simSetAirtime(0)


@test
def the_receive_flag_is_checked_against_the_frames_received():
    """A `yld` only means something if the flag it acts on is what Telink says it is, and a share of a few
    samples cannot say that: at one look per 10 ms a ~1 ms frame is seen one time in ten. So the flag is
    looked at every millisecond the radio listens, and set against what the frames it received predict:
    each holds it from sync to its last byte, (len + 3) * 32 us. fh and fe agree for a flag that is up
    for whole frames; fh below fe says it is up for part of them, 0 that it is dead, far above that it
    is stuck."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    tag, ver, data = burst_setup(sim, 0x3C)
    # 40 full check-ins. Each holds the flag from sync to its last byte, (len + 3) * 32 us. The simulated
    # flag is not tied to frames, so it is raised by hand for as long as they add up to, a millisecond
    # at a time.
    frames = [avail_data_req(bytes([0x5A, i] + [2] * 6)) for i in range(40)]
    for f in frames:
        sim.radio_in(f)
        sim.radio_out()
        sim.advance(20)
    # ... and 10 that failed their CRC: never handed to the AP, but they held the flag up just the same,
    # so their time belongs in fe - which is what tells a flag that rises early from corrupt frames
    for _ in range(10):
        lib.simRadioInBadCrc(40)
        sim.advance(20)
    br = block_request(tag, ver, 0)
    flag_ms = sum((len(f) + 3) * 32 for f in frames + [br]) / 1000 + 10 * (40 + 3) * 32 / 1000
    lib.simSetRxArriving(1)
    sim.advance(round(flag_ms))
    lib.simSetRxArriving(0)
    sim.radio_in(br)  # a burst, so the air line is due
    sim.advance(300)
    sim.radio_out()
    air = air_line(sim)
    # what the frames received predict - these, and the check-ins air_line() sends to keep windows
    # reported while it waits for the line, ~1.4 ms each
    extra = air["oth"] - len(frames)
    assert int(flag_ms) <= air["fe"] <= flag_ms + 1.4 * extra + 1, (air, flag_ms)
    assert abs(air["fh"] - flag_ms) <= 2, (air, flag_ms)  # ... and one look a millisecond saw that
    assert air["yld"] == 0, air
    rf = [l.split() for l in sim.logs if l.startswith("rf rx ")]
    assert rf and rf[-1][rf[-1].index("crc") + 1] == "10", sim.logs  # counted, since boot


@test
def a_one_block_image_learns_how_long_xfc_takes_and_does_not_wait_for_it():
    """What this installation does: every image fits one block, so a ZBS243's only "done" is XFC, and it
    sends it 0.5-1.5 s after the block - measured on hardware - after working on the whole image.
    That reading used to be thrown away for being longer than a receive window, so `dx` stayed 0
    through every transfer. It is kept now and reported. A wait that long leaves no repeat any room
    in the window, so the burst does not pause for it: its repeats go straight after the first pass,
    exactly as before anything was learned."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    lib.simSetAirtime(4)
    try:
        tag = bytes([0x3D] + [8] * 7)
        parts = list(range(10))
        for ver in (0xB01, 0xB02):
            data = make_file(1000)
            sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
            send_block(sim, ver, data, 0)
            sim.advance(1300)
            sim.radio_out()
            sim.radio_in(block_request(tag, ver, 0, parts=parts, partial=True))
            assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
            got = []
            for _ in range(300):
                sim.advance(1)
                got += parts_of(sim.radio_out())
                if len(got) >= 10:
                    break
            sim.advance(10)
            assert parts_of(sim.radio_out()), "the repeats follow at once: XFC is too slow to wait for"
            sim.advance(760)
            sim.radio_in(normal(tag, AP_MAC, PKT_XFER_COMPLETE))
            sim.radio_out()
            sim.pump_events()
        air = air_line(sim)
        assert air["dn"] == 0 and 750 <= air["dx"] <= 850, air
    finally:
        lib.simSetAirtime(0)


def timeline(sim, ms):
    """step the clock a millisecond at a time and return what went on air as (time, frame)"""
    out = []
    for _ in range(ms):
        sim.advance(1)
        out += [(sim.time(), dict(parse_normal(f), raw=f)) for f in sim.radio_out()]
    return out


@test
def a_tag_that_asks_during_another_burst_waits_its_turn_and_is_served():
    """One radio sends one burst at a time. A second tag used to be sent CANCEL_XFER for 1.2 s after any
    block request - on hardware 7 transfers in 110, each then waiting a whole check-in cycle, 40-110 s,
    for its image. Now it is acked with a wait that ends as the burst in front of it does, and its own
    burst follows: no cancel, and its tag listens only once the air is free."""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        a, b = bytes([0x41] + [8] * 7), bytes([0x42] + [8] * 7)
        da, db = make_file(4096, seed=1), make_file(4096, seed=2)
        for tag, ver, data in ((a, 0x1A, da), (b, 0x1B, db)):
            sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
            send_block(sim, ver, data, 0)
        sim.advance(1300)
        sim.radio_out()
        sim.radio_in(block_request(a, 0x1A, 0))
        assert parse_normal(sim.radio_out()[0])["type"] == PKT_BLOCK_REQUEST_ACK
        seen = timeline(sim, 60)
        assert any(f["dst"] == a and f["type"] == PKT_BLOCK_PART for _, f in seen), "a's burst is on"
        t_ack = sim.time()
        sim.radio_in(block_request(b, 0x1B, 0))
        out = [dict(parse_normal(f), raw=f) for f in sim.radio_out()]
        acks = [f for f in out if f["type"] == PKT_BLOCK_REQUEST_ACK and f["dst"] == b]
        assert acks, [f["type"] for f in out]
        pw = struct.unpack_from("<H", acks[0]["payload"], 1)[0]
        seen += [(sim.time(), f) for f in out if f["type"] == PKT_BLOCK_PART] + timeline(sim, 700)
        to_a = [tm for tm, f in seen if f["dst"] == a and f["type"] == PKT_BLOCK_PART]
        to_b = [tm for tm, f in seen if f["dst"] == b and f["type"] == PKT_BLOCK_PART]
        assert not any(f["type"] == PKT_CANCEL_XFER for _, f in seen)
        assert to_b and min(to_b) >= max(to_a), (max(to_a), min(to_b))  # one burst, then the other
        # b listens from the end of its wait, which counts from the end of the ack: its burst starts
        # BURST_MARGIN_MS after that. (Here the ack is 4 ms of simulated air and may wait for the part on
        # the air before it.)
        assert t_ack + pw <= min(to_b) <= t_ack + pw + 8 + 5 + 2, (t_ack, pw, min(to_b), max(to_a))
        check_burst([f["raw"] for _, f in seen if f["dst"] == a and f["type"] == PKT_BLOCK_PART], a, da, 0, range(42))
        check_burst([f["raw"] for _, f in seen if f["dst"] == b and f["type"] == PKT_BLOCK_PART], b, db, 0, range(42))
        served = [e for e in sim.pump_events() if e[0] == BLOCK_SERVED]
        assert [e[3][:8] for e in served] == [a, b], served
    finally:
        lib.simSetAirtime(0)


@test
def a_tag_that_asks_just_after_a_burst_is_served_at_once():
    """The case every one of the seven hardware cancels was: the other tag's one-block burst had already
    ended, 35-794 ms before. Nothing is on the air, so nothing waits."""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        a, b = bytes([0x43] + [8] * 7), bytes([0x44] + [8] * 7)
        for tag, ver in ((a, 0x2A), (b, 0x2B)):
            sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, 600, 0x20, 0, 0, 10, 0), expect=ACK)
            send_block(sim, ver, make_file(600), 0)
        sim.advance(1300)
        sim.radio_out()
        sim.radio_in(block_request(a, 0x2A, 0, parts=[0, 1, 2], partial=True))
        sim.advance(300)  # its six frames are long gone
        sim.radio_out()
        sim.advance(35)
        sim.radio_in(block_request(b, 0x2B, 0, parts=[0, 1, 2, 3, 4, 5], partial=True))
        ack = parse_normal(sim.radio_out()[0])
        assert ack["type"] == PKT_BLOCK_REQUEST_ACK and ack["dst"] == b, ack["type"]
        assert struct.unpack_from("<H", ack["payload"], 1)[0] == 0, "the plain cached wait, nothing in front"
    finally:
        lib.simSetAirtime(0)


@test
def the_first_tags_next_block_waits_behind_the_second_rather_than_being_cancelled():
    """Turn-taking goes both ways. While the second tag has the radio, the first one's next block request
    is the one that waits - before, whichever came second inside 1.2 s was cancelled, here that would be
    a tag in the middle of its image."""
    sim = Sim()
    start_v2(sim)
    lib.simSetAirtime(4)
    try:
        a, b = bytes([0x45] + [8] * 7), bytes([0x46] + [8] * 7)
        da, db = make_file(2 * 4096, seed=3), make_file(4096, seed=4)
        sim.request(SET_PENDING, a + struct.pack("<QIBBHHB", 0x3A, len(da), 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, 0x3A, da, 0)
        send_block(sim, 0x3A, da, 1)
        sim.request(SET_PENDING, b + struct.pack("<QIBBHHB", 0x3B, 4096, 0x20, 0, 0, 10, 0), expect=ACK)
        send_block(sim, 0x3B, db, 0)
        sim.advance(1300)
        sim.radio_out()
        sim.radio_in(block_request(a, 0x3A, 0))
        seen = timeline(sim, 50)
        sim.radio_in(block_request(b, 0x3B, 0))  # b waits for a's block 0
        seen += [(sim.time(), parse_normal(f)) for f in sim.radio_out()] + timeline(sim, 260)
        # a's block 0 is over and b's burst is due or running: a asks for block 1
        sim.radio_in(block_request(a, 0x3A, 1))
        out = [parse_normal(f) for f in sim.radio_out()]
        assert any(f["type"] == PKT_BLOCK_REQUEST_ACK and f["dst"] == a for f in out), [f["type"] for f in out]
        seen += [(sim.time(), f) for f in out] + timeline(sim, 900)
        assert not any(f["type"] == PKT_CANCEL_XFER for _, f in seen)
        parts = [(tm, f["dst"], f["payload"][1]) for tm, f in seen if f["type"] == PKT_BLOCK_PART]
        order = []
        for _, dst, blk in parts:
            if not order or order[-1] != (dst, blk):
                order.append((dst, blk))
        assert order == [(a, 0), (b, 0), (a, 1)], order
        served = [(e[3][:8], e[3][16]) for e in sim.pump_events() if e[0] == BLOCK_SERVED]
        assert served == [(a, 0), (b, 0), (a, 1)], served
    finally:
        lib.simSetAirtime(0)


@test
def a_tag_that_goes_quiet_after_its_burst_is_counted_by_its_timing():
    """Whether a tag got its block is read from the next frame it sends, so nothing has to be timed out:
    XFC, a request for the next block, or one for the parts it missed are all answers. A check-in or
    a ping first means it gave up on the transfer and came back on its next cycle - seen nowhere else,
    since no request round happened for `rereq` to count. Bursts after a wait of 0 (silz) and after any
    other wait (silw) are counted apart."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    zbs, other, zbs2 = bytes([0x51] + [8] * 7), bytes([0x52] + [8] * 7), bytes([0x53] + [8] * 7)
    vers = iter(range(0xC01, 0xC20))

    def burst(tag, cached=True):
        ver = next(vers)
        sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, 600, 0x20, 0, 0, 10, 0), expect=ACK)
        if cached:
            send_block(sim, ver, make_file(600), 0)
        sim.radio_in(block_request(tag, ver, 0, parts=[0, 1, 2], partial=True))
        if not cached:
            sim.pump_events()
            send_block(sim, ver, make_file(600), 0)
        sim.advance(400)
        sim.radio_out()
        sim.pump_events()
        return ver

    burst(zbs)
    sim.radio_in(avail_data_req(zbs))  # it checks in again instead of answering: silent, wait of 0
    burst(other)
    sim.radio_in(normal(other, AP_MAC, PKT_XFER_COMPLETE))  # answered
    burst(other, cached=False)
    sim.radio_in(bcast(other, PKT_PING))  # it lost the AP after this one: silent, after a wait
    ver = burst(zbs2)
    sim.radio_in(block_request(zbs2, ver, 0, parts=[1], partial=True))  # asked again for a part: answered
    sim.advance(400)
    sim.radio_in(avail_data_req(zbs2))  # ... and that burst it did not answer either: silent, wait of 0
    sim.radio_out()
    sim.pump_events()
    sim.idle(16000)
    logs = [l[3][1:].decode("utf-8", "replace") for l in sim.pump_events(ms=500) if l[0] == LOG]
    ci = [l.split() for l in logs if l.startswith("ci ")]
    assert ci, logs
    f = ci[-1]
    assert (f[f.index("silz") + 1], f[f.index("silw") + 1]) == ("2", "1"), ci[-1]
    assert len(" ".join(f)) < 98, f


@test
def a_stale_ping_is_still_answered():
    """A PING carries no hwType, and a tag that is pinging is one we have never had a full check-in
    from, so neither its family nor its reply window is known. Those windows run from 2 ms (ZBS243,
    Opticon) to 20 ms (EFR32xG22) - and every family calls radioRxFlush() once per channel scan and
    then leaves its receiver on, so a PONG that misses the window it was aimed at is still read on the
    next poll. The nRF52811 reads every PONG that way.

    So the limit is the widest window and nothing tighter. Refusing a ping costs the tag another scan
    attempt with its receiver on, and a scan round that fails leaves it on "No AP found", rescanning at
    20 x 2 ms of continuous receive per round for ever; answering costs 992 us of air."""
    sim = Sim()
    start_v2(sim)
    tag = bytes([0xE4] + [3] * 7)
    for age in (500, 2500, 5000, 10000, 19000, 19840):
        sim.radio_in(bcast(tag, PKT_PING), age_us=age)
        out = sim.radio_out()
        assert out, "a ping at %d us went unanswered" % age
        assert parse_normal(out[0])["type"] == PKT_PONG, age
    # beyond that the PONG cannot be placed inside even the widest window: 832 us of the tag's own
    # ping airtime plus 20 ms of window, less the 992 us the PONG itself takes on air
    sim.radio_in(bcast(tag, PKT_PING), age_us=19841)
    assert sim.radio_out() == []
    sim.radio_in(bcast(tag, PKT_PING), age_us=60000)
    assert sim.radio_out() == []
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[10] == 2, st  # the two that were refused, and only those


@test
def log_frames_only_when_enabled():
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x22] + [9] * 7)
    sim.radio_in(avail_data_req(tag), age_us=9000)
    sim.idle(61000)
    assert [f for f in sim.pump_events() if f[0] == LOG] == []
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    sim.radio_in(avail_data_req(tag), age_us=9000)
    sim.idle(61000)
    logs = [f for f in sim.pump_events() if f[0] == LOG]
    assert any(b"too late" in l[3] for l in logs), logs
    diag = [l[3][1:].decode() for l in logs if l[3][1:].startswith(b"ci ")]
    assert len(diag) == 1, logs  # one report: only the first 15 s had activity
    assert "late 1" in diag[0], diag
    for prefix in (b"xfer brq ", b"blk cxlb ", b"rf rx "):
        assert any(l[3][1:].startswith(prefix) for l in logs), (prefix, logs)
    for l in logs:
        assert len(l[3]) <= 101, l  # short lines
    # diagAdd() skips a field that will not fit without saying so, so the only way to see a line
    # outgrow the payload is to look for its last field. These are the ones that would go first.
    for prefix, last in ((b"ci ", b" late "), (b"xfer brq ", b" png "), (b"blk cxlb ", b" dw ")):
        line = [l[3][1:] for l in logs if l[3][1:].startswith(prefix)]
        assert line and last in line[-1], (prefix, last, line)


@test
def diagnostics_count_repeats_and_reply_time():
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    tag = bytes([0x23] + [9] * 7)
    sim.radio_in(avail_data_req(tag), age_us=800)
    sim.advance(6)
    sim.radio_in(avail_data_req(tag), age_us=1200)  # the tag asks again: it missed the reply
    sim.radio_out()
    sim.idle(16000)
    diag = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG and l[3][1:].startswith(b"ci ")]
    assert diag and diag[0].startswith("ci 2 data 0 rep 1 us 1000 max 1200 "), diag


@test
def the_transmit_power_never_moves():
    """Every frame goes out at the power the S3 configured, whatever the tag and however close it is:
    the per-tag controller that could lower it was removed unshipped (see "frames to the tags" in
    src/ap.c), and this is what a radio without it does - one power, set once."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 10, 1, 0), expect=ACK)
    tag = bytes([0x66] + [6] * 7)
    for i in range(30):
        sim.advance(500 if i % 7 else 50)  # some of these are repeats, which would reset a controller
        sim.radio_in(avail_data_req(tag), rssi=-40)  # as close by as a tag gets
        sim.radio_out()
        sim.pump_events()
        assert lib.simPower() == 10, (i, lib.simPower())
    sim.radio_in(bcast(tag, PKT_PING))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_PONG
    assert lib.simPower() == 10
    # and there is no controller to report on
    sim.pump_events()
    sim.idle(16000)
    logs = [l[3][1:].decode("utf-8", "replace") for l in sim.pump_events() if l[0] == LOG]
    assert not [l for l in logs if l.startswith("pwr ")], logs


@test_built_with("libap_debug.so")
def the_instrumented_build_reports_what_it_was_built_to_measure():
    """AP_DEBUG is one switch over every instrument, so one test over all of them: the lines they
    write, and the fact that they are written without the S3 having asked for LOG frames at all -
    which is what a bench needs and what the default build must never do (log_frames_only_when_enabled
    is the other half of that pair)."""
    sim = Sim()
    start_v2(sim)

    def drain_logs():
        # The ring holds about one report's worth of lines and the line carries one every
        # V2_LOG_GAP_MS, so each phase is read before the next one fills it.
        sim.idle(2000)
        return [l[3][1:].decode("utf-8", "replace") for l in sim.pump_events() if l[0] == LOG]

    tag = bytes([0x71] + [4] * 7)
    sim.idle(3000)  # the ping line is rate limited to one per 2 s, counted from a clock that starts at 0
    sim.pump_events()
    sim.radio_in(bcast(tag, PKT_PING))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_PONG
    # no SET_CONFIG anywhere in this test: an instrumented build logs whether it was asked to or not
    pinged = drain_logs()
    assert any(l.startswith("ping ") and " at " in l for l in pinged), pinged
    sim.radio_in(avail_data_req(tag))
    sim.radio_out()
    joined = drain_logs()
    assert any(l.startswith("joined ") and " last " in l for l in joined), joined
    assert any(l.startswith("fo ") and " cap " in l and " hi " in l for l in joined), joined
    window = []
    for _ in range(9):  # to the first diagnostics window, reading the queue as it fills
        window += drain_logs()
    assert any(l.startswith("rf rx ") for l in window), window
    for l in pinged + joined + window:
        assert len(l) <= 100, l  # a LOG payload is V2_LOG_MAX, and diagAdd drops a field in silence
@test
def one_request_from_the_s3_per_loop_pass():
    """The loop handles one of the S3's frames and goes back to the radio, rather than draining the
    serial ring to empty: the S3 sends the next chunk of a preload as soon as one is acknowledged, so
    draining to empty kept the radio unattended for a whole preload stream - 41 ms on hardware, where
    a check-in reply has 2.4. What is left in the ring waits for the next pass."""
    sim = Sim()
    start_v2(sim)
    sim.frames()
    seqs = [(sim.seq + 1) & 0xFF, (sim.seq + 2) & 0xFF, (sim.seq + 3) & 0xFF]
    sim.seq = seqs[-1]
    data = b"".join(frame(HEARTBEAT, q, 0, b"") for q in seqs)
    lib.simSerialIn(data, len(data))
    answered = []
    for want in seqs:
        lib.apLoop()
        got = [f[1] for f in sim.frames() if f[2] & F_RESPONSE]
        assert got == [want], (got, want)
        answered += got
    assert answered == seqs


@test
def a_stack_past_three_quarters_is_warned_about_once():
    """The stacks are 4/3 of the static bound the build computed for them (tools/stackdepth.py), so a
    mark past three quarters of one is deeper than the analysis said the code can go. The stack line
    that first carries such a mark goes out as a warning, with the sizes, in the stack line's own slot
    - the report stays at five lines - and only once: after that the line reports the mark as before."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    tag = bytes([0x7E] + [9] * 7)
    ver, data = 0x5455, make_file(4096)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    send_block(sim, ver, data, 0)

    def stk_lines():
        """the stack lines (level, text) of the next reports, until one comes; the hw line, due as
        its marks rise, takes turns with it"""
        for _ in range(6):
            run_block(sim, tag, ver, data, 0)
            got = []
            for _ in range(3):
                sim.idle(16000)
                got += [(l[3][0], l[3][1:].decode()) for l in sim.pump_events() if l[0] == LOG]
            report = [t for lv, t in got if t.startswith(("ci ", "xfer ", "blk ", "rf ", "hw ", "stk ", "air "))]
            assert len(report) <= 5 * 3, report
            stk = [(lv, t) for lv, t in got if t.startswith("stk ")]
            if stk:
                return stk
        raise AssertionError("no stack line")

    lib.simSetStacks(900, 100, 1248, 160)  # 72 % and 62 %: within the bound
    assert stk_lines() == [(3, "stk 900 irq 100 ")]
    lib.simSetStacks(940, 100, 1248, 160)  # 75.3 % of the main stack: past the bound
    (lv, line), = stk_lines()
    assert lv == 2 and line.startswith("stk 940 irq 100 of 1248 160 past 3/4"), (lv, line)
    assert len(line) <= 98, line
    lib.simSetStacks(1000, 121, 1248, 160)  # deeper still, the IRQ stack past too: no second warning
    assert stk_lines() == [(3, "stk 1000 irq 121 ")]


@test
def the_buffers_report_their_high_water_once_it_moves():
    """Every buffer whose size is a choice reports the most it has needed since boot, on one line
    that is sent only when a mark has risen - so the line says something whenever it appears, and a
    site that has settled pays nothing for it. Not alongside the boot facts, which would be seven
    lines where the outbound ring is sized for a report of six. The two stacks' marks follow the same
    rule on a line of their own, which also never shares a report with the hw line, so a report is at
    most five lines."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    tag = bytes([0x7D] + [9] * 7)
    ver, data = 0x5454, make_file(4096)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    send_block(sim, ver, data, 0)
    run_block(sim, tag, ver, data, 0)

    def report():
        """everything one diagnostics window sends, drained: LOG frames are spaced, so it takes more
        than the window itself"""
        lines = []
        for _ in range(3):
            sim.idle(16000)
            lines += [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
        return [l for l in lines if l.startswith("hw ")], lines

    hw, lines = report()
    assert any(l.startswith("boot ") for l in lines), lines
    assert hw == [], "never in the same report as the boot facts"
    assert not any(l.startswith("stk ") for l in lines), "nor is the stacks' line"

    run_block(sim, tag, ver, data, 0)
    hw, lines = report()
    assert len(hw) == 1, lines
    f = hw[0].split()
    assert f[5] == "in" and f[6].isdigit() and f[7].isdigit(), hw  # "in <part> <us>"
    assert f[8] == "rq" and f[9].isdigit() and f[10].isdigit(), hw  # "rq <type> <us>"
    f = f[:7] + f[8:10] + f[11:]  # leave the two unnamed times out of the pairs
    fields = dict(zip(f[1::2], f[2::2]))  # after the line's own "hw"
    assert set(fields) == {"urx", "lp", "in", "rq", "rxq", "evq", "obq", "logq", "lgd", "pend"}, hw
    assert int(fields["pend"]) == 1 and int(fields["evq"]) >= 1 and int(fields["logq"]) >= 1, hw
    # bytes of the ring: at least the LOG lines counted beside it, each at least its level and length
    assert 2 * int(fields["logq"]) <= int(fields["obq"]) <= RING_BYTES, hw
    assert int(fields["lgd"]) == 0, hw  # nothing in this test overfills the LOG queue
    assert len(hw[0]) <= 100, hw
    assert not any(l.startswith("stk ") for l in lines), "never in the same report as the hw line"

    # The stacks' marks, which were due in that report too and gave way to the hw line, come in the
    # next one with activity, on their own line and in place of the hw line: `stk <main> irq <irq>`.
    # The host's HAL measures nothing and says 0, which is still sent once, so a radio that reads 0
    # is seen to. Two 16-bit values make the line 20 characters at most, of the 98 a line may have.
    run_block(sim, tag, ver, data, 0)
    hw, lines = report()
    stk = [l for l in lines if l.startswith("stk ")]
    assert len(stk) == 1 and hw == [], lines
    g = stk[0].split()
    assert len(g) == 4 and g[0] == "stk" and g[2] == "irq" and g[1].isdigit() and g[3].isdigit(), stk
    assert int(g[1]) <= 0xFFFF and int(g[3]) <= 0xFFFF and len(stk[0]) <= 20, stk

    # The same transfer again raises no buffer's mark, so its report repeats none of them. `lp` is
    # left out of this: on the host it measures the simulator's jumps in time between passes, not
    # anything a pass did, so it may rise on its own and send the line. The stacks' 0s have not
    # risen either, so their line does not come again.
    run_block(sim, tag, ver, data, 0)
    hw, lines = report()
    assert not any(l.startswith("stk ") for l in lines), lines
    for l in hw:
        g = l.split()
        g = g[:7] + g[8:10] + g[11:]
        again = dict(zip(g[1::2], g[2::2]))
        for k in ("lp", "in", "rq"):  # simulator time, as above
            again.pop(k), fields.pop(k, None)
        assert again == fields, (again, fields)


# --- the SRAM the cache shares with the tag store -----------------------------------------------
# libap_split.so: an arena of 24672 bytes with a floor of 2 full blocks (8240), so the cache starts
# with room for 5 and 4072 beside them, and the store has 16432 bytes above the floor. (The fixed
# regions had 24592 beside their slots: the same 4072 beside five, and 16384 above the floor - the
# thresholds below are unchanged, and the 48 bytes more above the floor are one granule of offers more
# in the mass-update tests.) A tag costs 12 bytes while its family has
# a profile and 20 when it is a profile of its own, so this build reaches a threshold at 337, 673 and
# 1009 tags of a real fleet and at 197, 401 and 609 of strangers, and it holds the strangers it is
# guaranteed - 815, `tags` - with the cache at its floor. The firmware's own arena reaches its lower
# thresholds only on a fleet larger than its cap, which is why the mechanism is driven here.
SPLIT_LIB = "libap_split.so"


@test_built_with("libap_debug.so")
def the_arenas_maxima_are_derived_from_its_size():
    """The arena is whatever the link leaves, so none of its maxima is a constant: the cache works out
    its geometry once at boot from the bytes halArena() hands over, and the store's maxima follow from
    the room above the cache's floor of seven full blocks. This build has host_hal.c's default arena,
    54984 bytes, the release build's _cache_arena_size_ since blocks are stored at their own length, so
    these are the figures that image reports."""
    sim = Sim()
    r = start_v2(sim)
    cache_kb, slots = struct.unpack("<BBHB8sHHHH", r[3])[7:]
    # thirteen full blocks are 13 * 4120 = 53560, and the 1424 beside them hold one more block of
    # 1400 bytes: 13 * 4096 + 1400 = 54648 bytes of block data, 53 KB. (The fixed regions announced
    # 52, thirteen of 4 KB, and could not use the 1.4 KB at all.)
    assert cache_kb == 53 and full_blocks() == 13, cache_kb
    assert lib.cacheUsedKb() == 0
    assert lib.cacheListBytesMax() == 54984 - 7 * 4120  # 26144, above the floor of seven
    assert slots == 2176, slots  # (26144 - 32) / 12 = 2176 records, in whole granules of 16
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 20, 0xFF, 8, 0, 64), expect=ACK)
    cfg = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG and l[3][1:4] == b"cfg"]
    # tags: the cap, 1024, at 20 bytes a tag is 1028 * 8 + 1040 * 12 = 20704 in granules, and the
    # 5440 left are 340 offers
    assert cfg == ["cfg ch 20 pwr 8 lock 0 led 64 ckb 53 tags 1024 pend 2176 ofr 340 prof 0 "], cfg
    sim.request(ALLOW_ADD, site_mac(0), expect=ACK)
    assert full_blocks() == 13, "the store's first granule is 224 bytes, and 1424 were spare"
    # and nothing is lost by it that the S3 is told about: the 1200 bytes left beside thirteen full
    # blocks still hold 1176 of data, 54424 in all, which is still 53 KB (see the_cache_reports_
    # the_room_there_is for a store that does cost the S3 a kilobyte)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[7] == 0, st

def one_family(i):
    """MACs of one fleet: mac[4..7] shared, the low three a serial - so mac[1], which is the serial's
    middle byte here, makes a profile of every 256 tags: twelve bytes a tag, eight profiles at 2032"""
    return struct.pack("<Q", 0x032C00000000 | (0x100000 + i))


def allow_up_to(sim, n, have=0, mac=one_family):
    """add MACs until the list holds n of them, 30 per ALLOW_ADD as the S3 sends them"""
    while have < n:
        k = min(30, n - have)
        sim.request(ALLOW_ADD, b"".join(mac(i) for i in range(have, have + k)), expect=ACK)
        have += k
    return have


@test_built_with(SPLIT_LIB)
def the_allow_list_takes_cache_blocks_only_as_it_fills():
    """The split is dynamic, so a site pays for the tags it has rather than for the tags it could
    have. The cache starts with everything the arena holds and gives up room for a full block at each
    boundary the store crosses - and takes it back again when the list is cleared, because the S3
    re-sends the whole list after every HELLO and the radio must not be left poorer for it.

    What HELLO_REPLY announces is the maximum throughout; what the store has taken is in STATUS's
    used figure instead (the_cache_reports_the_room_there_is).

    The thresholds are the store's, re-derived from the allow-list's 4-and-8-byte ones: 12 bytes a
    record in granules of 16, beside one granule of profiles - 336 records and 32 bytes fit in the
    4072 beside five full blocks, 672 in the 8192 beside four, 1008 in the 12312 beside three. (The
    arena is 24672 for that: five blocks at 4120 and the 4072 the fixed regions had beside them.)"""
    sim = Sim()
    r = start_v2(sim)
    announced = struct.unpack("<BBHB8sHHHH", r[3])[7]
    assert announced == 23, announced  # five full blocks and 4048 bytes of a sixth: 24528 of data
    assert full_blocks() == 5
    assert lib.cacheListBytesMax() == 16432  # 24672 less the floor's two blocks at 4120

    n = allow_up_to(sim, 336)
    assert full_blocks() == 5, "336 tags still fit beside a full cache, at twelve bytes each"
    n = allow_up_to(sim, 337, n)
    assert full_blocks() == 4
    n = allow_up_to(sim, 672, n)
    assert full_blocks() == 4
    n = allow_up_to(sim, 673, n)
    assert full_blocks() == 3
    n = allow_up_to(sim, 1009, n)
    assert full_blocks() == 2, "the floor"
    # the cap, which sizes nothing and is the only thing that refuses a MAC here
    n = allow_up_to(sim, 1024, n)
    r = sim.request(ALLOW_ADD, one_family(9999), expect=NAK)
    assert r[3] == bytes([4])  # NO_SLOT, at the cap and nowhere before it
    assert lib.cacheKb() == 23, "the announced size does not move"

    sim.request(ALLOW_CLEAR, expect=ACK)
    assert full_blocks() == 5
    # and one tag at a time, as the S3 reports a deletion
    n = allow_up_to(sim, 673)
    assert full_blocks() == 3
    sim.request(ALLOW_REMOVE, one_family(672), expect=ACK)
    assert full_blocks() == 4


@test_built_with("libap_pend.so")
def the_pending_table_takes_room_as_tags_arrive_and_gives_it_back():
    """Pending entries are records and offers in the store, the arena's low-end tenant: they take room
    a granule at a time as tags get something pending, a cache block goes when the room is not there,
    and housekeeping gives the room back - and the block with it - once the entries are gone. What
    they may grow to is derived, and leaves the allow-list room to reach its cap in its widest
    encoding whatever they hold.

    Re-derived from the 26-byte table: these tags are not listed, so each costs a record and an offer,
    28 bytes - 140 of them and their seven profiles fit in the 4072 beside five blocks (granules of 16
    records and 4 offers)."""
    sim = Sim()
    r = start_v2(sim)
    slots = struct.unpack("<BBHB8sHHHH", r[3])[8]
    assert slots == 1360, slots  # (16432 - 32) / 12 records, in granules of 16
    assert full_blocks() == 5

    def pend(i):
        return site_mac(i) + struct.pack("<QIBBHHB", 0x6000 + i, 100, 0x20, 0, 0, 250, 0)

    for i in range(140):
        sim.request(SET_PENDING, pend(i), expect=ACK)
    assert full_blocks() == 5, "140 entries fit beside five blocks"
    sim.request(SET_PENDING, pend(140), expect=ACK)
    assert full_blocks() == 4

    # the list, in the same store, still answers to its cap - here with the widest tags there are
    allow_up_to(sim, 64, mac=stranger)
    r = sim.request(ALLOW_ADD, stranger(64), expect=NAK)
    assert r[3] == bytes([4])  # the cap, which is the only refusal
    sim.request(SET_PENDING, pend(141), expect=ACK)  # and the entries grow past it again
    for i in (0, 31, 63):
        sim.request(ALLOW_REMOVE, stranger(i), expect=ACK)
        sim.request(ALLOW_ADD, stranger(i), expect=ACK)

    # up to what the arena holds and no further: the list at its cap needs nothing more, so the
    # entries have everything else - room for 608 records (532 of these and the 64 strangers), 72
    # profiles (the strangers' and the seven families') and 532 offers is 16384 bytes, and the 48 more
    # this arena has above its floor are not the next granule of four offers
    n = 142
    while sim.request(SET_PENDING, pend(n))[0] == ACK:
        n += 1
    assert n == 532, n
    r = sim.request(SET_PENDING, pend(n), expect=NAK)
    assert r[3] == bytes([4]), r[3]  # NO_SLOT: "try at the next check-in", never the lock
    assert full_blocks() == 2, "the floor"
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(GET_STATS, expect=STATUS)[3])
    assert st[5] == n, st

    # every entry still resolves where the store moved its parts about underneath them
    for i in (0, 139, 140, 300, n - 1):
        sim.radio_in(avail_data_req(site_mac(i)))
        adi = parse_normal(sim.radio_out()[0])["payload"]
        sim.pump_events()
        assert struct.unpack_from("<Q", adi, 1)[0] == 0x6000 + i, i
    for i in (0, 32, 63):
        sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1 << 1, 0), expect=ACK)
        sim.radio_in(avail_data_req(stranger(i)))
        assert len(sim.radio_out()) == 1, i
        sim.pump_events()

    # gone again: housekeeping gives the room back, and the cache its blocks
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 0, 0), expect=ACK)
    sim.request(ALLOW_CLEAR, expect=ACK)
    for i in range(n):
        sim.request(CANCEL_PENDING, site_mac(i), expect=ACK)
    sim.idle(61000)
    assert full_blocks() == 5, full_blocks()


@test_built_with(SPLIT_LIB)
def a_block_the_allow_list_takes_goes_through_the_eviction_policy():
    """Which bytes of the arena the list needs is an address - the lowest block's, block 4 here; which
    BLOCK is lost for it is the eviction policy's decision (see cacheClaim), and the two are not the
    same block. The tag here has had block 0 and is walking upwards, so block 0 is the one worth least
    - and that is what goes, and the four below it move up into its place, not whichever block happens
    to sit at the list's edge. Giving a block to the list is an eviction like any other and is counted
    like one."""
    sim = Sim()
    start_v2(sim)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, 1, 0), expect=ACK)
    tag = bytes([0x7A] + [9] * 7)
    ver, data = 0x5151, make_file(5 * 4096)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for b in range(5):
        send_block(sim, ver, data, b)
    assert full_blocks() == 5
    assert run_block(sim, tag, ver, data, 0) == 0  # served, so the tag will not come back for it
    sim.advance(1300)
    # what the cache holds, on the line: five blocks, 20600 bytes of arena, 21 KB rounded up
    sim.idle(16000)
    before = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
    before = [l for l in before if l.startswith("blk cxlb ")]  # not the per-request 'blk N parts' line
    assert len(before) == 1 and " cb 5 cu 21 " in before[0], before

    allow_up_to(sim, 337)
    assert full_blocks() == 4
    # the blocks the tag has still to walk through are all there. Asked for back to back, with no
    # time passing, so no burst starts between them and the answers are the cache's alone.
    for b in (1, 2, 3, 4):
        assert ask_block(sim, tag, ver, b) == 0, b
        sim.pump_events()
    # and the one it was finished with is what the list was given
    assert ask_block(sim, tag, ver, 0) == 140
    sim.pump_events()

    sim.idle(16000)
    blk = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG]
    blk = [l for l in blk if l.startswith("blk cxlb ")]  # the window, not the per-request lines
    # one block thrown out, and it had already gone to its tag, so it cost that tag nothing
    assert len(blk) == 1 and "cev 1 cevu 0 " in blk[0], blk
    assert " cb 4 cu 17 " in blk[0], blk
    assert len(blk[0]) <= 100, blk[0]  # with the fields on it, the line still fits a LOG payload


@test_built_with(SPLIT_LIB)
def a_burst_survives_the_block_under_it_being_moved():
    """The bytes the list wants may be the ones holding the block a burst is reading, and that burst
    runs across main-loop passes - the same passes that handle the S3's ALLOW_ADDs. The block is
    therefore moved, with the blocks around it, to close the hole the evicted block left: its bytes
    move and its slot number does not, because ap.c holds that number across the whole burst
    (burstSlot, pinned), and a number that came to mean another block would send that block's bytes to
    the tag, checksummed and all.

    The check here is the burst's own data: every one of the 42 parts, against the file."""
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x7B] + [9] * 7)
    ver, data = 0x5252, make_file(5 * 4096)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for b in range(5):
        send_block(sim, ver, data, b)
    # block 4 went in last, so it is the lowest block, at the list's edge: the one in its way
    assert ask_block(sim, tag, ver, 4) == 0
    sim.pump_events()
    sim.advance(25)
    frames = sim.radio_out()
    assert frames, "the burst has started"

    allow_up_to(sim, 337)  # mid-burst, exactly as the S3 sends its database
    assert full_blocks() == 4
    # The pinned block is the one at the list's edge, so this is the moving path and not the easy one:
    # the arena was full, so the policy had to evict a block, and the one it chose - block 0, which the
    # tag (asking for 4) is past - is at the top, so every block below it moved up, the burst's with
    # them. Without that the test would still pass while proving nothing.
    assert lib.cacheEvictions() == 1

    sim.advance(400)
    sim.drain()
    frames += sim.radio_out()
    check_burst([f for f in frames if parse_normal(f)["type"] == PKT_BLOCK_PART], tag, data, 4, range(42))
    assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED]
    sim.advance(1300)
    assert ask_block(sim, tag, ver, 0) == 140, "block 0, at the top, is the one that went"
    sim.pump_events()


# Block lengths as this site's images have them: the S3 sent 131 blocks, median 7 parts (about 700
# bytes), mean 1061 bytes, the largest 28 parts (2.8 KB), none of them full. This is a cycle with that
# shape - median 712, mean 1031, 267 to 2742 - in the order the S3 might send them.
SITE_BLOCKS = [267, 366, 465, 564, 564, 663, 663, 663, 663, 762, 762, 861, 1059, 1257, 1653, 2049, 2544, 2742]


def block_cost(n):
    """what a block of n data bytes takes of the arena: the tag's header, the cache's 20, to 4"""
    return (n + 24 + 3) & ~3


def preload_one(sim, ver, size, seed):
    data = make_file(size, seed=seed)
    send_block(sim, ver, data, 0)
    return data


@test_built_with("libap_debug.so")
def the_sites_images_fit_four_times_as_many():
    """The reason blocks are stored at their own length. A fixed 4104-byte region held one of this
    site's blocks at 26 % on average, so the thirteen regions of the firmware's arena held thirteen
    images; at their own length the same 54984 bytes hold 53 of them (54316 bytes: their data and 24
    each), and the one that does not fit is the first eviction. Then one of them, from the middle of
    the arena, goes out to a tag byte for byte."""
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x7E] + [5] * 7)
    files = []
    for i in range(53):
        size = SITE_BLOCKS[i % len(SITE_BLOCKS)]
        if i == 30:  # offered to a tag, as the S3 preloads: SET_PENDING first
            sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 0x8000 + i, size, 0x20, 0, 0, 10, 0), expect=ACK)
        files.append(preload_one(sim, 0x8000 + i, size, i))
    assert sum(block_cost(len(f)) for f in files) == 54316
    assert lib.cacheBlocks() == 53 and lib.cacheEvictions() == 0, (lib.cacheBlocks(), lib.cacheEvictions())
    assert lib.cacheHeldBytes() == 54316
    # 54984 less the store's 288 for the one tag less the blocks leaves 380, under the next block's
    # 2768, so the oldest blocks, which nothing offers, go: the six oldest (292, 392, 492, 588, 588 and
    # 688 bytes) are the shortest run that makes the room, and the fifth of them is given back because
    # the other five already do - five evictions for one block, all of blocks no tag will ask for
    preload_one(sim, 0x8000 + 53, SITE_BLOCKS[53 % len(SITE_BLOCKS)], 53)
    assert lib.cacheEvictions() == 5 and lib.cacheBlocks() == 49, (lib.cacheEvictions(), lib.cacheBlocks())
    assert lib.cacheRoomBytes() == 54984 - 288 and lib.cacheHeldBytes() == 54316 - 2452 + 2768
    assert lib.cacheFind(ctypes.c_uint64(0x8004), 0) >= 0 and lib.cacheFind(ctypes.c_uint64(0x8005), 0) < 0
    # block 30 was stored in the middle of the arena and has moved up since; the tag gets it whole
    assert ask_block(sim, tag, 0x8000 + 30, 0) == 0
    sim.pump_events()
    sim.advance(25)
    sim.drain()
    check_burst(sim.radio_out(), tag, files[30], 0, range(42))


@test_built_with("libap_debug.so")
def full_blocks_are_no_worse_than_the_regions():
    """Images of full 4096-byte blocks gain nothing from blocks at their own length, and must lose
    nothing either. A full block costs 4120 now - 4096, the tag's 4-byte header and the cache's 20 -
    where a region was 4104 and its slot and region byte 25 more: 4129. So the firmware's arena holds
    the same thirteen, and with 1424 bytes beside them instead of the regions' 1304, the thirteenth
    lasts until the tag store passes 112 tags of a real fleet, where it went at 97 before."""
    sim = Sim()
    start_v2(sim)
    ver, data = 0x9100, make_file(14 * 4096)
    for b in range(13):
        send_block(sim, ver, data, b)
    assert lib.cacheBlocks() == 13 and lib.cacheEvictions() == 0
    assert lib.cacheHeldBytes() == 13 * 4120
    n = allow_up_to(sim, 112)  # 112 records and a granule of profiles: 1376 bytes
    assert lib.cacheEvictions() == 0 and lib.cacheBlocks() == 13
    allow_up_to(sim, 113, n)  # the next granule of 16 records is 1568
    assert lib.cacheEvictions() == 1 and lib.cacheBlocks() == 12
    # and a fourteenth needs a block to go, exactly one
    send_block(sim, ver, data, 13)
    assert lib.cacheEvictions() == 2 and lib.cacheBlocks() == 12


@test
def the_cache_gives_up_what_the_room_needs_in_policy_order():
    """With blocks of different sizes an eviction may need one block or several, and the policy's
    order decides which, not their size: what an eviction costs is a tag waiting 140 ms for the S3,
    the same for a small block as for a large one. Here the oldest block nothing offers is 267 bytes,
    the next one a full block, and a full block has to be made room for: the small one alone is not
    enough, so the full one goes too - and then the small one is given back, since the full one alone
    made the room."""
    sim = Sim()
    start_v2(sim)
    small = preload_one(sim, 0xA100, 267, 1)          # oldest: 292 bytes of arena
    preload_one(sim, 0xA101, 4096, 2)                 # next: 4120
    for i in range(5):
        preload_one(sim, 0xA110 + i, 4096, 3 + i)     # 20600 more: 25012, 3748 free
    assert lib.cacheEvictions() == 0 and lib.cacheHeldBytes() == 25012
    preload_one(sim, 0xA120, 4096, 9)                 # 372 short of 4120
    assert lib.cacheEvictions() == 1, lib.cacheEvictions()
    assert lib.cacheFind(ctypes.c_uint64(0xA100), 0) >= 0, "the small block is still there"
    assert lib.cacheFind(ctypes.c_uint64(0xA101), 0) < 0, "the full one went"
    assert lib.cacheHeldBytes() == 25012
    # and it is still served whole, from where the compaction moved it
    tag = bytes([0x7F] + [5] * 7)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 0xA100, 267, 0x20, 0, 0, 10, 0), expect=ACK)
    assert ask_block(sim, tag, 0xA100, 0) == 0
    sim.pump_events()
    sim.advance(25)
    sim.drain()
    check_burst(sim.radio_out(), tag, small, 0, range(42))


@test_built_with(SPLIT_LIB)
def a_short_block_moves_under_its_burst():
    """The burst survives its block being moved, with blocks of different sizes around it: a 700-byte
    last block is being sent while the tag store grows into the cache, the block the policy gives up for
    it is a 2742-byte one at the top of the arena, and closing that hole moves every block below it -
    the burst's among them - up by 2768 bytes. Every one of the 42 parts goes out as the file has them:
    the 700 bytes and the 0xFF after them."""
    sim = Sim()
    start_v2(sim)
    for i, size in enumerate((2742, 2544, 2049)):     # nothing offers these: they go first
        preload_one(sim, 0xB000 + i, size, i)
    tag = bytes([0x7D] + [9] * 7)
    ver, data = 0xB100, make_file(2 * 4096 + 700, seed=5)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for b in range(3):
        send_block(sim, ver, data, b)
    for i in range(4):
        preload_one(sim, 0xB010 + i, 1653, 10 + i)     # 1680 each
    # 7412 of unoffered blocks, the image's 4120 + 4120 + 724, the four 6720: 23096 of the 24384 the
    # store's 288 leave, so 1288 free
    assert lib.cacheHeldBytes() == 23096 and lib.cacheRoomBytes() == 24384
    assert lib.cacheEvictions() == 0 and lib.cacheBlocks() == 10
    assert run_block(sim, tag, ver, data, 0) == 0
    sim.advance(1300)
    assert run_block(sim, tag, ver, data, 1) == 0
    sim.advance(1300)
    assert ask_block(sim, tag, ver, 2) == 0
    sim.pump_events()
    sim.advance(25)
    frames = sim.radio_out()
    assert frames, "the burst has started"

    # mid-burst: 337 tags are 4320 bytes of store with the tag's offer, 4032 more than it had - 2744
    # more than was free, and the oldest block nothing offers is the 2742-byte one at the top, 2768
    allow_up_to(sim, 337)
    assert lib.cacheEvictions() == 1, lib.cacheEvictions()
    assert lib.cacheFind(ctypes.c_uint64(0xB000), 0) < 0, "the top block went"
    assert lib.cacheRoomBytes() == 24672 - 4320 and lib.cacheHeldBytes() == 23096 - 2768

    sim.advance(400)
    sim.drain()
    frames += sim.radio_out()
    check_burst([f for f in frames if parse_normal(f)["type"] == PKT_BLOCK_PART], tag, data, 2, range(42))
    assert [e[0] for e in sim.pump_events() if e[0] == BLOCK_SERVED] == [BLOCK_SERVED]
    for v in (0xB001, 0xB002, 0xB010, 0xB013):
        assert lib.cacheComplete(lib.cacheFind(ctypes.c_uint64(v), 0)), hex(v)


@test
def the_cache_reports_the_room_there_is():
    """HELLO_REPLY's cacheKb and STATUS's cacheUsedKb are the only two figures the S3 has, and all it
    does with them is cacheFreeKb = cacheKb - cacheUsedKb at every STATUS, take ceil(size / 4096) * 4
    KB off it for each item it preloads, and preload nothing that does not fit (V2Transport::
    wantPreload). Played here as the S3 plays it, a heartbeat a second:

    - full blocks are announced as fitting exactly when they do: 27 KB free takes an image of six
      (24720 bytes) and refuses one of seven (28840, 80 more than the arena);
    - images of 700 bytes, which the S3 counts at 4 KB each, are preloaded until under 4 KB of room is
      left - 35 of them, where six regions held six - and never into room that is not there: not one
      eviction on the way;
    - and the store's room is room the S3 is told is gone."""
    sim = Sim()
    r = start_v2(sim)
    kb = struct.unpack("<BBHB8sHHHH", r[3])[7]
    assert kb == 27

    def s3_free():
        return kb - struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])[7]

    assert s3_free() == 27
    assert 6 * 4 <= s3_free() < 7 * 4

    sent, free = 0, s3_free()
    while free >= 4:
        while free >= 4:
            preload_one(sim, 0xC000 + sent, 700, sent)
            sent += 1
            free -= 4
        free = s3_free()  # the next STATUS gives back what the 4 KB a block overstated
    assert sent == 35 and lib.cacheEvictions() == 0, (sent, lib.cacheEvictions())
    assert lib.cacheHeldBytes() == 35 * 724  # 25340: 3420 left, under a full block's 4120
    assert lib.cacheUsedKb() == 24, lib.cacheUsedKb()  # 27 less the 3396 bytes of data that still fit, 3 KB
    assert lib.cacheBlocks() == 35

    # the store: on a fresh link, 337 tags are 4256 bytes of it, and the free figure says so
    for v in range(sent):
        sim.request(DATA_DROP, struct.pack("<Q", 0xC000 + v), expect=ACK)
    assert s3_free() == 27
    allow_up_to(sim, 337)
    # 28760 - 4256 = 24504: five full blocks and 3880 bytes of a sixth, 24360 - 23 KB
    assert s3_free() == 23


@test
def a_block_whose_length_changes_is_started_again():
    """A block's length is fixed by its first chunk - the block's share of totalSize - and stored at
    that length. Chunks that imply another length (the S3 resending data whose size changed under the
    same version) start the block again at the new length, and the old block's bytes are freed rather
    than kept beside it; while a burst is reading the block it cannot change, and the chunk is refused."""
    sim = Sim()
    start_v2(sim)
    ver = 0xD100
    old = make_file(9000, seed=1)
    for off in range(0, 720, 240):  # three chunks of a full block 0
        sim.request(DATA_CHUNK, struct.pack("<QII", ver, off, len(old)) + old[off:off + 240], expect=ACK)
    assert lib.cacheBlocks() == 1 and lib.cacheHeldBytes() == 4120
    new = make_file(3000, seed=2)
    send_block(sim, ver, new, 0)
    assert lib.cacheBlocks() == 1 and lib.cacheHeldBytes() == block_cost(3000)  # 3024: the 4120 are free
    tag = bytes([0x7C] + [4] * 7)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(new), 0x20, 0, 0, 10, 0), expect=ACK)
    assert ask_block(sim, tag, ver, 0) == 0
    sim.pump_events()
    sim.advance(8)
    frames = sim.radio_out()
    assert frames, "the burst has started"
    # mid-burst the block is pinned: another length for it is refused, and the burst goes on
    r = sim.request(DATA_CHUNK, struct.pack("<QII", ver, 0, 5000) + old[:240], expect=NAK)
    assert r[3] == bytes([3])  # INVALID_PARAM
    sim.advance(400)
    sim.drain()
    frames += sim.radio_out()
    check_burst([f for f in frames if parse_normal(f)["type"] == PKT_BLOCK_PART], tag, new, 0, range(42))
    sim.pump_events()
    # and once it is over, the change is taken
    sim.request(DATA_CHUNK, struct.pack("<QII", ver, 0, 5000) + old[:240], expect=ACK)
    assert lib.cacheBlocks() == 1 and lib.cacheHeldBytes() == 4120
    assert not lib.cacheComplete(lib.cacheFind(ctypes.c_uint64(ver), 0))

@test_built_with(SPLIT_LIB)
def the_whole_database_is_accepted_with_the_cache_full():
    """The ordering a site actually produces: the S3 preloads for the tags it already knows while it
    is still sending its tag database, so every block the list needs has something in it. The list
    must still never be refused - one NAK NO_SLOT and the S3 stops using the radio-side lock until
    the link restarts, with one line in the web log - so the cache gives up three blocks here, one
    per threshold, and goes on serving from what is left. The tag with content here is not listed
    and holds an offer: room the guarantee was already keeping clear of."""
    sim = Sim()
    start_v2(sim)
    tag = bytes([0x7C] + [9] * 7)
    ver, data = 0x5353, make_file(5 * 4096)
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, len(data), 0x20, 0, 0, 10, 0), expect=ACK)
    for b in range(5):
        send_block(sim, ver, data, b)
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[7] == 20 and full_blocks() == 5, st

    # The whole database the store guarantees - `tags`, 815 - in 30-MAC requests, with every slot in
    # use throughout. Strangers, each a profile of its own at 20 bytes: the fleet the encoding cannot
    # share anything for, which is the one the guarantee is sized by, and it must still never be
    # refused. (The 4- and 8-byte allow-list took 2032 of them; the store's record carries the
    # pending entry too, which was 26 bytes more a tag elsewhere.)
    allow_up_to(sim, 815, mac=stranger)
    assert full_blocks() == 2, "the floor, with the widest list the guarantee covers"
    assert lib.cacheEvictions() == 3, lib.cacheEvictions()
    # Two blocks left, both still holding what they held - and the S3 is told the cache is full: the
    # floor's two blocks and the store's 16432 bytes are the whole arena, so cacheKb is all used. The
    # fixed-region cache said 8 here, and the S3 went on preloading into room the store had.
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(HEARTBEAT, expect=STATUS)[3])
    assert st[7] == 23 and lib.cacheBlocks() == 2, st

    # and the floor keeps working: the blocks that survived are served from cache, the rest from the S3
    waits = []
    for b in range(5):
        waits.append(ask_block(sim, tag, ver, b))
        sim.pump_events()
    assert waits.count(0) == 2, waits


@test_built_with("libap_debug.so")
def the_noise_histogram_reports_the_shape_of_the_channel():
    """The instrumented build WITHOUT the crystal sweep, which is the one `make AP_DEBUG=1` produces:
    the histogram and the trim aggregate share one slot on the 'rf' line and take turns by build, so
    the build above compiles the aggregate and this is the only one that compiles the histogram.

    Fed a known distribution rather than one fixed level, because one level exercises neither the
    bucket arithmetic nor the rule that matters most: a bucket holding any sample at all must read at
    least 1 and never 0, so that a brief strong burst is visible instead of rounding away. That rule
    is what a single occupancy fraction could not express, and it is what the trace level below
    tests."""
    sim = Sim()
    start_v2(sim)
    lines = []

    def harvest():
        lines.extend(l[3][1:].decode("utf-8", "replace") for l in sim.pump_events() if l[0] == LOG)

    harvest()

    def run(ms, step=5):
        # small steps so sampleNoise() actually fires - it is rate limited to one sample every 10 ms
        # of simulated time - with a heartbeat between chunks so the link does not fall back to ASCII
        end = sim.time() + ms
        while sim.time() < end:
            sim.advance(min(900, end - sim.time()), step)
            sim.request(HEARTBEAT, expect=STATUS)
            harvest()

    # Quiet for most of the window, a little traffic, and a trace of something very loud. It all has
    # to land inside ONE diagnostics window (15 s, and the first closes at about 16 s), because the
    # window is what the histogram describes.
    lib.simSetIdleRssi(-100)  # bucket 1: -102..-95
    run(11000)
    lib.simSetIdleRssi(-60)  # bucket 6: -62..-55
    run(2500)
    lib.simSetIdleRssi(-20)  # above the top edge, so it folds into bucket 7
    run(300)
    lib.simSetIdleRssi(-100)
    # A report is only sent for a window that saw radio activity, so give this one a ping - at the end
    # of the window, so the window it reports is the one the distribution was fed into.
    sim.radio_in(bcast(bytes([0x71] + [4] * 7), PKT_PING))
    assert parse_normal(sim.radio_out()[0])["type"] == PKT_PONG
    for _ in range(6):
        sim.idle(2000)
        harvest()
    rf = [l for l in lines if l.startswith("rf rx ")]
    assert rf, lines
    line = rf[-1]

    m = re.search(r" nf ([0-9A-F]{8}) ", line)
    assert m, line
    buckets = [int(c, 16) for c in m.group(1)]
    # the quiet level dominates, and it is bucket 1 and not bucket 0: -100 dBm is not the floor
    assert buckets[1] >= 10, (buckets, line)
    assert buckets[0] == 0, (buckets, line)
    # the traffic level lands where its dBm says and nowhere else
    assert buckets[6] >= 1, (buckets, line)
    # and the trace level is VISIBLE although it is far too small to earn a sixteenth of the window:
    # ~10 samples out of ~1500 is 0.1 sixteenths, which must still report 1
    assert buckets[7] >= 1, (buckets, line)
    # nothing in between, so no sample was filed in a bucket its dBm does not belong to
    assert buckets[2:6] == [0, 0, 0, 0], (buckets, line)
    # the window is one window's worth: the digits are sixteenths, so they cannot total far over 16
    assert sum(buckets) <= 18, (buckets, line)

    # the aggregate the histogram displaced is NOT here, and the fields the overrun used to eat are:
    # with both on the line it measured 116 characters against a 99-character LOG payload, so diagAdd
    # was dropping udmaf and the tail of udma in silence
    assert " cap " not in line, line
    assert " udma " in line and " udmaf " in line, line
    assert len(line) <= 100, (len(line), line)

    # busy is the decision input and is NOT derived from the buckets: -80 dBm is not a bucket edge, so
    # it counts the -60 and -20 samples and not the -100 ones, whatever the buckets say
    busy = int(re.search(r" busy (\d+) ", line).group(1))
    assert 1 <= busy <= 30, (busy, line)


# --- the tag store: exactness, profiles, unowned records, content at scale -------------------------
def answered(sim, tag):
    """with the lock on: does a check-in of this tag get a reply?"""
    sim.advance(1300)
    sim.request(HEARTBEAT, expect=STATUS)  # as the S3 does every second, or the link drops to ASCII
    sim.radio_out()
    sim.radio_in(avail_data_req(tag))
    got = len(sim.radio_out()) == 1
    sim.pump_events()  # or the event queue fills and the next request goes unanswered
    return got


def lock(sim, mode):
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 0xFF, 0xFF, 0x7F, mode << 1, 0), expect=ACK)


def checkin_reply(sim, tag):
    """the AvailDataInfo a check-in of this tag is answered with: (dataVer, dataType, nextCheckIn)"""
    sim.radio_in(avail_data_req(tag))
    adi = parse_normal(sim.radio_out()[0])["payload"]
    sim.pump_events()
    ver, size, dtype, darg, nci = struct.unpack_from("<QIBBH", adi, 1)
    return ver, dtype, nci


def pend_content(sim, tag, ver, size=4096, ttl=10):
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", ver, size, 0x20, 0, 0, ttl, 0), expect=ACK)


def idle_entry(sim, tag, ttl):
    sim.request(SET_PENDING, tag + struct.pack("<QIBBHHB", 0, 0, 0, 0, 30, ttl, 0), expect=ACK)


@test
def the_record_identifies_a_whole_mac():
    """A record is three of the MAC's bytes and a profile index, and that is only worth having if it is
    exact: two MACs share a record only if they share the three bytes and the profile, and sharing the
    profile means sharing the other five. This is the test of that claim, a byte at a time, because the
    failure it would otherwise hide is the worst kind - a tag wrongly refused looks exactly like a tag
    that has wandered off, and a tag wrongly accepted is the inventory lock quietly not working.

    The MAC is one of this installation's, and each twin differs from it in ONE byte: in mac[0],
    mac[2] or mac[3], which are the record's key, or in mac[1] or mac[4..7], which are the profile's."""
    sim = Sim()
    start_v2(sim)
    a = printed("0000032CA1B23E17")

    def twin(pos):
        b = bytearray(a)
        b[pos] ^= 0x01
        return bytes(b)

    key_twins = {p: twin(p) for p in (0, 2, 3)}
    profile_twins = {p: twin(p) for p in (1, 4, 5, 6, 7)}
    sim.request(ALLOW_CLEAR, expect=ACK)
    sim.request(ALLOW_ADD, a, expect=ACK)
    lock(sim, 1)
    assert answered(sim, a)
    for p, t in list(key_twins.items()) + list(profile_twins.items()):
        assert not answered(sim, t), "differs from a listed tag in mac[%d] only" % p

    # a twin in the key and a twin in the profile, beside the original
    sim.request(ALLOW_ADD, key_twins[2] + profile_twins[1], expect=ACK)
    assert answered(sim, a) and answered(sim, key_twins[2]) and answered(sim, profile_twins[1])
    for p in (0, 3):
        assert not answered(sim, key_twins[p]), p
    for p in (4, 5, 6, 7):
        assert not answered(sim, profile_twins[p]), p

    # each has its own pending entry, and a check-in is answered with its own
    for n, t in enumerate((a, key_twins[2], profile_twins[1])):
        pend_content(sim, t, 0x7100 + n)
    lock(sim, 0)
    for n, t in enumerate((a, key_twins[2], profile_twins[1])):
        sim.advance(1300)
        assert checkin_reply(sim, t)[0] == 0x7100 + n, n

    # removal has to pick the right one of records that share all but a byte
    lock(sim, 1)
    sim.request(ALLOW_REMOVE, a, expect=ACK)
    assert not answered(sim, a)
    assert answered(sim, key_twins[2]) and answered(sim, profile_twins[1]), "removing a took a twin with it"


@test
def a_fleet_with_more_profiles_than_a_byte_holds_is_still_exact():
    """A profile index is a byte in the record's key word and, past 256 profiles, a second byte in the
    record (profHi), so a fleet whose every tag shares nothing with the others - a profile each, the
    widest a tag is, 20 bytes - is still stored exactly and never refused. These 300 strangers have the
    SAME three key bytes, so stranger 5 and stranger 261 have equal key words, profiles 5 and 261:
    only the high byte tells them apart, and the lookup has to look at it."""
    sim = Sim()
    start_v2(sim)
    allow_up_to(sim, 300, mac=stranger)
    sim.request(SET_CONFIG, struct.pack("<BBbBB", 20, 0xFF, 8, 1 | 1 << 1, 64), expect=ACK)
    cfg = [l[3][1:].decode() for l in sim.pump_events() if l[0] == LOG and l[3][1:4] == b"cfg"]
    assert cfg and cfg[0].endswith(" prof 300 "), cfg  # a profile a tag: the fleet does not fit the key
    for i in (0, 5, 255, 256, 261, 299):
        assert answered(sim, stranger(i)), i
    assert not answered(sim, stranger(300))

    sim.request(ALLOW_REMOVE, stranger(5), expect=ACK)
    assert not answered(sim, stranger(5))
    assert answered(sim, stranger(261)), "same key word, the other profile"
    sim.request(ALLOW_REMOVE, stranger(261), expect=ACK)
    sim.request(ALLOW_ADD, stranger(5), expect=ACK)  # into the profile slot that came free
    assert answered(sim, stranger(5)) and not answered(sim, stranger(261))


@test_built_with(SPLIT_LIB)
def records_nobody_owns_are_the_first_given_up():
    """A pending entry that lapses leaves a record that owns nothing, unless the tag is listed. It is the
    first thing given up when room is needed.

    330 listed tags leave six records free in the 336 that fit beside five blocks; six idle entries take
    them and lapse; six more listed tags take them back rather than the store growing into the cache."""
    sim = Sim()
    start_v2(sim)
    allow_up_to(sim, 330)
    for i in range(6):
        idle_entry(sim, one_family(5000 + i), 1)
    assert full_blocks() == 5, "they cost nothing"
    sim.idle(61000)  # their entries lapse; the records are still there
    allow_up_to(sim, 336, have=330)
    assert full_blocks() == 5, "the list took the unowned records, not a block"
    allow_up_to(sim, 337, have=336)
    assert full_blocks() == 4


@test
def a_record_nobody_owns_is_kept_for_one_housekeeping_round():
    """...and it does not stay for ever: a record that owned nothing at one housekeeping round and still
    owns nothing at the next goes, and the room goes back to the cache. Sixteen records fit in a granule,
    so the seventeenth tag is what costs the store a second one."""
    sim = Sim()
    start_v2(sim)
    sim.request(ALLOW_ADD, site_mac(0), expect=ACK)
    small = lib.cacheRoomBytes()
    for i in range(1, 17):
        idle_entry(sim, site_mac(i), 1)
    large = lib.cacheRoomBytes()
    assert large < small
    sim.idle(61000)  # their entries lapse
    assert lib.cacheRoomBytes() == large
    sim.idle(61000)  # they are marked
    assert lib.cacheRoomBytes() == large, "kept for one round, for a SET_PENDING on its way"
    sim.idle(61000)  # and given up
    assert lib.cacheRoomBytes() == small, "given up after a whole round owning nothing"


def site_asleep(sim, n):
    """n of this site's tags as a settled site has them: listed, each with an idle entry"""
    allow_up_to(sim, n, mac=site_mac)
    for i in range(n):
        sim.request(SET_PENDING, site_mac(i) + struct.pack("<QIBBHHB", 0, 0, 0, 0, 30, 270, 0), expect=ACK)


def offer_all(sim, tags, base):
    """content for each of these tags at once; returns the ones refused, each with NO_SLOT"""
    refused = []
    for i in tags:
        r = sim.request(SET_PENDING, site_mac(i) + struct.pack("<QIBBHHB", base + i, 23000, 0x20, 0, 0, 30, 0))
        if r[0] == NAK:
            assert r[3] == bytes([4]), r[3]
            refused.append(i)
        else:
            assert r[0] == ACK, r
    return refused


@test_built_with(SPLIT_LIB)
def a_mass_update_offers_what_fits_and_the_rest_is_retried():
    """A template change gives every tag of a site content at once. 300 tags asleep cost a record
    each, 12 bytes; content takes an offer, 16 more, out of the room the list does not need - and the
    list keeps 20 bytes for each of the 515 tags still to come to its guarantee (815). So 152 offers
    are taken - in granules of four, and the 16432 bytes above this arena's floor hold one granule more
    than the fixed regions' 16384 did - and the rest are NO_SLOT, which the S3 retries at each tag's
    next check-in. A refused tag keeps its idle entry - it is told to sleep its 30 minutes, not
    "nothing, check in again now" - and as transfers complete, the retries are taken."""
    sim = Sim()
    start_v2(sim)
    N = 300
    site_asleep(sim, N)
    assert full_blocks() == 5, "300 sleeping tags beside the full cache"
    refused = offer_all(sim, range(N), 0x9000)
    assert refused == list(range(152, N)), (len(refused), refused[:3])
    st = struct.unpack("<IBBbBHHHIIHHHHB", sim.request(GET_STATS, expect=STATUS)[3])
    assert (st[5], st[6]) == (152, N - 152), st
    assert checkin_reply(sim, site_mac(151))[:2] == (0x9000 + 151, 0x20)
    sim.advance(1300)
    assert checkin_reply(sim, site_mac(N - 1)) == (0, 0, 30), "a refused tag keeps the entry it had"

    # ten transfers complete; the S3's retries for ten of the refused tags are taken
    for i in range(10):
        sim.advance(1300)
        sim.radio_in(normal(site_mac(i), AP_MAC, PKT_XFER_COMPLETE))
        sim.radio_out()
        sim.pump_events()
    assert offer_all(sim, refused[:10], 0x9000) == []
    sim.advance(1300)
    assert checkin_reply(sim, site_mac(refused[0]))[0] == 0x9000 + refused[0]
    assert offer_all(sim, refused[10:11], 0x9000) == refused[10:11], "and full again"

    # tags joining now are still listed: the room for them was never the offers'. (They share the
    # site's profiles, so each needs 8 bytes less than the guarantee held for it, and that is room for
    # content again: the 20 bytes are a promise to strangers, not what a real fleet is charged.)
    allow_up_to(sim, 10, mac=lambda i: site_mac(N + i))
    assert offer_all(sim, refused[10:11], 0x9000) == []


@test_built_with(SPLIT_LIB)
def the_list_is_never_refused_with_the_store_full_of_content():
    """The asymmetry the arena is built around, with the store as full of content as it will go: an
    offer refused costs a retry at the tag's next check-in, an ALLOW_ADD refused costs the inventory
    lock until the link restarts. So with every offer the store will take in place, the list must
    still reach its guarantee - here with strangers, the widest tags there are, 20 bytes each, which
    is what the room was held at - and the cache gives up its blocks to the floor for it."""
    sim = Sim()
    start_v2(sim)
    site_asleep(sim, 300)
    refused = offer_all(sim, range(300), 0xA000)
    assert len(refused) == 148, len(refused)
    allow_up_to(sim, 515, mac=stranger)  # 815 listed: `tags`
    assert full_blocks() == 2, "the floor"
    # the content is all still there, and still offered
    for i in (0, 77, 147):
        sim.advance(1300)
        assert checkin_reply(sim, site_mac(i))[0] == 0xA000 + i, i
    lock(sim, 1)
    for i in (0, 514):
        assert answered(sim, stranger(i)), i


def main():
    failed = 0
    for t in TESTS:
        # every test starts from a fresh firmware state
        global lib
        lib = _fresh_lib(t.__name__, getattr(t, "lib_so", "libap.so"))
        try:
            t()
            print("ok   %s" % t.__name__)
        except Exception as e:
            failed += 1
            import traceback
            traceback.print_exc()
            print("FAIL %s: %s" % (t.__name__, e))
    print("%d of %d passed" % (len(TESTS) - failed, len(TESTS)))
    return 1 if failed else 0


def _fresh_lib(name, so="libap.so"):
    # dlopen() returns the library already loaded for a path, with its state: load a copy
    import shutil
    path = os.path.join(HERE, "build", "run_%s.so" % name)
    shutil.copyfile(os.path.join(HERE, "build", so), path)
    l = ctypes.CDLL(path)
    os.unlink(path)
    _setup(l)
    return l


def _setup(l):
    l.apLoop.restype = ctypes.c_uint32
    l.simTime.restype = ctypes.c_uint32
    l.simBaud.restype = ctypes.c_uint32
    l.simChannel.restype = ctypes.c_uint8
    l.simPower.restype = ctypes.c_int8
    l.simLed.restype = ctypes.c_uint8
    l.simSerialOut.restype = ctypes.c_size_t
    l.cacheBlocks.restype = ctypes.c_uint8
    l.cacheRoomBytes.restype = ctypes.c_uint32
    l.cacheHeldBytes.restype = ctypes.c_uint32
    l.cacheUsedKb.restype = ctypes.c_uint16
    l.cacheComplete.restype = ctypes.c_bool
    l.cacheKb.restype = ctypes.c_uint16
    l.cacheListBytesMax.restype = ctypes.c_uint16
    l.cacheEvictions.restype = ctypes.c_uint16


if __name__ == "__main__":
    sys.exit(main())
