## What is this ?

This is a Claude authored script that creates a markdown file summarizing 
the contents of resources/tagtypes.

For example see [defined tagtypes](https://github.com/OpenEPaperLink/OpenEPaperLink/wiki/defined-tagtypes),

## Usage

```
python3 summarize_tagtypes.py [inputs ...] [-r] [-o output.md]
```

- `inputs`: JSON files and/or directories. Optional; see "Default input" below.
- `-r`: include subdirectories of any directory given.
- `-o`: output file. Optional; see "Default output" below.
- Files that fail to parse, or whose top level is not an object, are skipped with a warning on stderr.

### Default input

If no inputs are given and `../../resources/tagtypes/` exists relative to the directory containing the script, the files in that directory are summarized. If it does not exist, the script exits with an error. Explicit inputs always win. The default folder is not searched recursively unless `-r` is given.

### Default output

If `-o` is not given:

- If `../../../OpenEPaperLink.wiki/` exists relative to the directory containing the script, the output is `../../../OpenEPaperLink.wiki/defined-tagtypes.md`.
- Otherwise it is `defined-tagtypes.md` in the current directory.

Both defaults are relative to where the script file lives, not the current working directory (and a symlinked script uses the symlink's location). The output file is overwritten on each run.

## Document title

The first line is `# Tagtypes`. If the first input path is inside a git repository, the date and short id of the last commit are appended:

```
# Tagtypes as of 2026-03-05 @830a8dc
```

- The date is the commit date (`YYYY-MM-DD`), in the timezone recorded in the commit.
- The id is the last commit, not a check that the files are unmodified; uncommitted edits do not change it.
- Outside a repository, with no commits, or without git installed, the title is just `# Tagtypes`.

## Sort order

Width, then height, then bpp, with the filename as the final tie-breaker. Files missing a value sort last.

## Columns

Columns appear in this order:

| Column | Source |
|---|---|
| File | File name (relative to the input directory) |
| Name | `name` |
| Resolution | `width` and `height` joined with `x`, e.g. `600x448` (`?` for a missing half; blank if both are missing or the size is `0x0`) |
| Type | `BW`, plus `R` if the `colortable` has red and `Y` if it has yellow. If any other color is present, `N colors` (N = number of `colortable` entries), written with `&nbsp;` so it does not wrap |
| Options | Entries of the `options` array, comma separated |
| BPP | `bpp` |
| Comp | `g5` if `g5_compression` is present, `zlib` if `zlib_compression` is present, otherwise blank (both present: `g5, zlib`). Goes by key presence, not value |
| Base | `usetemplate`, shown as two-digit uppercase hex |
| Defined Templates | Keys of the `template` object, in numeric order (decimal, as written in the file). If `template` is a list, 0-based indexes |

Missing fields give blank cells. A `|` in a name is escaped so it does not break the table.

### Example row

```
| m3.json | M3 6.0" | 600x448 | BWR | button, led | 2 | zlib | 01 | 1, 9, 10, 11 |
```

