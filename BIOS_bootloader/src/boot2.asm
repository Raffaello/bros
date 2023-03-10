# 2nd Stage Bootloader

.code16
.intel_syntax noprefix
.text
.org 0x0

KERNEL_FILENAME_ATTRIB  = 0b00000111    # System, Hidden, Read-only
# KERNEL_SEG              = 0x1000    # where to load the kernel
# FAT_BUFFER_SEG          = 0x600     # where to store the values for the FAT Cluster linked list
# SYS_INFO_SEG            = 0x600     # total size 10 bytes for the header,
                                      # plus MEM_MAP array and 4 bytes end marker

.global _start

_start:
  jmp main
KERNEL_FILENAME:  .ascii "BROSKRNLSYS"
DrvNum:           .byte  0
  # nop

.include "bios/PrintStringNewLine.inc"
.include "bios/PrintStringDots.inc"
.include "arch/x86/GDT.inc"
.include "utils/GateA20.inc"
.include "filesystems/FAT12.inc"
.include "bios/PrintNumber.inc"
.include "bios/GetTotalMemory.inc"
.include "bios/GetMemoryMap.inc"

main:
  # in AL has been passed the DrvNum, storing it
  mov DrvNum, al
  # in the stack there is the tot low memory
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

  # Store SYS_INFO values
  lea si, sys_info_msg
  call PrintStringDots
  mov edi, SYS_INFO_SEG
  mov eax, 0x12345678           # begin_marker
  stosd
  call GetTotalMemorySize
  xor edx,edx
  pop dx                        # lower mem from 1st stage
  add eax, edx
  stosd
  mov al, DrvNum                # boot_device
  stosb
  push edi
  inc edi                       # 1 byte for num of mem map entries here, if 0 none.
  call GetMemoryMap             # EDI is already pointing correctly (SYS_INFO_SEG+10)
  mov ecx, edi                  # backup
  pop edi
  mov ax, bp
  stosb                         # store AL, expected BP < 256
  mov edi, ecx                  # restore edi to the end of mem entries
  mov eax, 0x87654321           # end_marker
  stosd
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
  mov esp, 0x1000               # stack start below the loaded kernel
  # Store kernel parameters
  mov eax, 0x42524F53           # Bootloader Magic value
  mov ebx, SYS_INFO_SEG         # System Info struct address
  jmp GDT_CODE_SEG:KERNEL_SEG   # not sure the kernel will never return, so no point to 'call'
  # dead code below, it will be overridden by kernel memory manager anyway
main32_stop:
  cli
  hlt
  jmp main32_stop


root_dir_msg:           .asciz "Loading Root Dir"
find_kernel_file_msg:   .asciz "Searching Kernel"
load_fat_msg:           .asciz "Loading FAT"
load_kernel_file_msg:   .asciz "Loading Kernel"
file_missing_msg:       .asciz "File Missing"
sys_info_msg:           .asciz "Store SYS_INFO"
a20_msg:                .asciz "Enabling A20"
gdt_msg:                .asciz "Loading GDT"
ok_msg:                 .asciz "OK"
pmode_msg:              .asciz "Enabling Protected Mode and starting Kernel"
press_a_key_msg:        .asciz "Press any key."

.fill ((_BytsPerSec * (_RsvdSecCnt - 1)) -(. - _start)), 1, 0
