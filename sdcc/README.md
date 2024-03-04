This subdirectory contains scripts to build the correct version of SDCC (4.2.0) for compiling 8051 based firmware on Linux.

To use simply source setup_sdcc.sh before building.  The first time setup_sdcc.sh is run proper version of SDCC sources will be checkout from github and built locally.
Finally the environment variable CC will be set to 'sdcc' and the path to the local version of sdcc will be added to the beginning of the path.

The script unset_sdcc.sh can be sourced to unset the CC environment variable and to restore the path.

For example

````
skip@Dell-7040:~/esl/OpenEPaperLink$ cd zbs243_AP_FW
skip@Dell-7040:~/esl/OpenEPaperLink/zbs243_AP_FW$ . ../sdcc/setup.sh
Cloning into 'sdcc'...

(truncated...)

Added /home/skip/esl/OpenEPaperLink/sdcc/sdcc-4.2.0/bin to PATH
skip@Dell-7040:~/esl/OpenEPaperLink/zbs243_AP_FW$ make
sdcc -c main.c -DAP_FW=1 --code-size 0xfc00 -Isoc/zbs243 -DSOC_ZBS243 --xram-loc 0xe000 --xram-size 0x2000 --model-large -Icpu/8051 -mmcs51 --std-c2x --opt-code-size --peep-file cpu/8051/peep.def --fomit-frame-pointer -Iboard/zbs29_ssd1619 -Isoc/zbs243 -Icpu/8051 -I. -o main.rel

(truncated...)

objcopy main.ihx main.bin -I ihex -O binary
skip@Dell-7040:~/esl/OpenEPaperLink/zbs243_AP_FW$ . ../sdcc/unset_sdcc.sh
skip@Dell-7040:~/esl/OpenEPaperLink/zbs243_AP_FW$
````

