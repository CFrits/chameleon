        .global midi_init
        .global midi_transmit
        .global midi_receive
        .global uart1_handler

.equ MCF5206E_ICR12,            0x1000001F              | Interrupt Control Register 12 - Uart 1 Interrupt
.equ MCF5206E_IMR,              0x10000036              | Interrupt Mask Register

# Addresses of the first Uart interface of the Coldfire. 
# On the Chameleon these are connected to the MIDI interface.
.equ MCF5206E_UMR1,             0x10000140              | Mode Register UMR1 and UMR2
.equ MCF5206E_USR1,             0x10000144              | UART Status Register when reading. Clock Select Register when writing.
.equ MCF5206E_UCR1,             0x10000148              | UART Command Register
.equ MCF5206E_UTB1,             0x1000014C              | UART Transmit Buffer (write only)

.equ MCF5206E_UISR1,            0x10000154              | Interrupt Status Register when reading.
.equ MCF5206E_UIMR1,            0x10000154              | Interrupt Mask Register when writing.
.equ MCF5206E_UIVR1,            0x10000170              | Interrupt Vector Register.

uart1_handler:
        lea     -60(%sp),%sp                            | Make room on stack for registers
        movem.l %d0-%d7/%a0-%a6,(%sp)                   | save registers on the stack

        jsr     midi_isr

        movem.l (%sp),%d0-%d7/%a0-%a6
        lea     60(%sp),%sp                             | Restore stack register.

        rte

# 
# The Autovector of Uart 1 is determined by Interrupt Control Register 12
#
midi_init:
        move.b  #0x04,%d0                               | Setup Uart 1 interrupt
                                                        | Bit     7 Autovector Enable = 0
                                                        | Bit   4:2 Interupt level = 1
                                                        | Bit   0:1 Interupt priority = 0
        move.b  %d0,MCF5206E_ICR12                      | Level 1 interrupt, Priority 0, Autovector OFF. Vector 24

        move.b  #24,%d0
        move.b  %d0,MCF5206E_UIVR1                      | Set interrupt vector to 24.

        move.b  #0x10,%d0
        move.b  %d0,MCF5206E_UCR1                       | Reset Mode Register. The Mode Register Ptr now points to UMR1

        move.b  #0x13,%d0
                                                        | Bit     7 RxRTS
                                                        | Bit     6 RxIRQ Receiver Interrupt Select
                                                        |           1 = FFULL is the source that generated an interrupt
                                                        |           0 = RxRDY is the source that generated an interrupt
                                                        | Bit   4:3 Parity mode "10" is no parity
                                                        | Bit   1:0 Bits per character "11" is 8 bits.
        move.b  %d0,MCF5206E_UMR1                       | User Mode Register1.

        move.b  #0x07,%d0                               | Normal mode.
        move.b  %d0,MCF5206E_UMR1                       | User Mode Register2.

        move.b  #2,%d0
        move.b  %d0,MCF5206E_UIMR1                      | Enable Interrupt FFULL (Fifo full).

        move.b  #5,%d0
        move.b  %d0,MCF5206E_UCR1                       | Enable Transmitter and Receiver.

        move.w  MCF5206E_IMR,%d0                        | Read current IMR value.
        andi.l  #0xEFFF,%d0                             | Enable Uart 1 interrupt
        move.w  %d0,MCF5206E_IMR

        rts


# The Uart transmit functions outputs the content of register d0.
midi_transmit:
        move.l  4(%sp),%d0                              | Get the argument from the stack frame. When called from C.
midi_wait:
        move.b  MCF5206E_USR1,%d1
        btst    #2,%d1
        beq     midi_wait                               | Wait until USR_TXRDY bit is set.
        move.b  %d0,MCF5206E_UTB1                       | Output register d0 in the Uart transmit buffer.
        rts

midi_timeout:
        move.l  #0xFFFFFFFF,%d0                         | Return -1
        rts

midi_receive:
        moveq   #0x00,%d1                               | Busy wait loop.
midi_rxwait:
        cmp.l   #0x00000FFF,%d1
        bcc     midi_timeout
        addq.l  #0x01,%d1

        move.b  MCF5206E_USR1,%d0
        btst    #0,%d0
        beq     midi_rxwait                             | Wait until USR_RXRDY bit is set.
        move.b  MCF5206E_UTB1,%d0
        rts
