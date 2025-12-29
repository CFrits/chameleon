#include <stdint.h>

// Use the minimal configuration.
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_MINIMUM)

#define MICROPY_BANNER_NAME_AND_VERSION         "ChamPython v0.1 based on MicroPython v1.27.0"
#define MICROPY_BANNER_MACHINE                  "Chameleon coldfire"

// MicroPython configuration.
#define MICROPY_ENABLE_COMPILER                 (1)
#define MICROPY_ENABLE_GC                       (1)

#define MICROPY_HELPER_REPL                     (1)

// Type definitions for the specific machine
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

#include <alloca.h>

#define MP_STATE_PORT MP_STATE_VM

