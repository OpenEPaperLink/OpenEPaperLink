## 'Filesystem' tools for 88MZ100 ##

- compile_fs_tools.sh
  - Compiles 'genprofile.cpp' and and 'mkfs.oepl.cpp' utilities. Expects g++ in paths
- mkfs.oepl.sh
  - Converts relevant images to binaries, generates a tag profile, and assembles a binary filesystem
- genprofile
  - Generates a binary tag profile from 'tagprofile.json'
- mkbinaryimage.py
  - Generates OEPL-compatible binary image file
- mkfs.oepl
  - Optionally compresses files and fonts and assembles them into a filesystem
- tagprofile.json
  - Tag profile. Used if the serial flasher is used to flash/program a tag. Contains type and mac address
    
![image](https://github.com/jjwbruijn/OpenEPaperLink/assets/2544995/adb53e40-0b8a-4130-9f28-6ac8f008d54b)
