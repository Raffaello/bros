OUTPUT_FORMAT(elf32-i386)
ENTRY(_start_entry)
PAGE_SIZE = 4096;
STACK_SIZE = PAGE_SIZE * 1;
SECTIONS
{
    .text KERNEL_SEG : {
        /* code = .; */
        *(.text._start)
        *(.text._start_entry)
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
        __stack_start = .;
        . += STACK_SIZE;
        __stack_end = .;
        . = ALIGN(PAGE_SIZE);
    }
    __end = .;
    __size = __end - (code_main);
}
