        .global _start
        .global uart_init
        .global uart_transmit

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

        bsr     uart_init                               | Initialize the uart.

        move.b  #0x04,%d0                               | Transmit lenght
        bsr     uart_transmit
        move.b  #0x81,%d0                               | Transmit SERIAL_LINK_END command
        bsr     uart_transmit
        move.b  #0x03,%d0                               | Transmit TRACE command
        bsr     uart_transmit
        move.b  #'>',%d0                                | Transmit prompt.
        bsr     uart_transmit
        halt

uart_init:
        move.b  #5,%d0
        move.b  %d0,MCF5206E_UCR                        | Enable Transmitter and Receiver.
        rts

# The Uart transmit functions outputs the content of register d0.
uart_transmit:
uart_wait:
        move.b  MCF5206E_USR,%d1
        btst    #2,%d1
        beq     uart_wait                               | Wait until USR_TXRDY bit is set.
        move.b  %d0,MCF5206E_UTB                        | Output register d0 in the Uart transmit buffer.
        rts
