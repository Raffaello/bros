# BIOS Read Sector from Drive
# Input:
# DH = Sectors to read count
# DL = Drive
# BX = Buffer address offset pointer
# -----------------------------
# int 13h, ah=02h parameters:
# AL = Sectors To Read Count
# CH = Cylinder
# CL = Sector
# DH = Head
# DL = Drive
# ES:BX = Buffer Address Pointer
#
# returns:
# CF = Set on error
# AH = return code
# AL = Actual Sectors Read Count

.ifndef DriverReadSectors

.include "utils/bios/PrintString.asm"
.include "utils/BootFailure.asm"

.func DriveReadSectors
DriveReadSectors:
  push dx
  mov ah, 0x02
  mov al, dh
  mov ch, 0x00
  mov dh, 0x00
  mov cl, 0x02
  int 0x13 #   read data to [es:bx] 
  jc DriveReadSectors_error
  pop dx
  cmp dh, al # if AL ( sectors read ) != DH ( sectors expected )
  jne DriveReadSectors_error
  ret
DriveReadSectors_error:
  lea si, diskerror_msg
  call BootFailure
.endfunc

diskerror_msg: .asciz "Disk read error\r\n"
.endif
