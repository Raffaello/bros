# 2nd Stage Bootloader
.code16
.intel_syntax noprefix
.text
.org 0x0

.global _start

_start:
  jmp main
  nop

.include "utils/bios/PrintString.asm"

main:
  lea si, hello_msg
  call PrintString
hang:
  jmp hang


hello_msg: .asciz "Hello 2nd Bootloader!\r\n"

# .fill 1024, 1, 1  # Pad 1K with 1-bytes to test file larger than 1 sector
.fill (1024-(.-_start)), 1, 1
