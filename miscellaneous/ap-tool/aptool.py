"""Command-line helper for testing an OpenEPaperLink AP over its web interface.

Mainly meant for agents (Claude Code and the like) and developers who test firmware on an AP.
Python standard library only. See README.md.
"""
import argparse
import base64
import functools
import hashlib
import json
import os
import socket
import struct
import sys
import threading
import time
import urllib.parse
import urllib.request
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer

TAG_CFG = {  # /get_db field -> /save_cfg parameter
    "alias": "alias",
    "contentMode": "contentmode",
    "modecfgjson": "modecfgjson",
    "rotate": "rotate",
    "lut": "lut",
    "invert": "invert",
}


class AP:
    def __init__(self, host):
        self.host = host
        self.base = "http://" + host

    def get(self, path, timeout=10):
        with urllib.request.urlopen(self.base + path, timeout=timeout) as r:
            return r.read().decode()

    def get_json(self, path, timeout=10):
        return json.loads(self.get(path, timeout))

    def post(self, path, form=None, timeout=10):
        data = urllib.parse.urlencode(form or {}).encode()
        with urllib.request.urlopen(self.base + path, data=data, timeout=timeout) as r:
            return r.status, r.read().decode()

    def tags(self):
        tags, pos = [], 0
        while True:
            d = self.get_json(f"/get_db?pos={pos}")
            tags += d["tags"]
            if not d.get("continu"):
                return tags
            pos = d["continu"]

    def tag(self, mac):
        tags = self.get_json("/get_db?mac=" + mac).get("tags", [])
        if not tags:
            sys.exit(f"tag {mac} not found")
        return tags[0]

    def wait_online(self, timeout=120):
        """Waits until /sysinfo answers again after a reboot; returns it."""
        end = time.time() + timeout
        time.sleep(3)
        while time.time() < end:
            try:
                return self.get_json("/sysinfo", timeout=3)
            except OSError:
                time.sleep(2)
        sys.exit("AP did not come back")


class WebSocket:
    """Minimal client for the AP's /ws: text frames only, enough to follow its messages."""

    def __init__(self, host, timeout=5):
        h, _, port = host.partition(":")
        self.sock = socket.create_connection((h, int(port or 80)), timeout=timeout)
        key = base64.b64encode(os.urandom(16)).decode()
        self.sock.sendall((f"GET /ws HTTP/1.1\r\nHost: {host}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"
                           f"Sec-WebSocket-Key: {key}\r\nSec-WebSocket-Version: 13\r\n\r\n").encode())
        head = b""
        while b"\r\n\r\n" not in head:
            chunk = self.sock.recv(1024)
            if not chunk:
                raise OSError("websocket handshake failed")
            head += chunk
        if b" 101 " not in head.split(b"\r\n", 1)[0]:
            raise OSError("websocket handshake refused")
        self.buf = head.split(b"\r\n\r\n", 1)[1]

    def _read(self, n):
        while len(self.buf) < n:
            chunk = self.sock.recv(4096)
            if not chunk:
                raise OSError("websocket closed")
            self.buf += chunk
        out, self.buf = self.buf[:n], self.buf[n:]
        return out

    def _send(self, opcode, payload=b""):
        mask = os.urandom(4)
        masked = bytes(b ^ mask[i % 4] for i, b in enumerate(payload))
        self.sock.sendall(bytes([0x80 | opcode, 0x80 | len(payload)]) + mask + masked)

    def recv(self):
        """Returns the next text message; raises socket.timeout when nothing arrives in time."""
        message = b""
        while True:
            b0, b1 = self._read(2)
            n = b1 & 0x7F
            if n == 126:
                n = struct.unpack(">H", self._read(2))[0]
            elif n == 127:
                n = struct.unpack(">Q", self._read(8))[0]
            payload = self._read(n)
            opcode = b0 & 0x0F
            if opcode == 0x9:
                self._send(0xA, payload)
            elif opcode == 0x8:
                raise OSError("websocket closed")
            elif opcode in (0x0, 0x1):
                message += payload
                if b0 & 0x80:
                    return message.decode(errors="replace")

    def close(self):
        self.sock.close()


def watch(host, seconds, until=(), out=None, console_only=False):
    """Prints /ws messages; returns the first message containing one of `until`, or None."""
    end = time.time() + seconds
    f = open(out, "a", encoding="utf-8") if out else None
    try:
        while time.time() < end:
            try:
                ws = WebSocket(host)
            except OSError:
                time.sleep(2)
                continue
            try:
                while time.time() < end:
                    try:
                        msg = ws.recv()
                    except socket.timeout:
                        continue
                    if console_only and not msg.startswith('{"console"'):
                        continue
                    if console_only and '"Progress: ' in msg and '0%"' not in msg:
                        continue  # flashing reports every percent; keep every tenth
                    line = time.strftime("%H:%M:%S ") + msg
                    print(line, flush=True)
                    if f:
                        f.write(line + "\n")
                        f.flush()
                    for u in until:
                        if u in msg:
                            return msg
            except OSError:
                time.sleep(2)
            finally:
                ws.close()
    finally:
        if f:
            f.close()
    return None


class QuietHandler(SimpleHTTPRequestHandler):
    def log_message(self, *args):
        pass


def serve(directory, ap_host):
    """Serves `directory` over HTTP on a free port; returns (server, base url as the AP sees it)."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect((ap_host.partition(":")[0], 80))  # no traffic, only picks the interface towards the AP
    local_ip = s.getsockname()[0]
    s.close()
    handler = functools.partial(QuietHandler, directory=directory)
    server = ThreadingHTTPServer(("0.0.0.0", 0), handler)
    threading.Thread(target=server.serve_forever, daemon=True).start()
    return server, f"http://{local_ip}:{server.server_address[1]}"


def cmd_info(ap, a):
    i = ap.get_json("/sysinfo")
    print(f"{i['alias']}  {i['env']}  {i['buildversion']} ({i['sha']}, {i['buildtime']})")
    print(f"radio {i['ap_typename']} version {i['ap_version']:04X}  channel {i['channel']}")
    print(f"reset reason {i['resetreason']}  crashed {i['crashed']}  rollback {i['rollback']}")


def cmd_tags(ap, a):
    now = time.time()
    tags = ap.tags()
    for t in tags:
        print(f'{t["mac"]} {now - t["lastseen"]:7.0f}s ago  hw {t["hwType"]:3}  '
              f'ext {t.get("isexternal", False)!s:5}  {t["alias"]}')
    print(len(tags), "tags")


def cmd_backup(ap, a):
    t = ap.tag(a.mac)
    with open(a.file, "w", encoding="utf-8") as f:
        json.dump(t, f, indent=1)
    print(f"saved {a.mac} ({t['alias'] or 'no alias'}, contentMode {t['contentMode']}) to {a.file}")


def cmd_restore(ap, a):
    with open(a.file, encoding="utf-8") as f:
        t = json.load(f)
    form = {"mac": t["mac"], **{p: t[k] for k, p in TAG_CFG.items()}}
    print(*ap.post("/save_cfg", form))
    now = ap.tag(t["mac"])
    bad = [k for k in TAG_CFG if now[k] != t[k]]
    print("restored, all fields match" if not bad else f"MISMATCH in {', '.join(bad)}")
    if bad:
        sys.exit(1)
    # /save_cfg only changes the database in RAM; /backup_db writes it to flash (a reboot without
    # saving, such as the one after a radio update, would otherwise undo the restore)
    ap.get("/backup_db", timeout=30)


def cmd_ota(ap, a):
    path = os.path.abspath(a.firmware)
    data = open(path, "rb").read()
    server, base = serve(os.path.dirname(path), ap.host)
    try:
        form = {"url": f"{base}/{urllib.parse.quote(os.path.basename(path))}",
                "md5": hashlib.md5(data).hexdigest(), "size": len(data)}
        print(*ap.post("/update_ota", form))
        msg = watch(ap.host, a.timeout, ("Firmware update successful", "Error", "Failed"), console_only=True)
    finally:
        server.shutdown()
    if not msg or "successful" not in msg:
        sys.exit("update failed or timed out; the AP was not rebooted")
    if a.no_reboot:
        return
    print(*ap.post("/reboot"))
    i = ap.wait_online()
    print(f"back online: {i['buildversion']} ({i['sha']})")


def cmd_radio(ap, a):
    directory = os.path.abspath(a.directory)
    if not any(n.startswith("firmware_") and n.endswith(".json") for n in os.listdir(directory)):
        sys.exit(f"no firmware_*.json in {directory}")
    server, base = serve(directory, ap.host)
    try:
        print(*ap.post("/update_c6", {"url": base}))
        msg = watch(ap.host, a.timeout, ("AP is now type", ":-("), console_only=True)
    finally:
        server.shutdown()
    if not msg or "AP is now type" not in msg:
        sys.exit("radio update failed or timed out")
    i = ap.wait_online()  # the AP reboots by itself after a successful radio update
    print(f"back online: radio {i['ap_typename']} version {i['ap_version']:04X}")


def cmd_upload(ap, a):
    if ":" in a.path:
        # Git Bash turns /www/x into C:/Program Files/Git/www/x; the AP then fails to write it
        sys.exit(f"{a.path} is a Windows path: pass the AP path without the leading slash, e.g. www/main.js.gz")
    path = "/" + a.path.lstrip("/")
    boundary = "----aptool" + os.urandom(8).hex()
    with open(a.file, "rb") as f:
        content = f.read()
    # the path field must come before the file: the AP reads it when the file starts
    body = (f'--{boundary}\r\nContent-Disposition: form-data; name="path"\r\n\r\n{path}\r\n'
            f'--{boundary}\r\nContent-Disposition: form-data; name="file"; filename="{os.path.basename(path)}"\r\n'
            f"Content-Type: application/octet-stream\r\n\r\n").encode() + content + f"\r\n--{boundary}--\r\n".encode()
    req = urllib.request.Request(ap.base + "/littlefs_put", data=body,
                                 headers={"Content-Type": "multipart/form-data; boundary=" + boundary})
    try:
        with urllib.request.urlopen(req, timeout=60) as r:
            print(r.status, r.read().decode(), path, len(content), "bytes")
    except urllib.error.HTTPError as e:
        sys.exit(f"{e.code} {e.read().decode(errors='replace')} ({path})")


def cmd_watch(ap, a):
    msg = watch(ap.host, a.seconds, a.until or (), a.out, a.console)
    if a.until and not msg:
        sys.exit("not seen within the time limit")


def cmd_reboot(ap, a):
    print(*ap.post("/reboot"))
    if a.wait:
        i = ap.wait_online()
        print(f"back online: {i['buildversion']}, reset reason {i['resetreason']}")


def main():
    p = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    p.add_argument("--ap", default=os.environ.get("OEPL_AP"),
                   help="AP address (host or host:port); default: environment variable OEPL_AP")
    sub = p.add_subparsers(dest="cmd", required=True)
    sub.add_parser("info", help="firmware, radio and reset reason")
    sub.add_parser("tags", help="all tags with seconds since last seen")
    s = sub.add_parser("backup", help="save a tag's record (for restore)")
    s.add_argument("mac")
    s.add_argument("file")
    s = sub.add_parser("restore", help="put back alias, content and settings from a backup")
    s.add_argument("file")
    s = sub.add_parser("ota", help="flash S3 firmware (firmware.bin) and reboot")
    s.add_argument("firmware")
    s.add_argument("--no-reboot", action="store_true")
    s.add_argument("--timeout", type=float, default=300)
    s = sub.add_parser("radio", help="flash the C6/H2 from a directory with firmware_*.json, e.g. binaries/ESP32-C6")
    s.add_argument("directory")
    s.add_argument("--timeout", type=float, default=300)
    s = sub.add_parser("upload", help="put a file on the AP's filesystem, e.g. a web file from data/www")
    s.add_argument("file")
    s.add_argument("path", help="path on the AP, e.g. www/main.js.gz (the leading / is optional)")
    s = sub.add_parser("watch", help="print /ws messages")
    s.add_argument("--seconds", type=float, default=60)
    s.add_argument("--until", action="append", help="stop at the first message containing this text")
    s.add_argument("--out", help="also append to this file")
    s.add_argument("--console", action="store_true", help="console messages only")
    s = sub.add_parser("reboot", help="reboot the AP")
    s.add_argument("--wait", action="store_true", help="wait until it answers again")
    a = p.parse_args()
    if not a.ap:
        p.error("no AP address: pass --ap or set OEPL_AP")
    globals()["cmd_" + a.cmd](AP(a.ap), a)


if __name__ == "__main__":
    main()
