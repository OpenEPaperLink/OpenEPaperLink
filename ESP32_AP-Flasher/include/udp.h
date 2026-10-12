#include <Arduino.h>

#include <map>

#include "AsyncUDP.h"
#include "tag_db.h"
#ifndef defudpcomm
#define defudpcomm

extern Config config;

class UDPcomm {
   public:
    UDPcomm();
    ~UDPcomm();
    void init();
    void getAPList();
    void netProcessDataReq(struct espAvailDataReq* eadr);
    void netProcessXferComplete(struct espXferComplete* xfc);
    void netProcessXferTimeout(struct espXferComplete* xfc);
    void netSendDataAvail(struct pendingData* pending);
    void netTaginfo(struct TagInfo* taginfoitem);
    // Lowest UDP_PROTO_VERSION among the APs seen in AP-list exchanges (0 if one runs older
    // firmware); UDP_PROTO_VERSION when no other AP has been seen.
    uint8_t minPeerVersion();

   private:
    AsyncUDP udp;
    std::map<uint32_t, uint8_t> peerVersions;  // AP IP -> its UDP_PROTO_VERSION
    void notePeer(const struct APlist& item);
    void processPacket(AsyncUDPPacket packet);
    void writeUdpPacket(uint8_t* buffer, uint16_t len, IPAddress senderIP);
};

#endif

void init_udp();
