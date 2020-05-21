#
# GBA Makefile for homebrew programming with Tonc
#

# Check if DEVKITARM environment variable is set
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

# Check if TONCCODE environment variable is set
ifeq ($(strip $(TONCCODE)),)
$(error "Please set TONCCODE in your environment. export TONCCODE=<path to>tonc_code")
endif

# --- Project details -------------------------------------------------

# Name of the project
# Rename it to the name of the project folder!
PROJ    := rtc-demo

TARGET  := $(PROJ)

# --- Libraries -------------------------------------------------------

LIBS            := -ltonc
TONC_LIB_DIR    := $(TONCCODE)/tonclib/lib

LIBS_DIR        := -L$(TONC_LIB_DIR)

# --- Header Files ---------------------------------------------------

TONC_INC_DIR    := $(TONCCODE)/tonclib/include

INC_DIR         := -I$(TONC_INC_DIR) -Iinclude

# --- Functions ------------------------------------------------------

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# --- Source Files ----------------------------------------------------

SOURCE_DIR			:= src


C_FILES				:= $(call rwildcard,$(SOURCE_DIR),*.c)

# --- Output Files ----------------------------------------------------

OUTPUT_DIR			:= output

COBJ_FILES			:= $(C_FILES:%.c=output/%.o)

ALL_OBJ_FILES	    := $(COBJ_FILES)

# --- Bash Scripting --------------------------------------------------

SHELL				:= /bin/bash
MOVE				:= @mv
XCOPY				:= @xcopy
MKDIR				:= @mkdir -p
REMOVE				:= @rm -fv --force --recursive

# --- Build defines ---------------------------------------------------

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
	
GBAFIX	:= -@gbafix

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

CFLAGS  := $(ARCH) -O2 -Wall -fno-strict-aliasing $(INC_DIR)
LDFLAGS := $(ARCH) $(SPECS) $(LIBS_DIR) $(LIBS)

.PHONY : build clean

# --- Build -----------------------------------------------------------
# Build process starts here!
build: $(TARGET).gba

# Strip and fix header (step 3,4)
$(TARGET).gba : $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@
	$(GBAFIX) $@

# Link (step 2)
$(TARGET).elf : $(COBJ_FILES)
	$(LD) $^ $(LDFLAGS) -o $@

# Compile (step 1)
$(OUTPUT_DIR)/%.o : %.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
		
# --- Clean -----------------------------------------------------------
# Clean the project
clean : 
	$(REMOVE) *.gba
	$(REMOVE) *.elf
	$(REMOVE) output

# --- EOF -------------------------------------------------------------