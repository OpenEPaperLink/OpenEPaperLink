#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void initRadio(uint8_t channel);
void radioSendData(uint8_t *out_buff, int len);
