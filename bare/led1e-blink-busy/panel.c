#include "uart.h"

enum Panelstate { LENGHT, CMDTYPE, SUBTYPE, DATA };

int received_ack = 0;

char panel_receive() {
    int len = 0, subcmd = 0, cmd = 0;
    enum Panelstate state = LENGHT;
    unsigned char ch;

    while (1) {
        ch = uart_receive();

        switch (state) {
            case LENGHT:
                len = ch;
                state = CMDTYPE;
                break;

            case CMDTYPE:
                cmd = ch;
                subcmd = 0;

                if (cmd == 0x80 || cmd == 0x81) {
                    state = SUBTYPE;
                } else {
                    state = DATA;
                }

                break;

            case SUBTYPE:
                subcmd = ch;
                state  = DATA;
                break;

            case DATA:
                break;
        }

        len--;

        if (len == 0) {

            // A complete command has been received. Send an ACK
            if (cmd == 0x40) {
                received_ack++;
            } else {
                uart_transmit(0x02);
                uart_transmit(0x40);
            }

            state = LENGHT;

            if (subcmd == 0x01) {
                asm("jsr reset");
            }

            if (subcmd == 0x03) {
                return ch;
            }
        }
    }
}

void panel_transmit(unsigned char ch) {

#ifdef CHAMELEON
    uart_transmit(0x04);
    uart_transmit(0x81);
    uart_transmit(0x03);
#endif
    uart_transmit(ch);
}
