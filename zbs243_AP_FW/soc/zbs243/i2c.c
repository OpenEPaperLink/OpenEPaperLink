#include "zbs243.h"
#include "i2c.h"


static volatile struct I2cTransaction __xdata * __xdata mCurTrans;
static volatile uint8_t __xdata mNumTrans;
static volatile uint8_t __xdata mResult;

#pragma callee_saves i2cInit
void i2cInit(void)
{
	uint8_t bkp;
	
	CLKEN |= 0x10;
	
	bkp = CFGPAGE;
	CFGPAGE = 0;
	
	IEN1 |= 4;		//int on
	I2CUNKNOWN |= 4;
	
	I2CCTL = 0x43;		//master mode
	I2CSPEED = 0x1a;	//100KHz
	I2CCTL |= 0x80;		//irq on
	
	CFGPAGE = bkp;
}

//this code assumes HW stat emachine acts as it should. for this chip that seems to be true
//this is an 8051 so we value speed & code size over defensive programming. So sue me!
void I2C_IRQ(void) __interrupt (8)
{
	uint8_t bkp, sta;
	
	bkp = CFGPAGE;
	CFGPAGE = 0;
	
	sta = I2CSTATE;
	
	switch (sta >> 3) {
		case 0x08 / 8:		//start completed
		case 0x10 / 8:		//restart completed
			I2CBUF = mCurTrans->deviceAddr;
			break;
		
		case 0x30 / 8:		//byte write was NAKed
			if (mCurTrans->numBytes) {	//we still had more bytes to send?
				I2CCTL |= 0x10;	//stop
				mResult = I2cNonLastDataByteNAKed;
				break;
			}
			//fallthough since it is ok to NAK last written byte
		
		case 0x28 / 8:		//byte write was ACKed
		case 0x18 / 8:		//addr ACKED in write mode
			
			if (mCurTrans->numBytes) {
				
				mCurTrans->numBytes--;
				I2CBUF = *mCurTrans->bytes++;
				break;
			}
	transaction_over:
			if (!--mNumTrans) {	//byte(s)written and no more transactions? STOP
					
				mResult = I2cOK;
				I2CCTL |= 0x10;	//stop
			}
			else {						//we have another transaction? RESTART
				
				mCurTrans++;
				I2CCTL |= 0x20;	//do a restart
			}
			break;
		
		case 0x20 / 8:		//addr NAKEd in write mode
			mResult = I2cWrAddrNAKed;
			I2CCTL |= 0x10;	//stop
			break;
		
		case 0x48 / 8:		//addr NACKed in read mode
			mResult = I2cRdAddrNAKed;
			I2CCTL |= 0x10;	//stop
			break;
		
		case 0x58 / 8:		//NAK to RXed byte sent (we still have th ebyte we got in buffer)
		case 0x50 / 8:		//got byte in read mode
			*mCurTrans->bytes++ = I2CBUF;
			mCurTrans->numBytes--;
			//fallthrough
		
		case 0x40 / 8:		//addr ACKED in read mode
			if (!mCurTrans->numBytes)
				goto transaction_over;
			else if (mCurTrans->numBytes == 1)
				I2CCTL &=~ 0x04;	//NAK
			else
				I2CCTL |= 0x04;		//ACK
			break;
			
		default:
			mResult = I2cInternalError;
			I2CCTL |= 0x10;	//stop
			mNumTrans = 0;
			break;
	}
	
	I2CCTL &=~ 0x08;
	CFGPAGE = bkp;
}

#pragma callee_saves i2cTransact
enum I2cResult i2cTransact(struct I2cTransaction __xdata *trans, uint8_t nTrans)
{
	mCurTrans = trans;
	mNumTrans = nTrans;
	
	mResult = I2cOK;
	
	if (nTrans) {
		
		I2CCTL &=~ 0x10;	//clear stop
		I2CCTL |= 0x20;		//issue start
		
		while (mNumTrans);			//wait
	}
	return mResult;
}
