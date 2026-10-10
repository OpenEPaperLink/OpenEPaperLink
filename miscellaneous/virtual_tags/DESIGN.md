# Virtual tags: simulator design

Goal: develop and test the AP without e-paper hardware, without changing the AP firmware.
Alternative for PR #590, which adds virtual tags inside the firmware.

## Idea

APs already share tags over UDP: a tag that checks in at one AP shows up as an *external* tag on the
other APs, which generate its content and hand the image to the AP the tag is connected to.

The simulator is a small Python script on a PC that behaves like such a second AP. Its virtual tags
"check in" over UDP, so the real AP treats them as external tags: it creates them in its tag
database, generates their content, and offers the image back to the simulator. Everything the AP does
for these tags is the normal firmware code path.

## Protocol (as used by `udp.cpp`)

Multicast group `239.10.0.1`, port `16033`. Every packet is one type byte followed by a packed struct
(little endian, see `oepl-proto.h` / `oepl-esp-ap-proto.h`).

| Direction | Type | Payload | Meaning |
|---|---|---|---|
| sim → AP | `0xE6` PKT_AVAIL_DATA_INFO | `espAvailDataReq` (src mac + `AvailDataReq`) | tag checks in; AP calls `processDataReq(local=false)` |
| AP → sim | `0xE5` PKT_AVAIL_DATA_REQ | `pendingData` (`AvailDataInfo` + attemptsLeft + targetMac) | data available for a tag |
| sim → AP | HTTP `GET /getdata?mac=<16 hex>&md5=<16 hex dataVer>` | | download the data (image) |
| sim → AP | `0xEA` PKT_XFER_COMPLETE | `espXferComplete` (src mac) | tag got the data; AP marks it delivered |
| sim → AP | `0xED` PKT_XFER_TIMEOUT | `espXferComplete` | simulate a failed transfer |
| both | `0x82` PKT_TAGINFO | `TagInfo` (sync version 0xAA01) | tag config sync between APs (optional) |

`AvailDataReq` fields the simulator sets: `lastPacketLQI`, `lastPacketRSSI` (must be non-zero, the AP
only generates content for tags with an RSSI), `temperature`, `batteryMv`, `hwType` (selects the tag
type json), `wakeupReason` (0 timed, 1 boot, 4/5/6 button 1/2/3, 0xFC first boot), `capabilities`,
`tagSoftwareVersion`, `currentChannel`.

The image format follows the tag type and `tagSoftwareVersion`: raw 1/2 bpp, zlib (`DATATYPE_IMG_ZLIB`,
when the version is at least the type's `zlib_compression` value) or G5. The simulator can report a low
software version to always get raw images, or decode zlib (4-byte length, then a zlib stream with the
6-byte header and the bit planes).

## Script outline (`oepl_virtual_tags.py`)

- `--ap <ip>`: the AP to talk to (for `/getdata`; UDP goes to the multicast group).
- `add <mac> --hwtype <hex>`: define a virtual tag (stored in a small json file next to the script).
- Main loop per tag: send a check-in every N seconds (or the `nextCheckIn` the AP asked for).
- On PKT_AVAIL_DATA_REQ for one of our macs: download via `/getdata`, decode to PNG using the tag type
  json from `resources/tagtypes` (width, height, bpp, rotation, color table), save it as
  `<mac>.png`, then send PKT_XFER_COMPLETE.
- Commands while running: `press <mac> 1|2|3` (check-in with a button wakeup reason), `reboot <mac>`
  (wakeup reason boot), `drop <mac>` (stop checking in, to test timeouts), `timeout <mac>`.
- Optional: a tiny local web page showing the PNGs, refreshed when a new image arrives.

## What it can test

- Content generation per content type and tag type, including button-driven content (timestamps).
- The pending queue: replacing images, xfer complete, timeouts, a tag that stops checking in.
- Scheduling: check-in intervals, `nextupdate`, `expectedNextCheckin` (#466), with exact timestamps.
- Many tags at once (load testing), without hardware.

## Limits

- The radio path (C6/H2, block transfers, `processBlockRequest`) is not exercised: external tags get
  their data through `/getdata`.
- Virtual tags end up in the AP's tag database like real external tags; delete them in the web UI when
  done (or use a separate test AP).
