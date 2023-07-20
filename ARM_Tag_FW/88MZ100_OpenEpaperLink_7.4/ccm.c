#include <string.h>
#include "mz100.h"
#include "mz100_aes.h"
#include "mz100_driver.h"
#include "ccm.h"


static bool aesCcmOp(void* dst, const void *src, uint16_t authSrcLen, uint16_t encSrcLen, const void *key, const void *nonce, bool dec)
{
	uint32_t tempB, nBytesNoMic = authSrcLen + encSrcLen, nBytesIn, nBytesOut;
	const uint32_t *inD = (const uint32_t*)src;
	uint32_t *outD = (uint32_t*)dst;
	uint_fast8_t i;
	bool success;
	
	if (dec) {
		nBytesIn = nBytesNoMic + AES_CCM_MIC_SIZE;
		nBytesOut = nBytesNoMic;
	}
	else {
		nBytesIn = nBytesNoMic;
		nBytesOut = nBytesNoMic + AES_CCM_MIC_SIZE;
	}
	
	do{
		AES->CTRL1.WORDVAL |= 2;
	} while (!(AES->STATUS.WORDVAL & 2));
	
	AES->CTRL2.WORDVAL |= 1;
	(void)AES->CTRL2.WORDVAL;
	AES->CTRL2.WORDVAL &=~ 1;
	AES->CTRL1.WORDVAL = 0x0005501e + (dec ? 0x8000 : 0);
	AES->IMR.WORDVAL = 0xffffffff;
	
	for(i = 0; i < 4; i++)
		AES->KEY[7 - i].WORDVAL = ((uint32_t*)key)[i];
	
	AES->MSTR_LEN.WORDVAL = encSrcLen;
	AES->ASTR_LEN.WORDVAL = authSrcLen;
	
	for(i = 0; i < 3; i++)
		AES->IV[i].WORDVAL = ((uint32_t*)nonce)[i];
	AES->IV[3].WORDVAL = ((uint8_t*)nonce)[12] + 0x200;	//2 byte lengths
	
	AES->CTRL1.WORDVAL |= 1;
	
	while (nBytesIn || nBytesOut) {
		
		if (!(AES->STATUS.WORDVAL & 0x10) && nBytesIn) {
			
			if (nBytesIn >= 4) {
				AES->STR_IN.WORDVAL = *inD++;
				nBytesIn -= 4;
			}
			else {
				memcpy(&tempB, inD, nBytesIn);
				AES->STR_IN.WORDVAL = tempB;
				nBytesIn = 0;
			}
		}
		
		if ((AES->STATUS.WORDVAL & 0x40) && nBytesOut) {
			
			if (nBytesOut >= 4) {
				*outD++ = AES->STR_OUT.WORDVAL;
				nBytesOut -= 4;
			}
			else {
				tempB = AES->STR_OUT.WORDVAL;
				memcpy(outD, &tempB, nBytesOut);
				nBytesOut = 0;
			}
			
		}
	}
	
	success = !((AES->STATUS.WORDVAL >> 11) & 7);
	
	AES->CTRL1.WORDVAL = 0;
	
	return success;
}

void aesCcmEnc(void* dst, const void *src, uint16_t authSrcLen, uint16_t encSrcLen, const void *key, const void *nonce)
{
	aesCcmOp(dst, src, authSrcLen, encSrcLen, key, nonce, false);
}

bool aesCcmDec(void* dst, const void *src, uint16_t authSrcLen, uint16_t encSrcLen, const void *key, const void *nonce)
{
	return aesCcmOp(dst, src, authSrcLen, encSrcLen, key, nonce, true);
}




