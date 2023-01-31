/* OUTPUT_FORMAT(elf32-i386) */
OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    /* . = 0x1000; */
    .text : { *(.text.start) *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
    __end = .;
}
