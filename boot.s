.global _start
.section .text
_start:
    msr DAIFSet, #0xf ; Disable Interrupts

    mov sp, #0x80000
    bl kernel_main

fails:
    wfi
    b fails