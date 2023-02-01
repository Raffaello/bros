OUTPUT_FORMAT(elf32-i386)
/* OUTPUT_FORMAT(binary) */
ENTRY(_start)
PAGE_SIZE = 4096;
SECTIONS
{
    /* . = KERNEL_SEG; */
    .text KERNEL_SEG : {
        /* code = .; */
        *(.text._start)
        *(.text._start_failure)
        code_main = .;
        *(.text.main)
        *(.text)
        *(.rodata)
        . = ALIGN(PAGE_SIZE);
    }
    .data : {
        /* data = .; */
        *(.data)
        . = ALIGN(PAGE_SIZE);
    }
    .bss  : {
        /* bss = .; */
        *(.bss)
        . = ALIGN(PAGE_SIZE);
    }
    __end = .;
    __size = __end - (code_main);
}
