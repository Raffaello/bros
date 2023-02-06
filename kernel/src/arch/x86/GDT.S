/* -*-Asm-*- */

#include <arch/x86/defs/asm.h>

.code32
.intel_syntax noprefix
.text

GDT:
# NULL Descriptor
  .quad 0

# Code Descriptor
GDT_KERNEL_CODE:
  .word 0xFFFF     # segment limit
  .word 0          # base low
  .byte 0          # base middle
  .byte 0b10011010 # P,DPL=0,11,non-conforming,readable
  .byte 0b11001111 # Granularity, Default / limit 4 lower bits
  .byte 0          # base high

# Data Descriptor
GDT_KERNEL_DATA:
  .word 0xFFFF      # size (Same as code)
  .word 0           # base low
  .byte 0           # base middle
  .byte 0b10010010  # P,DPL=-,10,E,W
  .byte 0b11001111  # granularity / limit
  .byte 0           # base high
GDT_USER_CODE:
    .word 0xFFFF
    .word 0
    .byte 0
    .byte 0b11111010
    .byte 0b11001111
    .byte 0
GDT_USER_DATA:
    .word 0xFFFF
    .word 0
    .word 0
    .byte 0
    .byte 0b11110010
    .byte 0b11001111
    .byte 0
# TASK_STATE_SEGMENT:
GDT_END:
GDT_TOC:
  .word GDT_END - GDT - 1   # Size of GDT
  .long GDT                 # base of GDT

# Constants for the GDT segment descriptor offsets.
# For example:
# when set DS = 0x10 in PMode, the CPU knows to use the
# segment described at offset 0x10 ( i.e. 16 bytes ) in GDT,
# which is the DATA segment (0x0 -> NULL ; 0x08 -> CODE ; 0x10 -> DATA )
.equ GDT_KERNEL_CODE_SEG, GDT_KERNEL_CODE - GDT
.equ GDT_KERNEL_DATA_SEG, GDT_KERNEL_DATA - GDT

# ASM_BEGIN(GDT_load_asm)
ASM_BEGIN(GDT_init)
  cli
  lgdt [GDT_TOC]
reloadSegments:
   # Reload CS register containing code selector:
   JMP   GDT_KERNEL_CODE_SEG:reload_CS
reload_CS:
  # Reload data segment registers:
  MOV   AX, GDT_KERNEL_DATA_SEG
  MOV   DS, AX
  MOV   ES, AX
  MOV   FS, AX
  MOV   GS, AX
  MOV   SS, AX
  ret
# ASM_END(GDT_load_asm)
ASM_END(GDT_init)

# ASM_BEGIN(GDT_reload_segments)
#     jmp 0x8:GDT_reload_segments_CS
# GDT_reload_segments_CS:
#     mov ax, 0x10 # data segment
#     mov ds, ax
#     mov es, ax
#     mov fs, ax
#     mov gs, ax
#     mov ss, ax
#     ret
# ASM_END(GDT_reload_segments)
