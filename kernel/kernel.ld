OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

PAGE_SIZE  = 4096;
ALIGN_SIZE = 16;

PHDRS {
    text PT_LOAD FLAGS(0x5); /* R + X */
    data PT_LOAD FLAGS(0x6); /* R + W */
}

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
    } : text
    .rodata : ALIGN(ALIGN_SIZE) {
         *(.rodata)
    } : text
    .data : ALIGN(ALIGN_SIZE) {
        *(.data)
    } : data
    .bss  : ALIGN(ALIGN_SIZE) {
        *(.bss)
        *(COMMON)
    } : data
    __end = .;
    /* __size = __end - (__code_main); */
}
