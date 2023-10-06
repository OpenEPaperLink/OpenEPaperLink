import os
import json

version = "0022"  # You can set your desired version here.

types = {
    0x00: "SOLUM_154_SSD1619-tag-00-" + version + ".bin",
    0x01: "SOLUM_29_SSD1619-tag-01-" + version + ".bin",
    0xF0: "Tag_FW_Segmented_UK.bin",
    0x02: "SOLUM_42_SSD1619-tag-02-" + version + ".bin",
    0x11: "SOLUM_29_UC8151-tag-11-" + version + ".bin",
}

binpath = "../binaries/Tag"
tocmaxsize = 512

toc = []
output = b'\0' * tocmaxsize  # Initialize as bytes

binaries = [file for file in os.listdir(binpath) if 'Pack' not in file and version in file]
for file in binaries:
    file = file.strip()
    type = -1
    for typeid, typefile in types.items():
        if typefile == file:
            type = typeid
    if type != -1:
        print("Adding filetype <{}>".format(file))
    with open(os.path.join(binpath, file), 'rb') as binary_file:
        binary = binary_file.read()
        length = len(binary)
        offset = len(output)
        subarr = {
            'type': type,
            'version': version,
            'name': file,
            'offset': offset,
            'length': length,
        }
        toc.append(subarr)
        output += binary

jtoc = json.dumps(toc)
jtoc = jtoc.replace("'", '"')
tocsize = len(jtoc)
if tocsize > tocmaxsize:
    raise ValueError("TOC is too big! (" + str(tocsize) + "). Adjust size and try again")

# Encode jtoc as bytes
jtoc = jtoc.encode('utf-8')

# Concatenate bytes and write to the file
output = jtoc + output[len(jtoc):]
with open(os.path.join(binpath, "Tag_FW_Pack.bin"), 'wb') as output_file:
    output_file.write(output)

print(toc)
print("All done.")
