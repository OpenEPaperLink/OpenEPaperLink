#include "stdint.h"
#include "stdbool.h"


#define AONSHADOW_SIZE 2048

extern bool aonRamValid;

bool loadValidateAonRam();
void saveAonRam();
void clearAonRam();