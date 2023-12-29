#ifndef _EPD_UC8179_H_
#define _EPD_UC8179_H_

class uc8179 : public epdInterface {
   public:
    void epdSetup();
    void epdEnterSleep();
    void draw();
    void drawNoWait();
    void epdWaitRdy();
    void epdWriteDisplayData();
    void selectLUT(uint8_t lut);
};

#endif