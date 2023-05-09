#include <Arduino.h>

extern void addCRC(void* p, uint8_t len);
extern bool checkCRC(void* p, uint8_t len);

extern void processBlockRequest(struct espBlockRequest* br);
extern void prepareIdleReq(uint8_t* dst, uint16_t nextCheckin);
extern bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin);
extern void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP);
extern void processXferComplete(struct espXferComplete* xfc, bool local);
extern void processXferTimeout(struct espXferComplete* xfc,bool local);
extern void processDataReq(struct espAvailDataReq* adr, bool local);

extern bool sendAPSegmentedData(uint8_t* dst, String data, uint16_t icons, bool inverted);
extern bool showAPSegmentedInfo(uint8_t* dst);

void refreshAllPending();
void setAPchannel();
