#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>


#define AES_KEY_SIZE		16
#define AES_BLOCK_SIZE		16


#pragma callee_saves aesPrvSetKey
void aesSetKey(const uint8_t __xdata *key);

#pragma callee_saves aesPrvEnc
void aesEnc(uint8_t __xdata *data);




#endif
