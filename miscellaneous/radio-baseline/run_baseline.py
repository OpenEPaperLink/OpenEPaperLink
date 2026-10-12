#!/usr/bin/env python3
"""Repeatable radio-chain baseline (measurements M2 + M4): image transfers to one tag, measured
on air (radio sniffer) and in the tag's current (PPK2), then analysed with analyze_baseline.py.

What it does:
  1. generates a fixed set of noise test images (seeded, so every run sends exactly the same data;
     noise compresses badly, so each image takes several 4 KB blocks; JPEG, the only format /imgupload
     decodes, quality 100; same Pillow version = same bytes),
  2. starts the sniffer (--sync-mac on the tag, pcap) and the PPK2 capture in parallel,
  3. waits until the tag has rebooted (every PPK2 capture power-cycles it) and checked in,
  4. uploads the images one by one via POST /imgupload, --interval seconds apart,
  5. runs analyze_baseline.py on the result.

Example (Nic's setup):
  python run_baseline.py --tag 0000021EDAAE3B17 --ap http://192.168.178.198 --sniffer COM11 \
      --out ../../measurements/baseline-2026-10-12
"""

import argparse
import os
import random
import subprocess
import sys
import time
import urllib.request
import uuid

from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
SNIFFER = os.path.join(HERE, "..", "radio-sniffer", "host", "oepl_sniff.py")
PPK2 = os.path.join(HERE, "..", "ppk2-measure", "ppk2_capture.py")
ANALYZE = os.path.join(HERE, "analyze_baseline.py")


def make_image(path, width, height, seed, colors):
    rnd = random.Random(seed)
    palette = [(255, 255, 255), (0, 0, 0), (255, 0, 0)][:colors]
    img = Image.new("RGB", (width, height))
    img.putdata([rnd.choice(palette) for _ in range(width * height)])
    # /imgupload only decodes JPEG; quality 100 without chroma subsampling keeps the noise sharp
    img.save(path, format="JPEG", quality=100, subsampling=0)


def upload(ap, mac, path):
    boundary = uuid.uuid4().hex
    with open(path, "rb") as f:
        data = f.read()
    parts = []
    for name, value in (("mac", mac), ("dither", "0")):
        parts.append(f'--{boundary}\r\nContent-Disposition: form-data; name="{name}"\r\n\r\n{value}\r\n'.encode())
    parts.append(f'--{boundary}\r\nContent-Disposition: form-data; name="file"; filename="{os.path.basename(path)}"\r\n'
                 f'Content-Type: image/jpeg\r\n\r\n'.encode() + data + b"\r\n")
    parts.append(f"--{boundary}--\r\n".encode())
    req = urllib.request.Request(ap.rstrip("/") + "/imgupload", data=b"".join(parts), method="POST",
                                 headers={"Content-Type": f"multipart/form-data; boundary={boundary}"})
    with urllib.request.urlopen(req, timeout=30) as r:
        return r.status, r.read().decode(errors="replace").strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tag", required=True, help="tag MAC as in the web UI")
    ap.add_argument("--ap", required=True, help="AP base URL, e.g. http://192.168.178.198")
    ap.add_argument("--sniffer", required=True, help="sniffer serial port")
    ap.add_argument("--ppk", help="PPK2 measurement port (default: auto-detect)")
    ap.add_argument("--channel", type=int, default=25)
    ap.add_argument("--voltage", type=int, default=3000, help="tag supply in mV (default 3000)")
    ap.add_argument("--reps", type=int, default=5, help="number of image transfers (default 5)")
    ap.add_argument("--interval", type=float, default=100, help="seconds between uploads (default 100; "
                    "an upload is sent to the tag at its next check-in, ≈ every 40 s)")
    ap.add_argument("--settle", type=float, default=30, help="seconds to wait for the tag reboot before the first upload")
    ap.add_argument("--tail", type=float, default=60, help="seconds to keep measuring after the last upload")
    ap.add_argument("--size", default="296x128", help="image size WxH (default 296x128, 2.9\")")
    ap.add_argument("--colors", type=int, default=3, choices=(2, 3), help="2 = black/white, 3 = black/white/red")
    ap.add_argument("--seed", type=int, default=1000, help="first image seed (default 1000)")
    ap.add_argument("--out", required=True, help="output base path; files <out>.pcap, <out>-ppk.*, <out>-*.txt")
    args = ap.parse_args()

    width, height = (int(x) for x in args.size.lower().split("x"))
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    images = []
    for i in range(args.reps):
        path = f"{args.out}-img{i}.jpg"
        make_image(path, width, height, args.seed + i, args.colors)
        images.append(path)

    total = args.settle + args.reps * args.interval + args.tail
    py = sys.executable
    sniff_log = open(args.out + "-sniff.txt", "w")
    sniff = subprocess.Popen([py, SNIFFER, "--port", args.sniffer, "--channel", str(args.channel),
                              "--duration", str(total + 5), "--sync-mac", args.tag, "--pcap", args.out + ".pcap"],
                             stdout=sniff_log, stderr=subprocess.STDOUT)
    time.sleep(2)  # sniffer drains its stale buffer and sets the sync MAC first
    ppk_cmd = [py, PPK2, "--duration", str(total), "--voltage", str(args.voltage), "--out", args.out + "-ppk"]
    if args.ppk:
        ppk_cmd += ["--port", args.ppk]
    ppk_log = open(args.out + "-ppk.txt", "w")
    ppk = subprocess.Popen(ppk_cmd, stdout=ppk_log, stderr=subprocess.STDOUT)
    start = time.time()

    log = open(args.out + "-uploads.txt", "w")
    for i, path in enumerate(images):
        due = start + args.settle + i * args.interval
        time.sleep(max(0, due - time.time()))
        try:
            status, text = upload(args.ap, args.tag, path)
        except Exception as e:  # keep going; the analysis shows which transfers happened
            status, text = "error", str(e)
        line = f"{time.time() - start:7.1f} s  upload {i} ({os.path.basename(path)}): {status} {text}"
        print(line, flush=True)
        log.write(line + "\n")
    log.close()

    ppk.wait()
    sniff.wait()
    ppk_log.close()
    sniff_log.close()
    print(f"capture done ({total:.0f} s); analysing", flush=True)
    subprocess.run([py, ANALYZE, "--pcap", args.out + ".pcap", "--ppk", args.out + "-ppk.npz",
                    "--tag", args.tag, "--out", args.out + "-analysis"])


if __name__ == "__main__":
    main()
