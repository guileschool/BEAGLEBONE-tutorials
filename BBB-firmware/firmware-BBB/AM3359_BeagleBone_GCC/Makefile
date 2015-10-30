USE_THUMB_MODE=NO
# use 1 or 0 only
DEBUG=-g
OPTIM=-O0
RUN_MODE=RUN_FROM_RAM
LDSCRIPT=omap3-ram.ld
TEXT_BASE = 0x80300000

# use this to print debug messages for task interleaving
DBGMSG=NO

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
ARCH=arm-none-eabi-ar
CRT0=boot.s
WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused

#
# CFLAGS common to both the THUMB and ARM mode builds

#CFLAGS=-std=c99 $(WARNINGS) -D DEBUG -D $(RUN_MODE) -D GCC_AM335_BeagleBoard -I. -I../../Source/include \
		-I../Common/include $(DEBUG) -mcpu=cortex-a8 -T$(LDSCRIPT) -Ttext $(TEXT_BASE) \
	 $(OPTIM) -fomit-frame-pointer -fno-strict-aliasing -mno-unaligned-access
CFLAGS=$(WARNINGS) -D DEBUG -D $(RUN_MODE) -D GCC_AM335_BeagleBoard -I. -I../../Source/include \
		-I../Common/include $(DEBUG) -mcpu=cortex-a8 -T$(LDSCRIPT) -Ttext $(TEXT_BASE) \
	 $(OPTIM) -fomit-frame-pointer -fno-strict-aliasing -mno-unaligned-access

ifeq ($(USE_THUMB_MODE),YES)
	CFLAGS += -mthumb-interwork -D THUMB_INTERWORK
	THUMB_FLAGS=-mthumb
endif

LINKER_FLAGS=-Xlinker -o$(TARGET).elf -Xlinker -M -Xlinker -Map=$(TARGET).map

RTOS_SOURCE_DIR=../../Source
DEMO_SOURCE_DIR=../Common/Minimal
#
# Source files that can be built to THUMB mode.
#
THUMB_SRC = \
main.c \
serial.c \
syscalls.c

#
# Source files that must be built to ARM mode.
#
ARM_SRC = 
#
# Define all object files.
#
ARM_OBJ = $(ARM_SRC:.c=.o)
THUMB_OBJ = $(THUMB_SRC:.c=.o)

ifeq ($(USE_THUMB_MODE),YES)
	TARGET=firmware-t
else
	TARGET=firmware-a
endif

$(TARGET).bin : $(TARGET).elf
	$(OBJCOPY) $(TARGET).elf -O binary $(TARGET).bin
	cp $(TARGET).bin /tftpboot/rtosdemo.bin

$(TARGET).hex : $(TARGET).elf
	$(OBJCOPY) $(TARGET).elf -O ihex $(TARGET).hex

$(TARGET).elf : $(ARM_OBJ) $(THUMB_OBJ) $(CRT0) Makefile
	$(CC) $(CFLAGS) $(ARM_OBJ) $(THUMB_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)

$(THUMB_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm -rf *.o
	rm -rf *.elf *.hex *.map *.bin
	touch Makefile

