# FAT12/16 Boot Sector
# Loading the FAT Reserved Sector (boot2.asm)
# and Execute it

.code16
.intel_syntax noprefix
.text
.org 0x0

BOOT2_SEG = 0x600 # load the boot loader to segment

.global _start

_start:
  jmp short main # jump to beginning of code
  # nop

# OEM Parameter Block
.org 0x3			/* Ensure the BPB starts at the correct place */
.include "bootsector.asm"

.include "bios/PrintString.asm"
.include "bios/DriveReadSectors.asm"
.include "utils/BootFailure.asm"
.include "bios/GetMemorySize.asm"
.include "bios/PrintNumber.asm"
.include "bios/ResetDrive.asm"

main:
# Setup segments:
  cli                # clear interrupts
  mov  DrvNum, dl    # save what drive we booted from (should be 0x0)
  mov  ax, cs        # CS = 0x0, since that's where boot sector is (0x07c00)
  mov  ds, ax        # DS = CS = 0x0
  mov  es, ax        # ES = CS = 0x0
  mov  ss, ax        # SS = CS = 0x0
  mov  sp, 0x7C00    # Stack grows down from offset 0x7C00 toward 0x0000.
  # sti                # enable interrupts

# Display "loading" message:
  lea  si, loadmsg
  call PrintString

# Display Total Memory (it can be removed as it is a waste of instrunctions)
  lea si, mem_msg
  call PrintString
  call GetMemorySize
  call PrintNumber
  lea si, kb_unit_msg
  call PrintString

# Reset disk system. Jump to bootFailure on error.
  mov  dl, DrvNum # drive to reset
  call ResetDrive
  jc   bootFailure   # display error message if carry set (error)

# Read Drive sectors, FAT Reserved Sector(s)
  lea si, read_rsv_sec_msg
  call PrintString

  mov dl, DrvNum
  mov bx, BOOT2_SEG  # where to load
  mov al, RsvdSecCnt    # Reading the FAT reserved sector(s), where the 2nd stage bootloader is located
  mov ch, 0             # Cylinder 0
  mov dh, 0             # Head 0
  mov cl, 2             # 2nd sector, 1 is boot sector (this one), next one is the 1st reserved sector
  call DriveReadSectors
  lea si, ok_msg
  call PrintString

  jmp BOOT2_SEG

  # End of loader, shouldn't reach here
  jmp short $

bootFailure:
  lea  si, diskerror_msg
  call BootFailure

# PROGRAM DATA
loadmsg:      .asciz "\r\n       #### |-=*.*=-| BROS FAT Bootlader |-=*.*=-| ####\r\n\r\n"
mem_msg:      .asciz "Real Mode Free Memory (BIOS): "
kb_unit_msg: .asciz "KB\r\n"
read_rsv_sec_msg: .asciz "Reading Reserved Sector(s)..."
ok_msg: .asciz "OK\r\n"

.fill (510-(.-_start)), 1, 0  # Pad with nulls up to 510 bytes (excl. boot magic)
BootMagic:  .word 0xAA55     # magic word for BIOS
