# µBonda

A kernel space KV storage engine for raspberry pi5 specifically

## building the project

```sh
# create a kernel8.img file for raspberry pi 5
make

# run for raspberry pi 4 in qemu
make debug
qemu-system-aarch64 -M raspi4b -kernel kernel8.img -serial stdio

# clean *.o *.elf *.img files
make clean
```
