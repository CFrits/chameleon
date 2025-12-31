#include "uart.h"
#include "panel.h"
#include "timer.h"

void enable_interrupts();
void display_interrupt();

void cmd_led (char state) {
    uart_transmit(0x03);
    uart_transmit(0x02);
    uart_transmit(state);
}

int state = 0;

/* This function is called from assembler */
void timer_isr () {

    if (state == 0) {
        cmd_led(0x00);
        state = 1;
    } else if (state == 1) {
        cmd_led(0x01);
        state = 0;
    } else {
        state = 0;
    }
}

void main () {
    char ch;

    uart_init();
    timer_init();

    uart_transmit_string("Monitor\nC>");

    // Turn led on to start with
    cmd_led(0x01);

    state = 0;

    enable_interrupts();

    while (1) {
        ch = panel_receive();
        panel_transmit(ch);

        if (ch == '\n' || ch == '\r') {
            panel_transmit(ch);
            uart_transmit_string("C>");
        }
    }
}
