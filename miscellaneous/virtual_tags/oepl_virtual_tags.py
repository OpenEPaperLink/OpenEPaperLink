#!/usr/bin/env python3
"""Virtual OpenEPaperLink tags, simulated as a second AP over the UDP sync protocol.

The real AP sees the virtual tags as external tags: it generates their content and offers the
data back over UDP, the simulator downloads it via /getdata, saves images as PNG and reports
xfer complete. See DESIGN.md. Standard library only.

Usage:
  python oepl_virtual_tags.py add 00FFEE0000000001 --hwtype 02
  python oepl_virtual_tags.py list
  python oepl_virtual_tags.py run --ap 192.168.178.198 --local-ip 192.168.178.247

While running, commands are read from stdin (one per line), so a scenario can be piped in:
  press <mac> <1|2|3>   check in with a button wakeup reason
  reboot <mac>          check in with wakeup reason "boot"
  drop <mac>            stop checking in (and stop answering data) for this tag
  resume <mac>          start checking in again
  timeout <mac>         report a transfer timeout for this tag
  checkin <mac>         check in now
  sleep <seconds>       wait (for piped scenarios)
  quit
"""

import argparse
import json
import os
import re
import socket
import struct
import sys
import threading
import time
import urllib.request
import zlib

GROUP = "239.10.0.1"
PORT = 16033

PKT_AVAIL_DATA_REQ = 0xE5   # AP -> sim: data available (pendingData)
PKT_AVAIL_DATA_INFO = 0xE6  # sim -> AP: check-in (espAvailDataReq)
PKT_XFER_COMPLETE = 0xEA
PKT_XFER_TIMEOUT = 0xED

WAKEUP_TIMED, WAKEUP_BOOT = 0, 1
WAKEUP_BUTTON = {1: 4, 2: 5, 3: 6}
WAKEUP_FIRSTBOOT = 0xFC

DATATYPE_IMG_RAW_1BPP, DATATYPE_IMG_RAW_2BPP, DATATYPE_IMG_ZLIB, DATATYPE_IMG_G5 = 0x20, 0x21, 0x30, 0x31

HERE = os.path.dirname(os.path.abspath(__file__))
TAGTYPES = os.path.join(HERE, "..", "..", "resources", "tagtypes")


def log(msg):
    print(time.strftime("%H:%M:%S"), msg, flush=True)


def mac_bytes(mac_hex):
    """The AP prints macs most significant byte first; on the wire byte 0 comes first."""
    return bytes.fromhex(mac_hex)[::-1]


def mac_hex(mac):
    return mac[::-1].hex().upper()


def with_checksum(body):
    """addCRC(): byte 0 is the sum of the other bytes."""
    return bytes([sum(body[1:]) & 0xFF]) + body[1:]


def load_tagtype(hwtype):
    with open(os.path.join(TAGTYPES, "%02X.json" % hwtype), encoding="utf-8") as f:
        text = re.sub(r"(?m)^\s*//.*$", "", f.read())
    return json.loads(text)


# ---------------------------------------------------------------- PNG output


def write_png(path, width, height, rgb_rows):
    def chunk(kind, data):
        c = struct.pack(">I", len(data)) + kind + data
        return c + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF)

    raw = b"".join(b"\x00" + bytes(row) for row in rgb_rows)
    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)


def decode_image(data, data_type, tagtype):
    """Return (width, height, rgb rows) in buffer orientation, or None if unsupported."""
    if data_type == DATATYPE_IMG_ZLIB:
        payload = zlib.decompress(data[4:])
        header_size = payload[0]
        planes = payload[5]
        data = payload[header_size:]
        data_type = DATATYPE_IMG_RAW_2BPP if planes == 2 else DATATYPE_IMG_RAW_1BPP
    if data_type not in (DATATYPE_IMG_RAW_1BPP, DATATYPE_IMG_RAW_2BPP):
        return None
    w, h = tagtype["width"], tagtype["height"]
    if tagtype.get("rotatebuffer", 0) % 2:
        w, h = h, w
    plane = (w * h + 7) // 8
    palette = list(tagtype.get("colortable", {"white": [255] * 3, "black": [0] * 3}).values())
    while len(palette) < 4:
        palette.append(palette[-1])
    two = data_type == DATATYPE_IMG_RAW_2BPP and len(data) >= 2 * plane
    rows = []
    for y in range(h):
        row = []
        for x in range(w):
            i = y * w + x
            bit = 7 - (i % 8)
            b1 = (data[i // 8] >> bit) & 1 if i // 8 < len(data) else 0
            b2 = (data[plane + i // 8] >> bit) & 1 if two else 0
            row.extend(palette[b1 | (b2 << 1)][:3])
        rows.append(row)
    return w, h, rows


# ---------------------------------------------------------------- simulator


class Simulator:
    def __init__(self, tags, ap, local_ip, interval, outdir):
        self.tags = tags  # mac hex -> {"hwtype": int, ...}
        self.ap = ap
        self.local_ip = local_ip
        self.interval = interval
        self.outdir = outdir
        self.state = {mac: {"next": 0, "wakeup": WAKEUP_FIRSTBOOT, "dropped": False} for mac in tags}
        self.lock = threading.Lock()
        self.running = True

        self.rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.rx.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.rx.bind(("", PORT))
        mreq = socket.inet_aton(GROUP) + socket.inet_aton(local_ip)
        self.rx.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        self.rx.settimeout(1)

        self.tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 1)
        self.tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(local_ip))

    def send(self, packet):
        self.tx.sendto(packet, (GROUP, PORT))

    def checkin(self, mac, wakeup=None):
        tag = self.tags[mac]
        st = self.state[mac]
        if wakeup is None:
            wakeup = st["wakeup"]
        st["wakeup"] = WAKEUP_TIMED
        # struct AvailDataReq: checksum, lqi, rssi, temperature, batteryMv, hwType, wakeupReason,
        # capabilities, tagSoftwareVersion, currentChannel, customMode, reserved[8]
        adr = struct.pack("<BBbbHBBBHBB8s", 0, 100, -60, 21, tag.get("battery", 3000), tag["hwtype"], wakeup,
                          tag.get("capabilities", 0), tag.get("swversion", 0x10), 25, 0, bytes(8))
        adr = with_checksum(adr)
        body = with_checksum(b"\x00" + mac_bytes(mac) + adr)
        self.send(bytes([PKT_AVAIL_DATA_INFO]) + body)
        log("%s check-in (wakeup 0x%02X)" % (mac, wakeup))

    def xfer(self, mac, kind):
        body = with_checksum(b"\x00" + mac_bytes(mac))
        self.send(bytes([kind]) + body)
        log("%s %s" % (mac, "xfer complete" if kind == PKT_XFER_COMPLETE else "xfer timeout"))

    def handle_data(self, payload):
        # struct pendingData: AvailDataInfo (checksum, dataVer, dataSize, dataType, dataTypeArgument,
        # nextCheckIn), attemptsLeft, targetMac[8]
        if len(payload) < 27:
            return
        _, ver, size, dtype, arg, next_checkin, _attempts = struct.unpack_from("<BQIBBHH", payload)
        mac = mac_hex(payload[19:27])
        if mac not in self.tags or self.state[mac]["dropped"]:
            return
        log("%s data available: type 0x%02X arg 0x%02X size %d nextCheckIn %d ver %016X" % (mac, dtype, arg, size, next_checkin, ver))
        threading.Thread(target=self.fetch, args=(mac, ver, size, dtype, next_checkin), daemon=True).start()

    def fetch(self, mac, ver, size, dtype, next_checkin):
        url = "http://%s/getdata?mac=%s&md5=%016X" % (self.ap, mac, ver)
        try:
            data = urllib.request.urlopen(url, timeout=15).read()
        except Exception as e:
            log("%s download failed: %s" % (mac, e))
            return
        name = os.path.join(self.outdir, "%s_%s" % (mac, time.strftime("%H%M%S")))
        try:
            img = decode_image(data, dtype, load_tagtype(self.tags[mac]["hwtype"]))
        except Exception as e:
            img = None
            log("%s decode failed: %s" % (mac, e))
        if img:
            write_png(name + ".png", *img)
            log("%s image saved: %s.png (%d bytes)" % (mac, name, len(data)))
        else:
            with open(name + ".bin", "wb") as f:
                f.write(data)
            log("%s data saved: %s.bin (%d bytes)" % (mac, name, len(data)))
        self.xfer(mac, PKT_XFER_COMPLETE)
        with self.lock:
            # minutes, or seconds when bit 15 is set; 0 = tag default
            wait = (next_checkin & 0x7FFF) if next_checkin & 0x8000 else next_checkin * 60
            self.state[mac]["next"] = time.time() + (wait or self.interval)

    def receiver(self):
        while self.running:
            try:
                packet, sender = self.rx.recvfrom(2048)
            except socket.timeout:
                continue
            except OSError:
                break
            if sender[0] == self.local_ip or not packet:
                continue  # our own multicast
            if packet[0] == PKT_AVAIL_DATA_REQ:
                self.handle_data(packet[1:])

    def scheduler(self):
        while self.running:
            now = time.time()
            with self.lock:
                due = [m for m, st in self.state.items() if not st["dropped"] and now >= st["next"]]
                for m in due:
                    self.state[m]["next"] = now + self.interval
            for m in due:
                self.checkin(m)
            time.sleep(0.5)

    def command(self, line):
        parts = line.split()
        if not parts:
            return True
        cmd, args = parts[0].lower(), parts[1:]
        if cmd == "quit":
            return False
        if cmd == "sleep":
            time.sleep(float(args[0]))
            return True
        mac = args[0].upper() if args else None
        if mac not in self.tags:
            log("unknown tag %s" % mac)
            return True
        if cmd == "press":
            self.checkin(mac, WAKEUP_BUTTON[int(args[1])])
        elif cmd == "reboot":
            self.checkin(mac, WAKEUP_BOOT)
        elif cmd == "checkin":
            self.checkin(mac)
        elif cmd == "drop":
            self.state[mac]["dropped"] = True
            log("%s dropped" % mac)
        elif cmd == "resume":
            self.state[mac]["dropped"] = False
            self.state[mac]["next"] = 0
        elif cmd == "timeout":
            self.xfer(mac, PKT_XFER_TIMEOUT)
        else:
            log("unknown command %s" % cmd)
        return True

    def run(self, duration):
        threading.Thread(target=self.receiver, daemon=True).start()
        threading.Thread(target=self.scheduler, daemon=True).start()
        end = time.time() + duration if duration else None
        try:
            if sys.stdin.isatty() and end is None:
                for line in sys.stdin:
                    if not self.command(line):
                        break
            else:
                if not sys.stdin.isatty():
                    for line in sys.stdin:
                        if not self.command(line):
                            break
                while end is None or time.time() < end:
                    time.sleep(0.5)
        except KeyboardInterrupt:
            pass
        self.running = False
        self.rx.close()


# ---------------------------------------------------------------- cli


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--state", default=os.path.join(HERE, "vtags.json"), help="file with the virtual tags")
    sub = p.add_subparsers(dest="cmd", required=True)
    a = sub.add_parser("add", help="define a virtual tag")
    a.add_argument("mac")
    a.add_argument("--hwtype", default="02", help="tag type (hex), default 02 (M2 4.2\" BWR)")
    a.add_argument("--swversion", default="10", help="tag firmware version (hex); low = raw images")
    r = sub.add_parser("remove", help="forget a virtual tag (delete it in the AP web UI too)")
    r.add_argument("mac")
    sub.add_parser("list")
    run = sub.add_parser("run")
    run.add_argument("--ap", required=True, help="IP address of the AP")
    run.add_argument("--local-ip", required=True, help="IP address of this PC on the AP's network")
    run.add_argument("--interval", type=int, default=60, help="seconds between check-ins (default 60)")
    run.add_argument("--duration", type=int, default=0, help="stop after this many seconds (0 = until quit)")
    run.add_argument("--out", default=os.path.join(HERE, "images"), help="directory for received images")
    args = p.parse_args()

    tags = {}
    if os.path.exists(args.state):
        with open(args.state, encoding="utf-8") as f:
            tags = json.load(f)

    if args.cmd in ("add", "remove"):
        mac = args.mac.upper()
        if not re.fullmatch(r"[0-9A-F]{16}", mac):
            sys.exit("mac must be 16 hex digits")
        if args.cmd == "add":
            load_tagtype(int(args.hwtype, 16))  # check that the tag type exists
            tags[mac] = {"hwtype": int(args.hwtype, 16), "swversion": int(args.swversion, 16)}
        else:
            tags.pop(mac, None)
        with open(args.state, "w", encoding="utf-8") as f:
            json.dump(tags, f, indent=1)
        print("%d virtual tag(s)" % len(tags))
    elif args.cmd == "list":
        for mac, t in tags.items():
            print(mac, "hwtype %02X" % t["hwtype"], "swversion %X" % t.get("swversion", 0x10))
    elif args.cmd == "run":
        if not tags:
            sys.exit("no virtual tags, use 'add' first")
        os.makedirs(args.out, exist_ok=True)
        Simulator(tags, args.ap, args.local_ip, args.interval, args.out).run(args.duration)


if __name__ == "__main__":
    main()
