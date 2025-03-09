.global _start
.section .text
_start:
    msr DAIFSet, #0xf 

    mov sp, #0x8000
    bl kernel_main

fails:
    wfi
    b fails
