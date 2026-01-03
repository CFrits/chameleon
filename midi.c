#include "mpconfigport.h"
#include "py/runtime.h"
#include "midi.h"
#include "interrupts.h"

static volatile unsigned char mringbuf[MIDI_BUFFER_LENGTH];
static volatile int mreadptr = 0;
static volatile int mwriteptr = 0;

void midi_isr() {
    unsigned char ch;

    disable_interrupts();

    // An interrupt occurred. Now get the received character.
    ch = midi_receive();

    mringbuf[mwriteptr] = ch;
    mwriteptr = (mwriteptr + 1) % MIDI_BUFFER_LENGTH;

    // TODO: Check if buffer is full.

    enable_interrupts();
}

// A blocking read.
unsigned char midi_read() {
    unsigned char ch;
    
    while (mwriteptr == mreadptr) {
        MICROPY_EVENT_POLL_HOOK;
    }

    ch = mringbuf[mreadptr]; 
    mreadptr = (mreadptr + 1) % MIDI_BUFFER_LENGTH;

    return ch;
}
