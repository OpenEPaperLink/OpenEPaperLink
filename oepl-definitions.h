#define SOLUM_154_SSD1619 0
#define SOLUM_29_SSD1619 0x01
#define SOLUM_29_BW_SSD1619 0x21
#define SOLUM_29_UC8151 0x11
#define SOLUM_42_SSD1619 0x02
#define SOLUM_SEG_UK 0xF0
#define SOLUM_SEG_EU 0xF1
#define SOLUM_NODISPLAY 0xFF
#define ESP32_C6 0xC6

#define SOLUM_M2_BWR_16 0x20
#define SOLUM_M2_BWR_29 0x23
#define SOLUM_M2_BWR_42 0x24
#define SOLUM_M2_BWR_75 0x26

#define SOLUM_M3_BWR_16 0x30
#define SOLUM_M3_BWY_16 0x38
#define SOLUM_M3_BWR_22 0x31
#define SOLUM_M3_BWY_22 0x39
#define SOLUM_M3_BWR_26 0x32
#define SOLUM_M3_BWY_26 0x3A
#define SOLUM_M3_BWR_29 0x33
#define SOLUM_M3_BWY_29 0x3B
#define SOLUM_M3_BWR_42 0x34
#define SOLUM_M3_BWY_42 0x3C
#define SOLUM_M3_BWR_60 0x35
#define SOLUM_M3_BWY_60 0x3D
#define SOLUM_M3_BWR_75 0x36
#define SOLUM_M3_BWY_75 0x3E
#define SOLUM_M3_BWR_116 0x37
#define SOLUM_M3_BWY_116 0x3F

#define CAPABILITY_SUPPORTS_COMPRESSION 0x02
#define CAPABILITY_SUPPORTS_CUSTOM_LUTS 0x04
#define CAPABILITY_ALT_LUT_SIZE 0x08
#define CAPABILITY_HAS_EXT_POWER 0x10
#define CAPABILITY_HAS_WAKE_BUTTON 0x20
#define CAPABILITY_HAS_NFC 0x40
#define CAPABILITY_NFC_WAKE 0x80

#define DATATYPE_NOUPDATE 0
#define DATATYPE_IMG_BMP 2
#define DATATYPE_FW_UPDATE 3
#define DATATYPE_IMG_DIFF 0x10             // always 1BPP
#define DATATYPE_IMG_RAW_1BPP 0x20         // 2888 bytes for 1.54"  / 4736 2.9" / 15000 4.2"
#define DATATYPE_IMG_RAW_2BPP 0x21         // 5776 bytes for 1.54"  / 9472 2.9" / 30000 4.2"
#define DATATYPE_IMG_RAW_1BPP_DIRECT 0x3F  // only for 1.54", don't write to EEPROM, but straightaway to the EPD
#define DATATYPE_UK_SEGMENTED 0x51         // Segmented data for the UK Segmented display type (contained in availableData Reply)
#define DATATYPE_EU_SEGMENTED 0x52         // Segmented data for the EU/DE Segmented display type (contained in availableData Reply)
#define DATATYPE_NFC_RAW_CONTENT 0xA0      // raw memory content for the NT3H1101
#define DATATYPE_NFC_URL_DIRECT 0xA1       // URL format for NT3H1101
#define DATATYPE_TAG_CONFIG_DATA 0xA8      // Config data for tag
#define DATATYPE_COMMAND_DATA 0xAF         // Command for the tag to execute  (contained in availableData Reply)
#define DATATYPE_CUSTOM_LUT_OTA 0xB0       // Custom OTA updated LUT

#define CMD_DO_REBOOT 0
#define CMD_DO_SCAN 1
#define CMD_DO_RESET_SETTINGS 2
#define CMD_DO_DEEPSLEEP 3
#define CMD_DO_LEDFLASH 4
#define CMD_ERASE_EEPROM_IMAGES 5
#define CMD_ENTER_SLIDESHOW_FAST 0x06
#define CMD_ENTER_SLIDESHOW_MEDIUM 0x07
#define CMD_ENTER_SLIDESHOW_SLOW 0x08
#define CMD_ENTER_SLIDESHOW_GLACIAL 0x09
#define CMD_ENTER_NORMAL_MODE 0x0F

#define WAKEUP_REASON_TIMED 0
#define WAKEUP_REASON_GPIO 2
#define WAKEUP_REASON_NFC 3
#define WAKEUP_REASON_BUTTON1 4
#define WAKEUP_REASON_BUTTON2 5
#define WAKEUP_REASON_RF 0x0F
#define WAKEUP_REASON_FIRSTBOOT 0xFC
#define WAKEUP_REASON_NETWORK_SCAN 0xFD
#define WAKEUP_REASON_WDT_RESET 0xFE


#define EPD_LUT_DEFAULT 0
#define EPD_LUT_NO_REPEATS 1
#define EPD_LUT_FAST_NO_REDS 2
#define EPD_LUT_FAST 3
#define EPD_LUT_OTA 0x10

// these are the 'custom image' arguments that will be sent in addition to their 'type'. 
#define CUSTOM_IMAGE_NOCUSTOM 0x00         // regular image type
#define CUSTOM_IMAGE_SPLASHSCREEN 0x01     // will show at first boot/powerup
#define CUSTOM_IMAGE_LOST_CONNECTION 0x02  // this image will be shown (if it exists on the tag) if the tag looses its connection
#define CUSTOM_IMAGE_APFOUND 0x03          // this image will be shown during bootup if an AP was found
#define CUSTOM_IMAGE_NOAPFOUND 0x04        // shown if during bootup no AP was found
#define CUSTOM_IMAGE_LONGTERMSLEEP 0x05     // shown if long term sleep is entered via command
// UNUSED: 0x06-0x0F
#define CUSTOM_IMAGE_SLIDESHOW 0x0F  // image is part of a slideshow
#define CUSTOM_IMAGE_BUTTON1 0x10
#define CUSTOM_IMAGE_BUTTON2 0x11
// UNUSED: 0x12 to 0x1B
#define CUSTOM_IMAGE_RF_WAKE 0x1C
#define CUSTOM_IMAGE_GPIO 0x1D
#define CUSTOM_IMAGE_NFC_WAKE 0x1E

#define TAG_CUSTOM_MODE_NONE 0x00
#define TAG_CUSTOM_SLIDESHOW_FAST 0x06
#define TAG_CUSTOM_SLIDESHOW_MEDIUM 0x07
#define TAG_CUSTOM_SLIDESHOW_SLOW 0x08
#define TAG_CUSTOM_SLIDESHOW_GLACIAL 0x09