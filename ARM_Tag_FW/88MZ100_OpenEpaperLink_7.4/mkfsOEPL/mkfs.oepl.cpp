#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//#include <iostream>
#include <vector>
#include <string.h>
#include <zlib.h>

#pragma pack(1)


/*
	This prepares a FS binary for use with the 88mz100-based OEPL Tag Firmware
	Jelmer Bruijn 2024
*/


#define PROGMEM 
//#define WINDOW_SIZE 12 // 4096 bytes
#define WINDOW_SIZE 10 // 1024 bytes
#define FILENAME_LENGTH 32


typedef struct {
    uint16_t bitmapOffset;  ///< Pointer into GFXfont->bitmap
    uint8_t width;          ///< Bitmap dimensions in pixels
    uint8_t height;         ///< Bitmap dimensions in pixels
    uint8_t xAdvance;       ///< Distance to advance cursor (x axis)
    int8_t xOffset;         ///< X dist from cursor pos to UL corner
    int8_t yOffset;         ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
    uint8_t *bitmap;   ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;   ///< Glyph array
    uint16_t first;    ///< ASCII extents (first char)
    uint16_t last;     ///< ASCII extents (last char)
    uint8_t yAdvance;  ///< Newline distance (y axis)
} GFXfont;


typedef struct {
	uint16_t first;
	uint16_t last;
	uint8_t yAdvance;
	char glyphFile[FILENAME_LENGTH];
	char bitmapFile[FILENAME_LENGTH];
} GFXFontOEPL;

typedef struct {
	char name[FILENAME_LENGTH];
	uint32_t offset;
	uint32_t len;
} OEPLFile;

std::vector<OEPLFile*> files;
uint8_t* buffer = nullptr; // temporary holds the entire FS
uint32_t curOffset = 0;
char OEPLHeader[12] = "---OEPL_FS!";
FILE *wptr;


void dump(const uint8_t *a, const uint16_t l) {
    printf("\n        ");
#define ROWS 16
    for (uint8_t c = 0; c < ROWS; c++) {
        printf(" %02X", c);
    }
    printf("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        printf("---");
    }
    for (uint16_t c = 0; c < l; c++) {
        if ((c % ROWS) == 0) {
            printf("\n0x%04X | ", c);
        }
        printf("%02X ", a[c]);
    }
    printf("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        printf("---");
    }
    printf("\n");
}


unsigned long doCompress(uint8_t* in, uint32_t in_len, uint8_t* out) {
    *((uint32_t*)out) = in_len;
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = (uInt)in_len;
    stream.next_in = (Bytef*)in;
    stream.avail_out = (uInt)(in_len+1337);
    stream.next_out = (Bytef*)(out+4);
    *((uint32_t*)out) = in_len;

    if (deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, WINDOW_SIZE, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        fprintf(stderr, "Error initializing zlib for compression\n");
        return 1;
    }
    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        fprintf(stderr, "Error compressing data\n");
        deflateEnd(&stream);
        return 1;
    }
    if (deflateEnd(&stream) != Z_OK) {
        fprintf(stderr, "Error finalizing compression\n");
        return 1;
    }
    //dump(out, 16);
    return stream.total_out+4;
}



void addFile(uint8_t* filedata, uint16_t len, char* filename, bool compress){
	char newfilename[FILENAME_LENGTH];
	if(compress){
		if(strlen(filename)>(FILENAME_LENGTH-3)){
        	        printf("Filename '%s' is too long\n", filename);
                	return;
        	}
		uint8_t* tmpbuffer = (uint8_t*)malloc(len + 1337);
		unsigned long out_len = doCompress(filedata, len, tmpbuffer);
		if(out_len==1){
			printf("Compression of %s failed\n", filename);
			return;
		}
		filedata = tmpbuffer;
		uint8_t rounded = (uint8_t)((float)100*out_len/len);
		printf("Compressed %s \033[50D\033[40C: %u \033[50D\033[47C -> %lu \033[70D\033[58C(%d%%)\n", filename, len, out_len, rounded);
		len = out_len;
	        uint8_t flen = strlen(filename);
		strncpy(newfilename,filename,FILENAME_LENGTH-3);
        	strcpy(newfilename + flen, ".z");
		filename = newfilename;
	}



	if(strlen(filename)>(FILENAME_LENGTH-1)){
		printf("Filename '%s' is too long\n", filename);
		return;
	}
	if(!buffer){
		buffer = (uint8_t*)malloc(len);
	} else {
		buffer = (uint8_t*)realloc((void*)buffer, curOffset + len);
	}
	memcpy(buffer+curOffset, filedata, len);
	if(compress)free(filedata);
	OEPLFile* file = new OEPLFile;
	strcpy(file->name,filename);
	file->offset = curOffset;
	file->len = len;
	files.push_back(file);
	curOffset+=len;
}

void addFileFromFS(const char* filename, bool compress){
    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char* fileContent = (char*)malloc(fileSize);

    if (fileContent == NULL) {
        fprintf(stderr, "Error allocating memory for file content\n");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(fileContent, 1, fileSize, file);
    addFile((uint8_t*)fileContent, fileSize, (char*)filename, compress);
}


void printFAT(){
	printf("OEPL Filesystem Summary:\n");
	uint16_t tableSize = files.size() * sizeof(OEPLFile);
	tableSize += sizeof(OEPLHeader)-1;
	fwrite(OEPLHeader,sizeof(OEPLHeader)-1,1,wptr);
	uint32_t fssize = 0;
	for(OEPLFile* file : files){
		file->offset += tableSize;
		fwrite((uint8_t*)file,sizeof(OEPLFile),1,wptr);
	        printf("file: %s \033[50D\033[37C size=%u \033[50D\033[51C offset=0x%04X\n",file->name, file->len, file->offset);
		fssize = file->len + file->offset;
	}
	printf("Total FS size: %u\n", fssize);
}

void saveFontData(const GFXfont* font, char* name){
        if(strlen(name)>(FILENAME_LENGTH-7)){
                printf("Font filename '%s' is too long\n", name);
                return;
        }
	GFXFontOEPL fontdata;
	fontdata.first = font->first;
	fontdata.last = font->last;
	fontdata.yAdvance = font->yAdvance;
	strcpy(fontdata.glyphFile, name);
        strcpy(fontdata.bitmapFile, name);
	uint8_t len = strlen(fontdata.glyphFile);
	strcpy(fontdata.glyphFile + len, ".glyph");
	strcpy(fontdata.bitmapFile + len, ".bmp");
	addFile((uint8_t*)&fontdata, sizeof(GFXFontOEPL), name, false);


	uint16_t glyphLen = font->last - font->first;
	uint16_t lastOffset = font->glyph[glyphLen].bitmapOffset;
	uint16_t lastBitmapSize = font->glyph[glyphLen].width * font->glyph[glyphLen].height;
	uint8_t lastBitmapBytes = lastBitmapSize/8;
	if(lastBitmapSize%8)lastBitmapBytes++;
	uint16_t bitmapSize = lastOffset + lastBitmapBytes;
	int16_t glyphSize = (glyphLen + 1) * sizeof(GFXglyph);

	addFile((uint8_t*)font->glyph, glyphSize, fontdata.glyphFile, false);
	addFile((uint8_t*)font->bitmap, bitmapSize, fontdata.bitmapFile, false);
}



#include "../../common/fonts/FreeSans9pt7b.h"
#include "../../common/fonts/FreeSansBold18pt7b.h"
#include "../../common/fonts/FreeSansBold24pt7b.h"

int main(){
	wptr = fopen("../build/fs.img","wb");  // w for write, b for binary

	/* FONTS */
	saveFontData(&FreeSansBold24pt7b, (char*)"font/FreeSansBold24pt7b");
        saveFontData(&FreeSansBold18pt7b, (char*)"font/FreeSansBold18pt7b");
	saveFontData(&FreeSans9pt7b, (char*)"font/FreeSans9pt7b");

	/* OTHER STUFF */
	addFileFromFS("marker.txt", false);
	addFileFromFS("tagprofile.bin", false);
	addFileFromFS("norf.bin", true);
	addFileFromFS("lowbat.bin", true);
	addFileFromFS("sadpanda.bin", true);
        addFileFromFS("tbird2.bin", true);
	addFileFromFS("jet.bin", true);
	printFAT();
	fwrite(buffer,curOffset,1,wptr);
	fclose(wptr);
	return 0;
}
