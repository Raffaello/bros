/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text
GDT:
# NULL Descriptor
  .quad 0

# Code Descriptor
GDT_CODE:
  .word 0xFFFF     # size
  .word 0          # base low
  .byte 0          # base middle
  .byte 0b10011010 # access
  .byte 0b11001111 # limit / granularity
  .byte 0          # base high

# Data Descriptor
GDT_DATA:
  .word 0xFFFF      # size (Same as code)
  .word 0           # base low
  .byte 0           # base middle
  .byte 0b10010010  # access
  .byte 0b11001111  # limit / granularity
  .byte 0           # base high
GDT_END:
GDT_TOC:
  .word GDT_END - GDT - 1   # Size of GDT
  .long GDT                 # base of GDT

# Constants for the GDT segment descriptor offsets.
# For example:
# when set DS = 0x10 in PMode, the CPU knows to use the
# segment described at offset 0x10 ( i.e. 16 bytes ) in GDT,
# which is the DATA segment (0x0 -> NULL ; 0x08 -> CODE ; 0x10 -> DATA )
.equ GDT_CODE_SEG, GDT_CODE - GDT
.equ GDT_DATA_SEG, GDT_DATA - GDT

.globl GDT_load_asm
GDT_load_asm:
  cli
  sgdt [GDT_TOC]
  lgdt [GDT_TOC]
reloadSegments:
   # Reload CS register containing code selector:
   JMP   GDT_CODE_SEG:reload_CS # 0x08 is a stand-in for your code segment
reload_CS:
   # Reload data segment registers:
   MOV   AX, GDT_DATA_SEG # 0x10 is a stand-in for your data segment
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX
  ret
.type GDT_load_asm, @function
.size GDT_load_asm, .-GDT_load_asm
