
#ifndef Touch_GT911_H
#define Touch_GT911_H

#include "Arduino.h"
#include <Wire.h>

#define GT911_ADDR1 (uint8_t)0x5D
#define GT911_ADDR2 (uint8_t)0x14

// Real-time command (Write only)
#define GT911_COMMAND       (uint16_t)0x8040
#define GT911_ESD_CHECK     (uint16_t)0x8041
#define GT911_COMMAND_CHECK (uint16_t)0x8046

#define GT911_STRETCH_R0               (uint16_t)0x805E
#define GT911_STRETCH_R1               (uint16_t)0x805F
#define GT911_STRETCH_R2               (uint16_t)0x8060
#define GT911_STRETCH_RM               (uint16_t)0x8061
#define GT911_DRV_GROUPA_NUM           (uint16_t)0x8062
#define GT911_CONFIG_START             (uint16_t)0x8047
#define GT911_CONFIG_VERSION           (uint16_t)0x8047
#define GT911_X_OUTPUT_MAX_LOW         (uint16_t)0x8048
#define GT911_X_OUTPUT_MAX_HIGH        (uint16_t)0x8049
#define GT911_Y_OUTPUT_MAX_LOW         (uint16_t)0x804A
#define GT911_Y_OUTPUT_MAX_HIGH        (uint16_t)0x804B
#define GT911_TOUCH_NUMBER             (uint16_t)0x804C
#define GT911_MODULE_SWITCH_1          (uint16_t)0x804D
#define GT911_MODULE_SWITCH_2          (uint16_t)0x804E
#define GT911_SHAKE_COUNT              (uint16_t)0x804F
#define GT911_FILTER                   (uint16_t)0x8050
#define GT911_LARGE_TOUCH              (uint16_t)0x8051
#define GT911_NOISE_REDUCTION          (uint16_t)0x8052
#define GT911_SCREEN_TOUCH_LEVEL       (uint16_t)0x8053
#define GT911_SCREEN_RELEASE_LEVEL     (uint16_t)0x8054
#define GT911_LOW_POWER_CONTROL        (uint16_t)0x8055
#define GT911_REFRESH_RATE             (uint16_t)0x8056
#define GT911_X_THRESHOLD              (uint16_t)0x8057
#define GT911_Y_THRESHOLD              (uint16_t)0x8058
#define GT911_SPACE_TOP_BOTTOM         (uint16_t)0x805B
#define GT911_PANEL_TX_GAIN            (uint16_t)0x806B
#define GT911_PANEL_RX_GAIN            (uint16_t)0x806C
#define GT911_PANEL_DUMP_SHIFT         (uint16_t)0x806D
#define GT911_DRV_FRAME_CONTROL        (uint16_t)0x806E
#define GT911_CHARGING_LEVEL_UP        (uint16_t)0x806F
#define GT911_MODULE_SWITCH3           (uint16_t)0x8070
#define GT911_GESTURE_DIS              (uint16_t)0X8071
#define GT911_GESTURE_LONG_PRESS_TIME  (uint16_t)0x8072
#define GT911_X_Y_SLOPE_ADJUST         (uint16_t)0X8073
#define GT911_GESTURE_CONTROL          (uint16_t)0X8074
#define GT911_GESTURE_SWITCH1          (uint16_t)0X8075
#define GT911_GESTURE_SWITCH2          (uint16_t)0X8076
#define GT911_GESTURE_REFRESH_RATE     (uint16_t)0x8077
#define GT911_GESTURE_TOUCH_LEVEL      (uint16_t)0x8078
#define GT911_NEWGREENWAKEUPLEVEL      (uint16_t)0x8079
#define GT911_FREQ_HOPPING_START       (uint16_t)0x807A
#define GT911_CONFIG_CHKSUM            (uint16_t)0X80FF
#define GT911_CONFIG_FRESH             (uint16_t)0X8100
#define GT911_CONFIG_SIZE              (uint16_t)0xFF-0x46
// Coordinate information
#define GT911_PRODUCT_ID        (uint16_t)0X8140
#define GT911_FIRMWARE_VERSION  (uint16_t)0X8140
#define GT911_RESOLUTION        (uint16_t)0X8140
#define GT911_VENDOR_ID         (uint16_t)0X8140
#define GT911_IMFORMATION       (uint16_t)0X8140
#define GT911_POINT_INFO        (uint16_t)0X814E
#define GT911_POINT_1           (uint16_t)0X814F
#define GT911_POINT_2           (uint16_t)0X8157
#define GT911_POINT_3           (uint16_t)0X815F
#define GT911_POINT_4           (uint16_t)0X8167
#define GT911_POINT_5           (uint16_t)0X816F
#define GT911_POINTS_REG        {GT911_POINT_1, GT911_POINT_2, GT911_POINT_3, GT911_POINT_4, GT911_POINT_5}

class TPoint {
  public:
    TPoint(void);
    TPoint(uint8_t id, uint16_t x, uint16_t y, uint16_t size);

    bool operator==(TPoint);
    bool operator!=(TPoint);

    uint8_t id;
    uint16_t x;
    uint16_t y;
    uint8_t size;
};

class Touch_GT911 {
  public:
    Touch_GT911(uint8_t _sda, uint8_t _scl,uint16_t _width, uint16_t _height);
    void begin(uint8_t _addr=GT911_ADDR1);
    void setRotation(uint8_t rot);
    void setResolution(uint16_t _width, uint16_t _height);
    uint8_t getGesture(void);
    void read(void);
    uint8_t isLargeDetect;
    uint8_t touches = 0;
    bool isTouched = false;
    TPoint points[5];
  private:
    void calculateChecksum();
    void reConfig();
    TPoint readPoint(uint8_t *data);
    void writeByteData(uint16_t reg, uint8_t val);
    uint8_t readByteData(uint16_t reg);
    void writeBlockData(uint16_t reg, uint8_t *val, uint8_t size);
    void readBlockData(uint8_t *buf, uint16_t reg, uint8_t size);
    uint8_t addr;
    uint8_t pinSda;
    uint8_t pinScl;
    uint16_t width;
    uint16_t height;
    uint8_t configBuf[GT911_CONFIG_SIZE];
};

#endif // Touch_GT911_H
