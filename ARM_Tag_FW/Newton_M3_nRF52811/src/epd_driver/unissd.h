#ifndef _EPD_UNISSD_H_
#define _EPD_UNISSD_H_

class unissd : public epdInterface {
   public:
    void epdSetup();
    void epdEnterSleep();
    void draw();
    void drawNoWait();
    void epdWaitRdy();
    void epdWriteDisplayData();
    void selectLUT(uint8_t lut);

   protected:
   void commandReadBegin(uint8_t cmd);
   void commandReadEnd();
   uint8_t epdReadByte();
   void setWindowX(uint16_t start, uint16_t end);
   void setWindowY(uint16_t start, uint16_t end);
   void setPosXY(uint16_t x, uint16_t y);
};

#endif