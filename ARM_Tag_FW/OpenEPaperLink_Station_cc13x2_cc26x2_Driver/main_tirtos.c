#include <stdint.h>
#include <pthread.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

extern void *mainThread(void *arg0);

#define THREADSTACKSIZE    2096

int main(void)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;

    Board_initGeneral();
    GPIO_init();

    pthread_attr_init(&attrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &priParam);

    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        while (1);
    }

    retc = pthread_create(&thread, &attrs, mainThread, NULL);
    if (retc != 0) {
        while (1);
    }

    BIOS_start();
    return (0);
}
