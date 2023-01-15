# 2nd Stage Bootloader

.code16
.intel_syntax noprefix
.text
.org 0x0

.global _start

_start:
  jmp main
DrvNum:      .byte  0
  # nop

.include "bios/PrintString.asm"
.include "GDT.asm"
.include "utils/GateA20.asm"
.include "filesystems/FAT12.asm"
.include "bios/PrintNumber.asm"

main:
  # in AL has been passed the DrvNum, storing it
  mov DrvNum, al
  # Preloading Kernel
  lea si, preload_kernel_msg
  call PrintString
  # TODO
  # 1. read FAT root dir
  # 2. find file
  # 3. load file
  

  lea si, ok_msg
  call PrintString
  # Enable Gate A20
  lea si, a20_msg
  call PrintString
  call CheckGateA20
  # cmp ax,0
  jne a20_enabled
  call EnableA20
a20_enabled:
  lea si, ok_msg
  call PrintString

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
  jmp GDT_CODE_SEG:main32

.code32
main32:
  # Set Registers
  mov ax, GDT_DATA_SEG  # set Data segment to data selector (0x10)
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov esp, 0x9000 # stack start at 9000h
  # sti
 
 # *** test *** TODO: remove 
  mov dword PTR [0xB8000], 0x074B074F

# stop
  cli
  hlt


preload_kernel_msg: .asciz "Preloading Kernel..."
a20_msg:            .asciz "Enabling A20..."
gdt_msg:            .asciz "Loading GDT..."
ok_msg:             .asciz "OK\r\n"
pmode_msg:          .asciz "Enabling Protected Mode and Loading Kernel..."
# .fill 1024, 1, 1  # Pad 1K with 1-bytes to test file larger than 1 sector
.fill (512-(.-_start)), 1, 0
