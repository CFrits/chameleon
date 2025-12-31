        .global _start
        .global uart_init
        .global uart_transmit
        .global uart_receive

.equ SRAM_START,                0x20000000
.equ SRAM_END,                  0x20002000

# Addresses of the second Uart interface of the Coldfire. 
# On the Chameleon these are connected to the RS232 interface.
.equ MCF5206E_USR,              0x10000184              | UART Status Register
.equ MCF5206E_UCR,              0x10000188              | UART Command Register
.equ MCF5206E_UTB,              0x1000018C              | UART Transmit Buffer (write only)

        .short  0x0000
        .long   0xBCBBBABD                              | Some magic number
        .long   0x000500A0                              | Build nummer and modelid
        .long   0x00000000

_start:
        move.w  #0x3700,%sr                             | Supervisor mode, interrupts off.

        movea.l #SRAM_END,%sp
        jsr     main
        halt

uart_init:
        move.b  #5,%d0
        move.b  %d0,MCF5206E_UCR                        | Enable Transmitter and Receiver.
        rts

# The Uart transmit functions outputs the content of register d0.
uart_transmit:
        move.l  4(%sp),%d0                              | Get the argument from the stack frame. When called from C.
uart_wait:
        move.b  MCF5206E_USR,%d1
        btst    #2,%d1
        beq     uart_wait                               | Wait until USR_TXRDY bit is set.
        move.b  %d0,MCF5206E_UTB                        | Output register d0 in the Uart transmit buffer.
        rts

uart_receive:
        move.b  MCF5206E_USR,%d1
        btst    #0,%d1
        beq     uart_receive                            | Wait until USR_RXRDY bit is set.
        move.b  MCF5206E_UTB,%d0
        rts
