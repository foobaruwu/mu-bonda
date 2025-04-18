CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -O3 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a76
GCCPATH = aarch64-none-elf-gcc
DEBUGFLAGS = -DDEBUG

all: clean kernel8.img

debug: GCCFLAGS += $(DEBUGFLAGS)
debug: clean kernel8.img

boot.o: boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c boot.S -o boot.o

%.o: %.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	aarch64-none-elf-ld -nostdlib boot.o $(OFILES) -T linker.ld -o kernel8.elf
	aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img


clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true
