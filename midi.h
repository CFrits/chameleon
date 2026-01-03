void midi_init();
void midi_transmit(unsigned char c);
unsigned char midi_receive();

unsigned char midi_read();

#define MIDI_BUFFER_LENGTH   32
