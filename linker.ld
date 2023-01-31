/* OUTPUT_FORMAT(elf32-i386) */
OUTPUT_FORMAT(binary)
ENTRY(_start)
PAGE_SIZE = 4096;
SECTIONS
{
    /* . = 0x1000; */
    .text : /*AT(0x1000)*/ {
        code = .;
        /* *(.text.start) */
        *(.text)
        *(.rodata)
        /* . = ALIGN(PAGE_SIZE); */
    }
    .data : /*AT(data - code)*/ {
        data = .;
        *(.data)
        /* . = ALIGN(PAGE_SIZE); */
    }
    .bss  : /*AT(bss - code)*/ {
        bss = .;
        *(.bss)
        /* . = ALIGN(PAGE_SIZE); */
    }
    __end = .;
}
