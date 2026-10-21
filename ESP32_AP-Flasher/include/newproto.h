#include <Arduino.h>
#include <FS.h>

#include "commstructs.h"

struct v2BlockServed;

struct PendingItem {
    struct pendingData pendingdata;
    char filename[50];
    uint8_t* data;
    uint32_t len;
};

extern void addCRC(void* p, uint8_t len);
extern bool checkCRC(void* p, uint8_t len);

extern void processBlockRequest(struct espBlockRequest* br);
extern void processPreload(struct espBlockRequest* br);
extern void processBlockServed(const struct v2BlockServed* bs);
extern void prepareCancelPending(const uint8_t dst[8]);
extern void prepareIdleReq(const uint8_t* dst, uint16_t nextCheckin);
extern void prepareDataAvail(const uint8_t* dst);
extern void prepareDataAvail(uint8_t* data, uint16_t len, uint8_t dataType, const uint8_t* dst);
extern bool prepareDataAvail(String& filename, uint8_t dataType, uint8_t dataTypeArgument, const uint8_t* dst, uint16_t nextCheckin, bool resend = false);
extern void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP);
extern void processXferComplete(struct espXferComplete* xfc, bool local);
extern void processXferTimeout(struct espXferComplete* xfc, bool local);
// Which link carried this check-in; sets tagRecord::lastHeardOnRadio. CHECKIN_VIA_OTHER (a remote
// AP's report over UDP, the AP's own display) leaves it alone.
enum CheckinVia : uint8_t { CHECKIN_VIA_OTHER = 0, CHECKIN_VIA_RADIO, CHECKIN_VIA_BLE };
extern void processDataReq(struct espAvailDataReq* adr, bool local, IPAddress remoteIP = IPAddress(0, 0, 0, 0), CheckinVia via = CHECKIN_VIA_OTHER);
extern void processTagReturnData(struct espTagReturnData* trd, uint8_t len, bool local);

extern bool sendTagCommand(const uint8_t* dst, uint8_t cmd, bool local, const uint8_t* payload = nullptr);
bool sendTagMac(const uint8_t* dst, const uint64_t newmac, bool local);
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
void dequeueAllItems(const uint8_t* targetMac);
uint16_t countQueueItem(const uint8_t* targetMac);
// A returned PendingItem stays valid for a while after it is dequeued (see freeRetiredQueueItems)
extern PendingItem* getQueueItem(const uint8_t* targetMac);
extern PendingItem* getQueueItem(const uint8_t* targetMac, const uint64_t dataVer);
void checkQueue(const uint8_t* targetMac);
bool queueDataAvail(struct pendingData* pending, bool local);
uint8_t* getDataForFile(fs::File& file);
bool loadQueueItemData(PendingItem* queueItem);
void freeRetiredQueueItems();
