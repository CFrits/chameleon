#include "uart.h"
#include "panel.h"

void delay() {
    long count = 5000000;

    while (count >= 0) {
        count--;
    }
}

void cmd_led (char state) {
    uart_transmit(0x03);
    uart_transmit(0x02);
    uart_transmit(state);
}

void main () {

    uart_init();

    // With this loop the Chameleon can't receive any message
    // This applies also to the reset message, so a power cycle is needed
    // to recover from this state.
    while (1) {
        // Turn led ctrl 3 on.
        cmd_led(0x01);
        delay();

        // Turn all leds off.
        cmd_led(0x00);
        delay();
    }
}
