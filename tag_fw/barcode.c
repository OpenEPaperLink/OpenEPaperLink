#include <stdbool.h>
#include "barcode.h"
#include "asmUtil.h"

//code128 generator (c) 2021 Dmitry Grinberg https://dmitry.gr
//non-commercial use only, contact licensing@dmitry.gr for other options

#pragma nogcse

static const uint16_t __code mCode128[] = {
	0b00110011011, 0b00110110011, 0b01100110011, 0b00011001001, 0b00110001001, 0b00110010001, 0b00010011001, 0b00100011001,
	0b00100110001, 0b00010010011, 0b00100010011, 0b00100100011, 0b00111001101, 0b00111011001, 0b01110011001, 0b00110011101,
	0b00110111001, 0b01100111001, 0b01001110011, 0b00111010011, 0b01110010011, 0b00100111011, 0b00101110011, 0b01110110111,
	0b00110010111, 0b00110100111, 0b01100100111, 0b00100110111, 0b00101100111, 0b01001100111, 0b00011011011, 0b01100011011,
	0b01101100011, 0b00011000101, 0b00011010001, 0b01100010001, 0b00010001101, 0b00010110001, 0b01000110001, 0b00010001011,
	0b00010100011, 0b01000100011, 0b00011101101, 0b01110001101, 0b01110110001, 0b00011011101, 0b01100011101, 0b01101110001,
	0b01101110111, 0b01110001011, 0b01110100011, 0b00010111011, 0b01000111011, 0b01110111011, 0b00011010111, 0b01100010111,
	0b01101000111, 0b00010110111, 0b01000110111, 0b01011000111, 0b01011110111, 0b01000010011, 0b01010001111, 0b00001100101,
	0b00110000101, 0b00001101001, 0b01100001001, 0b00110100001, 0b01100100001, 0b00001001101, 0b00100001101, 0b00001011001,
	0b01000011001, 0b00101100001, 0b01001100001, 0b01001000011, 0b00001010011, 0b01011101111, 0b00101000011, 0b01011110001,
	0b00111100101, 0b00111101001, 0b01111001001, 0b00100111101, 0b00101111001, 0b01001111001, 0b00100101111, 0b00101001111,
	0b01001001111, 0b01111011011, 0b01101111011, 0b01101101111, 0b00011110101, 0b01111000101, 0b01111010001, 0b00010111101,
	0b01000111101, 0b00010101111, 0b01000101111, 0b01111011101, 0b01110111101, 0b01111010111, 0b01110101111
};


#define CODE128_START_B			(0b00001001011)
#define CODE128_STOP			(0b1101011100011)

#define CODE128_IDX_START_A		(103)
#define CODE128_IDX_START_B		(104)
#define CODE128_IDX_START_C		(105)
#define CODE128_IDX_CODE_STOP	(106)


enum BarCodeState {
	BarCodeInit,
	BarCodeEmittingChar,
	BarCodeEmittingChecksum,
	BarCodeEmittingStop,
	BarCodeDone,
};




__bit barcodeIsDone(struct BarcodeInfo __xdata *bci)
{
	return bci->state == BarCodeDone;
}

__bit barcodeNextBar(struct BarcodeInfo __xdata *bci)
{
	uint8_t t;
	__bit ret;
	
	if (!bci->barsLeft) switch (bci->state) {
		case BarCodeInit:
			bci->curBars = CODE128_START_B;
			bci->barsLeft = 11;
			bci->state = BarCodeEmittingChar;
			bci->csum = CODE128_IDX_START_B;
			break;
		
		case BarCodeEmittingChar:
			t = charsPrvDerefAndIncGenericPtr(&bci->str);
			if (t) {
				t -= 0x20;
				if (t >= 0x60)
					t = '?' - 0x20;
				bci->csum = mathPrvMod16x8(mathPrvMul8x8(++bci->csumMul, t) + bci->csum, 103);
			}
			else {
				
				bci->state = BarCodeEmittingChecksum;
				t = bci->csum;
			}
			bci->curBars = mCode128[t];
			bci->barsLeft = 11;
			break;
		
		case BarCodeEmittingChecksum:
			bci->state = BarCodeEmittingStop;
			bci->curBars = CODE128_STOP;
			bci->barsLeft = 13;
			break;
		
		case BarCodeEmittingStop:
			bci->state = BarCodeDone;
			//fallthrough
		
		case BarCodeDone:
		default:
			return false;
	}
	
	ret = bci->curBars & 1;
	bci->curBars >>= 1;
	bci->barsLeft--;
	return ret;
}
