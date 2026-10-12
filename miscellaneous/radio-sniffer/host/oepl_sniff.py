#!/usr/bin/env python3
"""Host side of the OEPL radio sniffer (see ../firmware).

Reads frames from the sniffer over its USB serial port, decodes the OpenEPaperLink packets,
measures how fast the AP answers each tag request and how block bursts are timed, and writes a
pcap file for Wireshark (LINKTYPE_IEEE802_15_4_NOFCS).

Times are on-air times from the sniffer's SFD timestamps:
  reply time = end of the tag's request -> end of the AP's reply
That is what a tag needs to fit in its receive window (2 ms ping on ZBS243, 5-6 ms otherwise).

Example:
  python oepl_sniff.py --port COM11 --channel 25 --duration 600 --pcap run1.pcap
"""

import argparse
import collections
import statistics
import struct
import sys
import time

import serial

US_PER_BYTE = 32  # 250 kbit/s O-QPSK

PKT = {
    0xE1: "TAG_RETURN_DATA", 0xE2: "TAG_RETURN_DATA_ACK", 0xE3: "AVAIL_DATA_SHORTREQ",
    0xE4: "BLOCK_REQUEST", 0xE5: "AVAIL_DATA_REQ", 0xE6: "AVAIL_DATA_INFO",
    0xE7: "BLOCK_PARTIAL_REQUEST", 0xE8: "BLOCK_PART", 0xE9: "BLOCK_REQUEST_ACK",
    0xEA: "XFER_COMPLETE", 0xEB: "XFER_COMPLETE_ACK", 0xEC: "CANCEL_XFER",
    0xED: "PING", 0xEE: "PONG",
}
TAG_TO_AP = {0xE1, 0xE3, 0xE4, 0xE5, 0xE7, 0xEA, 0xED}
# request type -> (reply types that end the wait, label, tag windows in ms to check against)
REPLIES = {
    0xED: ({0xEE}, "ping", (2, 5)),
    0xE5: ({0xE6}, "checkin", (5,)),
    0xE3: ({0xE6}, "checkin", (5,)),
    0xE4: ({0xE9, 0xE8, 0xEC}, "blockreq", (6, 7)),
    0xE7: ({0xE9, 0xE8, 0xEC}, "blockreq", (6, 7)),
    0xEA: ({0xEB}, "xfc", (6,)),
    0xE1: ({0xE2}, "returndata", (5,)),
}
OEPL_PANS = {0x4447, 0x1337}
REPLY_TIMEOUT_US = 100_000
SESSION_GAP_US = 300_000
BLOCK_PART_DATA = 99
BLOCK_MAX_PARTS = 42


def mac_str(b):
    """MAC as the OEPL web UI shows it (last byte first)."""
    return b[::-1].hex().upper()


def decode(frame):
    """Decode an 802.15.4 frame (FCS included). Returns a dict or None."""
    if len(frame) < 5:
        return None
    fcf = frame[0] | (frame[1] << 8)
    ftype = fcf & 7
    pan_comp = (fcf >> 6) & 1
    dst_mode = (fcf >> 10) & 3
    src_mode = (fcf >> 14) & 3
    d = {"ftype": ftype, "seq": frame[2], "dst": None, "src": None, "pan": None, "type": None, "payload": b""}
    off = 3
    try:
        if dst_mode:
            d["pan"] = struct.unpack_from("<H", frame, off)[0]
            off += 2
            n = 8 if dst_mode == 3 else 2
            d["dst"] = bytes(frame[off:off + n])
            off += n
        if src_mode:
            if not pan_comp or not dst_mode:
                if d["pan"] is None:
                    d["pan"] = struct.unpack_from("<H", frame, off)[0]
                off += 2
            n = 8 if src_mode == 3 else 2
            d["src"] = bytes(frame[off:off + n])
            off += n
    except struct.error:
        return None
    body = frame[off:-2]  # strip FCS
    if ftype == 1 and body:
        d["type"] = body[0]
        d["payload"] = bytes(body[1:])
    return d


class Reader:
    """Splits the sniffer's byte stream into records, resyncing on the magic bytes."""

    def __init__(self, ser):
        self.ser = ser
        self.buf = bytearray()
        self.bad = 0

    def records(self):
        while True:
            chunk = self.ser.read(4096)
            if chunk:
                self.buf += chunk
            while True:
                i = self.buf.find(b"\xA5\x5A")
                if i < 0:
                    del self.buf[:-1]
                    break
                if i:
                    del self.buf[:i]
                if len(self.buf) < 5:
                    break
                rtype = self.buf[2]
                plen = self.buf[3] | (self.buf[4] << 8)
                if plen > 200:
                    del self.buf[:2]
                    self.bad += 1
                    continue
                if len(self.buf) < 6 + plen:
                    break
                payload = bytes(self.buf[5:5 + plen])
                if (sum(payload) & 0xFF) != self.buf[5 + plen]:
                    del self.buf[:2]
                    self.bad += 1
                    continue
                del self.buf[:6 + plen]
                yield rtype, payload
            if not chunk:
                yield None, None


class Pcap:
    def __init__(self, path):
        self.f = open(path, "wb")
        self.f.write(struct.pack("<IHHiIII", 0xA1B2C3D4, 2, 4, 0, 0, 65535, 230))

    def write(self, t, frame):
        data = frame[:-2]
        self.f.write(struct.pack("<IIII", int(t), int((t % 1) * 1e6), len(data), len(data)))
        self.f.write(data)

    def close(self):
        self.f.close()


class Analyzer:
    def __init__(self, verbose):
        self.verbose = verbose
        # A session is one exchange as the tag experiences it: its requests of one kind (retries
        # included, until a gap of SESSION_GAP_US) and the AP's replies to them.
        self.open = {}  # (tag, label) -> session dict
        self.sessions = collections.defaultdict(list)  # label -> closed sessions
        self.hwtype = {}
        self.ap_macs = set()
        self.ap_seq = {}
        self.ap_seq_gaps = 0
        self.types = collections.Counter()
        self.foreign = 0  # frames from other 802.15.4 networks on this channel
        self.blocks = []  # finished bursts
        self.burst = {}  # tag -> current burst
        self.requested = {}  # tag -> set of requested parts

    def end_us(self, ts, frame_len):
        return ts + (1 + frame_len) * US_PER_BYTE

    def feed(self, ts, rssi, frame):
        d = decode(frame)
        if not d or d["type"] is None or d["pan"] not in OEPL_PANS:
            self.foreign += 1
            return
        t = d["type"]
        self.types[PKT.get(t, hex(t))] += 1
        end = self.end_us(ts, len(frame))

        if t in TAG_TO_AP:
            tag = d["src"]
            if tag is None:
                return
            if t in (0xE5,) and len(d["payload"]) >= 7:
                self.hwtype[tag] = d["payload"][6]
            self.close_burst(tag)  # before `requested` changes: it belongs to the burst that just ended
            if t in (0xE4, 0xE7) and len(d["payload"]) >= 17:
                # blockRequest: checksum, ver[8], blockId, type, requestedParts[6]
                bits = d["payload"][11:17]
                self.requested[tag] = {i for i in range(BLOCK_MAX_PARTS) if bits[i // 8] & (1 << (i % 8))}
            label = REPLIES[t][1]
            s = self.open.get((tag, label))
            if s and ts - s["last_req"] < SESSION_GAP_US:
                s["requests"].append(end)
                s["last_req"] = ts
            else:
                if s:
                    self.sessions[label].append(s)
                self.open[(tag, label)] = {"tag": tag, "reply_types": REPLIES[t][0], "requests": [end],
                                           "replies": [], "last_req": ts}
            if self.verbose:
                print(f"{ts / 1e6:12.6f}  tag {mac_str(tag)} -> {PKT.get(t, hex(t))} rssi {rssi}")
            return

        # AP -> tag
        ap, tag = d["src"], d["dst"]
        if ap is not None:
            self.ap_macs.add(ap)
            last = self.ap_seq.get(ap)
            if last is not None and (d["seq"] - last) & 0xFF not in (1,):
                self.ap_seq_gaps += 1
            self.ap_seq[ap] = d["seq"]
        if tag is None:
            return
        for (stag, label), s in self.open.items():
            if stag == tag and t in s["reply_types"] and end - s["requests"][0] <= REPLY_TIMEOUT_US:
                s["replies"].append(end)
                break
        if t == 0xE9 and len(d["payload"]) >= 3:
            please_wait = struct.unpack_from("<H", d["payload"], 1)[0]
            self.close_burst(tag)
            self.burst[tag] = {"ack": ts, "wait_ms": please_wait, "parts": [], "first": None, "last": None}
        elif t == 0xE8 and len(d["payload"]) >= 3:
            b = self.burst.setdefault(tag, {"ack": None, "wait_ms": None, "parts": [], "first": None, "last": None})
            if b["first"] is None:
                b["first"] = ts
            b["last"] = end
            b["parts"].append((ts, d["payload"][2]))
        if self.verbose:
            print(f"{ts / 1e6:12.6f}    AP -> {mac_str(tag)} {PKT.get(t, hex(t))}")

    def close_burst(self, tag):
        b = self.burst.pop(tag, None)
        if not b or not b["parts"]:
            return
        req = self.requested.get(tag, set())
        seen, complete_at = set(), None
        for ts, part in b["parts"]:
            seen.add(part)
            if complete_at is None and req and req <= seen:
                complete_at = ts
        after = sum(1 for ts, _ in b["parts"] if complete_at is not None and ts > complete_at)
        b.update({
            "tag": tag,
            "frames": len(b["parts"]),
            "distinct": len(seen),
            "requested": len(req),
            "missing": len(req - seen),
            "start_vs_wait_ms": None if b["ack"] is None or b["wait_ms"] is None
            else (b["first"] - b["ack"]) / 1000 - b["wait_ms"],
            "duration_ms": (b["last"] - b["first"]) / 1000,
            "after_complete": after,
        })
        self.blocks.append(b)

    def summary(self, status):
        out = []
        out.append("=== OEPL sniffer summary ===")
        if status:
            out.append(f"sniffer during this run: received {status['received']}, dropped in sniffer queue {status['dq']}, "
                       f"dropped on USB {status['du']}, channel {status['channel']}")
        out.append("packets: " + ", ".join(f"{k} {v}" for k, v in self.types.most_common()))
        out.append(f"frames from other 802.15.4 networks (ignored): {self.foreign}")
        out.append(f"AP sequence gaps (AP did not send, or sniffer missed): {self.ap_seq_gaps}")
        out.append("")
        out.append("exchanges (a tag's requests of one kind incl. retries, and the AP's replies):")
        out.append("  first reply = end of the tag's first request -> end of the AP's first reply (ms)")
        out.append("  in window W = a reply ended within W ms after the end of one of the tag's requests")
        for label in ("ping", "checkin", "blockreq", "xfc", "returndata"):
            ss = self.sessions.get(label, []) + [s for (_, l), s in self.open.items() if l == label]
            if not ss:
                continue
            windows = next(w for r, (_, l, w) in REPLIES.items() if l == label)
            answered = [s for s in ss if s["replies"]]
            nreq = [len(s["requests"]) for s in ss]
            extra = sum(max(0, len(s["replies"]) - 1) for s in ss)
            line = (f"  {label:10s} n={len(ss):4d} unanswered={len(ss) - len(answered):3d}"
                    f"  requests per exchange: mean {statistics.mean(nreq):.2f} max {max(nreq)}"
                    f"  extra replies {extra}")
            out.append(line)
            if answered:
                ms = sorted((s["replies"][0] - s["requests"][0]) / 1000 for s in answered)
                p = lambda q: ms[min(len(ms) - 1, int(q * len(ms)))]
                line = f"  {'':10s} first reply median {statistics.median(ms):6.2f}  p95 {p(0.95):6.2f}  max {ms[-1]:6.2f}"
                for w in windows:
                    ok = sum(1 for s in answered
                             if any(0 < r - q <= w * 1000 for r in s["replies"] for q in s["requests"]))
                    line += f"  in {w} ms window: {ok}/{len(ss)}"
                out.append(line)
        if self.blocks:
            out.append("")
            out.append("block bursts:")
            for b in self.blocks:
                start = "?" if b["start_vs_wait_ms"] is None else f"{b['start_vs_wait_ms']:+.1f}"
                out.append(f"  tag {mac_str(b['tag'])}: wait {b['wait_ms']} ms, start vs wait {start} ms, "
                           f"{b['frames']} frames ({b['distinct']} distinct, {b['requested']} requested, "
                           f"{b['missing']} requested not seen), {b['duration_ms']:.1f} ms, "
                           f"{b['after_complete']} frames after all requested parts were sent")
        if self.hwtype:
            out.append("")
            out.append("tags seen (hwType): " + ", ".join(f"{mac_str(m)}={h:02X}" for m, h in self.hwtype.items()))
        return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", help="sniffer serial port (not needed with --replay)")
    ap.add_argument("--replay", help="analyse a pcap written earlier by this script instead of live data")
    ap.add_argument("--channel", type=int, default=25)
    ap.add_argument("--duration", type=float, default=0, help="seconds, 0 = until Ctrl-C")
    ap.add_argument("--pcap", help="write frames to this pcap file")
    ap.add_argument("--sync-mac", help="toggle the sniffer's sync GPIO on frames to/from this tag (MAC as in the web UI)")
    ap.add_argument("--summary-every", type=float, default=0, help="print a summary every N seconds")
    ap.add_argument("-v", "--verbose", action="store_true", help="print every OEPL packet")
    args = ap.parse_args()

    if args.replay:
        analyzer = Analyzer(args.verbose)
        with open(args.replay, "rb") as f:
            magic, _, _, _, _, _, linktype = struct.unpack("<IHHiIII", f.read(24))
            if magic != 0xA1B2C3D4 or linktype != 230:
                sys.exit("not a pcap from this script (expected LINKTYPE 230, microsecond timestamps)")
            while hdr := f.read(16):
                sec, usec, incl, _ = struct.unpack("<IIII", hdr)
                frame = f.read(incl) + b"\0\0"  # decode() expects the FCS the sniffer delivers
                analyzer.feed(sec * 1_000_000 + usec, 0, frame)
        for tag in list(analyzer.burst):
            analyzer.close_burst(tag)
        print(analyzer.summary(None))
        return
    if not args.port:
        sys.exit("--port is required (or use --replay)")

    ser = serial.Serial()
    ser.port = args.port
    ser.baudrate = 115200
    ser.timeout = 0.2
    ser.dtr = False
    ser.rts = False
    ser.open()
    # The sniffer keeps up to 16 KB of records buffered while nobody reads the port; those are
    # from before this run. Read and drop them before starting.
    drain_until = time.time() + 0.5
    while time.time() < drain_until:
        ser.read(4096)
    ser.write(f"C{args.channel}\n".encode())
    if args.sync_mac:
        ser.write(("M" + bytes.fromhex(args.sync_mac)[::-1].hex() + "\n").encode())
    else:
        ser.write(b"M\n")

    reader = Reader(ser)
    analyzer = Analyzer(args.verbose)
    pcap = Pcap(args.pcap) if args.pcap else None
    status = None
    status0 = None
    t0_host, t0_dev = None, None
    start = time.time()
    last_summary = start
    try:
        for rtype, payload in reader.records():
            now = time.time()
            if rtype == 1:
                ts, rssi, lqi, ch, flen = struct.unpack_from("<QbBBB", payload)
                frame = payload[12:12 + flen]
                if t0_host is None:
                    t0_host, t0_dev = now, ts
                if pcap:
                    pcap.write(t0_host + (ts - t0_dev) / 1e6, frame)
                analyzer.feed(ts, rssi, frame)
            elif rtype == 2:
                ts, rec, dq, du, ch, sync = struct.unpack_from("<QIIIBB", payload)
                if status0 is None:
                    status0 = (rec, dq, du)  # counters at the start of this run
                status = {"received": rec - status0[0], "dq": dq - status0[1], "du": du - status0[2],
                          "channel": ch, "sync": sync}
            elif rtype == 3:
                print(f"sniffer firmware v{payload[0]}, channel {payload[1]}", file=sys.stderr)
            if args.summary_every and now - last_summary >= args.summary_every:
                last_summary = now
                print(analyzer.summary(status), flush=True)
            if args.duration and now - start >= args.duration:
                break
    except KeyboardInterrupt:
        pass
    finally:
        for tag in list(analyzer.burst):
            analyzer.close_burst(tag)
        if pcap:
            pcap.close()
        ser.close()
    print(analyzer.summary(status))
    if reader.bad:
        print(f"(corrupt records skipped: {reader.bad})")


if __name__ == "__main__":
    main()
