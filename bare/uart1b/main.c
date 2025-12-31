void uart_init();
void uart_transmit(unsigned char c);

void uart_transmit_string (char *msg, unsigned char len) {
    int counter = 0;

#ifdef CHAMELEON
    uart_transmit(len + 3);
    uart_transmit(0x81);
    uart_transmit(0x03);
#endif

    while (counter < len) {
        uart_transmit(msg[counter]);
        counter = counter + 1;
    }
}

int main () {

    uart_init();

    // The string can not be longer than 29 characters.
    uart_transmit_string("Hello World!\n", 13);

    while (1) {
    }
}

