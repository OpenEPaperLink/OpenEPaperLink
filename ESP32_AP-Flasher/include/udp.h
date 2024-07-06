#include <Arduino.h>

#include "AsyncUDP.h"

#ifndef defudpcomm
#define defudpcomm

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
    private:
		AsyncUDP udp;
		AsyncUDP udp2;
		void processPacket(AsyncUDPPacket packet);
};

#endif

void init_udp();
