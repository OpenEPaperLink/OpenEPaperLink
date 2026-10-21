# Builds the C6/H2 radio firmware: C6, C6 for the 4inch AP (Uart0 pins) and H2.
#
#   powershell -ExecutionPolicy Bypass -File ARM_Tag_FW\build_radio.ps1            build only
#   powershell -ExecutionPolicy Bypass -File ARM_Tag_FW\build_radio.ps1 -Publish   also copy the
#       app binaries to binaries\ESP32-C6 and binaries\ESP32-H2 and set their version in
#       firmware_C6.json, firmware_C6_Uart0.json and firmware_H2.json
#
# The version comes from `uint16_t version = 0x....;` in OpenEPaperLink_esp32_C6_AP\main\main.c
# (the H2 builds from the same sources). Bootloader and partition table are not copied.
# The builds go to OpenEPaperLink_esp32_C6_AP\build\release\{c6,c6u0,h2}.
param(
    [switch]$Publish,
    # ESP-IDF to use; the release binaries are built with 5.5.5
    [string]$Idf = (Join-Path $env:USERPROFILE 'esp\v5.5.5\esp-idf')
)
$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$c6 = Join-Path $PSScriptRoot 'OpenEPaperLink_esp32_C6_AP'
$h2 = Join-Path $PSScriptRoot 'OpenEPaperLink_esp32_H2_AP'
$out = Join-Path $c6 'build\release'

$main = Get-Content -Raw (Join-Path $c6 'main\main.c')
if ($main -notmatch 'uint16_t version = 0x([0-9A-Fa-f]{4});') { throw 'version not found in main.c' }
$version = $Matches[1].ToUpper()
Write-Host "Radio firmware version $version"

. (Join-Path $Idf 'export.ps1') | Out-Null
$ErrorActionPreference = 'Continue'   # idf.py writes progress to stderr

Remove-Item -Recurse -Force $out -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $out | Out-Null
Set-Content -Encoding ascii (Join-Path $out '4inch.defaults') 'CONFIG_OEPL_HARDWARE_PROFILE_4inch=y'

function Build($proj, $name, $target, $defaults) {
    $b = Join-Path $out $name
    Write-Host "=== $name"
    $cfg = @('-C', $proj, '-B', $b, '-D', "SDKCONFIG=$b.sdkconfig", '-D', "SDKCONFIG_DEFAULTS=$defaults")
    idf.py @cfg set-target $target
    if ($LASTEXITCODE) { throw "set-target $name failed" }
    idf.py @cfg build
    if ($LASTEXITCODE) { throw "build $name failed" }
}

Build $c6 'c6' 'esp32c6' "$c6\sdkconfig.defaults"
Build $c6 'c6u0' 'esp32c6' "$c6\sdkconfig.defaults;$out\4inch.defaults"
Build $h2 'h2' 'esp32h2' "$h2\sdkconfig.defaults"
Write-Host "ALL BUILDS OK ($version)"
if (-not $Publish) { exit 0 }

$ErrorActionPreference = 'Stop'
function Publish($build, $bin, $dir, $published, $json) {
    Copy-Item (Join-Path $out "$build\$bin") (Join-Path $root "binaries\$dir\$published")
    $path = Join-Path $root "binaries\$dir\$json"
    $text = [IO.File]::ReadAllText($path)
    $pattern = '("filename":\s*"' + [regex]::Escape($published) + '",\s*"address":\s*"[^"]*",\s*"version":\s*")[0-9A-Fa-f]{4}'
    if ($text -notmatch $pattern) { throw "$published not found in $json" }
    [IO.File]::WriteAllText($path, ([regex]::Replace($text, $pattern, "`${1}$version")))
    Write-Host "binaries\$dir\$published -> $version in $json"
}

Publish 'c6' 'OpenEPaperLink_esp32_C6.bin' 'ESP32-C6' 'OpenEPaperLink_esp32_C6.bin' 'firmware_C6.json'
Publish 'c6u0' 'OpenEPaperLink_esp32_C6.bin' 'ESP32-C6' 'OpenEPaperLink_esp32_C6uart0.bin' 'firmware_C6_Uart0.json'
Publish 'h2' 'OpenEPaperLink_esp32_H2.bin' 'ESP32-H2' 'OpenEPaperLink_esp32_H2.bin' 'firmware_H2.json'
