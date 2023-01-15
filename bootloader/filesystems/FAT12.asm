# FAT12

.ifndef __FAT12__
__FAT12__:

.include "filesystems/FAT_bootsector.asm" # Needed for variables values
# Variables
ROOT_DIR_ENTRY_SIZE     = 32
ROOT_DIR_TOTAL_SIZE     = ROOT_DIR_ENTRY_SIZE * _RootEntCnt
ROOT_DIR_TOTAL_SECTORS  = ROOT_DIR_TOTAL_SIZE / _BytsPerSec
ROOT_DIR_OFFSET         = _RsvdSecCnt + (_NumFATs * _FATSz16)

ROOT_DIR_SEG = 0x500 # ?

.include "bios/DriveReadSectors.asm"


# ************************************* #
# Load Root Directory of FAT12/16       #
# ------------------------------------- #
# Parameters:                           #
#
# Returns:                              #
#
# ************************************* #
.func LoadRootDirectory
LoadRootDirectory:
  mov dh, ROOT_DIR_TOTAL_SECTORS
  mov dl, DrvNum
  mov bx, ROOT_DIR_SEG
  call DriveReadSectors

  ret
.endfunc
.endif
