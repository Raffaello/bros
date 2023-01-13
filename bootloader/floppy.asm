.code16
.intel_syntax noprefix
.text
.org 0x0

LOAD_SEGMENT = 0x1000 # load the boot loader to segment 1000h

.global _start

_start:
  jmp short main # jump to beginning of code
  nop

# OEM Parameter Block
bootsector:
  OEM:          .ascii "--BROS--"    # OEM String
  sectSize:     .word  0x200         # bytes per sector
  clustSize:    .byte  1             # sectors per cluster
  resSect:      .word  1             # #of reserved sectors
  fatCnt:       .byte  2             # #of FAT copies
  rootSize:     .word  224           # size of root directory
  totalSect:    .word  2880          # total # of sectors if over 32 MB
  media:        .byte  0xF0          # media Descriptor
  fatSize:      .word  9             # size of each FAT
  trackSect:    .word  9             # sectors per track, 18 if 1.44MB Floppy
  headCnt:      .word  2             # number of read-write heads
  hiddenSect:   .int   0             # number of hidden sectors
  sect32:       .int   0             # # sectors for over 32 MB
   BOOT_DRIVE:    .byte  0             # holds drive that the boot sector came from
  reserved:     .byte  0             # reserved, empty
  bootSign:     .byte  0x29          # extended boot sector signature
  volID:        .ascii "seri"        # disk serial
  volumeLabel:  .ascii "MYVOLUME   " # volume label
  fsType:       .ascii "FAT16   "    # file system type


.include "utils/bios/PrintString.asm"
.include "utils/bios/DriveReadSectors.asm"

.ifndef "BootFailure"
.include "utils/BootFailure.asm"
.endif

.include "utils/bios/GetMemorySize.asm"
.include "utils/bios/PrintNumber.asm"

main:
# Setup segments:
  cli                # clear interrupts
  mov  BOOT_DRIVE, dl # save what drive we booted from (should be 0x0)
  mov  ax, cs        # CS = 0x0, since that's where boot sector is (0x07c00)
  mov  ds, ax        # DS = CS = 0x0
  mov  es, ax        # ES = CS = 0x0
  mov  ss, ax        # SS = CS = 0x0
  mov  sp, 0x7C00    # Stack grows down from offset 0x7C00 toward 0x0000.
  sti                # enable interrupts

# Display "loading" message:
  lea  si, loadmsg
  call PrintString

# Display Total Memory (it can be removed as it is a waste of instrunctions)
  lea si, mem_msg
  call PrintString
  call GetMemorySize
  call PrintNumber
  lea si, mem_unit_msg
  call PrintString

# Reset disk system. Jump to bootFailure on error.
  mov  dl, BOOT_DRIVE # drive to reset
  xor  ah, ah        # subfunction 0
  int  0x13          # call interrupt 13h
  jc   bootFailure   # display error message if carry set (error)

# Read Drive sectors
  lea si, msg1
  call PrintString
  mov dl, BOOT_DRIVE
  mov dh, 2
  mov bx, LOAD_SEGMENT
  call DriveReadSectors
  lea si, msg2
  call PrintString
  mov ax, LOAD_SEGMENT
  # mov ds, ax
  jmp LOAD_SEGMENT


  # End of loader, for now. Reboot.
  call Reboot

bootFailure:
  lea  si, diskerror_msg
  call BootFailure

# PROGRAM DATA
loadmsg:      .asciz "Booting BROS...\r\n"
mem_msg:      .asciz "Free Memory: "
mem_unit_msg: .asciz "KB\r\n"
msg1: .asciz "read disk\r\n"
msg2: .asciz "done\r\n"

.fill (510-(.-_start)), 1, 0  # Pad with nulls up to 510 bytes (excl. boot magic)
BootMagic:  .word 0xAA55     # magic word for BIOS
