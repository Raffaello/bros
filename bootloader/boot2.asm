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
.include "bootloader/GDT.asm"

main:
  # Load GDT
  lea si, gdt_msg
  call PrintString
  call LoadGDT
  lea si, ok_msg
  call PrintString

  # Enable Protected Mode
  lea si, pmode_msg
  call PrintString
  cli
  mov eax, cr0
  or eax, 1    # enable bit 0
  mov cr0, eax
  jmp 0x8:main32

.code32
main32:
  # Set Registers
  mov ax, 0x10  # set Data segment to data selector (0x10)
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov esp, 0x9000 # stack start at 9000h
  sti
  # *** test *** TODO: remove 
  mov word PTR [0xB8000], 0x074F
  mov word PTR [0xB8002], 0x074b


# stop
  cli
  hlt


gdt_msg:    .asciz "Loading GDT..."
ok_msg:     .asciz "OK\r\n"
pmode_msg:  .asciz "Enabling Protected Mode..."
# .fill 1024, 1, 1  # Pad 1K with 1-bytes to test file larger than 1 sector
.fill (512-(.-_start)), 1, 0
