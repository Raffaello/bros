# FAT12 Boot Sector
# Loading the FAT 2nd Reserved Sector (boot2.asm)
# and Execute it

.code16
.intel_syntax noprefix
.text
.org 0x0

.include "filesystems/FAT_bootsector.asm"

# TODO needs to self-relocate to 0x600 and load BOOT2 into 0x7c00 replacing this one.
BOOT_RELOCATE_SEG = 0x600
BIOS_BOOT_SEG  = 0x7c00
BOOT2_SEG = BIOS_BOOT_SEG # load the boot loader to segment
.global _start

_start:
  jmp short main # jump to beginning of code

# OEM Parameter Block
.org 0x3                            # Ensure the BPB starts at the correct place
bootsector:
OEM:          .ascii  "--BROS--"    # OEM String (starting at offset 3)
  BytsPerSec:   .word  _BytsPerSec  # bytes per sector
  SecPerClus:   .byte  _SecPerClus  # sectors per cluster
  RsvdSecCnt:   .word  _RsvdSecCnt  # #of reserved sectors (2nd stage bootloader)
  NumFATs:      .byte  _NumFATs     # #of FAT copies
  RootEntCnt:   .word  _RootEntCnt  # size of root directory (512 FAT16)
  TotSec16:     .word  _TotSec16    # total # of sectors if over 32 MB
  media:        .byte  0xF0         # media Descriptor (F0=removible)
  FATSz16:      .word  9            # size of each FAT
  SecPerTrk:    .word  _SecPerTrk   # sectors per track, 18 if 1.44MB Floppy
  NumHeads:     .word  _NumHeads    # number of read-write heads
  HiddSec:      .int   0            # number of hidden sectors
  TotSec32:     .int   0            # # sectors for over 32 MB

  # --- END FAT12/16 compatibility with FAT32 ---

  DrvNum:      .byte  0             # holds drive that the boot sector came from
  Reserved1:   .byte  0             # reserved, empty
  BootSig:     .byte  0x29          # extended boot sector signature
  volID:       .ascii "seri"        # This ID should be generated by simply combining the current date and time into a 32-bit value.
  volumeLab:   .ascii "NO NAME    " # volume label
  FilSysType:  .ascii "FAT     "    # file system type

.include "bios/PrintString.asm"
.include "bios/PrintStringDots.asm"
.include "bios/PrintStringNewLine.asm"
.include "bios/DriveReadSectors.asm"
.include "utils/BootFailure.asm"
.include "bios/GetMemorySize.asm"
.include "bios/PrintNumber.asm"
.include "bios/ResetDrive.asm"

main:
# Setup segments:
  cli                # clear interrupts
  mov  ax, cs        # CS = 0x0, since that's where boot sector is (0x07c00)
  mov  ds, ax        # DS = CS = 0x0
  mov  es, ax        # ES = CS = 0x0
  mov  ss, ax        # SS = CS = 0x0
  # sti                # enable interrupts
  
  # *** Self Relocating Boot sector ***
  mov cx, 256        # 512 bytes => 256 words
  mov di, BOOT_RELOCATE_SEG
  mov si, BIOS_BOOT_SEG
  rep movsw         # relocating from 0x7c00 to 0x600
  # jmp 0:BOOT_RELOCATE_SEG + (main_relocated - _start)
  jmp 0: main_relocated

main_relocated:
  mov  sp, BOOT_RELOCATE_SEG - 2     # Stack grows down from offset 0x600 toward 0x0000.
  mov  DrvNum, dl                    # save what drive we booted from (should be 0x0)

# Display "loading" message:
  lea  si, banner_msg
  call PrintString
# Display "boot relocated"
  lea si, relocated_msg
  call PrintStringNewLine
# Display Total Memory (it can be removed as it is a waste of space)
  lea si, mem_msg
  call PrintString
  call GetMemorySize
  call PrintNumber
  lea si, kb_unit_msg
  call PrintStringNewLine

# Reset disk system. Jump to bootFailure on error.
  mov  dl, DrvNum # drive to reset
  call ResetDrive
  jc   bootFailure   # display error message if carry set (error)

# Read Drive sectors, FAT Reserved Sector(s)
  lea si, read_rsv_sec_msg
  call PrintStringDots

  mov dl, DrvNum
  mov bx, BOOT2_SEG  # where to load
  mov al, RsvdSecCnt    # Reading the FAT reserved sector(s), where the 2nd stage bootloader is located
  mov ch, 0             # Cylinder 0
  mov dh, 0             # Head 0
  mov cl, 2             # 2nd sector, 1 is boot sector (this one), next one is the 1st reserved sector
  call DriveReadSectors
  lea si, ok_msg
  call PrintStringNewLine

  xor ax, ax
  mov al, DrvNum
  jmp BOOT2_SEG

  # End of loader, shouldn't reach here
  hlt

bootFailure:
  lea  si, diskerror_msg
  call BootFailure

# PROGRAM DATA
relocated_msg:    .asciz "Self Relocated"
banner_msg:       .asciz "\r\n     #### |-=*.*=-| BROS FAT Bootlader |-=*.*=-| ####\r\n\r\n"
mem_msg:          .asciz "Real Mode Free Memory (BIOS): "
kb_unit_msg:      .asciz "KB"
read_rsv_sec_msg: .asciz "Reading Reserved Sectors"
ok_msg: .asciz "OK"

.fill (510-(.-_start)), 1, 0  # Pad with nulls up to 510 bytes (excl. boot magic)
BootMagic:  .word 0xAA55     # magic word for BIOS
