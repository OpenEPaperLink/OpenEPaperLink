#include <Arduino.h>

extern void addCRC(void* p, uint8_t len);
extern bool checkCRC(void* p, uint8_t len);

extern void processBlockRequest(struct espBlockRequest* br);
extern bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin);
extern void processJoinNetwork(struct espJoinNetwork* xjn);
extern void processXferComplete(struct espXferComplete* xfc);
extern void processDataReq(struct espAvailDataReq* adr);