#pragma once

#define install_int_callback(intPeriph, intType, cbFun) install_int_callback_##intPeriph(intType, cbFun)
void install_int_callback_INT_UART1(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_UART2(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_RTC(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_I2C1(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_I2C2(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_SSP1(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_SSP2(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_GPT1(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_GPT2(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_CRC(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_AES(uint32_t intType, intCallback_Type * cbFun);
//void install_int_callback_INT_BG(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_ADC(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DAC(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_ACOMP(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_GPIO(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DMA_CH0(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DMA_CH1(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DMA_CH2(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DMA_CH3(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DMA_CH4(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_DMA_CH5(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_WDT(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_RC32M(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_XTAL32M(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_MPU(uint32_t intType, intCallback_Type * cbFun);

/*add in digger2 for new ip core*/
void install_int_callback_INT_THREEDG(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_KEYSCAN(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_IR(uint32_t intType, intCallback_Type * cbFun);
void install_int_callback_INT_TRACKBALL(uint32_t intType, intCallback_Type * cbFun);
