# ==============================================
# Makefile para GBA_OS
# ==============================================

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -Wextra -std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -nostartfiles -mthumb -mcpu=cortex-m3
LDFLAGS = -T linker.ld -Wl,-Map,gba_os.map

SRCS = main.c \
       cpu_arm.c \
       cpu_thumb.c \
       cpu_memoria.c \
       ppu.c \
       ppu_sprites.c \
       gba_timers.c \
       gba_sintetizador.c \
       power.c

OBJS = $(SRCS:.c=.o)

TARGET = gba_os.elf
BIN = gba_os.bin

all: $(TARGET) $(BIN)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

$(BIN): $(TARGET)
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(BIN) gba_os.map

.PHONY: all clean