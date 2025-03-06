ENTRY(kernel_main)

SECTIONS {
    . = 0x80000; /* Typical kernel load address for Raspberry Pi */

    .text : {
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        bss_start = .;
        *(.bss)
        *(COMMON)
        bss_end = .;
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment)
    }
}

PROVIDE(end = .);
