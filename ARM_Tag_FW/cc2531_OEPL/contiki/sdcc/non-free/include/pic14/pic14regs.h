#ifndef __DEVICE_INCLUDE_PIC14REGS_H__
#define __DEVICE_INCLUDE_PIC14REGS_H__ 1

#if defined(__SDCC_PROCESSOR)
  #ifndef _CONCAT
  #undef  _CONCAT2
  #define _CONCAT(a,b)  _CONCAT2(a,b)
  #define _CONCAT2(a,b) a ## b
  #endif

  /* May not paste proc ## .h as pic16f877.h is no valid
   * preprocessor token. So this is a bit fragile... */
  #define INC_PIC(proc) <_CONCAT(pic,proc).h>

  #include INC_PIC(__SDCC_PROCESSOR)
#else
  /* __SDCC_PROCESSOR undefined */
  #warning __SDCC_PROCESSOR undefined, no processor-specific header included.
#endif

#endif /* __DEVICE_INCLUDE_PIC14REGS_H__ */

