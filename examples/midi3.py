import midi

COMMAND  = 1
NOTE     = 2
VELOCITY = 3

class Event:

    def __init__(self):
        self.cmd = 0

        self.channel = 0
        self.note = 0
        self.velocity = 0

    def receive(self):
        state = COMMAND

        while True:
            ev = midi.read()

            if state == COMMAND:
                self.cmd = ev & 0xF0
                self.channel = ev & 0x0F
                state = NOTE
                continue

            if state == NOTE:
                self.note = ev
                state = VELOCITY
                continue

            if state == VELOCITY:
                self.velocity = ev
                return

ev = Event()

while True:
    ev.receive()
    print(f"{ev.cmd:02X} {ev.channel:02X} {ev.note:02X} {ev.velocity:02X}")
