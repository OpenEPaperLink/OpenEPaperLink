#include <Arduino.h>

#include "AsyncUDP.h"

class UDPcomm {
	public:
		UDPcomm();
		~UDPcomm();
		void init();
		void send(uint8_t* output);
		void processDataReq(struct espAvailDataReq* eadr);
	
	private:
		AsyncUDP udp;
		void processPacket(AsyncUDPPacket packet);
};

void init_udp();