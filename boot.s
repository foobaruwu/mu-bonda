.global _start
.section .text

.align 8
core_stacks:
    .quad 0x80000
    .quad 0x90000
    .quad 0xA0000
    .quad 0xB0000

core_entry_points:
    .quad primary_entry      
    .quad secondary_entry    
    .quad secondary_entry    
    .quad secondary_entry    

_start:
    mrs x0, mpidr_el1
    and x0, x0, #3
    
    lsl x1, x0, #3          
    adr x2, core_stacks
    add x2, x2, x1
    ldr x1, [x2]
    mov sp, x1
    
    lsl x1, x0, #3
    adr x2, core_entry_points
    add x2, x2, x1
    ldr x1, [x2]
    br x1

primary_entry:
    msr DAIFSet, #0xf 
    bl kernel_main
    b halt

secondary_entry:
    bl secondary_main
    b halt

halt:
    wfi
    b halt
