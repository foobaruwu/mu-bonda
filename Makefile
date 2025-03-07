CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a53
GCCPATH = aarch64-none-elf-gcc

all: clean kernel8.img

boot.o: boot.S
    $(GCCPATH) $(GCCFLAGS) -c boot.S -o boot.o

%.o: %.c
    $(GCCPATH) $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
    $(GCCPATH) -nostdlib --ffreestanding -O2 boot.o $(OFILES) -T linker.ld -o kernel8.elf
    aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
    /bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true
