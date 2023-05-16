import os
import json

types = {
    0x00: "AP_FW_1.54.bin",
    0x01: "AP_FW_2.9.bin",
    0xF0: "AP_FW_Segmented_UK.bin",
    0xFF: "AP_FW_Nodisplay.bin"
}

binpath = "../binaries/"
tocmaxsize = 512

toc = []
output = bytearray(tocmaxsize)

# Read version from main.c
with open("main.c") as file:
    lines = file.readlines()
    version_line = next(line for line in lines if "version" in line and "uint16_t" in line)
    _, version = version_line.split("= 0x")
    version = int(version.strip().split(";")[0], 16)

binaries = [file for file in os.listdir(binpath) if file.startswith("AP_FW") and not file.endswith("Pack")]
for file in binaries:
    file = file.strip()
    type_id = -1
    for typeid, typefile in types.items():
        if typefile == file:
            type_id = typeid
            break
    if type_id == -1:
        raise ValueError("We don't recognize filetype <{}>, sorry...".format(file))
    with open(os.path.join(binpath, file), "rb") as binary_file:
        binary = binary_file.read()
        length = len(binary)
        offset = len(output)
        subarr = {
            'type': type_id,
            'version': version,
            'name': file,
            'offset': offset,
            'length': length
        }
        toc.append(subarr)
        output.extend(binary)

jtoc = json.dumps(toc)
tocsize = len(jtoc)
if tocsize > tocmaxsize:
    raise ValueError("TOC is too big! Adjust size and try again.")
output[:len(jtoc)] = jtoc.encode()
with open(os.path.join(binpath, "AP_FW_Pack.bin"), "wb") as output_file:
    output_file.write(bytes(output))

print(toc)
print("All done.")
