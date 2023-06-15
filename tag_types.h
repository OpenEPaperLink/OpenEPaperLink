#define SOLUM_154_SSD1619 0
#define SOLUM_29_SSD1619 0x01
#define SOLUM_29_UC8151 0x11
#define SOLUM_42_SSD1619 0x02
#define SOLUM_SEG_UK 0xF0
#define SOLUM_SEG_EU 0xF1
#define SOLUM_NODISPLAY 0xFF

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