#!/usr/bin/env python3
"""Analyse a baseline run: the sniffer's pcap and the PPK2 trace of one tag, aligned through D0.

The sniffer toggles PPK2 logic input D0 once for every frame to or from the tag, so the k-th D0
edge in the trace is the k-th tag frame in the pcap (the edge comes right after the frame ends).
From that the script derives, per block of each image transfer:

  late start    burst start - (ACK end + pleaseWaitMs): how late the AP started sending
  listen        tag radio on before the first block part arrived, and its charge (wasted)
  rx            tag radio on from waking up until it switched the radio off (block complete)
  after off     block-part frames the AP sent after the tag had switched its radio off
  charge        tag charge from the block request to radio off

and per transfer: data size and type, blocks, time and charge from the check-in that announced
the image to the XFC, and including the display refresh. It also prints the sniffer's own summary
(all tags) for the AP-side reply times.

Example:
  python analyze_baseline.py --pcap run.pcap --ppk run-ppk.npz --tag 0000021EDAAE3B17 --out run-analysis
"""

import argparse
import json
import os
import statistics
import struct
import subprocess
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
SNIFF_DIR = os.path.join(HERE, "..", "radio-sniffer", "host")
sys.path.insert(0, SNIFF_DIR)
from oepl_sniff import OEPL_PANS, decode  # noqa: E402

US_PER_BYTE = 32
RADIO_ON_UA = 20_000  # ZBS243: RX ≈ 28 mA, TX ≈ 36 mA, CPU only ≈ 7 mA
IDLE_UA = 200


def read_tag_frames(pcap, mac):
    frames = []
    with open(pcap, "rb") as f:
        f.read(24)
        while hdr := f.read(16):
            sec, usec, incl, _ = struct.unpack("<IIII", hdr)
            raw = f.read(incl)
            d = decode(raw + b"\0\0")
            if not d or d["pan"] not in OEPL_PANS or mac not in (d["src"], d["dst"]):
                continue
            d["from_tag"] = d["src"] == mac
            d["t"] = sec + usec / 1e6
            d["airtime_us"] = (6 + incl + 2) * US_PER_BYTE  # preamble+SFD+PHR + PSDU incl. FCS
            frames.append(d)
    return frames


def first_run_below(c, start, level, run, max_s=30, fs=100_000):
    """Index of the first sample >= start that begins `run` consecutive samples below `level`,
    searching at most max_s seconds ahead (the trace can be tens of millions of samples)."""
    below = (c[start:start + int(max_s * fs) + run] < level).astype(np.int32)
    if len(below) < run:
        return None
    sums = np.convolve(below, np.ones(run, dtype=np.int32), mode="valid")
    hit = np.flatnonzero(sums == run)
    return start + int(hit[0]) if len(hit) else None


def first_above(c, start, stop, level):
    hit = np.flatnonzero(c[start:stop] > level)
    return start + int(hit[0]) if len(hit) else None


def charge_uc(c, a, b, fs):
    return float(c[a:b].sum()) / fs


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--pcap", required=True)
    ap.add_argument("--ppk", required=True, help="npz from ppk2_capture.py")
    ap.add_argument("--tag", required=True, help="tag MAC as in the web UI")
    ap.add_argument("--out", help="base path for <out>.txt and <out>.json")
    args = ap.parse_args()

    mac = bytes.fromhex(args.tag)[::-1]
    frames = read_tag_frames(args.pcap, mac)
    npz = np.load(args.ppk)
    c = npz["current_uA"]
    fs = int(npz["sample_rate"])
    edges = np.flatnonzero(np.diff(npz["logic"] & 1)) + 1
    out = []
    p = out.append
    p(f"tag {args.tag}: {len(frames)} frames in pcap, {len(edges)} D0 edges in trace")
    # The sniffer usually starts before the PPK2, so the first frames have no edge. Find the offset
    # where the gaps between frames match the gaps between edges best.
    t = np.array([f["t"] for f in frames])
    e = edges / fs
    best = None
    for of in range(0, min(30, len(t))):
        for oe in range(0, min(30, len(e))) if of == 0 else (0,):
            m = min(len(t) - of, len(e) - oe, 300)
            if m < 5:
                continue
            err = float(np.median(np.abs(np.diff(t[of:of + m]) - np.diff(e[oe:oe + m]))))
            if best is None or err < best[0]:
                best = (err, of, oe)
    _, of, oe = best
    n = min(len(frames) - of, len(edges) - oe)
    p(f"alignment: frames from #{of}, edges from #{oe}, median gap mismatch {best[0] * 1e6:.0f} µs")
    frames = frames[of:of + n]
    for k in range(n):
        frames[k]["end"] = int(edges[oe + k])  # sample index of the frame end

    transfers, blocks = [], []
    cur = None
    k = 0
    while k < n:
        f = frames[k]
        t = f["type"]
        if t == 0xE6 and not f["from_tag"] and len(f["payload"]) >= 14 and f["payload"][13] != 0:
            pl = f["payload"]
            cur = {"start": f["end"], "data_type": pl[13], "data_size": struct.unpack_from("<I", pl, 9)[0],
                   "blocks": []}
        elif t in (0xE4, 0xE7) and f["from_tag"] and cur is not None:
            req = f
            pl = f["payload"]
            requested = set()
            if len(pl) >= 17:
                requested = {i for i in range(42) if pl[11 + i // 8] & (1 << (i % 8))}
            ack, parts = None, []
            j = k + 1
            while j < n and not frames[j]["from_tag"]:
                if frames[j]["type"] == 0xE9 and ack is None:
                    ack = frames[j]
                elif frames[j]["type"] == 0xE8:
                    parts.append(frames[j])
                j += 1
            b = {"block_id": pl[9] if len(pl) > 9 else None, "requested": len(requested), "frames": len(parts)}
            if ack is not None and len(ack["payload"]) >= 3 and parts:
                pw = struct.unpack_from("<H", ack["payload"], 1)[0]
                first_start = parts[0]["end"] - int(parts[0]["airtime_us"] * fs / 1e6)
                wake = first_above(c, ack["end"] + int(0.002 * fs), first_start + 1, RADIO_ON_UA) if pw else ack["end"]
                if wake is None:
                    wake = first_start
                off = first_run_below(c, parts[0]["end"], RADIO_ON_UA, int(0.002 * fs)) or parts[-1]["end"]
                seen, complete_at = set(), None
                for q in parts:
                    seen.add(q["payload"][2] if len(q["payload"]) > 2 else None)
                    if complete_at is None and requested and requested <= seen:
                        complete_at = q["end"]
                req_start = req["end"] - int(req["airtime_us"] * fs / 1e6)
                b.update({
                    "please_wait_ms": pw,
                    "late_start_ms": (first_start - ack["end"]) * 1000 / fs - pw,
                    "listen_ms": (first_start - wake) * 1000 / fs,
                    "listen_uC": charge_uc(c, wake, first_start, fs),
                    "rx_ms": (off - wake) * 1000 / fs,
                    "frames_after_off": sum(1 for q in parts if q["end"] - int(q["airtime_us"] * fs / 1e6) > off),
                    "frames_after_all_sent": sum(1 for q in parts if complete_at is not None and q["end"] > complete_at),
                    "burst_ms": (parts[-1]["end"] - first_start) * 1000 / fs,
                    "charge_uC": charge_uc(c, req_start, off, fs),
                })
            cur["blocks"].append(b)
            blocks.append(b)
            k = j
            continue
        elif t == 0xEA and f["from_tag"] and cur is not None:
            done = f["end"]
            idle = first_run_below(c, done, IDLE_UA, int(0.05 * fs)) or len(c)
            cur.update({
                "duration_s": (done - cur["start"]) / fs,
                "charge_to_xfc_uC": charge_uc(c, cur["start"], done, fs),
                "charge_with_refresh_uC": charge_uc(c, cur["start"], idle, fs),
                "refresh_s": (idle - done) / fs,
            })
            transfers.append(cur)
            cur = None
        k += 1

    # Check-ins: a tag's AVAIL_DATA_REQ/SHORTREQ requests (retries included) and the AP's AVAIL_DATA_INFO.
    # Awake time and charge come from the active episode around them (current above IDLE_UA).
    checkins = []
    k = 0
    while k < n:
        f = frames[k]
        if f["from_tag"] and f["type"] in (0xE3, 0xE5):
            reqs, info = [f], None
            j = k + 1
            while j < n and (frames[j]["end"] - reqs[-1]["end"]) < 0.3 * fs:
                g = frames[j]
                if g["from_tag"] and g["type"] in (0xE3, 0xE5) and info is None:
                    reqs.append(g)
                elif not g["from_tag"] and g["type"] == 0xE6 and info is None:
                    info = g
                else:
                    break
                j += 1
            first_start = reqs[0]["end"] - int(reqs[0]["airtime_us"] * fs / 1e6)
            before = np.flatnonzero(c[max(0, first_start - fs):first_start] < IDLE_UA)
            wake = max(0, first_start - fs) + int(before[-1]) + 1 if len(before) else first_start
            sleep = first_run_below(c, (info or reqs[-1])["end"], IDLE_UA, int(0.005 * fs)) or len(c)
            data = bool(info and len(info["payload"]) >= 14 and info["payload"][13] != 0)
            checkins.append({
                "t_s": first_start / fs,
                "requests": len(reqs),
                "answered": info is not None,
                "new_data": data,
                "first_reply_ms": (info["end"] - reqs[0]["end"]) * 1000 / fs if info else None,
                "awake_ms": (sleep - wake) * 1000 / fs,
                "charge_uC": charge_uc(c, wake, sleep, fs),
            })
            k = j
            continue
        k += 1

    # Boot: from the start of the capture (the PPK2 power-cycles the tag) to the end of the first check-in.
    boot = None
    pings = [f for f in frames if f["type"] == 0xED]
    pongs = [f for f in frames if f["type"] == 0xEE]
    if checkins:
        first_ci = checkins[0]
        end = int((first_ci["t_s"]) * fs) + int(first_ci["awake_ms"] * fs / 1000)
        boot = {"duration_s": end / fs, "charge_uC": charge_uc(c, 0, end, fs),
                "pings": sum(1 for f in pings if f["end"] < end), "pongs": sum(1 for f in pongs if f["end"] < end)}

    p("")
    if boot:
        p(f"boot (power-on to end of first check-in): {boot['duration_s']:.1f} s, {boot['charge_uC'] / 1000:.1f} mC, "
          f"{boot['pings']} pings, {boot['pongs']} pongs")
    plain = [x for x in checkins if not x["new_data"] and x["answered"]]
    p(f"check-ins: {len(checkins)} ({len(plain)} answered without new data, "
      f"{sum(1 for x in checkins if x['new_data'])} with new data, {sum(1 for x in checkins if not x['answered'])} unanswered)")
    for label, sel in (("single request", [x for x in plain if x["requests"] == 1]),
                       ("with retries", [x for x in plain if x["requests"] > 1])):
        if sel:
            p(f"  no new data, {label:14s}: n={len(sel):3d}  requests {statistics.mean(x['requests'] for x in sel):.2f}"
              f"  first reply median {statistics.median(x['first_reply_ms'] for x in sel):6.2f} ms"
              f"  awake median {statistics.median(x['awake_ms'] for x in sel):6.1f} ms"
              f"  charge median {statistics.median(x['charge_uC'] for x in sel):6.0f} µC"
              f"  mean {statistics.mean(x['charge_uC'] for x in sel):6.0f} µC")
    xfcs = []
    for k in range(n - 1):
        if frames[k]["from_tag"] and frames[k]["type"] == 0xEA:
            nxt = next((g for g in frames[k + 1:k + 20] if not g["from_tag"] and g["type"] == 0xEB), None)
            if nxt:
                xfcs.append((nxt["end"] - frames[k]["end"]) * 1000 / fs)
    if xfcs:
        p(f"XFC ack: n={len(xfcs)}, median {statistics.median(xfcs):.2f} ms, max {max(xfcs):.2f} ms")

    p("")
    p("blocks:")
    p("  block  req  frames  wait ms  late ms  listen ms  listen µC  rx ms  after off  after all sent  burst ms  charge µC")
    for b in blocks:
        if "late_start_ms" not in b:
            p(f"  {b['block_id']!s:>5}  incomplete (no ACK or no parts seen)")
            continue
        p(f"  {b['block_id']:>5}  {b['requested']:>3}  {b['frames']:>6}  {b['please_wait_ms']:>7}  {b['late_start_ms']:7.1f}  "
          f"{b['listen_ms']:9.1f}  {b['listen_uC']:9.0f}  {b['rx_ms']:5.0f}  {b['frames_after_off']:>9}  "
          f"{b['frames_after_all_sent']:>14}  {b['burst_ms']:8.0f}  {b['charge_uC']:9.0f}")
    p("")
    p("transfers:")
    p("  type  bytes  blocks  INFO->XFC s  charge to XFC µC  refresh s  charge incl. refresh µC")
    for t in transfers:
        p(f"  0x{t['data_type']:02X}  {t['data_size']:>5}  {len(t['blocks']):>6}  {t['duration_s']:11.2f}  "
          f"{t['charge_to_xfc_uC']:16.0f}  {t['refresh_s']:9.2f}  {t['charge_with_refresh_uC']:23.0f}")

    full = [b for b in blocks if "late_start_ms" in b]
    agg = {}
    if full:
        def m(key):
            return statistics.mean(b[key] for b in full)
        agg = {
            "blocks": len(full),
            "late_start_ms_mean": m("late_start_ms"),
            "late_start_ms_max": max(b["late_start_ms"] for b in full),
            "listen_ms_mean": m("listen_ms"),
            "listen_uC_mean": m("listen_uC"),
            "rx_ms_mean": m("rx_ms"),
            "frames_mean": m("frames"),
            "frames_after_off_mean": m("frames_after_off"),
            "charge_uC_per_block_mean": m("charge_uC"),
        }
        if transfers:
            agg.update({
                "transfers": len(transfers),
                "charge_to_xfc_uC_mean": statistics.mean(t["charge_to_xfc_uC"] for t in transfers),
                "charge_with_refresh_uC_mean": statistics.mean(t["charge_with_refresh_uC"] for t in transfers),
                "duration_s_mean": statistics.mean(t["duration_s"] for t in transfers),
            })
        p("")
        p("per block (mean): " + ", ".join(f"{k} {v:.1f}" if isinstance(v, float) else f"{k} {v}" for k, v in agg.items()))

    sniff = subprocess.run([sys.executable, os.path.join(SNIFF_DIR, "oepl_sniff.py"), "--replay", args.pcap],
                           capture_output=True, text=True)
    p("")
    p("AP side, all tags (sniffer replay):")
    p(sniff.stdout.strip())

    text = "\n".join(out)
    print(text)
    if args.out:
        with open(args.out + ".txt", "w", encoding="utf-8") as f:
            f.write(text + "\n")
        with open(args.out + ".json", "w") as f:
            json.dump({"tag": args.tag, "aggregate": agg, "boot": boot, "checkins": checkins, "xfc_ack_ms": xfcs,
                       "transfers": transfers, "blocks": blocks}, f, indent=1)


if __name__ == "__main__":
    main()
