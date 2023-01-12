.code16
.intel_syntax noprefix
.text
.org 0x0

LOAD_SEGMENT = 0x1000 # load the boot loader to segment 1000h

.global _start

_start:
  jmp short main # jump to beginning of code
  nop

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
  trackSect:    .word  9             # sectors per track
  headCnt:      .word  2             # number of read-write heads
  hiddenSect:   .int   0             # number of hidden sectors
  sect32:       .int   0             # # sectors for over 32 MB
  bootDrive:    .byte  0             # holds drive that the boot sector came from
  reserved:     .byte  0             # reserved, empty
  bootSign:     .byte  0x29          # extended boot sector signature
  volID:        .ascii "seri"        # disk serial
  volumeLabel:  .ascii "MYVOLUME   " # volume label
  fsType:       .ascii "FAT16   "    # file system type

.func WriteString
WriteString:
  lodsb              # load byte at ds:si into al (advancing si)
  or     al, al      # test if character is 0 (end)
  jz     WriteEnd    # jump to end if 0.
  mov    ah, 0xE     # Subfunction 0xe of int 10h (video teletype output)
  mov    bx, 0x9     # Set bh (page nr) to 0, and bl (attribute) to white (9)
  int    0x10        # call BIOS interrupt.
  jmp    WriteString # Repeat for next character.
WriteEnd:
  retw # return
.endfunc

.func Reboot
 Reboot:
  lea    si, rebootmsg # Load address of reboot message into si
  call   WriteString   # print the string
  xor    ax, ax        # subfuction 0
  int    0x16          # call bios to wait for key
  .byte  0xEA          # machine language to jump to FFFF:0000 (reboot)
  .word  0x0000
  .word  0xFFFF
.endfunc

main:
  # Setup segments:
  cli                # disable interrupts
  mov  bootDrive, dl # save what drive we booted from (should be 0x0)
  mov  ax, cs        # CS = 0x0, since that's where boot sector is (0x07c00)
  mov  ds, ax        # DS = CS = 0x0
  mov  es, ax        # ES = CS = 0x0
  mov  ss, ax        # SS = CS = 0x0
  mov  sp, 0x7C00    # Stack grows down from offset 0x7C00 toward 0x0000.
  sti                # enable interrupts

# Display "loading" message:
  lea  si, loadmsg
  call WriteString

# Reset disk system.
# Jump to bootFailure on error.
  mov  dl, bootDrive # drive to reset
  xor  ax, ax        # subfunction 0
  int  0x13           # call interrupt 13h
  jc   bootFailure   # display error message if carry set (error)  

  # End of loader, for now. Reboot.
  call Reboot

bootFailure:
  lea  si, diskerror
  call WriteString
  call Reboot

# PROGRAM DATA
loadmsg:          .asciz "Loading BROS...\r\n"
diskerror:        .asciz "Disk error."
rebootmsg:        .asciz "Press any key to reboot.\r\n"

.fill (510-(.-_start)), 1, 0  # Pad with nulls up to 510 bytes (excl. boot magic)
BootMagic:  .word 0xAA55     # magic word for BIOS
