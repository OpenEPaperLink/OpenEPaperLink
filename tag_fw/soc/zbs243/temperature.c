#include "asmUtil.h"
#include "flash.h"
#include "timer.h"
#include "cpu.h"

static volatile uint8_t __xdata mTempRet[4];


void TEMP_ISR(void) __interrupt (10)
{
	uint8_t i;
	
	i = CFGPAGE;
	CFGPAGE = 4;
	mTempRet[0] = TEMPRETH;
	mTempRet[1] = TEMPRETL;
	CFGPAGE = i;
	IEN1 &=~ 0x10;
}

int8_t adcSampleTemperature(void)
{
	uint16_t sum = 0;
	uint8_t i;
	
	CLKEN |= 0x80;
	
	//adcConfig
	i = CFGPAGE;
	CFGPAGE = 4;
	TEMPCFG = 0x81;	//0x08 set when done
	TEMPCAL2 = 0x22;
	TEMPCAL1 = 0x55;
	TEMPCAL4 = 0;
	TEMPCAL3 = 0;
	TEMPCAL6 = 3;
	TEMPCAL5 = 0xff;
	TEMPCFG &=~ 0x08;
	CFGPAGE = i;
	IEN1 &=~ 0x10;
	//adcConfig over
	
	/*
		TEMPCAL2: (seemingly analog settings: gain + ???)
			removing 0x02 lowers vals a little (9%)				\__ these stack just as you'd expect
			setting 0x04 moves the values up a bit (18%)		/
			setting 0x08 adds 2.5% or so
			0x30 is ADC gain/bitness. (x4 for each increment)
			0x80 seems to make no difference
		
		TEMPCAL1: (seemingly analog settings)
			removing 1 decreases value by 5.7%	\__ these stack
			removing 4 decreases by 22%			/
			removing 0x10 increments value by 1%
			removnig 0x40 increases by 5%
			
			using 0xaa increases width but decreases average
		
		other TEMPCALx values seem to have no effect
	*/
	
	for (i = 0; i < 9; i++) {
		
		//int on
		IEN1 |= 0x10;
	
		//wait for it to self-turn-off
		while (IEN1 & 0x10);
		
		if (i) {	//skip first
			
			sum += ((uint16_t)mathPrvU8bitswap(mTempRet[0])) << 2;
			if (mTempRet[1] & 1)
				sum += 2;
			if (mTempRet[1] & 2)
				sum += 1;
		}
		
		timerDelay(TIMER_TICKS_PER_SECOND / 10000);
	}
	//turn it off
	CLKEN &=~ 0x80;
		
	//reuse tempRet to get calib data
	if (!flashRead(FLASH_INFOPAGE_ADDR + 0x0b, mTempRet, 4) || (mTempRet[0] == 0xff && mTempRet[1] == 0xff) || (mTempRet[2] == 0xff && mTempRet[3] == 0xff)) {
	
		//no calibration data - reporting 20 degrees, same as stock firmware
		return 20;
	}
	else {
		
		uint16_t __xdata valB = mathPrvU16from2xU8(mTempRet[0], mTempRet[1]);	//temp sensor reading at 30 degrees C
		uint16_t __xdata valD = mathPrvU16from2xU8(mTempRet[2], mTempRet[3]);	//temp sensor reading at 45 degrees C
		uint16_t __xdata slope = valD - valB;
		
		sum = (sum + 4) / 8;	//average it
		
		//we can get 1/10 degree accuracy as this is well calibrated, but our api only need degrees so let's do that
		/*
			//high-precision code is this:
			if (sum > valB)			//our math routines are unsigned
				temp = (uint16_t)mathPrvDiv32x16(mathPrvMul16x16(sum - valB, 150) + (slope / 2), slope) + 300;
			else
				temp = 300 - (uint16_t)mathPrvDiv32x16(mathPrvMul16x16(valB - sum, 150) + (slope / 2), slope);
		*/
		
		if (sum > valB)			//our math routines are unsigned
			return mathPrvDiv16x8(5 + (uint16_t)mathPrvDiv32x16(mathPrvMul16x16(sum - valB, 150) + (slope / 2), slope), 10) + 30;
		else
			return 30 - mathPrvDiv16x8(5 + (uint16_t)mathPrvDiv32x16(mathPrvMul16x16(valB - sum, 150) + (slope / 2), slope), 10);
	}
}