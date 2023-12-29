#ifndef _EPD_029_VAR1_H_
#define _EPD_029_VAR1_H_

class epdvar29 : public epdInterface {
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