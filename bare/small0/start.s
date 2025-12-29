        .global _start

        .short  0x0000
        .long   0xBCBBBABD                              | Some magic number
        .long   0x000500A0                              | Build nummer and modelid
        .long   0x00000000

_start:
        bra     _start
