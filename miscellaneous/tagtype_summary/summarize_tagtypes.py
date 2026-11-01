#!/usr/bin/env python3
"""
summarize_tagtypes.py - Build one Markdown table (one row per JSON file) from a set of
JSON files, sorted by width, height, then bpp.

Usage:
    python3 summarize_tagtypes.py path/to/jsons/            # writes defined-tagtypes.md
    python3 summarize_tagtypes.py a.json b.json -o out.md
    python3 summarize_tagtypes.py path/to/jsons/ -r         # include subdirectories

Default input: if no inputs are given and ../../resources/tagtypes/ exists relative
to the directory containing this script, the files in that directory are used.

Default output file: if ../../../OpenEPaperLink.wiki/ exists relative to the
directory containing this script, ../../../OpenEPaperLink.wiki/defined-tagtypes.md
is written; otherwise defined-tagtypes.md in the current directory. Use -o to
choose a different file.

Edit COLUMNS below to add, remove or reorder columns.
"""

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

def compression(data):
    """"g5" if the file has g5_compression, "zlib" if it has zlib_compression."""
    found = []
    if "g5_compression" in data:
        found.append("g5")
    if "zlib_compression" in data:
        found.append("zlib")
    return ", ".join(found)


def panel_type(data):
    """"BW" plus "R" if red and "Y" if yellow; "<n> colors" if any other color."""
    table = data.get("colortable")
    if isinstance(table, dict):
        names = set(str(k).lower() for k in table)
    elif isinstance(table, list):
        names = set(str(k).lower() for k in table)
    else:
        names = set()
    if names - set(["black", "white", "red", "yellow"]):
        return "%d&nbsp;colors" % len(table)  # non-breaking space: no wrapping
    result = "BW"
    if "red" in names:
        result += "R"
    if "yellow" in names:
        result += "Y"
    return result


def templates(data):
    """Keys of the "template" object (numeric order), or 0-based indexes if it is a list."""
    entries = data.get("template", data.get("templates"))
    if isinstance(entries, dict):
        def order(k):
            k = str(k)
            return (0, int(k), k) if k.isdigit() else (1, 0, k)
        return ", ".join(str(k) for k in sorted(entries, key=order))
    if isinstance(entries, list):
        return ", ".join(str(i) for i in range(len(entries)))
    return ""


def resolution(data):
    """"<width>x<height>", e.g. 640x384 ("?" for a missing half; blank if both are
    missing or the size is 0x0)."""
    w, h = data.get("width"), data.get("height")
    if (w is None and h is None) or (w == 0 and h == 0):
        return ""
    return "%sx%s" % ("?" if w is None else w, "?" if h is None else h)


def hex2(value):
    """Decimal number -> two-digit uppercase hex (5 -> "05", 255 -> "FF")."""
    if isinstance(value, int) and not isinstance(value, bool):
        return "%02X" % value
    return fmt(value)


# (column heading, source). The source is a key in the JSON file, None for the
# file name, or a function that takes the parsed data and returns the text.
COLUMNS = [
    ("File", None),
    ("Name", "name"),
    ("Resolution", resolution),
    ("Type", panel_type),
    ("Options", "options"),
    ("BPP", "bpp"),
    ("Comp", compression),
    ("Base", lambda d: hex2(d.get("usetemplate"))),
    ("Defined Templates", templates),
]


def fmt(value):
    if value is None:
        return ""
    if isinstance(value, list):
        return ", ".join(str(v) for v in value)
    return str(value)


def cell(text):
    return text.replace("|", "\\|").replace("\n", " ").strip()


def num(value):
    """Sort key: numbers sort numerically, anything missing goes last."""
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        return value
    return float("inf")


def script_relative(*parts):
    """Path relative to the directory containing this script (not the current directory)."""
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.normpath(os.path.join(here, *parts))


def default_input():
    """../../resources/tagtypes/ relative to the script, or None if it does not exist."""
    folder = script_relative("..", "..", "resources", "tagtypes")
    return folder if os.path.isdir(folder) else None


def default_output():
    """OpenEPaperLink.wiki/defined-tagtypes.md three levels above the script's
    directory if that wiki directory exists, otherwise defined-tagtypes.md."""
    wiki = script_relative("..", "..", "..", "OpenEPaperLink.wiki")
    if os.path.isdir(wiki):
        return os.path.join(wiki, "defined-tagtypes.md")
    return "defined-tagtypes.md"


def git_commit_info(path):
    """(date, short id) of the last commit in the git repository containing path,
    or None if there is no repository (or git is not installed)."""
    p = Path(path)
    folder = p if p.is_dir() else p.parent
    try:
        out = subprocess.run(
            ["git", "-C", str(folder), "log", "-1", "--date=short", "--format=%cd %h"],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, universal_newlines=True)
    except OSError:  # git is not installed
        return None
    parts = out.stdout.split()
    if out.returncode != 0 or len(parts) != 2:
        return None
    return parts[0], parts[1]


def find_files(inputs, recursive):
    found = []
    for item in inputs:
        p = Path(item)
        if p.is_dir():
            it = p.rglob("*.json") if recursive else p.glob("*.json")
            found += [(f.relative_to(p).as_posix(), f) for f in sorted(it) if f.is_file()]
        elif p.is_file():
            found.append((p.name, p))
        else:
            print("warning: %s not found" % item, file=sys.stderr)
    return found


def main():
    ap = argparse.ArgumentParser(description="Summarize JSON files as a Markdown table.")
    ap.add_argument("inputs", nargs="*",
                    help="JSON files and/or directories (default: ../../resources/tagtypes/ "
                         "relative to this script, if it exists)")
    ap.add_argument("-o", "--output",
                    help="output file (default: ../../../OpenEPaperLink.wiki/defined-tagtypes.md "
                         "relative to this script if that directory exists, else defined-tagtypes.md)")
    ap.add_argument("-r", "--recursive", action="store_true", help="recurse into directories")
    args = ap.parse_args()
    if not args.inputs:
        folder = default_input()
        if folder is None:
            ap.error("no inputs given and ../../resources/tagtypes/ was not found relative to the script")
        args.inputs = [folder]
    if not args.output:
        args.output = default_output()

    rows = []
    for display, path in find_files(args.inputs, args.recursive):
        try:
            with open(path, encoding="utf-8-sig") as f:
                data = json.load(f)
        except (OSError, ValueError) as e:
            print("skipped %s: %s" % (display, e), file=sys.stderr)
            continue
        if not isinstance(data, dict):
            print("skipped %s: top level is not an object" % display, file=sys.stderr)
            continue
        rows.append((display, data))

    if not rows:
        sys.exit("No usable JSON files found.")

    rows.sort(key=lambda r: (num(r[1].get("width")), num(r[1].get("height")),
                             num(r[1].get("bpp")), r[0]))

    title = "Tagtypes"
    commit = git_commit_info(args.inputs[0])
    if commit:
        title += " as of %s @%s" % commit

    lines = [
        "# " + title,
        "",
        "| " + " | ".join(h for h, _ in COLUMNS) + " |",
        "|" + "|".join("---" for _ in COLUMNS) + "|",
    ]
    for display, data in rows:
        cells = []
        for _, source in COLUMNS:
            if source is None:
                cells.append(display)
            elif callable(source):
                cells.append(source(data))
            else:
                cells.append(fmt(data.get(source)))
        lines.append("| " + " | ".join(cell(c) for c in cells) + " |")
    lines.append("")

    with open(args.output, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote %d row(s) to %s" % (len(rows), args.output))


if __name__ == "__main__":
    main()
