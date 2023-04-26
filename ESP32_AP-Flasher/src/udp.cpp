#include <Arduino.h>
#include <WiFi.h>

#include "AsyncUDP.h"
#include "commstructs.h"
#include "newproto.h"

#define PKT_AVAIL_DATA_REQ 0xE5
#define PKT_AVAIL_DATA_INFO 0xE6
#define PKT_XFER_COMPLETE 0xEA
#define PKT_XFER_TIMEOUT 0xED
#define PKT_CANCEL_XFER 0xEC
#define PKT_ID_APS 0x80

UDPcomm udpsync;

uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

void init_udp() {
    udpsync.init();
}

UDPcomm::UDPcomm() {
    // Constructor
}

UDPcomm::~UDPcomm() {
    // Destructor
}

void UDPcomm::init() {
    if (udp.listenMulticast(IPAddress(239, 10, 0, 1), 16033)) {
        udp.onPacket([this](AsyncUDPPacket packet) {
            this->processPacket(packet);
        });
    }
}

void UDPcomm::processPacket(AsyncUDPPacket packet) {
    if (packet.data()[0] == PKT_AVAIL_DATA_INFO) {
        espAvailDataReq* adr = (espAvailDataReq*)&packet.data()[1];
        adr->src[7] = 0xFF;
        processDataReq(adr);
    }
    if (packet.data()[0] == PKT_XFER_COMPLETE) {
        espXferComplete* xfc = (espXferComplete*)&packet.data()[1];
        processXferComplete(xfc);
    }
    if (packet.data()[0] == PKT_XFER_TIMEOUT) {
        espXferComplete* xfc = (espXferComplete*)&packet.data()[1];
        processXferTimeout(xfc);
    }
    if (packet.data()[0] == PKT_AVAIL_DATA_REQ) {
        pendingData* pending = (pendingData*)&packet.data()[1];
        prepareExternalDataAvail(pending, packet.remoteIP());
    }
    if (packet.data()[0] == PKT_ID_APS) {
        Serial.println("ap list req");
        IPAddress senderIP = packet.remoteIP();
        unsigned int senderPort = packet.remotePort();
        //todo: autoselect channel 
    }
}

void UDPcomm::netProcessDataReq(struct espAvailDataReq* eadr) {
    uint8_t buffer[sizeof(struct espAvailDataReq) + 1];
    buffer[0] = PKT_AVAIL_DATA_INFO;
    memcpy(buffer + 1, eadr, sizeof(struct espAvailDataReq));
    udp.writeTo(buffer, sizeof(buffer), IPAddress(239, 10, 0, 1), 16033);
}

void UDPcomm::netProcessXferComplete(struct espXferComplete* xfc) {
    uint8_t buffer[sizeof(struct espXferComplete) + 1];
    buffer[0] = PKT_XFER_COMPLETE;
    memcpy(buffer + 1, xfc, sizeof(struct espXferComplete));
    udp.writeTo(buffer, sizeof(buffer), IPAddress(239, 10, 0, 1), 16033);
}

void UDPcomm::netProcessXferTimeout(struct espXferComplete* xfc) {
    uint8_t buffer[sizeof(struct espXferComplete) + 1];
    buffer[0] = PKT_XFER_TIMEOUT;
    memcpy(buffer + 1, xfc, sizeof(struct espXferComplete));
    udp.writeTo(buffer, sizeof(buffer), IPAddress(239, 10, 0, 1), 16033);
}

void UDPcomm::netSendDataAvail(struct pendingData* pending) {
    uint8_t buffer[sizeof(struct pendingData) + 1];
    buffer[0] = PKT_AVAIL_DATA_REQ;
    memcpy(buffer + 1, pending, sizeof(struct pendingData));
    udp.writeTo(buffer, sizeof(buffer), IPAddress(239, 10, 0, 1), 16033);
}