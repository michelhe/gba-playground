#
# GBA Makefile for homebrew programming with Tonc
#

# Check if DEVKITARM environment variable is set
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

# --- Project details -------------------------------------------------

# Name of the project
# Rename it to the name of the project folder!
PROJ    := redline

TARGET  := $(PROJ)

# --- Functions ------------------------------------------------------

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# --- Source Files ----------------------------------------------------

SOURCE_DIR			:= src

ASM_FILES			:= $(call rwildcard,$(SOURCE_DIR),*.S)

# --- Output Files ----------------------------------------------------

OUTPUT_DIR			:= output

ALL_OBJ_FILES	    := $(ASM_FILES:%.S=output/%.o)

# --- Bash Scripting --------------------------------------------------

SHELL				:= /bin/bash
MOVE				:= @mv
XCOPY				:= @xcopy
MKDIR				:= @mkdir -p
REMOVE				:= @rm -fv --force --recursive

# --- Build defines ---------------------------------------------------

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
AS      := $(PREFIX)as
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
	
GBAFIX	:= -@gbafix

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

CFLAGS  := $(ARCH) -O2 -Wall -fno-strict-aliasing $(INC_DIR)
LDFLAGS := $(ARCH) $(SPECS)

.PHONY : build clean

# --- Build -----------------------------------------------------------
# Build process starts here!
build: $(TARGET).gba

# Strip and fix header (step 3,4)
$(TARGET).gba : $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@
	$(GBAFIX) $@

# Link (step 2)
$(TARGET).elf : $(ALL_OBJ_FILES)
	$(LD) $^ $(LDFLAGS) -o $@

# Compile (step 1)
$(OUTPUT_DIR)/%.o : %.S
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
		
# --- Clean -----------------------------------------------------------
# Clean the project
clean : 
	$(REMOVE) *.gba
	$(REMOVE) *.elf
	$(REMOVE) output

# --- EOF -------------------------------------------------------------