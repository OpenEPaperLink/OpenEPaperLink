#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "settings.h"
extern "C" {
#include "mz100/mz100_flash.h"
#include "mz100/printf.h"
#include "mz100/util.h"
}

// #pragma pack(1)

#include "oepl_fs.h"

extern "C" {
__attribute__((section(".aonshadow"))) uint32_t fsEntry;
__attribute__((section(".aonshadow"))) uint32_t fsEnd;
}

extern void dump(const uint8_t *a, const uint16_t l);

OEPLFs oeplfs;
OEPLFs *fs = &oeplfs;

void OEPLFs::init() {
    if (!this->isStarted) {
        this->files.clear();
        if (fsEntry == 0) {
            silent = false;
            fsEntry = this->findEntry();
        } else {
            silent = true;
        }
        if (fsEntry) this->populateFiles();
        this->isStarted = true;
    }
}

OEPLFs::OEPLFs() {
}

OEPLFs::~OEPLFs() {
}

uint32_t OEPLFs::findEntry() {
#ifdef DEBUG_FS
    printf("FS: Trying to find OEPL FS...\n");
#endif
    uint8_t *scan = (uint8_t *)malloc(1024);
    uint32_t offset = 0;
    // scan flash with some overlap, to ensure the entire string is in the buffer at some point
    for (uint32_t c = 60000; c < 180000; c += 512) {
        FLASH_Read(FLASH_FAST_READ_QUAD_OUT, c, scan, 1024);
        uint32_t scan_offset = 0;
        bool nextblock = false;
        while (!nextblock) {
            uint8_t *p = (uint8_t *)memchr(scan + scan_offset, '-', 1024 - scan_offset);
            if (p) {
                // see if the rest of the string is also there
                int8_t match = memcmp(p + 1, "--OEPL_FS!", 10);
                if (match == 0) {
                    offset = (uint32_t)(p - scan);
                    offset += c;
                    nextblock = true;
                    break;
                } else {
                    scan_offset = (p - scan) + 1;
                }
            } else {
                nextblock = true;
            }
        }
    }

    if (offset) {
#ifdef DEBUG_FS
        printf("FS: Found at 0x%08X\n", offset);
#endif
    } else {
        printf("FS: Not found. Did you forget to add it?\n");
        FLASH_Read(FLASH_FAST_READ_QUAD_OUT, 0x109C0, scan, 1024);
        dump(scan, 1024);
    }
    free(scan);
    return offset;
}

void OEPLFs::deleteFile(char *name) {
    // get file list inside of the vector
    uint8_t *buffer = (uint8_t *)malloc(1024);
    FLASH_Read(FLASH_FAST_READ_QUAD_OUT, fsEntry, buffer, 1024);
    bool firstFile = true;
    uint8_t *fatEnd = nullptr;
    uint8_t *curFileP = buffer;
    curFileP += 11;  // set to begin file table
    while (1) {
        OEPLFSFile *file;
        file = new OEPLFSFile;
        memcpy((void *)file, curFileP, sizeof(OEPLFSFile));

        if (firstFile) {
            firstFile = false;
            fatEnd = buffer + file->offset - 1;
        }

        if (strncmp(name, file->name, FILENAME_LENGTH) == 0) {
            char overwriteName[FILENAME_LENGTH];
            memset(overwriteName, 0x00, FILENAME_LENGTH);
            FLASH_Write(FLASH_PROGRAM_NORMAL, fsEntry + (curFileP - buffer), (uint8_t *)overwriteName, FILENAME_LENGTH);
            break;
        }

        curFileP += sizeof(OEPLFSFile);

        if (curFileP > fatEnd) {
            break;
        }
    }
    free(buffer);
}

void OEPLFs::populateFiles() {
    // get file list inside of the vector
    uint8_t *buffer = (uint8_t *)malloc(1024);
    FLASH_Read(FLASH_FAST_READ_QUAD_OUT, fsEntry, buffer, 1024);
    bool done = false;
    bool firstFile = true;
    uint8_t *fatEnd = nullptr;
    uint8_t *curFileP = buffer;
    curFileP += 11;  // set to begin file table
    uint32_t fsend = 0;
    while (!done) {
        OEPLFSFile *file;
        file = new OEPLFSFile;
        memcpy((void *)file, curFileP, sizeof(OEPLFSFile));
        this->files.push_back(file);
        if (firstFile) {
            firstFile = false;
            fatEnd = buffer + file->offset - 1;
        }
        curFileP += sizeof(OEPLFSFile);

        // print reference
        char tmp[32];
        memcpy(tmp, file->name, 32);
        tmp[31] = 0x00;
#ifdef DEBUG_FS
        if (!silent) printf("name=%s, size=%u, offset=%u\n", tmp, file->len, file->offset);
#endif
        fsend = file->len + file->offset;
        if (curFileP > fatEnd) {
            done = true;
            break;
        }
    }
    fsEnd = fsend + fsEntry;
    fsEnd = flashRoundUp(fsEnd);
    free(buffer);
}

OEPLFile *OEPLFs::getFile(char *name) {
    // if (files.size() == 0) this->init();
    for (uint8_t c = 0; c < files.size(); c++) {
        if (strncmp(name, files.at(c)->name, FILENAME_LENGTH) == 0) {
            OEPLFile *file = new OEPLFile(files.at(c)->offset + fsEntry);
            file->length = files.at(c)->len;
            return file;
        }
    }
    printf("file '%s' not found in %d files\n", name, files.size());
    return nullptr;
}

uint8_t OEPLFile::operator()(uint32_t i) {
    return this->readByte(i);
}

void OEPLFile::getBlock(uint32_t offset, uint8_t *target, uint32_t len) {
    // get a block of data from within the 'file'
    FLASH_Read(FLASH_FAST_READ_QUAD_OUT, this->eepromOffset + offset, target, len);
}

uint8_t OEPLFile::readByte(uint32_t i) {
    if (!buffer) {
        this->buffer = (uint8_t *)malloc(OEPLFS_CACHE_SIZE);
    }

    // calculate requested eeprom position
    uint32_t eepromPos = i + this->eepromOffset;

    // check if this currently buffered
    if ((eepromPos >= this->bufferBegin) && (eepromPos < (this->bufferBegin + OEPLFS_CACHE_SIZE))) {
        // cache hit
    } else {
        // cache miss
        FLASH_Read(FLASH_FAST_READ_QUAD_OUT, this->eepromOffset + i, buffer, OEPLFS_CACHE_SIZE);
        this->bufferBegin = this->eepromOffset + i;
    }
    return buffer[(this->eepromOffset + i) - this->bufferBegin];
}

OEPLFile::OEPLFile(uint32_t offset) {
    this->eepromOffset = offset;
}

OEPLFile::~OEPLFile() {
    if (this->buffer) {
        free(this->buffer);
    }
}