#include <Arduino.h>
#include <WiFi.h>

#include "AsyncUDP.h"
#include "commstructs.h"
#include "newproto.h"

#define PKT_AVAIL_DATA_SHORTREQ 0xE3
#define PKT_AVAIL_DATA_REQ 0xE5
#define PKT_AVAIL_DATA_INFO 0xE6
#define PKT_BLOCK_PARTIAL_REQUEST 0xE7
#define PKT_BLOCK_REQUEST_ACK 0xE9
#define PKT_BLOCK_REQUEST 0xE4
#define PKT_BLOCK_PART 0xE8
#define PKT_XFER_COMPLETE 0xEA
#define PKT_XFER_COMPLETE_ACK 0xEB
#define PKT_CANCEL_XFER 0xEC
#define PKT_PING 0xED
#define PKT_PONG 0xEE

UDPcomm udpsync;

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
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([this](AsyncUDPPacket packet) {
            this->processPacket(packet);
        });
    }
}

void UDPcomm::send(uint8_t* output) {
    udp.writeTo(output, strlen((char*)output), IPAddress(239, 10, 0, 1), 16572);
}

void UDPcomm::processPacket(AsyncUDPPacket packet) {
    if (packet.data()[0] == 0xFD) {
        espAvailDataReq* adr = (espAvailDataReq*)&packet.data()[1];
        processDataReq(adr);
    }
}

void UDPcomm::processDataReq(struct espAvailDataReq* eadr) {
    uint8_t buffer[sizeof(struct espAvailDataReq) + 1];
    buffer[0] = PKT_AVAIL_DATA_INFO;
    memcpy(buffer + 1, eadr, sizeof(struct espAvailDataReq));
    udp.writeTo(buffer, sizeof(buffer), IPAddress(239, 10, 0, 1), 16572);
}

