rm mkfs.oepl genprofile
g++ mkfs.oepl.cpp -lz -o mkfs.oepl
g++ genprofile.cpp -o genprofile
chmod +x mkfs.oepl
chmod +x genprofile


