struct tagHardwareProfile {
    uint8_t macAddress[8];
    uint16_t xRes;
    uint16_t yRes;
    uint8_t bpp;
    uint8_t controllerType;
    uint8_t OEPLType;
    uint32_t RC32Mcal;
    uint32_t RC32Kcal;
    uint8_t reserved[8];
} __attribute__((packed));
