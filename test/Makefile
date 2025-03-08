all:
	aarch64-none-elf-gcc -c boot.s -o boot.o
	aarch64-none-elf-gcc -nostdlib -nostartfiles -ffreestanding boot.o -O2 -T linker.ld -o kernel.elf kernel.c
	aarch64-none-elf-objcopy -O binary kernel.elf kernel8.img

clean:
	rm *.o *.img *.elf > /dev/null 1> /dev/null 2> /dev/null || true
