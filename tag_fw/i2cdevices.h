#ifndef _I2CDEV_H_
#define _I2CDEV_H_
#include <stdint.h>

void loadURLtoNTag();
void loadRawNTag(uint16_t blocksize);
bool i2cCheckDevice(uint8_t address);

#endif