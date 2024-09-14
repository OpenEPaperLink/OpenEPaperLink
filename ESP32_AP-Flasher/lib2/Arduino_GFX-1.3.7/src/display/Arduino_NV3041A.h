#ifndef _ARDUINO_NV3041A_H_
#define _ARDUINO_NV3041A_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define NV3041A_TFTWIDTH 480
#define NV3041A_TFTHEIGHT 272

#define NV3041A_RST_DELAY 120    ///< delay ms wait for reset finish
#define NV3041A_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define NV3041A_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define NV3041A_NOP 0x00
#define NV3041A_SWRESET 0x01

#define NV3041A_SLPIN 0x10
#define NV3041A_SLPOUT 0x11

#define NV3041A_INVOFF 0x20
#define NV3041A_INVON 0x21
#define NV3041A_DISPOFF 0x28
#define NV3041A_DISPON 0x29

#define NV3041A_CASET 0x2A
#define NV3041A_RASET 0x2B
#define NV3041A_RAMWR 0x2C

#define NV3041A_MADCTL 0x36
#define NV3041A_COLMOD 0x3A

#define NV3041A_MADCTL_MY 0x80
#define NV3041A_MADCTL_MX 0x40
#define NV3041A_MADCTL_MV 0x20
#define NV3041A_MADCTL_ML 0x10
#define NV3041A_MADCTL_RGB 0x00

static const uint8_t nv3041a_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, 0xff,
    0xa5,

    WRITE_C8_D8, 0x36, //
    0xc0,

    WRITE_C8_D8, 0x3A, //
    0x01,              // 01---565，00---666

    WRITE_C8_D8, 0x41,
    0x03, // 01--8bit, 03-16bit

    WRITE_C8_D8, 0x44, // VBP  ?????
    0x15,              // 21

    WRITE_C8_D8, 0x45, // VFP  ?????
    0x15,              // 21

    WRITE_C8_D8, 0x7d, // vdds_trim[2:0]
    0x03,

    WRITE_C8_D8, 0xc1, // avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0]
    0xbb,              // 0xbb	 88		  a2

    WRITE_C8_D8, 0xc2, // vgl_clp_en vgl_clp[2:0]
    0x05,

    WRITE_C8_D8, 0xc3, // vgl_clp_en vgl_clp[2:0]
    0x10,

    WRITE_C8_D8, 0xc6, // avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0]
    0x3e,              // 35

    WRITE_C8_D8, 0xc7, // mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0]
    0x25,              // 2e

    WRITE_C8_D8, 0xc8, //	VGL_CLK_sel
    0x11,              //

    WRITE_C8_D8, 0x7a, //	user_vgsp
    0x5f,              // 4f:0.8V		3f:1.04V	5f

    WRITE_C8_D8, 0x6f, //	user_gvdd
    0x44,              // 1C:5.61	  5f	 53		   2a	    3a

    WRITE_C8_D8, 0x78, //	user_gvcl
    0x70,              // 50:-3.22	  75			58	     	66

    WRITE_C8_D8, 0xc9, //
    0x00,

    WRITE_C8_D8, 0x67, //
    0x21,

    // gate_ed

    WRITE_C8_D8, 0x51, // gate_st_o[7:0]
    0x0a,

    WRITE_C8_D8, 0x52, // gate_ed_o[7:0]
    0x76,              // 76

    WRITE_C8_D8, 0x53, // gate_st_e[7:0]
    0x0a,              // 76

    WRITE_C8_D8, 0x54, // gate_ed_e[7:0]
    0x76,
    ////sorce
    WRITE_C8_D8, 0x46, // fsm_hbp_o[5:0]
    0x0a,

    WRITE_C8_D8, 0x47, // fsm_hfp_o[5:0]
    0x2a,

    WRITE_C8_D8, 0x48, // fsm_hbp_e[5:0]
    0x0a,

    WRITE_C8_D8, 0x49, // fsm_hfp_e[5:0]
    0x1a,

    WRITE_C8_D8, 0x56, // src_ld_wd[1:0] src_ld_st[5:0]
    0x43,

    WRITE_C8_D8, 0x57, // pn_cs_en src_cs_st[5:0]
    0x42,

    WRITE_C8_D8, 0x58, // src_cs_p_wd[6:0]
    0x3c,

    WRITE_C8_D8, 0x59, // src_cs_n_wd[6:0]
    0x64,

    WRITE_C8_D8, 0x5a, // src_pchg_st_o[6:0]
    0x41,              // 41

    WRITE_C8_D8, 0x5b, // src_pchg_wd_o[6:0]
    0x3c,

    WRITE_C8_D8, 0x5c, // src_pchg_st_e[6:0]
    0x02,              // 02

    WRITE_C8_D8, 0x5d, // src_pchg_wd_e[6:0]
    0x3c,              // 3c

    WRITE_C8_D8, 0x5e, // src_pol_sw[7:0]
    0x1f,

    WRITE_C8_D8, 0x60, // src_op_st_o[7:0]
    0x80,

    WRITE_C8_D8, 0x61, // src_op_st_e[7:0]
    0x3f,

    WRITE_C8_D8, 0x62, // src_op_ed_o[9:8] src_op_ed_e[9:8]
    0x21,

    WRITE_C8_D8, 0x63, // src_op_ed_o[7:0]
    0x07,

    WRITE_C8_D8, 0x64, // src_op_ed_e[7:0]
    0xe0,

    WRITE_C8_D8, 0x65, // chopper
    0x02,

    WRITE_C8_D8, 0xca, // avdd_mux_st_o[7:0]
    0x20,

    WRITE_C8_D8, 0xcb, // avdd_mux_ed_o[7:0]
    0x52,              // 52

    WRITE_C8_D8, 0xcc, // avdd_mux_st_e[7:0]
    0x10,

    WRITE_C8_D8, 0xcD, // avdd_mux_ed_e[7:0]
    0x42,

    WRITE_C8_D8, 0xD0, // avcl_mux_st_o[7:0]
    0x20,

    WRITE_C8_D8, 0xD1, // avcl_mux_ed_o[7:0]
    0x52,

    WRITE_C8_D8, 0xD2, // avcl_mux_st_e[7:0]
    0x10,

    WRITE_C8_D8, 0xD3, // avcl_mux_ed_e[7:0]
    0x42,

    WRITE_C8_D8, 0xD4, // vgh_mux_st[7:0]
    0x0a,

    WRITE_C8_D8, 0xD5, // vgh_mux_ed[7:0]
    0x32,

    // 2-1
    ////gammma  weihuan pianguangpian 0913
    WRITE_C8_D8, 0x80, // gam_vrp0	0					6bit
    0x00,
    WRITE_C8_D8, 0xA0, // gam_VRN0		 0-
    0x00,

    WRITE_C8_D8, 0x81, // gam_vrp1	1				   6bit
    0x07,
    WRITE_C8_D8, 0xA1, // gam_VRN1		 1-
    0x06,

    WRITE_C8_D8, 0x82, // gam_vrp2	 2					6bit
    0x02,
    WRITE_C8_D8, 0xA2, // gam_VRN2		 2-
    0x01,

    WRITE_C8_D8, 0x86, // gam_prp0	 7bit	8			7bit
    0x11,              // 33
    WRITE_C8_D8, 0xA6, // gam_PRN0	 	8-
    0x10,              // 2a

    WRITE_C8_D8, 0x87, // gam_prp1	 7bit	 40			 7bit
    0x27,              // 2d
    WRITE_C8_D8, 0xA7, // gam_PRN1	 	40-
    0x27,              // 2d

    WRITE_C8_D8, 0x83, // gam_vrp3	 61				 6bit
    0x37,
    WRITE_C8_D8, 0xA3, // gam_VRN3		61-
    0x37,

    WRITE_C8_D8, 0x84, // gam_vrp4	  62			 6bit
    0x35,
    WRITE_C8_D8, 0xA4, // gam_VRN4		62-
    0x35,

    WRITE_C8_D8, 0x85, // gam_vrp5	  63			 6bit
    0x3f,
    WRITE_C8_D8, 0xA5, // gam_VRN5		63-
    0x3f,
    //

    WRITE_C8_D8, 0x88, // gam_pkp0	  	 4			   5bit
    0x0b,              // 0b
    WRITE_C8_D8, 0xA8, // gam_PKN0		4-
    0x0b,              // 0b

    WRITE_C8_D8, 0x89, // gam_pkp1	  5					5bit
    0x14,              // 14
    WRITE_C8_D8, 0xA9, // gam_PKN1		5-
    0x14,              // 14

    WRITE_C8_D8, 0x8a, // gam_pkp2	  7					 5bit
    0x1a,              // 1a
    WRITE_C8_D8, 0xAa, // gam_PKN2		7-
    0x1a,              // 1a

    WRITE_C8_D8, 0x8b, // gam_PKP3	  10				 5bit
    0x0a,
    WRITE_C8_D8, 0xAb, // gam_PKN3		10-
    0x0a,

    WRITE_C8_D8, 0x8c, // gam_PKP4	   16				 5bit
    0x14,
    WRITE_C8_D8, 0xAc, // gam_PKN4		16-
    0x08,

    WRITE_C8_D8, 0x8d, // gam_PKP5		22				 5bit
    0x17,
    WRITE_C8_D8, 0xAd, // gam_PKN5		22-
    0x07,

    WRITE_C8_D8, 0x8e, // gam_PKP6		28				 5bit
    0x16,              // 16 change
    WRITE_C8_D8, 0xAe, // gam_PKN6		28-
    0x06,              // 13change

    WRITE_C8_D8, 0x8f, // gam_PKP7		34				  5bit
    0x1B,
    WRITE_C8_D8, 0xAf, // gam_PKN7		34-
    0x07,

    WRITE_C8_D8, 0x90, // gam_PKP8		 46				   5bit
    0x04,
    WRITE_C8_D8, 0xB0, // gam_PKN8		46-
    0x04,

    WRITE_C8_D8, 0x91, // gam_PKP9		 52					5bit
    0x0A,
    WRITE_C8_D8, 0xB1, // gam_PKN9		52-
    0x0A,

    WRITE_C8_D8, 0x92, // gam_PKP10		58					5bit
    0x16,
    WRITE_C8_D8, 0xB2, // gam_PKN10		58-
    0x15,

    WRITE_C8_D8, 0xff,
    0x00,

    WRITE_C8_D8, 0x11,
    0x00,
    END_WRITE,

    DELAY, 120,

    BEGIN_WRITE,
    WRITE_C8_D8, 0x29,
    0x00,
    END_WRITE,

    DELAY, 100};

class Arduino_NV3041A : public Arduino_TFT
{
public:
  Arduino_NV3041A(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = NV3041A_TFTWIDTH, int16_t h = NV3041A_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;

  void setRotation(uint8_t r) override;

  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;

  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;

private:
};

#endif
