#pragma once
#ifdef HAS_BLE_WRITER

void BLETask(void* parameter);
bool BLE_taskHung();
// drops the link to peer (BLE byte order) when the calls in between take longer than timeoutMs
void BLE_guardStart(const uint8_t peer[6], uint32_t timeoutMs);
void BLE_guardStop();

#endif
