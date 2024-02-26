#ifndef _EPD_DUALSSD_H_
#define _EPD_DUALSSD_H_

class dualssd : public epdInterface {
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