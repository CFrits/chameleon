        .global timer_init
        .global timer_counter
        .global timer_handler

.equ MCF5206E_SIMR,             0x10000003              | SIM Configuration Register
.equ MCF5206E_ICR9,             0x1000001C              | Interrupt Control Register 9 - Timer 1 Interrupt

.equ MCF5206E_IMR,              0x10000036              | Interrupt Mask Register

.equ MCF5206E_TMR,              0x10000100              | Timer Mode Register
.equ MCF5206E_TRR,              0x10000104              | Timer Reference Register
.equ MCF5206E_TCR,              0x10000108              | Timer Capture Register
.equ MCF5206E_TCN,              0x1000010C              | Timer Counter
.equ MCF5206E_TER,              0x10000111              | Timer Event Register

timer_init:
        move.b  #0x84,%d0                               | Set up Timer 1 interrupt
        move.b  %d0,MCF5206E_ICR9                       | Level 2 interrupt, Priority 0, Autovertor ON. Vector 24 + 1 = 25

        move.w  #0xFDFF,%d0                             | Enable Timer 1 interrupt
        move.w  %d0,MCF5206E_IMR

        # Reset the timer by causing a transition from high to low 
        # Coldfire manual page 14-5
        # RST Reset Timer
        # This bit performs a software timer reset identical to that of an external reset. All timer
        # registers takes on their corresponding reset values. While this bit is zero, the other register
        # values can still be written, if necessary. A transition of this bit from one to zero is what resets
        # the register values. The counter/timer/prescaler is not clocked unless the timer is enabled.
        move.w  #0x0001,%d0                             | Enable
        move.w  %d0,MCF5206E_TMR
        move.w  #0x0000,%d0                             | Disable
        move.w  %d0,MCF5206E_TMR

        # Is this needed ?
        move.w  #0x0001,%d0                             | Enable
        move.w  %d0,MCF5206E_TMR

        move.w  #0xFA3D,%d0                             | Setup the Timer mode register (TMR1)
                                                        | Bits 15:8 sets the prescale to 250 ($FA)
                                                        | Bits  7:6 set for no interrupt ("00")
                                                        | Bit     5 sets output mode for "toggle".
                                                        | Bit     4 Enable interrupt upon reaching the reference value
                                                        | Bits    3 set for "restart" ("1")
                                                        | Bits  2:1 Input Clock Source for the Timer set the source to system clock/16 ("10")
                                                        | Bit     1 enables/disables the timer ("1")

        move.w  %d0,MCF5206E_TMR                        | 

        | 16*250/40.000000 = 0.0001 s

        move.w  #10000,%d0                              | Setup the Timer reference.
        move.w  %d0,MCF5206E_TRR

        move.w  #0x0000,%d0                             | writing to the timer counter with any value resets it to zero.
        move.w  %d0,MCF5206E_TCN

        move.w  MCF5206E_TMR,%d0                        | Enable Timer 
        ori.l   #0x1,%d0
        move.w  %d0,MCF5206E_TMR
        rts

timer_counter:
        move.w  MCF5206E_TCN,%d0                        | Return the current counter value.
        rts

timer_handler:
        lea     -60(%sp),%sp                            | Make room on stack for registers
        movem.l %d0-%d7/%a0-%a6,(%sp)                   | save registers on the stack

        lea     MCF5206E_TER,%a0                        | Clear Timer Event Register 1 by writing 
        move.b  #0x03,(%a0)                             | ones to bits 0:1.

        jsr     timer_isr                               | Call C code.
        movem.l (%sp),%d0-%d7/%a0-%a6
        lea     60(%sp),%sp                             | Restore stack register.
        rte
