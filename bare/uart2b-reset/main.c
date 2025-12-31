void uart_init();
void uart_transmit(unsigned char c);
unsigned char uart_receive();

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


void uart_transmit_string (char *msg) {
    int counter = 0;
    unsigned char len = 0;

    // Calculate the length of the message
    while (msg[counter++] != 0) {
        len++;
    }

    if (len > 29) {
       len = 29;
    }

#ifdef CHAMELEON
    uart_transmit(len + 3);
    uart_transmit(0x81);
    uart_transmit(0x03);
#endif

    counter = 0;

    while (counter < len) {
        uart_transmit(msg[counter]);
        counter = counter + 1;
    }
}

int main () {
    char ch;

    uart_init();

    // The string can not be longer than 29 characters.
    uart_transmit_string("Monitor\nC>");

    while (1) {
        ch = panel_receive();

#ifdef CHAMELEON
        // Echo the character.
        uart_transmit(0x04);
        uart_transmit(0x81);
        uart_transmit(0x03);
#endif
        uart_transmit(ch);

        if (ch == 'r') {
            asm("jsr reset");
        }

        if (ch == '\n' || ch == '\r') {
#ifdef CHAMELEON
            if (ch == '\r') {
                uart_transmit(0x04);
                uart_transmit(0x81);
                uart_transmit(0x03);
#endif
                uart_transmit('\n');
            }
            uart_transmit_string("C>");
        }
    }
}
