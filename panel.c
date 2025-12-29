/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Frits Wiersma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>

#include "py/runtime.h"

#include "uart.h"
#include "interrupts.h"

void mp_hal_delay_ms(mp_uint_t ms);

static volatile unsigned char ringbuf[BUFFER_LENGTH];
static volatile int readptr = 0;
static volatile int writeptr = 0;

enum Panelstate { LENGTH, CMDTYPE, SUBTYPE, DATA };

int received_ack = 0;

void wait_for_ack() {
    long count = 500000;

    while (count >= 0) {

        if (received_ack > 0) {
            return;
        }
        count--;
    }
}

void panel_receive(unsigned char ch) {
    static int len = 0, subcmd = 0, cmd = 0;
    static enum Panelstate state = LENGTH;

    switch (state) {
        case LENGTH:
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
            if (subcmd == 0x03) {
                ringbuf[writeptr] = ch;
                writeptr = (writeptr + 1) % BUFFER_LENGTH;
            }

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

        if (subcmd == 0x01) {
            //asm("jsr reset");
        }

        state = LENGTH;
        cmd = 0;
        len = 0;
        subcmd = 0;
    }
}

void panel_transmit(unsigned char ch) {
    received_ack = 0;

    uart_transmit(0x04);
    uart_transmit(0x81);
    uart_transmit(0x03);
    uart_transmit(ch);

    // Wait for an Ack.
    wait_for_ack();
}

void panel_transmit_string (const char *msg, size_t len) {
    int l;
    char message[32];

    // The chameleon does not support messages longer than 32 characters.
    // The string need to be split up.
    for (size_t i = 0; i < len; i += 29) {
        received_ack = 0;

        l = (len - i < 29) ? (len - i) : 29;

        message[0] = l + 3;
        message[1] = 0x81;
        message[2] = 0x03;

        for (int j = 0; j < l; j++) {
            message[3 + j] = msg[i + j];
        }

        // TODO: Experimental. This code should be as short as possible.
        // The uart interrupt may not send an ACK in the middle of another message.
        disable_interrupts();
        for (int j = 0; j < l + 3; j++) {
            uart_transmit(message[j]);
        }
        enable_interrupts();

        // Wait for an Ack.
        wait_for_ack();

        // Wait some time anyway.
        mp_hal_delay_ms(20);
    }
}

void uart2_isr() {
    unsigned char ch;

    //disable_interrupts();

    // An interrupt occurred. Now get the received character.
    ch = uart_receive();

    // And process it with the panel protocol.
    panel_receive(ch);

    // TODO: Check if buffer is full.

    //enable_interrupts();
}

// A blocking read.
char panel_read() {
    unsigned char ch;

    while (writeptr == readptr) {
    }

    ch = ringbuf[readptr];
    readptr = (readptr + 1) % BUFFER_LENGTH;

    return ch;
}
