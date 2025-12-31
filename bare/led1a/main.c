void uart_init();
void uart_transmit(unsigned char c);

void led(int state) {
    uart_transmit(0x03);
    uart_transmit(0x02);
    uart_transmit(state);
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

    uart_init();

    // The string can not be longer than 29 characters.
    uart_transmit_string("Turn led 1 on\n");

    // Turn led on
    led(0x01);

    while (1) {
    }
}
