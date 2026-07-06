OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
PAGE_SIZE  = 4096;
ALIGN_SIZE = 16;
/* TODO: remove STACK section from linker */
STACK_SIZE = PAGE_SIZE * 2; /* 8KB */
SECTIONS
{
    /* __kernel_stack_bottom = .;
    .kernel_stack : ALIGN(ALIGN_SIZE){
        . = . + STACK_SIZE;
    }
    __kernel_stack_top = .; */
    .text KERNEL_SEG : ALIGN(ALIGN_SIZE) {
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
        /* . = ALIGN(ALIGN_SIZE); */
    }
    .data : ALIGN(ALIGN_SIZE) {
        /* data = .; */
        *(.rodata)
        *(.data)
        /* . = ALIGN(ALIGN_SIZE); */
    }
    .bss  : ALIGN(ALIGN_SIZE) {
        /* bss = .; */
        *(.bss)
        *(COMMON)
        /* __stack_start = .; */
        /* . += STACK_SIZE; */
        /* __stack_end = .;  */
        /* . = ALIGN(ALIGN_SIZE); */
    }
    __end = .;
    /* __size = __end - (__code_main); */
}
