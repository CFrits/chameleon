import midi
import lcd
import led

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
                self.cmd = ev
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

lcd.print(0, 0, "  Midi Monitor  ")
lcd.print(1, 0, "   (Waiting)    ")

cmdname = { 0x80 : "Key Off", 0x90 : "Key On " }
notename = ["C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"] 

while True:
    ev.receive()

    command = ev.cmd & 0xF0

    try:
        cname = cmdname[command]
        nname = notename[ev.note % 12]

        #                 1234567890123456
        lcd.clear()
        lcd.print(0, 0, f"  {cname} {nname}")
        lcd.print(1, 0, f"   {ev.cmd:02X} {ev.note:02X} {ev.velocity:02X}")
        print(f"{ev.cmd:02X} {ev.channel:02X} {ev.note:02X} {ev.velocity:02X}")
    except:
        cname = "UNKNOWN"
        lcd.print(0, 0, f"  {cname}")
        lcd.print(1, 0, f"   {ev.cmd:02X}")
        print(f"UNKNOWN COMMAND: {ev.cmd:02X}")


