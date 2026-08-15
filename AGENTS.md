# Gicisky 2.9" BWRY Development

## Goal
Complete support for the Gicisky 2.9" BWRY BLE electronic shelf label in OpenEPaperLink.

## Working branch
Use `gicisky-bwry-29`. Do not change `master`.

## Starting point
This branch is now based directly on the exact head commit from upstream OpenEPaperLink PR #592 (`376b193709b4521115178466a372e1d1f2a50162`). The PR changes are already present. Do not re-apply PR #592.

PR #592 adds:
- Gicisky hardware type `0x2E`
- OpenEPaperLink type `0xBB`
- `resources/tagtypes/BB.json`
- 296x128, 2 bpp, white/black/red/yellow

Detection works on the test hardware, but image upload/display does not.

## Test hardware
- PlatformIO environment: `BLE_ONLY_AP`
- ESP32-S3
- 16 MB flash
- 8 MB PSRAM

## Confirmed working
- `BLE_ONLY_AP` builds successfully.
- Hardware type `0x2E` is detected.
- Tag appears as OpenEPaperLink type `0xBB`.
- `BB.json` is valid enough for recognition.
- OTA deployment to the test AP works.

## Current problem
The tag is visible and recognised, but image upload/display is not working correctly. Focus on the Gicisky BLE transfer protocol and BWRY image encoding rather than discovery.

## Research leads
OpenEPaperLink issue #584 documents the same device. The PR author got recognition working but later suspected the BWRY protocol had changed.

Known-working third-party Gicisky BWRY implementations indicate packed 2-bit pixels, four pixels per byte. Verify exact packing/order before implementation. Reported logical colour values:
- `00` black
- `01` white
- `10` yellow
- `11` red

Also compare:
- BLE command sequence
- packet/chunk size
- acknowledgements
- retries
- inter-packet delay
- start/end commands

Do not assume `"bpp": 2` in `BB.json` is sufficient.

## Primary code areas
Start with:
- `ESP32_AP-Flasher/src/ble_filter.cpp`
- the Gicisky BLE writer/upload path
- `compress_image()` and related image conversion code
- `oepl-definitions.h`
- `resources/tagtypes/BB.json`

Compare behaviour for existing Gicisky BWR/BWR1 tags and keep BWRY-specific changes isolated.

## Build
From `ESP32_AP-Flasher`:

```powershell
python gzip_wwwfiles.py
python -m platformio run -e BLE_ONLY_AP
python -m platformio run -e BLE_ONLY_AP -t buildfs
```

Firmware output:
`ESP32_AP-Flasher/.pio/build/BLE_ONLY_AP/firmware.bin`

## Test/development approach
1. Inspect the existing Gicisky upload path.
2. Compare it with a known-working BWRY implementation.
3. Make the smallest BWRY-specific change.
4. Build `BLE_ONLY_AP` after each meaningful change.
5. Add useful diagnostic logging for hardware tests.
6. Keep commits small and testable.
7. Do not broadly refactor unrelated OpenEPaperLink code.

## Validation
Success requires:
- detection as `0xBB`
- successful BLE transfer
- correct black/white
- correct red
- correct yellow
- correct four-colour image
- correct orientation
- reliable repeated updates
- no regression to existing Gicisky tag types
