        .global _start
        .global setjmp
        .global longjmp
        .global _setjmp
        .global _longjmp
        .global enable_irq
        .global disable_irq
        .global disable_interrupts
        .global enable_interrupts

.equ MBAR_START,                0x10000000

.equ SRAM_START,                0x20000000
.equ SRAM_END,                  0x20002000

.equ VECTOR_START,              0x20000000
.equ VECTOR_END,                0x20000400

        .short  0x0000
        .long   0xBCBBBABD                              | Some magic number
        .long   0x000500A0                              | Build nummer and modelid
        .long   0x00018723

_start:
        move.w  #0x3700,%sr                             | Supervisor mode, interrupts off.

init:
        movea.l #MBAR_START,%a0                         | Load Memory Base Address Register.
        move.w  #0xFFFE,%d0                             | 
        move.w  %d0,%a0@(54)                            | Interrupt Mask Register
        move.w   #0x0100,%d0
        move.w   %d0,%a0@(198)                          | Default Memory Control Register
        move.w   #0,%d0
        move.w   %d0,%a0@(100)                          | Chip Select Address Register
        moveq   #0,%d0
        move.l   %d0,%a0@(104)                          | Chip Select Mask Register
        move.w   #0,%d0
        move.w   %d0,%a0@(110)                          | Chip Select Control Register

|
| Clear RAM
|
_clear_sram:
        movea.l  #SRAM_START,%a0                        | start of .bss
        movea.l  #SRAM_END,%a1                          | end of .bss
        clrl    %d0

_clear_sram_loop:
        cmpa.l  %a0,%a1
        beq.s   _clear_sram_end
        move.l  %d0,%a0@+                               | to zero out uninitialized
        bra.s   _clear_sram_loop
_clear_sram_end:

        movea.l #VECTOR_START,%a0
        movea.l #VECTOR_END,%a1
        move.l  #default_handler,%d0                    | Default interrupt handlers

|
| Clear Vectors. Set all vectors to the default handler.
|
_clear_vector_loop:
        move.l  %d0,%a0@+
        cmpa.l  %a0,%a1
        bne.s   _clear_vector_loop

        movea.l #VECTOR_START,%a0
        movec   %a0,%vbr                                | Set Interrupt Vector table location

|
| Clear BSS.
|
_clear_bss:
        movea.l #BSS_START,%a0                          | start of .bss
        movea.l #BSS_END,%a1                            | end of .bss
        clr.l   %d0

_clear_bss_loop:
        cmpa.l  %a0,%a1
        beq.s    _clear_bss_end
        move.l  %d0,%a0@+                               | to zero out uninitialized
        bra.s   _clear_bss_loop
_clear_bss_end:

#
# Initialisation of interrupt vectors.
#
# 01: Initial PC
.equ VECTOR_RESET,              0x20000004
        movea.l #VECTOR_RESET,%a0
        move.l  #_start,(%a0)                           | Hard coded breakpoint handler.

# 09: Trace (9 * 4 = 36 = 0x24)
.equ VECTOR_TRACE,              0x20000024              | Trace vector.
        movea.l #VECTOR_TRACE,%a0
        move.l  #default_handler,(%a0)                  | Hard coded breakpoint handler.

# Autovector 0 ((24 + 0) * 4 = 96 = 0x60)
.equ VECTOR_UART1,              0x20000060              | Set vector for interrupt for Uart 1.
        movea.l #VECTOR_UART1,%a0
        move.l  #uart1_handler,(%a0)                    | Hard coded breakpoint handler.

# Autovector 1 ((24 + 1) * 4 = 100 = 0x64)
.equ VECTOR_UART2,              0x20000064              | Set vector for interrupt for Uart 2.
        movea.l #VECTOR_UART2,%a0
        move.l  #uart2_handler,(%a0)                    | Hard coded breakpoint handler.

        movea.l #SRAM_END,%sp                           | Set the stack to the end of ram.

        jsr     main
        halt

bad:
        bra.s   bad

# Interrupt functions used by micropython.
disable_irq:
        move.w  %sr,%d1                                 | Read status register
        move.w  %d1,%d0                                 | Make a copy to return to C.
        ori.l   #0x0700,%d1                             | Set interrupt bits
        move.w  %d1,%sr                                 | Set status register with interrupts disabled.
        rts                                             | Return old interrupt bits in d0.

enable_irq:
        move.l  4(%sp),%d0                              | Get the argument from the stack frame. When called from C.
        move.w  %d0,%sr
        rts

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

setjmp:
_setjmp:
        move.l  4(%sp),%a0                              | jmp_buf
        move.l  (%sp),(%a0)+                            | save setjmp() return address
        movem.l %d2-%d7/%a2-%a7,(%a0)                   | save setjmp() caller registers
        moveq   #0,%d0                                  | return from setjmp()
        rts

longjmp:
_longjmp:
        move.l  4(%sp),%a0                              | jmp_buf
        move.l  8(%sp),%d0                              | requested setjmp() return value
        bne.s   nonzero
        moveq   #1,%d0                                  | if was 0, force it to 1
nonzero:
        move.l  (%a0)+,%d1                              | get setjmp() return address
        movem.l (%a0),%d2-%d7/%a2-%a7                   | restore setjmp() caller registers
        move.l  %d1,(%sp)                               | force return address
        rts                                             | return to setjmp() caller

default_handler:
        move.l %sp@+,%d0
        move.l %sp@+,%d1
        rte
