#ifndef _EPD_043_VAR1_H_
#define _EPD_043_VAR1_H_

class epdvar43 : public epdInterface {
   public:
     void epdSetup() ;
     void epdEnterSleep() ;
     void draw();
     void drawNoWait();
     void epdWaitRdy();
     void epdWriteDisplayData();
     void selectLUT(uint8_t lut);
};

#endif