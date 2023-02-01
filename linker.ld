OUTPUT_FORMAT(elf32-i386)
/* OUTPUT_FORMAT(binary) */
ENTRY(_start)
PAGE_SIZE = 4096;
SECTIONS
{
    . = KERNEL_SEG;
    .text : AT(KERNEL_SEG){
        code = .;
        *(.text._start)
        *(.text)
        *(.rodata)
        . = ALIGN(PAGE_SIZE);
    }
    .data : AT(KERNEL_SEG + data - code) {
        data = .;
        *(.data)
        . = ALIGN(PAGE_SIZE);
    }
    .bss  : AT(KERNEL_SEG + bss - code) {
        bss = .;
        *(.bss)
        . = ALIGN(PAGE_SIZE);
    }
    __end = .;
}
