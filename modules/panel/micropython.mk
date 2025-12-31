PANEL_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD_C += $(PANEL_MOD_DIR)/lcdmodule.c
SRC_USERMOD_C += $(PANEL_MOD_DIR)/ledmodule.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(PANEL_MOD_DIR)
