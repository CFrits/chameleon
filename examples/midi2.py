import midi

class Event:

    def __init__(self):
        self.cmd = 0

        self.channel = 0
        self.note = 0
        self.velocity = 0

    def receive(self):
        ev = midi.read()

        # Only Note ON and Note OFF are expected at the moment.
        if ev & 0x80 == 0x80:
            # Highest bit is set, so it's a command
            self.cmd = ev & 0xF0
            self.channel = ev & 0x0F

            # Read the note
            self.note = midi.read()

            # Read the velocity
            self.velocity = midi.read()

ev = Event()

while True:
    ev.receive()
    print(f"{ev.cmd:02X} {ev.channel:02X} {ev.note:02X} {ev.velocity:02X}")
