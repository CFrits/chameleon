APP = firmware.elf

OUTDIR = out

DEFINES    :=  -D__mc68000__ -D__BIG_ENDIAN__ -D__m68k__ -D__rtems__ -D__mot68 $(CCDEFINES)
INCLUDES   := $(addprefix -I, $(CCINCLUDES))

SHELL      := sh
ECHO       := echo
RM         := rm -f
SED        := sed
MKDIR      := mkdir -p

CC         := m68k-rtems6-gcc
CPP        := m68k-rtems6-g++
AS         := m68k-rtems6-as
AR         := m68k-rtems6-ar
LD         := m68k-rtems6-ld
STRIP      := m68k-rtems6-strip
OBJCOPY    := m68k-rtems6-objcopy

# -s Remove all symbol table and relocation information from the executable.
CFLAGS     := -m5206e -mdiv -msoft-float -malign-int -Wall -fno-hosted -fno-builtin $(DEFINES) $(INCLUDES)
ASFLAGS    := -m5206e $(ASDEFINES)
LDFLAGS    := -m5200 -nostartfiles -Wl,-n
ARFLAGS    := -rc

ifeq ($(findstring debug, $(MAKECMDGOALS)), debug)
  CFLAGS   += -DDEBUG -DCHAMELEON
  ASFLAGS  += --defsym DEBUG=1
else
  CFLAGS   += -O4 -fomit-frame-pointer -DNDEBUG -DCHAMELEON
  LDFLAGS  += -Wl,-O,1 -Wl,--gc-sections
endif

OBJBASE = .

OBJDIR    = $(OBJBASE)/$(OUTDIR)

SRC_CC   ?= $(wildcard *.c)
SRC_CPP  ?= $(wildcard *.cpp)
SRC_AS   ?= $(wildcard *.s)

OBJ_CC   = $(patsubst %.c, %.o, $(addprefix $(OBJDIR)/, $(notdir $(SRC_CC))))
OBJ_CPP  = $(patsubst %.cpp, %.o, $(addprefix $(OBJDIR)/, $(notdir $(SRC_CPP))))
OBJ_AS   = $(patsubst %.s, %.o, $(addprefix $(OBJDIR)/, $(notdir $(SRC_AS))))

SRC_FILE = "$(addprefix $(CURDIR)/, $(notdir $<))"
DST_FILE = "$@"

.PHONY  : all
all	:
	$(MAKE) debug

ifneq ($(findstring clean, $(MAKECMDGOALS)), clean)
  COMMON = common
  .PHONY  : $(COMMON)
  $(COMMON) : $(OBJDIR) $(EXTRAGOALS) $(OBJ_CC) $(OBJ_CPP) $(OBJ_AS) $(OBJDIR)/$(APP) ;
	@echo "Application \"$(APP)\" built successfully."
endif

.PHONY  : release
release : $(COMMON);

.PHONY  : debug
debug	: $(COMMON);

.PHONY  : clean
clean	:
	@echo "Removing output files in $(OUTDIR) ..."
	-$(RM) -r $(OUTDIR)

#------------------------------------------------------------------------------

$(OBJDIR)/%.o : %.c
	@echo "Compiling \"$(SRC_FILE)\"..."
	$(CC) $(CFLAGS) $(SRC_FILE) -c -o $(DST_FILE)

$(OBJDIR)/%.o : %.cpp
	@echo "Compiling \"$(SRC_FILE)\"..."
	$(CPP) $(CFLAGS) $(SRC_FILE) -c -o $(DST_FILE)

$(OBJDIR)/%.o : %.s
	@echo "Assembling \"$(SRC_FILE)\"..."
	$(AS) $(ASFLAGS) -o $(DST_FILE) $(SRC_FILE)

$(OBJDIR)/%.d: %.c
	-$(MKDIR) $(OBJDIR)
	@echo "Generating dependencies for \"$(CURDIR)/$<\"..."
	$(CC) -M -MG $(CFLAGS) $< | $(SED) 's@^\(.*\)\.o:@$(OBJDIR)/\1.d $(OBJDIR)/\1.o:@' > $@

$(OBJDIR)/%.d: %.cpp
	-$(MKDIR) $(OBJDIR)
	@echo "Generating dependencies for \"$(CURDIR)/$<\"..."
	$(CPP) -M -MG $(CFLAGS) $< | $(SED) 's@^\(.*\)\.o:@$(OBJDIR)/\1.d $(OBJDIR)/\1.o:@' > $@

$(OBJDIR)/%.d: %.s
	-$(MKDIR) $(OBJDIR)
	@echo "Generating dependencies for \"$(CURDIR)/$<\"..."
	$(AS) -MD tmp $(ASFLAGS) -o $(<:.s=.o) $<
	-$(RM) $(<:.s=.o)
	$(SED) -e 's@^\(.*\)\.o:@$(OBJDIR)/\1.d $(OBJDIR)/\1.o:@' tmp > $@
	-$(RM) tmp

#------------------------------------------------------------------------------

ifneq ($(findstring clean, $(MAKECMDGOALS)), clean)
  ifneq ($(findstring rebuild, $(MAKECMDGOALS)), rebuild)
    -include $(OBJ_CC:.o=.d) $(OBJ_CPP:.o=.d) $(OBJ_AS:.o=.d)
  endif
endif

#------------------------------------------------------------------------------

$(OBJDIR):
	$(MKDIR) $@

LD_SCRIPT  := linker.ld

$(OBJDIR)/$(APP) : $(OBJ_AS) $(OBJ_CC) $(OBJ_CPP) $(addprefix $(OBJBASE)/, $(EXTRAOBJS))
	$(CC) -o $(DST_FILE) -nostdlib -nostartfiles -ffreestanding -Wl,--build-id=none -Wl,--cref -Wl,-Map,$(basename $@).map -Wl,-T$(LD_SCRIPT) -Wl,-u,start $(LDFLAGS) $^ -lgcc
	$(OBJCOPY) -O binary $(OBJDIR)/$(APP) $(OBJDIR)/firmware.bin
