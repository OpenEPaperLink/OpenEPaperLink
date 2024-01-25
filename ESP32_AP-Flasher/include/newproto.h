#include <Arduino.h>
#include <FS.h>

#include "commstructs.h"

struct PendingItem {
    struct pendingData pendingdata;
    char filename[50];
    uint8_t* data;
    uint32_t len;
};

extern void addCRC(void* p, uint8_t len);
extern bool checkCRC(void* p, uint8_t len);

extern void processBlockRequest(struct espBlockRequest* br);
extern void prepareCancelPending(const uint8_t dst[8]);
extern void prepareIdleReq(const uint8_t* dst, uint16_t nextCheckin);
extern void prepareDataAvail(uint8_t* data, uint16_t len, uint8_t dataType, const uint8_t* dst);
extern bool prepareDataAvail(String& filename, uint8_t dataType, uint8_t dataTypeArgument, const uint8_t* dst, uint16_t nextCheckin, bool resend = false);
extern void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP);
extern void processXferComplete(struct espXferComplete* xfc, bool local);
extern void processXferTimeout(struct espXferComplete* xfc, bool local);
extern void processDataReq(struct espAvailDataReq* adr, bool local, IPAddress remoteIP = IPAddress(0, 0, 0, 0));
extern void processTagReturnData(struct espTagReturnData* trd, uint8_t len, bool local);

extern bool sendTagCommand(const uint8_t* dst, uint8_t cmd, bool local, const uint8_t* payload = nullptr);
extern bool sendAPSegmentedData(const uint8_t* dst, String data, uint16_t icons, bool inverted, bool local);
extern bool showAPSegmentedInfo(const uint8_t* dst, bool local);
extern void updateTaginfoitem(struct TagInfo* taginfoitem, IPAddress remoteIP);
bool checkMirror(struct tagRecord* taginfo, struct pendingData* pending);

void refreshAllPending();
void updateContent(const uint8_t* dst);
void setAPchannel();

void enqueueItem(const PendingItem& item);
bool dequeueItem(const uint8_t* targetMac);
bool dequeueItem(const uint8_t* targetMac, const uint64_t dataVer);
uint16_t countQueueItem(const uint8_t* targetMac);
extern PendingItem* getQueueItem(const uint8_t* targetMac);
extern PendingItem* getQueueItem(const uint8_t* targetMac, const uint64_t dataVer);
void checkQueue(const uint8_t* targetMac);
bool queueDataAvail(struct pendingData* pending);
uint8_t* getDataForFile(fs::File& file);
