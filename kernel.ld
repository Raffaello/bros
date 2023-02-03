OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
PAGE_SIZE = 4096;
/* TODO: remove STACK section from linker */
STACK_SIZE = PAGE_SIZE * 1;
SECTIONS
{
    .text KERNEL_SEG : {
        __code_start = .;
        *(.text._start)
        *(.text._start_entry)
        *(.text._start_failure)
        __code_main = .;
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
        *(COMMON)
        __stack_start = .;
        . += STACK_SIZE;
        __stack_end = .;
        . = ALIGN(PAGE_SIZE);
    }
    __end = .;
    __size = __end - (__code_main);
}
