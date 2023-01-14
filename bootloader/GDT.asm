# GDT

.ifndef GDT
GDT:
# NULL Descriptor
  .quad 0

# Code Descriptor
GDT_CODE:
  .word 0xFFFF     # limit low
  .word 0          # base low
  .byte 0          # base middle
  .byte 0b10011010 # access
  .byte 0b11001111 # granularity
  .byte 0          # base high

# Data Descriptor
GDT_DATA:
  .word 0x0FFFF     # limit low (Same as code)
  .word 0           # base low
  .byte 0           # base middle
  .byte 0b10010010  # access
  .byte 0b11001111  # granularity
  .byte 0           # base high
GDT_END:
GDT_TOC:
  .word GDT_END - GDT - 1   # limit (Size of GDT)
  .long GDT               # base of GDT

# Constants for the GDT segment descriptor offsets.
# For example:
# when set DS = 0x10 in PMode, the CPU knows to use the
# segment described at offset 0x10 ( i.e. 16 bytes ) in GDT,
# which is the DATA segment (0x0 -> NULL ; 0x08 -> CODE ; 0x10 -> DATA )
.equ GDT_CODE_SEG, GDT_CODE - GDT
.equ GDT_DATA_SEG, GDT_DATA - GDT


.func LoadGDT
LoadGDT:
  cli
  pusha
  lgdt [GDT_TOC]
  # sti
  popa
  ret
.endfunc
.endif
