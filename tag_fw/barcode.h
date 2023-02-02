#ifndef _BARCODE_H_
#define _BARCODE_H_

#include <stdint.h>

//code128 generator (c) 2021 Dmitry Grinberg https://dmitry.gr
//non-commercial use only, contact licensing@dmitry.gr for other options


struct BarcodeInfo {			//zero-init this except the string ptr
	const char *str;
	uint16_t curBars;
	uint8_t barsLeft;
	uint8_t state;
	uint8_t csum;
	uint8_t csumMul;
};

#define barcodeWidth(nChars)	(11 * (nChars) + 11 /*start */+ 11 /*check sum */ + 13 /* stop */)

#pragma callee_saves barcodeIsDone
__bit barcodeIsDone(struct BarcodeInfo __xdata *bci);

#pragma callee_saves barcodeNextBar
__bit barcodeNextBar(struct BarcodeInfo __xdata *bci);



#endif
