#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

extern uint16_t __xdata mAdcSlope;
extern uint16_t __xdata mAdcIntercept;


uint16_t adcSampleBattery(void);		//in mV
int8_t adcSampleTemperature(void);		//in degrees C




#endif
