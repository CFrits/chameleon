        .global _start
        .global reset
        .global enable_interrupts
        .global disable_interrupts

.equ MBAR_START,                0x10000000

.equ VECTOR_START,              0x20000000
.equ VECTOR_END,                0x20000400

.equ SRAM_START,                0x20000000
.equ SRAM_END,                  0x20002000

        .short  0x0000
        .long   0xBCBBBABD                              | Some magic number
        .long   0x000500A0                              | Build nummer and modelid
        .long   0x00000000

_start:
        move.w  #0x3700,%sr                             | Supervisor mode, interrupts off.

        movea.l #MBAR_START,%a0                         | Load Memory Base Address Register.
        move.w  #0xFFFE,%d0                             | 
        move.b  %d0,%a0@(54)                            | Interrupt Mask Register

        movea.l #VECTOR_START,%a0
        movea.l #VECTOR_END,%a1
        move.l  #default_handler,%d0                    | Default interrupt handlers

_clear_vector_loop:
        move.l  %d0,%a0@+
        cmpa.l  %a0,%a1
        bne.w   _clear_vector_loop

        movea.l #VECTOR_START,%a0
        movea.l #0,%a1
        move.q  #0,%d0
        move.l  %a1,%a0@(0,%d0:l:4)

        movea.l #_start,%a1                             | Set RESET vector
        move.q  #1,%d0
        move.l  %a1,%a0@(0,%d0:l:4)

        movea.l #spurious_handler,%a1                   | Set Spurious interrupt vector.
        move.q  #24,%d0
        move.l  %a1,%a0@(0,%d0:l:4)

        movea.l #timer_handler,%a1
        move.q  #25,%d0                                 | Set Autovector 1 interrupt for Timer.
        move.l  %a1,%a0@(0,%d0:l:4)

        movec   %a0,%vbr                                | Set Interrupt Vector table location

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

default_handler:
        rte

bad0:
        bra.w   bad0
        halt

spurious_handler:
        rte

disable_interrupts:
        move.w  %sr,%d0
        ori.l   #0x0700,%d0
        move.w  %d0,%sr
        rts

enable_interrupts:
        move.w  %sr,%d0
        andi.l  #0xF8FF,%d0
        move.w  %d0,%sr
        rts
