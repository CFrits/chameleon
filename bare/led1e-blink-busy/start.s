        .global _start
        .global reset

.equ SRAM_START,                0x20000000
.equ SRAM_END,                  0x20002000

        .short  0x0000
        .long   0xBCBBBABD                              | Some magic number
        .long   0x000500A0                              | Build nummer and modelid
        .long   0x00000000

_start:
        move.w  #0x3700,%sr                             | Supervisor mode, interrupts off.

        movea.l #SRAM_END,%sp
        jsr     main
        halt

reset:
        movea.l #0x407FFFFC,%a0                         | End of DRAM. Boot debugger.
        move.l  #0x12345678,(%a0)                       | Set boot debugger flag.

        | 5206e_um.pdf Coldfire MCF5206e UserManual page 8-17
        | PAR7 - Pin Assignment Bit 7
        | 1 = Output UART 2 request to send signal on RTS[2]/RSTO pin
        | On simulator.
        |move.b  #80,%d0
        |move.b  %d0,0x100000CA                          | Pin assignment register PAR(Pin Assignment Register)

        | On real chameleon.
        move.l  #0xFFFF8000,%d0
        move.l  %d0,0x100000CA                          | Pin assignment register PAR(Pin Assignment Register)

        | After reading or writing UMR1, the pointer points to UMR2.
        move.b  #01,%d0
        move.b  %d0,0x10000180                          | Set mode register to UMR2

        move.b  #0xC0,%d0
        move.b  %d0,0x10000041                          | System Protection Control Register (SYPCR)
                                                        | Enable the software watchdog timer
                                                        | Software watchdog timeout generates an internal reset and RSTO is asserted
        | Wait for the watchdog reset.
watchdog:
        jmp     watchdog
