#!/usr/bin/env python3
"""Generate image-transfer traffic on an AP for a long run (overnight), e.g. for measurement M3
(serial integrity) and to watch concurrent transfers on air.

- The test tag (powered by the PPK2, which this script keeps switched on) gets a new image every
  --test-interval seconds.
- The other tags get a new image in rounds of --tag-interval seconds. The rounds alternate:
  "burst" (all tags at once: their transfers overlap) and "staggered" (spread over the round). No tag
  ever gets an image more often than once per --tag-interval; a burst therefore starts when every tag
  may get one again.
- No uploads during the AP's sleep window (--quiet), when tags don't check in.
- The radio sniffer runs for the whole time (pcap + hourly summaries).
- /imgupload switches a tag to content mode 24 ("external image") and replaces its content config.
  The script saves every tag's config first (<out>/tag-configs.json) and restores it at the end;
  run it with --restore <that file> if it was stopped before.

Example (Nic's setup):
  python night_traffic.py --ap http://192.168.178.198 --test-tag 0000021EDAAE3B17 \
      --tags 000002BD0A923B1C,0000018938B83B11,... --sniffer COM11 --ppk COM3 --hours 11 \
      --out ../../measurements/night-2026-10-12
"""

import argparse
import datetime
import io
import json
import os
import random
import subprocess
import sys
import time
import urllib.parse
import urllib.request
import uuid

from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
SNIFFER = os.path.join(REPO, "miscellaneous", "radio-sniffer", "host", "oepl_sniff.py")
CONFIG_FIELDS = (("contentmode", "contentMode"), ("modecfgjson", "modecfgjson"), ("alias", "alias"),
                 ("rotate", "rotate"), ("lut", "lut"), ("invert", "invert"))
NOISE_PIXELS_MAX = 48_000  # keeps big displays at a few blocks instead of dozens


def http_json(url):
    with urllib.request.urlopen(url, timeout=15) as r:
        return json.loads(r.read().decode())


def post_form(url, fields):
    data = urllib.parse.urlencode(fields).encode()
    with urllib.request.urlopen(urllib.request.Request(url, data=data, method="POST"), timeout=15) as r:
        return r.status, r.read().decode(errors="replace").strip()


def upload(ap, mac, jpeg):
    boundary = uuid.uuid4().hex
    body = (f'--{boundary}\r\nContent-Disposition: form-data; name="mac"\r\n\r\n{mac}\r\n'
            f'--{boundary}\r\nContent-Disposition: form-data; name="dither"\r\n\r\n0\r\n'
            f'--{boundary}\r\nContent-Disposition: form-data; name="file"; filename="test.jpg"\r\n'
            f'Content-Type: image/jpeg\r\n\r\n').encode() + jpeg + f"\r\n--{boundary}--\r\n".encode()
    req = urllib.request.Request(ap.rstrip("/") + "/imgupload", data=body, method="POST",
                                 headers={"Content-Type": f"multipart/form-data; boundary={boundary}"})
    with urllib.request.urlopen(req, timeout=30) as r:
        return r.status, r.read().decode(errors="replace").strip()


def tag_info(ap, mac):
    d = http_json(f"{ap.rstrip('/')}/get_db?mac={mac}")
    return d["tags"][0]


def tag_size(hwtype):
    with open(os.path.join(REPO, "resources", "tagtypes", f"{hwtype:02X}.json"), encoding="utf-8") as f:
        t = json.load(f)
    # some tag types have no "colors"; 2 bits per pixel means a third colour (red or yellow)
    return int(t["width"]), int(t["height"]), int(t.get("colors") or (3 if int(t.get("bpp", 1)) >= 2 else 2))


def make_image(width, height, colors, mac, n):
    """Header with text, the rest white except a noise band (so the image needs several blocks)."""
    rnd = random.Random(f"{mac}-{n}")
    img = Image.new("RGB", (width, height), "white")
    d = ImageDraw.Draw(img)
    try:
        font = ImageFont.truetype("arial.ttf", max(12, height // 10))
    except OSError:
        font = ImageFont.load_default()
    bar = max(16, height // 6)
    d.rectangle([0, 0, width - 1, bar], fill="black")
    d.text((4, 2), "OEPL radio test", font=font, fill="white")
    d.text((4, bar + 4), f"{datetime.datetime.now():%H:%M}  #{n}  {mac[-6:]}", font=font, fill="black")
    palette = [(255, 255, 255), (0, 0, 0)] + ([(255, 0, 0)] if colors >= 3 else [])
    band_h = min(height - 2 * bar - 8, max(8, NOISE_PIXELS_MAX // width))
    noise = Image.new("RGB", (width, band_h))
    noise.putdata([rnd.choice(palette) for _ in range(width * band_h)])
    img.paste(noise, (0, height - band_h))
    buf = io.BytesIO()
    img.save(buf, format="JPEG", quality=100, subsampling=0)
    return buf.getvalue()


def restore(ap, path, log):
    with open(path, encoding="utf-8") as f:
        saved = json.load(f)
    for mac, cfg in saved.items():
        fields = {"mac": mac}
        for form, key in CONFIG_FIELDS:
            if key in cfg and cfg[key] is not None:
                fields[form] = cfg[key]
        try:
            status, text = post_form(f"{ap.rstrip('/')}/save_cfg", fields)
        except Exception as e:
            status, text = "error", str(e)
        log(f"restore {mac}: contentMode {cfg.get('contentMode')} -> {status} {text}")


def in_quiet(now, quiet):
    start, end = quiet
    h = now.hour + now.minute / 60
    return start <= h < end if start < end else (h >= start or h < end)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--ap", required=True)
    ap.add_argument("--test-tag", help="tag on the PPK2")
    ap.add_argument("--test-interval", type=float, default=180)
    ap.add_argument("--tags", default="", help="comma-separated MACs of the other tags")
    ap.add_argument("--tag-interval", type=float, default=600, help="minimum seconds between images per tag")
    ap.add_argument("--quiet", default="1-7", help="AP sleep window in hours, e.g. 1-7: no uploads then")
    ap.add_argument("--hours", type=float, default=10)
    ap.add_argument("--sniffer", help="sniffer port; runs for the whole time")
    ap.add_argument("--channel", type=int, default=25)
    ap.add_argument("--ppk", help="PPK2 measurement port: keeps the test tag powered")
    ap.add_argument("--voltage", type=int, default=3000)
    ap.add_argument("--out", help="output folder")
    ap.add_argument("--restore", help="only restore tag configs from this tag-configs.json and exit")
    ap.add_argument("--state", default=os.path.join(REPO, "measurements", "traffic-last-upload.json"),
                    help="file with the last upload time per tag, kept between runs so the intervals hold "
                         "across runs too (default: measurements/traffic-last-upload.json)")
    args = ap.parse_args()

    if args.restore:
        restore(args.ap, args.restore, print)
        return
    if not args.out:
        sys.exit("--out is required")
    os.makedirs(args.out, exist_ok=True)
    logf = open(os.path.join(args.out, "traffic.log"), "a", encoding="utf-8")

    def log(msg):
        line = f"{datetime.datetime.now():%Y-%m-%d %H:%M:%S}  {msg}"
        print(line, flush=True)
        logf.write(line + "\n")
        logf.flush()

    tags = [m.strip().upper() for m in args.tags.split(",") if m.strip()]
    everyone = ([args.test_tag.upper()] if args.test_tag else []) + tags
    quiet = tuple(float(x) for x in args.quiet.split("-"))

    # Save configs before touching anything.
    saved, sizes = {}, {}
    for mac in everyone:
        info = tag_info(args.ap, mac)
        saved[mac] = {key: info.get(key) for _, key in CONFIG_FIELDS}
        sizes[mac] = tag_size(int(info["hwType"]))
        log(f"tag {mac}: hwType {info['hwType']:02X}, {sizes[mac][0]}x{sizes[mac][1]}, "
            f"contentMode {info.get('contentMode')}, saved")
    cfg_path = os.path.join(args.out, "tag-configs.json")
    with open(cfg_path, "w", encoding="utf-8") as f:
        json.dump(saved, f, indent=1)

    ppk = None
    if args.ppk and args.test_tag:
        from ppk2_api.ppk2_api import PPK2_API
        ppk = PPK2_API(args.ppk, timeout=1, write_timeout=1, exclusive=True)
        ppk.get_modifiers()
        ppk.use_source_meter()
        ppk.set_source_voltage(args.voltage)
        ppk.toggle_DUT_power("ON")
        log(f"PPK2 on {args.ppk}: test tag powered at {args.voltage} mV")

    sniff = None
    total = args.hours * 3600
    if args.sniffer:
        sniff_out = open(os.path.join(args.out, "sniff.txt"), "w")
        cmd = [sys.executable, SNIFFER, "--port", args.sniffer, "--channel", str(args.channel),
               "--duration", str(total), "--pcap", os.path.join(args.out, "night.pcap"), "--summary-every", "3600"]
        if args.test_tag:
            cmd += ["--sync-mac", args.test_tag]
        sniff = subprocess.Popen(cmd, stdout=sniff_out, stderr=subprocess.STDOUT)
        log(f"sniffer on {args.sniffer}, channel {args.channel}")

    start = time.time()
    last = {mac: 0.0 for mac in everyone}
    try:
        with open(args.state, encoding="utf-8") as f:
            last.update({m: t for m, t in json.load(f).items() if m in last})
    except (OSError, ValueError):
        pass
    count = {mac: 0 for mac in everyone}
    next_test = start + 30
    round_no = 0
    next_round = start + 60
    planned = []  # (time, mac) for the current round

    def send(mac):
        count[mac] += 1
        w, h, colors = sizes[mac]
        try:
            status, text = upload(args.ap, mac, make_image(w, h, colors, mac, count[mac]))
        except Exception as e:
            status, text = "error", str(e)
        last[mac] = time.time()
        log(f"upload {mac} #{count[mac]}: {status} {text}")
        try:
            os.makedirs(os.path.dirname(args.state), exist_ok=True)
            with open(args.state, "w", encoding="utf-8") as f:
                json.dump(last, f)
        except OSError:
            pass

    try:
        while time.time() - start < total:
            now = time.time()
            quiet_now = in_quiet(datetime.datetime.now(), quiet)
            if args.test_tag and now >= next_test:
                next_test += args.test_interval
                if not quiet_now and now - last[args.test_tag.upper()] >= args.test_interval - 1:
                    send(args.test_tag.upper())
            if tags and now >= next_round:
                mode = "burst" if round_no % 2 == 0 else "staggered"
                if mode == "burst":
                    # all at the same moment: wait until every tag may get an image again
                    t0 = max([now] + [last[m] + args.tag_interval for m in tags])
                    planned = [(t0, mac) for mac in tags]
                else:
                    t0 = now
                    step = args.tag_interval / len(tags)
                    planned = [(now + i * step, mac) for i, mac in enumerate(tags)]
                if quiet_now:
                    planned = []
                else:
                    log(f"round {round_no}: {mode}, starts {datetime.datetime.fromtimestamp(t0):%H:%M:%S}")
                round_no += 1
                next_round = t0 + args.tag_interval
            for item in list(planned):
                t, mac = item
                if time.time() >= t and time.time() - last[mac] >= args.tag_interval:
                    planned.remove(item)
                    send(mac)
            time.sleep(1)
    except KeyboardInterrupt:
        log("stopped by Ctrl-C")
    finally:
        log("restoring tag configs")
        restore(args.ap, cfg_path, log)
        if sniff:
            try:
                sniff.wait(timeout=30)  # it stops by itself after --duration and prints its summary
            except subprocess.TimeoutExpired:
                sniff.terminate()
        if ppk:
            ppk.toggle_DUT_power("OFF")
        log("uploads per tag: " + ", ".join(f"{m} {c}" for m, c in count.items()))
        logf.close()


if __name__ == "__main__":
    main()
