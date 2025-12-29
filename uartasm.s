        .global uart_init
        .global uart_transmit
        .global uart_receive
        .global uart2_handler

.equ MCF5206E_ICR13,            0x10000020              | Interrupt Control Register 13 - Uart 2 Interrupt
.equ MCF5206E_IMR,              0x10000036              | Interrupt Mask Register


# Addresses of the second Uart interface of the Coldfire. 
# On the Chameleon these are connected to the RS232 interface.
.equ MCF5206E_UMR,              0x10000180              | Mode Register UMR1 and UMR2
.equ MCF5206E_USR,              0x10000184              | UART Status Register when reading. Clock Select Register when writing.
.equ MCF5206E_UCR,              0x10000188              | UART Command Register
.equ MCF5206E_UTB,              0x1000018C              | UART Transmit Buffer (write only)
.equ MCF5206E_UISR,             0x10000194              | Interrupt Status Register when reading.
.equ MCF5206E_UIMR,             0x10000194              | Interrupt Mask Register when writing.
.equ MCF5206E_UIVR,             0x100001B0              | Interrupt Vector Register.

uart2_handler:
        lea     -60(%sp),%sp                            | Make room on stack for registers
        movem.l %d0-%d7/%a0-%a6,(%sp)                   | save registers on the stack

        jsr     uart2_isr

        movem.l (%sp),%d0-%d7/%a0-%a6
        lea     60(%sp),%sp                             | Restore stack register.

        rte

# 
# The Autovector of Uart 2 is determined by Interrupt Control Register 13
#
uart_init:
        move.b  #0x04,%d0                               | Setup Uart 2 interrupt
                                                        | Bit     7 Autovector Enable = 0
                                                        | Bit   4:2 Interupt level = 1
                                                        | Bit   0:1 Interupt priority = 0
        move.b  %d0,MCF5206E_ICR13                      | Level 1 interrupt, Priority 0, Autovector OFF. Vector 24

        move.b  #24,%d0
        move.b  %d0,MCF5206E_UIVR                       | Set interrupt vector to 24.

        move.b  #0x10,%d0
        move.b  %d0,MCF5206E_UCR                        | Reset Mode Register. The Mode Register Ptr now points to UMR1

        move.b  #0x13,%d0
                                                        | Bit     7 RxRTS
                                                        | Bit     6 RxIRQ Receiver Interrupt Select
                                                        |           1 = FFULL is the source that generated an interrupt
                                                        |           0 = RxRDY is the source that generated an interrupt
                                                        | Bit   4:3 Parity mode "10" is no parity
                                                        | Bit   1:0 Bits per character "11" is 8 bits.
        move.b  %d0,MCF5206E_UMR                        | User Mode Register1.

        move.b  #0x07,%d0                               | Normal mode.
        move.b  %d0,MCF5206E_UMR                        | User Mode Register2.

        move.b  #2,%d0
        move.b  %d0,MCF5206E_UIMR                       | Enable Interrupt FFULL (Fifo full).

        move.b  #5,%d0
        move.b  %d0,MCF5206E_UCR                        | Enable Transmitter and Receiver.

        move.w  MCF5206E_IMR,%d0                        | Read current IMR value.
        andi.l  #0xDFFF,%d0                             | Enable Uart 2 interrupt
        move.w  %d0,MCF5206E_IMR

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
