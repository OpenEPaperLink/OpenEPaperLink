#ifndef _AP_EMULATE_H_
#define _AP_EMULATE_H_

void fakeTagCheckIn();

extern uint8_t __xdata fakeTagMac[8];
extern bool __xdata fakeTagTrafficPending;
extern void fakePendingData(struct pendingData* pd);
extern void fakeTagGetData();

#endif