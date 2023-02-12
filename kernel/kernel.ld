OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
PAGE_SIZE = 4096;
/* TODO: remove STACK section from linker */
STACK_SIZE = PAGE_SIZE * 1;
SECTIONS
{
    .text KERNEL_SEG : ALIGN(PAGE_SIZE) {
        /* __code_start = .; */
        *(.text._start)
        *(.text._start_init)
        *(.text._start_VGA_init)
        *(.text._start_boot_info)
        *(.text._start_PMM_init)
        /* . = ALIGN(16); */
        /* __code_main = .; */
        *(.text.main)
        *(.text)
        /* . = ALIGN(PAGE_SIZE); */
    }
    .data : ALIGN(PAGE_SIZE) {
        /* data = .; */
        *(.rodata)
        *(.data)
        /* . = ALIGN(PAGE_SIZE); */
    }
    .bss  : ALIGN(PAGE_SIZE) {
        /* bss = .; */
        *(.bss)
        *(COMMON)
        /* __stack_start = .; */
        /* . += STACK_SIZE; */
        /* __stack_end = .;  */
        /* . = ALIGN(PAGE_SIZE); */
    }
    __end = .;
    /* __size = __end - (__code_main); */
}
