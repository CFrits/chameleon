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

#include <string.h>
#include "py/stream.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "interrupts.h"

#include "uart.h"
#include "panel.h"

#include <stdio.h>
#include <stdlib.h>

extern int received_ack;

void mp_hal_init(void) {
    uart_init();
    enable_interrupts();
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}

mp_uint_t mp_hal_ticks_us(void) {
    return 0;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return 0;
}

void mp_hal_delay_us(mp_uint_t us) {
    // Tuned for fixed CPU frequency
    for (int i = us; i > 0; i--) {
        for (volatile int j = 0; j < 3; j++) {
        }
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    // Tuned for fixed CPU frequency
    for (int i = ms; i > 0; i--) {
        for (volatile int j = 0; j < 3000; j++) {
        }
    }
}

int mp_interrupt_char = -1;

void mp_hal_set_interrupt_char(char c) {
    mp_interrupt_char = c;
}

void mp_machine_idle(void) {
}

#define CHAR_CTRL_C (3)

int mp_hal_stdin_rx_chr(void) {
    unsigned char ch;

    ch = panel_read();

    return ch;
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    mp_uint_t ret = len;

    panel_transmit_string(str, len);

    return ret;
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    mp_hal_stdout_tx_strn(str, len);
}
