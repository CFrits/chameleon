void uart_init();
void uart_transmit(unsigned char c);

void uart_transmit_string (char *msg) {
    int counter = 0;
    unsigned char len = 0;
    char i, l;

    // Calculate the length of the message
    while (msg[counter++] != 0) {
        len++;
    }

    // The chameleon does not support messages longer than 32 characters.
    // The string need to be split up.
    i = 0;

    while (i < len) {
        l = i % 29;

        if (l == 0) {
            #ifdef CHAMELEON
            if (len - i > 29) { // There are still more than 29 characters left.
                uart_transmit(32);
                uart_transmit(0x81);
                uart_transmit(0x03);
            } else {
                uart_transmit(len - i + 3);
                uart_transmit(0x81);
                uart_transmit(0x03);
            }
            #endif
        }

        uart_transmit(*msg++);
        i++;
    }

}


int main () {

    uart_init();

    // The string can now be longer than 29 characters.
    uart_transmit_string("Hello World!. This message is longer than 29 characters.\n");

    while (1) {
    }
}

