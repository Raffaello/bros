# 2nd Stage Bootloader

.code16
.intel_syntax noprefix
.text
.org 0x0

KERNEL_FILENAME_ATTRIB  = 0b00000111 # System, Hidden, Read-only
KERNEL_SEG              = 0x1000   # where to load the kernel
FAT_BUFFER_SEG          = 0x600    # where to store the values for the FAT Cluster linked list

.global _start

_start:
  jmp main
KERNEL_FILENAME:  .ascii "BROSKRNLSYS"
DrvNum:           .byte  0
  # nop

.include "bios/PrintStringNewLine.asm"
.include "bios/PrintStringDots.asm"
.include "GDT.asm"
.include "utils/GateA20.asm"
.include "filesystems/FAT12.asm"
.include "bios/PrintNumber.asm"

main:
  # in AL has been passed the DrvNum, storing it
  mov DrvNum, al
  # Loading Kernel...
  
  # 1. read FAT Root Dir
  lea si, root_dir_msg
  call PrintStringDots
  mov dl, DrvNum
  call LoadRootDirectory
  lea si, ok_msg
  call PrintStringNewLine
  
  # 2. find kernel file
  lea si, find_kernel_file_msg
  call PrintStringDots
  lea si, KERNEL_FILENAME
  mov dl, KERNEL_FILENAME_ATTRIB
  call RootDirFindFile
  cmp AX, 0xFFFF
  jne load_fat
  lea si, file_missing_msg
  call BootFailure
load_fat:
  # push ax   # Store Kernel RootDir Index AL
  push bx   # Store NumCluster
  # push cx   # Store Hi 16 bits FileSize
  # push dx   # Store lo 16 bits FileSize
  # --- #
  lea si, ok_msg
  call PrintStringNewLine
  
  # 3. load FAT
  lea si, load_fat_msg
  call PrintStringDots
  call LoadFAT
  # here we have FAT into FAT_SEG memory
  lea si, ok_msg
  call PrintStringNewLine

  # 4. load file
  lea si, load_kernel_file_msg
  call PrintStringDots

  # pop dx  # dx
  # pop cx  # cx
  pop ax  # bx (Numluster)
  # pop ax  # ax Restore Kernel RootDir Index
  # -- #
  mov bx, KERNEL_SEG
  mov dl, DrvNum
  mov di, FAT_BUFFER_SEG
  call LoadFile

  lea si, ok_msg
  call PrintStringNewLine

  # Enable Gate A20
  lea si, a20_msg
  call PrintStringDots
  call CheckGateA20
  jne a20_enabled
  call EnableA20
a20_enabled:
  lea si, ok_msg
  call PrintStringNewLine

  # Load GDT
  lea si, gdt_msg
  call PrintStringDots
  call LoadGDT
  lea si, ok_msg
  call PrintStringNewLine

  # Enable Protected Mode
  lea si, pmode_msg
  call PrintStringDots
  call PrintNewLine
  lea si, press_a_key_msg
  call PrintStringNewLine
  call WaitKey
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
  mov esp, 0x9000       # stack start at 9000h
  # sti                 # should it be enabled by the kernel?
  call GDT_CODE_SEG:KERNEL_SEG

main32_stop:
  cli
  hlt
  jmp main32_stop


root_dir_msg:         .asciz "Loading Root Dir"
find_kernel_file_msg: .asciz "Searching Kernel"
load_fat_msg:         .asciz "Loading FAT"
load_kernel_file_msg: .asciz "Loading Kernel"
file_missing_msg:   .asciz "File Missing"
a20_msg:            .asciz "Enabling A20"
gdt_msg:            .asciz "Loading GDT"
ok_msg:             .asciz "OK"
pmode_msg:          .asciz "Enabling Protected Mode and starting Kernel"
press_a_key_msg:    .asciz "Press any key." # this is partially duplicate with Reboot section

.fill ((_BytsPerSec * (_RsvdSecCnt)) -(. - _start)), 1, 0

