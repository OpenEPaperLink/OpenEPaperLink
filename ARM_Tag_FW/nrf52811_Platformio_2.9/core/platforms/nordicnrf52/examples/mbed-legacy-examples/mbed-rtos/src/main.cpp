#include "mbed.h"
#include "rtos.h"

Thread thread;
DigitalOut led1(LED1);
volatile bool running = true;

void blink(DigitalOut *led) {
    while (running) {
        *led = !*led;
        wait(1);
    }
}

int main() {
    thread.start(callback(blink, &led1));
    wait(5);
    running = false;
    thread.join();
}