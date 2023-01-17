# *************************************
# BIOS Read Sector from Drive         *
# Input:                              *
# AL = Sectors to read count          *
# DL = Drive                          *
# BX = Buffer address offset pointer  *
# ----------------------------------- *
# INT 13h, AH=02h parameters:         *
#                                     *
# AL = tot sector count, 0 is illegal *
#      cannot cross ES page boundary, *
#      or a cylinder boundary,        *
#      and must be < 128              *
#                                     *
# CH = Low 8 bits of cylinder         *
# CL = High 2 bits of cylinder,       *
#      6 bits for sector              *
# DH = Head number                    *
# DL = Drive number                   *
# ES:BX = Buffer Address Pointer      *
#                                     *
# returns:                            *
# CF = Set on error                   *
# AH = return code                    *
# AL = Actual Sectors Read Count      *
# *************************************
.ifndef DriverReadSectors

.include "bios/PrintNumber.asm"
.include "utils/BootFailure.asm"

.func DriveReadSectors
DriveReadSectors:
  mov ah, 0x02
  push ax
  int 0x13 #   read data to [es:bx]
  jc DriveReadSectors_error
  pop dx
  cmp dl, al # if AL ( sectors read ) != DH ( sectors expected )
  jne DriveReadSectors_error
  ret
DriveReadSectors_error:
  shr ax, 8 # move AH into AL
  call PrintNumber
  lea si, diskerror_msg
  call BootFailure
.endfunc

diskerror_msg: .asciz "Disk read error"
.endif
