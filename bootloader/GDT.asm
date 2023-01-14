# GDT

.ifndef GDT
GDT:
# NULL Descriptor
  .quad 0

# Code Descriptor
  .word 0xFFFF     # limit low
  .word 0          # base low
  .byte 0          # base middle
  .byte 0b10011010 # access
  .byte 0b11001111 # granularity
  .byte 0          # base high

# Data Descriptor
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


.func LoadGDT
LoadGDT:
  cli
  pusha
  lgdt [GDT_TOC]
  sti
  popa
  ret
.endif
