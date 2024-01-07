makeit.exe clean all
flashing\telink_flasher.exe 1 8258 ac
rem flashing\telink_flasher.exe 1 8258 wf 0 -s 512k -e
flashing\telink_flasher.exe 1 8258 wf 0 -i TLSR_OpenEPaperLink.bin
flashing\telink_flasher.exe 1 8258 rst -f
