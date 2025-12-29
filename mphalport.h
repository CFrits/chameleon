#ifndef MICROPY_INCLUDED_CHAMELEON_MPHALPORT_H
#define MICROPY_INCLUDED_CHAMELEON_MPHALPORT_H

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#include "py/mpconfig.h"
#include "board.h"

void mp_hal_init(void);
mp_uint_t mp_hal_ticks_ms(void);
void mp_hal_set_interrupt_char(char c);

void mp_hal_delay_us(mp_uint_t us);
void mp_hal_delay_ms(mp_uint_t ms);

#endif // MICROPY_INCLUDED_CHAMELEON_MPHALPORT_H
