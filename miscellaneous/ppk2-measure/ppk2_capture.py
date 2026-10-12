#!/usr/bin/env python3
"""Current measurement of one tag with a Nordic PPK2 (measurement M4 of the radio chain work).

The PPK2 powers the tag in source-meter mode and samples its current at 100 kS/s together with the
8 logic inputs. The OEPL radio sniffer (../radio-sniffer) can toggle logic input D0 on every frame
to or from the tag, so current and radio frames share one timeline.

The script captures for a given time, splits the current trace into active episodes (current above a
threshold, short gaps merged), and prints per episode: start, duration, charge, peak and the number
of D0 edges (= radio frames to/from the tag seen by the sniffer). It saves the raw trace as .npz for
later analysis.

Example:
  python ppk2_capture.py --duration 600 --voltage 3000 --out ../../measurements/zbs29_baseline
"""

import argparse
import json
import sys
import threading
import time

import numpy as np
import serial.tools.list_ports
from ppk2_api.ppk2_api import PPK2_API

SAMPLE_RATE = 100_000  # PPK2 samples per second


def find_port():
    """The PPK2 has two serial interfaces; the measurement one is 'nRF Connect USB CDC ACM' (MI_01)."""
    for p in serial.tools.list_ports.comports():
        if p.vid == 0x1915 and p.pid == 0xC00A and ("CDC ACM" in (p.description or "") or (p.location or "").endswith("x.1")):
            return p.device
    return None


class Capture:
    """Reads the PPK2 in a background thread and decodes chunk by chunk into numpy arrays."""

    def __init__(self, ppk):
        self.ppk = ppk
        self.current = []  # float32 chunks, µA
        self.logic = []  # uint8 chunks, bit n = logic input Dn
        self.stop = threading.Event()
        self.thread = threading.Thread(target=self.run, daemon=True)

    def run(self):
        while not self.stop.is_set():
            data = self.ppk.get_data()
            if data:
                samples, bits = self.ppk.get_samples(data)
                if samples:
                    self.current.append(np.asarray(samples, dtype=np.float32))
                    self.logic.append(np.asarray(bits, dtype=np.uint8))
            else:
                time.sleep(0.005)

    def arrays(self):
        if not self.current:
            return np.zeros(0, np.float32), np.zeros(0, np.uint8)
        return np.concatenate(self.current), np.concatenate(self.logic)


def episodes(current, logic, threshold_ua, merge_ms):
    """Contiguous stretches above threshold, merging gaps shorter than merge_ms."""
    active = current > threshold_ua
    if not active.any():
        return []
    edges = np.flatnonzero(np.diff(active.astype(np.int8)))
    starts = list(edges[~active[edges]] + 1)
    ends = list(edges[active[edges]] + 1)
    if active[0]:
        starts.insert(0, 0)
    if active[-1]:
        ends.append(len(active))
    merged = []
    gap = int(merge_ms * SAMPLE_RATE / 1000)
    for s, e in zip(starts, ends):
        if merged and s - merged[-1][1] <= gap:
            merged[-1][1] = e
        else:
            merged.append([s, e])
    d0 = logic & 1
    out = []
    for s, e in merged:
        seg = current[s:e]
        out.append({
            "start_s": s / SAMPLE_RATE,
            "duration_ms": (e - s) * 1000 / SAMPLE_RATE,
            "charge_uC": float(seg.sum()) / SAMPLE_RATE,
            "mean_mA": float(seg.mean()) / 1000,
            "peak_mA": float(seg.max()) / 1000,
            "d0_edges": int(np.count_nonzero(np.diff(d0[max(s - 1, 0):e]))),
        })
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", help="PPK2 measurement port (default: auto-detect the 'nRF Connect USB CDC ACM' port)")
    ap.add_argument("--voltage", type=int, default=3000, help="source voltage in mV (default 3000)")
    ap.add_argument("--duration", type=float, default=60, help="seconds to capture (default 60)")
    ap.add_argument("--out", help="base path for <out>.npz (raw trace) and <out>.json (summary)")
    ap.add_argument("--threshold-ua", type=float, default=200, help="active above this current (default 200 µA)")
    ap.add_argument("--merge-ms", type=float, default=5, help="merge active stretches closer than this (default 5 ms)")
    ap.add_argument("--power-off", action="store_true", help="switch the tag off after the capture (default: leave it on)")
    args = ap.parse_args()

    port = args.port or find_port()
    if not port:
        sys.exit("PPK2 not found; pass --port")
    ppk = PPK2_API(port, timeout=1, write_timeout=1, exclusive=True)
    if not ppk.get_modifiers():
        sys.exit(f"no calibration data from {port}; is this the PPK2 measurement port?")
    ppk.use_source_meter()
    ppk.set_source_voltage(args.voltage)
    ppk.toggle_DUT_power("ON")
    time.sleep(0.05)

    cap = Capture(ppk)
    ppk.start_measuring()
    started = time.time()
    cap.thread.start()
    try:
        while time.time() - started < args.duration:
            time.sleep(0.2)
    except KeyboardInterrupt:
        pass
    cap.stop.set()
    cap.thread.join()
    ppk.stop_measuring()
    if args.power_off:
        ppk.toggle_DUT_power("OFF")

    current, logic = cap.arrays()
    seconds = len(current) / SAMPLE_RATE
    if not len(current):
        sys.exit("no samples")
    eps = episodes(current, logic, args.threshold_ua, args.merge_ms)
    idle = current[current <= args.threshold_ua]
    summary = {
        "port": port,
        "voltage_mV": args.voltage,
        "captured_s": seconds,
        "samples": int(len(current)),
        "mean_uA": float(current.mean()),
        "total_charge_uC": float(current.sum()) / SAMPLE_RATE,
        "idle_median_uA": float(np.median(idle)) if len(idle) else None,
        "active_ms": float(np.count_nonzero(current > args.threshold_ua)) * 1000 / SAMPLE_RATE,
        "d0_edges": int(np.count_nonzero(np.diff(logic & 1))),
        "threshold_uA": args.threshold_ua,
        "episodes": eps,
    }

    print(f"captured {seconds:.1f} s at {args.voltage} mV ({len(current)} samples)")
    print(f"mean {summary['mean_uA']:.1f} µA, charge {summary['total_charge_uC']:.0f} µC "
          f"({summary['total_charge_uC'] / 3600:.2f} µAh), idle median "
          f"{summary['idle_median_uA'] if summary['idle_median_uA'] is not None else float('nan'):.2f} µA, "
          f"active {summary['active_ms']:.0f} ms, D0 edges {summary['d0_edges']}")
    print(f"{len(eps)} active episodes (> {args.threshold_ua:g} µA, gaps < {args.merge_ms:g} ms merged):")
    print("   start s   duration ms   charge µC   mean mA   peak mA   D0 edges")
    for e in eps:
        print(f"  {e['start_s']:8.3f}   {e['duration_ms']:11.1f}   {e['charge_uC']:9.1f}   "
              f"{e['mean_mA']:7.2f}   {e['peak_mA']:7.2f}   {e['d0_edges']:8d}")

    if args.out:
        np.savez_compressed(args.out + ".npz", current_uA=current, logic=logic,
                            sample_rate=SAMPLE_RATE, voltage_mV=args.voltage)
        with open(args.out + ".json", "w") as f:
            json.dump(summary, f, indent=1)
        print(f"saved {args.out}.npz and {args.out}.json")


if __name__ == "__main__":
    main()
