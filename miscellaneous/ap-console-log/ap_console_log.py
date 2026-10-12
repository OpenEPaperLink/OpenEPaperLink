#!/usr/bin/env python3
"""Long-running log of the AP's USB console, counting serial-protocol errors (measurement M3, stage A).

The S3 prints errors in the S3 <-> radio protocol only to its USB console, not to log.txt, and it
echoes the radio's debug UART there too. This script logs that console with timestamps and counts
the known error messages, without changing any firmware.

Opening an ESP32 port normally resets it through DTR/RTS; this script clears both before opening,
so the AP keeps running.

Example (Yellow AP on COM7, overnight):
  python ap_console_log.py --port COM7 --hours 10 --out ../../measurements/console-2026-10-12
"""

import argparse
import collections
import re
import time

import serial

# message -> what it means (see docs/design/radio-chain-analysis.md)
PATTERNS = {
    "block send failed": "S3: no ACK> to >D> (block data not sent)",
    "Failed sending block": "S3: block data not sent",
    "SDA send failed": "S3: SDA> retry (no ACK>)",
    "SDA failed to send": "S3: SDA> given up after 5 tries",
    "CXD send failed": "S3: CXD> retry",
    "CXD failed to send": "S3: CXD> given up",
    "SCP send failed": "S3: SCP> retry",
    "SCP failed to send": "S3: SCP> given up",
    "Failed CRC on a blockrequest": "S3: RQB> payload checksum wrong",
    "wait... tx busy": "S3: transmit lock contention",
    "UART Timeout": "radio: inter-character timeout in a command",
    "receive buffer full": "radio: 802.15.4 receive buffer full",
    "modem stuck": "S3: radio reset after 'receive buffer full'",
    "Serial monitor stopped because of flooding": "S3: stopped echoing the radio's debug UART",
    "ping failed": None,  # handled below: "ping" ... " failed"
}
PING_FAIL = re.compile(r"ping\s*failed")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", required=True, help="the AP's USB serial port (S3)")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--hours", type=float, default=8)
    ap.add_argument("--out", required=True, help="base path: <out>.log (full console) and <out>-summary.txt")
    ap.add_argument("--summary-every", type=float, default=60, help="minutes between intermediate summaries")
    args = ap.parse_args()

    def open_port():
        s = serial.Serial()
        s.port = args.port
        s.baudrate = args.baud
        s.timeout = 0.5
        s.dtr = False
        s.rts = False
        s.open()
        return s

    ser = open_port()
    reconnects = 0

    counts = collections.Counter()
    first_seen, last_seen = {}, {}
    lines = 0
    start = time.time()
    next_summary = start + args.summary_every * 60
    next_flush = start + 5
    buf = b""
    log = open(args.out + ".log", "a", encoding="utf-8", errors="replace")

    def summary():
        hours = (time.time() - start) / 3600
        out = [f"{time.strftime('%Y-%m-%d %H:%M:%S')}  {hours:.2f} h logged, {lines} lines, "
               f"port reopened {reconnects}x"]
        for key, meaning in PATTERNS.items():
            if key == "ping failed":
                key, meaning = "ping failed", "S3: RDY? ping to the radio failed"
            n = counts.get(key, 0)
            if n:
                out.append(f"  {n:6d}  {key:45s} {meaning}  (first {first_seen[key]}, last {last_seen[key]})")
        if len(out) == 1:
            out.append("  no known error messages")
        return "\n".join(out)

    try:
        while time.time() - start < args.hours * 3600:
            try:
                buf += ser.read(4096)
            except (serial.SerialException, OSError):
                # the AP rebooted (nightly reboot) and its USB port went away for a moment
                try:
                    ser.close()
                except Exception:
                    pass
                time.sleep(2)
                try:
                    ser = open_port()
                    reconnects += 1
                    log.write(f"{time.strftime('%H:%M:%S')} --- serial port reopened (AP restart?) ---\n")
                except (serial.SerialException, OSError):
                    pass
                continue
            while b"\n" in buf:
                raw, buf = buf.split(b"\n", 1)
                text = raw.decode("utf-8", errors="replace").rstrip("\r")
                stamp = time.strftime("%H:%M:%S")
                log.write(f"{stamp} {text}\n")
                lines += 1
                for key in PATTERNS:
                    hit = PING_FAIL.search(text) if key == "ping failed" else key in text
                    if hit:
                        counts[key] += 1
                        first_seen.setdefault(key, stamp)
                        last_seen[key] = stamp
            if time.time() >= next_flush:  # so the log can be read while it runs
                next_flush = time.time() + 5
                log.flush()
            if time.time() >= next_summary:
                next_summary += args.summary_every * 60
                log.flush()
                print(summary(), flush=True)
    except KeyboardInterrupt:
        pass
    finally:
        log.close()
        ser.close()
    text = summary()
    print(text)
    with open(args.out + "-summary.txt", "w", encoding="utf-8") as f:
        f.write(text + "\n")


if __name__ == "__main__":
    main()
