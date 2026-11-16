// Entry point on the TLSR825x: board start, then the AP loop (ap.c). The AP is mains powered and
// never sleeps; the loop runs continuously.

#include "tl_common.h"

#include "../ap.h"

void halBoardInit(void);
void halBoardPoll(void);

int main(void) {
    halBoardInit();
    apInit();
    while (1) {
        apLoop();
        halBoardPoll();
    }
    return 0;
}
