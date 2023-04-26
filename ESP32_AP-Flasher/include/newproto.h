#include <Arduino.h>

extern void addCRC(void* p, uint8_t len);
extern bool checkCRC(void* p, uint8_t len);

extern void processBlockRequest(struct espBlockRequest* br);
extern void prepareIdleReq(uint8_t* dst, uint16_t nextCheckin);
extern bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin);
extern void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP);
extern void processXferComplete(struct espXferComplete* xfc);
extern void processXferTimeout(struct espXferComplete* xfc);
extern void processDataReq(struct espAvailDataReq* adr);
void refreshAllPending();