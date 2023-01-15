# FAT12

.ifndef __FAT12__
__FAT12__:

.include "filesystems/FAT_bootsector.asm" # Needed for variables values
# Variables

ROOT_DIR_ENTRY_SIZE     = 32
ROOT_DIR_TOTAL_SIZE     = ROOT_DIR_ENTRY_SIZE * _RootEntCnt
ROOT_DIR_TOTAL_SECTORS  = (ROOT_DIR_TOTAL_SIZE + _BytsPerSec - 1) / _BytsPerSec
ROOT_DIR_SEC_OFFSET         = _RsvdSecCnt + (_NumFATs * _FATSz16) + 1
ROOT_DIR_SEG = 0x1000 # temporary loading here,
                      # then take the information required
                      # load the fat and take the information required.
                      # then load kernel here

.include "bios/DriveReadSectors.asm"
.include "utils/LBA2CHS.asm"


# ************************************* #
# Load Root Directory of FAT12/16       #
# ------------------------------------- #
# Parameters:                           #
#   DL Drive Number                     #
# Returns:                              #
#                                       #
# ************************************* #
.func LoadRootDirectory
LoadRootDirectory:
  mov ax, ROOT_DIR_SEC_OFFSET
  call LBA2CHS
  # here CH,CL, DH are set
  # missing, AL, ES:BX
  mov al, ROOT_DIR_TOTAL_SECTORS
#   mov dl, DrvNum
  mov bx, ROOT_DIR_SEG
  call DriveReadSectors

  ret
.endfunc

# ************************************* #
# Find File in the FAT Root Directory   #
# ------------------------------------- #
# Params:                               #
#  DS:SI    filename                    #
#  DL       file attributes             #
# Returns:                              #
#  AX       RootDir Index               #
#           not found FFFFh             #
# ************************************* #
.func RootDirFindFile
RootDirFindFile:
  cld
  xor cx,cx
  mov cl, _RootEntCnt
  mov di, ROOT_DIR_SEG # ES:DI RootDir Table
  mov bx, si
  and dx, 0xFF
RootDirFindFile_loop:
    # check attributes first
    cmp dl, BYTE PTR [DI]
    jne RootDirFindFile_skip
    # check filename
    push cx
    mov cx, 11
    mov si, bx
    push di
    repe stosb # todo could be done 1 byte cmp then using stosw
    pop di
    je RootDirFindFile_found
    pop cx
RootDirFindFile_skip:
    add di, ROOT_DIR_ENTRY_SIZE
    loop RootDirFindFile_loop
    # Not Found
    mov ax, 0xFFFF
    ret
RootDirFindFile_found:
    pop bx # it was push cx, loop counter
    mov al, _RootEntCnt
    sub al, bl # index value
    ret
.endfunc
.endif
