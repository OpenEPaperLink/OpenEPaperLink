#include "asmUtil.h"
#include "printf.h"
#include "aes.h"
#include "cpu.h"
#include "ccm.h"


//TI does not provide nearly enough docs to get AES-CCM working on the CC1110. Le Sigh...


static uint8_t __xdata mBlockOut[AES_BLOCK_SIZE];
static uint8_t __xdata mBlockIn[AES_BLOCK_SIZE];
static uint8_t __xdata mMic[4];



//sdcc cannot inline things so we do for it
#define aesCcmPrvCopyNonceSetHalfword(_firstByte, nonce, _val)			\
do {																	\
	mBlockOut[0] = (_firstByte);										\
	xMemCopyShort(mBlockOut + 1, nonce, 13);							\
	mBlockOut[14] = 0;		/* normally val.hi, but for us - zero */	\
	mBlockOut[15] = (_val);												\
} while(0)

//leaves result in mBlockOut
#pragma callee_saves aesCcmPrvCalcUnencryptedMic
static void aesCcmPrvCalcUnencryptedMic(const uint8_t __xdata *src, const struct AesCcmInfo __xdata *ccmInfo) __reentrant
{
	uint8_t done;
	
	//create block 0
	aesCcmPrvCopyNonceSetHalfword(ccmInfo->authSrcLen ? 0x49 : 0x09, ccmInfo->nonce, ccmInfo->encDataLen);
	
	//encrypt it
	aesEnc(mBlockOut);

	if (ccmInfo->authSrcLen) {

		uint8_t __xdata *blk = mBlockIn;
		uint8_t now, already = 2;
		
		*blk++ = 0;		//authSrcLen.hi
		*blk++ = ccmInfo->authSrcLen;
		now = 14;	//since we already used 2
		done = 0;
		
		while (done < ccmInfo->authSrcLen) {
			
			uint8_t i;
		
			if (now > (uint8_t)(ccmInfo->authSrcLen - done))
				now = (uint8_t)(ccmInfo->authSrcLen - done);
			
			xMemCopyShort(blk, src, now);
			src += now;
			xMemSet(blk + now, 0, 16 - already - now);
			
			for (i = 0; i < 16; i++)
				mBlockOut[i] ^= mBlockIn[i];
			
			aesEnc(mBlockOut);
			blk = mBlockIn;
			done += now;
			now = 16;
			already = 0;
		}
	}
	
	done = 0;
	while (done < ccmInfo->encDataLen) {
		
		uint8_t i, now;
		
		now = 16;
		if (now > ccmInfo->encDataLen - done)
			now = ccmInfo->encDataLen - done;
		
		for (i = 0; i < now; i++)
			mBlockOut[i] ^= *src++;

		done += now;
		
		aesEnc(mBlockOut);
	}
}

void aesCcmEnc(void __xdata *dstP, const void __xdata *srcP, const struct AesCcmInfo __xdata *ccmInfo) __reentrant
{
	const uint8_t __xdata *src = (const uint8_t __xdata*)srcP;
	uint8_t __xdata *dst = (uint8_t*)dstP;
	uint8_t i, done = 0, now, ctr = 0;
	
	aesSetKey(ccmInfo->key);
	
	//it goes after encrypted data
	aesCcmPrvCalcUnencryptedMic(src, ccmInfo);
	xMemCopyShort(mMic, mBlockOut, sizeof(mMic));

	//copy authed data
	xMemCopyShort(dst, src, ccmInfo->authSrcLen);
	src += ccmInfo->authSrcLen;
	dst += ccmInfo->authSrcLen;

	//now we encrypt
	now = 0;	//first block not used
	while (done < ccmInfo->encDataLen) {
		
		if (now > (uint8_t)(ccmInfo->encDataLen - done))
			now = (uint8_t)(ccmInfo->encDataLen - done);
		
		aesCcmPrvCopyNonceSetHalfword(1, ccmInfo->nonce, ctr++);
		aesEnc(mBlockOut);
		
		if (!now)	//first block
			
			for (i = 0; i < sizeof(mMic); i++)
				mMic[i] ^= mBlockOut[i];
		else {
			
			for (i = 0; i < now; i++)
				*dst++ = *src++ ^ mBlockOut[i];
		}
		done += now;
		now = 16;
	}
	
	xMemCopyShort(dst, mMic, sizeof(mMic));
}

__bit aesCcmDec(void __xdata *dstP, const void __xdata *srcP, const struct AesCcmInfo __xdata *ccmInfo) __reentrant
{
	const uint8_t __xdata *src = (const uint8_t __xdata*)srcP;
	uint8_t __xdata *dst = (uint8_t*)dstP;
	uint8_t i, done, now, ctr = 0;
	
	aesSetKey(ccmInfo->key);
	
	//copy authed data
	xMemCopyShort(dst, src, ccmInfo->authSrcLen);
	src += ccmInfo->authSrcLen;
	dst += ccmInfo->authSrcLen;
	
	//then we decrypt
	done = 0;
	now = 0;	//first block not used
	while (done < ccmInfo->encDataLen) {
		
		if (now > (uint8_t)(ccmInfo->encDataLen - done))
			now = (uint8_t)(ccmInfo->encDataLen - done);
		
		aesCcmPrvCopyNonceSetHalfword(1, ccmInfo->nonce, ctr++);
		
		aesEnc(mBlockOut);
		
		if (!now) {	//first block
			
			//given mic is after data
			for (i = 0; i < sizeof(mMic); i++)
				mMic[i] = src[ccmInfo->encDataLen + i] ^ mBlockOut[i];
		}
		else {
			
			for (i = 0; i < now; i++)
				*dst++ = *src++ ^ mBlockOut[i];
		}
		done += now;
		now = 16;
	}
	
	aesCcmPrvCalcUnencryptedMic(dstP, ccmInfo);
	return xMemEqual(mMic, mBlockOut, sizeof(mMic));
}




