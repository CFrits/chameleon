include ../../py/mkenv.mk

CROSS = 0

# Qstr definitions (must come before including py.mk)
QSTR_DEFS =
QSTR_GLOBAL_DEPENDENCIES = mpconfigport.h

FROZEN_MANIFEST ?=

# Include py core make definitions
include $(TOP)/py/py.mk

CC         := /opt/rtems6/bin/m68k-rtems6-gcc
AS         := /opt/rtems6/bin/m68k-rtems6-as
AR         := /opt/rtems6/bin/m68k-rtems6-ar
LD         := /opt/rtems6/bin/m68k-rtems6-ld
STRIP      := /opt/rtems6/bin/m68k-rtems6-strip
SIZE       := /opt/rtems6/bin/m68k-rtems6-size
OBJCOPY    := /opt/rtems6/bin/m68k-rtems6-objcopy

DEFINES    :=  -D__mc68000__ -D__BIG_ENDIAN__ -D__m68k__ -D__rtems__ -D__mot68

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS  += $(INC)
CFLAGS  += -m5206e -mdiv -msoft-float -malign-int -fno-hosted -fno-builtin $(DEFINES)
CFLAGS  += -Os -fomit-frame-pointer -DNDEBUG -DCHAMELEON -Wall

AFLAGS  += -m5206e

LDFLAGS += -m5200 -nostdlib -nostartfiles -ffreestanding -Wl,--build-id=none -nostartfiles -Wl,-n -Wl,-Map,chameleon.map -Wl,-Tlinker.ld
LDFLAGS += -Wl,-O,1 -Wl,--gc-sections

LIBS += -lm

SRC_C = \
	main.c \
	cham_mphal.c \
	panel.c \
	midi.c \
	shared/libc/string0.c \
	shared/libc/printf.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \

SRC_AS = \
	start.s \
	uartasm.s \
	midiasm.s \

SRC_QSTR += $(SRC_C)

OBJ += $(addprefix $(BUILD)/, $(SRC_AS:.s=.o))
OBJ += $(PY_O)
OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

all: $(BUILD)/firmware.elf

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) -Wl,-u,start $(LDFLAGS) -o $@  $^ $(LIBS) -lgcc
	$(Q)$(STRIP) $@
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O ihex -R .eeprom $< $@

include $(TOP)/py/mkrules.mk
QSTR_GEN_CFLAGS += -E
