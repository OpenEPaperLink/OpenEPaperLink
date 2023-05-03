#include <Arduino.h>

String lookupFirmwareFile(uint16_t &version);
bool performDeviceFlash();

void getFirmwareMD5(class ZBS_interface* zbs, uint8_t* md5p);
void getInfoPageMac(class ZBS_interface* zbs, uint8_t* mac);