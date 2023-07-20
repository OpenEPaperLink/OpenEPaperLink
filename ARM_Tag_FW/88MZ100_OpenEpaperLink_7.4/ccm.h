#ifndef _CCM_H_
#define _CCM_H_

//CCM defined for T = 4 (mic is 4 bytes), Q = 2 (max data len 65536), N = 13( nonse is 13 bytes), 128-bit AES (16 bytes of key)
//no more than 0xff00 auth data bytes allowed

#include <stdbool.h>
#include <stdint.h>

#define AES_CCM_MIC_SIZE		4
#define AES_CCM_NONCE_SIZE		13

//encrypted data follows auth data. both in and out. 4 bytes of MIC appended on the encrypt path, checke don the decrypt path

void aesCcmEnc(void* dst, const void *src, uint16_t authSrcLen, uint16_t encSrcLen, const void *key, const void *nonce);
bool aesCcmDec(void* dst, const void *src, uint16_t authSrcLen, uint16_t encSrcLen, const void *key, const void *nonce);



#endif
