# 2nd Stage Bootloader

.code16
.intel_syntax noprefix
.text
.org 0x0

KERNEL_FILENAME_ATTRIB = 0b00000111 # System, Hidden, Read-only

.global _start

_start:
  jmp main
KERNEL_FILENAME:  .ascii "BROSKRNLSYS"
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
  mov dl, DrvNum
  call LoadRootDirectory
  # 2. find file
  lea si, KERNEL_FILENAME
  mov dl, KERNEL_FILENAME_ATTRIB
  call RootDirFindFile
  cmp AX, 0xFFFF
  jne load_fat
  lea si, file_missing_msg
  call BootFailure
  # 3. load file
load_fat:
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
  mov dword PTR [0xB8000], 0x154B154F

# stop
  cli
  hlt


file_missing_msg:   .asciz "File Missing\r\n"
preload_kernel_msg: .asciz "Preloading Kernel..."
a20_msg:            .asciz "Enabling A20..."
gdt_msg:            .asciz "Loading GDT..."
ok_msg:             .asciz "OK\r\n"
pmode_msg:          .asciz "Enabling Protected Mode and Loading Kernel..."

.fill ((_BytsPerSec*_RsvdSecCnt) -(.-_start)), 1, 0

