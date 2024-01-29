#ifndef _CCM_H_
#define _CCM_H_

//CCM defined for T = 4 (mic is 4 bytes), Q = 2 (max data len 65536), N = 13( nonse is 13 bytes), 128-bit AES (16 bytes of key)
//no more than 0xff00 auth data bytes allowed

#include <stdbool.h>
#include <stdint.h>

#define AES_CCM_MIC_SIZE		4
#define AES_CCM_NONCE_SIZE		13

//encrypted data follows auth data. both in and out. 4 bytes of MIC appended on the encrypt path, checked on the decrypt path
//data size limited to about 240 bytes..i can live with that

struct AesCcmInfo {
	uint8_t authSrcLen;
	uint8_t encDataLen;
	const uint8_t __xdata *key;
	uint8_t nonce[AES_CCM_NONCE_SIZE];
};


#pragma callee_saves aesCcmEnc
void aesCcmEnc(void __xdata *dst, const void __xdata *src, const struct AesCcmInfo __xdata *ccmInfo) __reentrant;

#pragma callee_saves aesCcmDec
__bit aesCcmDec(void __xdata *dst, const void __xdata *src, const struct AesCcmInfo __xdata *ccmInfo) __reentrant;



#endif
