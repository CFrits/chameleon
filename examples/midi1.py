import midi

while True:
    byte = midi.read()
    print(f"{byte:02X} ")
