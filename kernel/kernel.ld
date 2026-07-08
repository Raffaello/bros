OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

PAGE_SIZE  = 4096;
ALIGN_SIZE = 16;

SECTIONS
{
    .text KERNEL_HIMEM : ALIGN(ALIGN_SIZE) {
        *(.text._start)
        *(.text._start_init)
        *(.text._start_VGA_init)
        *(.text._start_boot_info)
        *(.text._start_PMM_init)
        *(.text.main)
        *(.text)
    }
    .rodata : ALIGN(ALIGN_SIZE) {
         *(.rodata)
    }
    .data : ALIGN(ALIGN_SIZE) {
        *(.data)
    }
    .bss  : ALIGN(ALIGN_SIZE) {
        *(.bss)
        *(COMMON)
    }
    __end = .;
    /* __size = __end - (__code_main); */
}
