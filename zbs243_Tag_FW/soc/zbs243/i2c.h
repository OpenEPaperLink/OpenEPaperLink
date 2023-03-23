#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

enum I2cResult {
	I2cOK,
	I2cWrAddrNAKed,
	I2cRdAddrNAKed,
	I2cNonLastDataByteNAKed,
	I2cInternalError,
};

struct I2cTransaction {
	uint8_t deviceAddr;
	uint8_t numBytes;		//will be updated as we go along so you can figure out what was NAKed
	uint8_t __xdata *bytes;
};

#pragma callee_saves i2cInit
void i2cInit(void);

#pragma callee_saves i2cTransact
uint8_t i2cTransact(struct I2cTransaction __xdata *trans, uint8_t nTrans);	//returns enum I2cResult 

void I2C_IRQ(void) __interrupt (8);






#endif

