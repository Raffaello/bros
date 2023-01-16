# FAT12

.ifndef __FAT12__
__FAT12__:

.include "filesystems/FAT_bootsector.asm" # Needed for variables values

# Variables FAT Section
FAT_TOTAL_SECTORS = _FATSz16
FAT_SEC_OFFSET = _RsvdSecCnt
FAT_SEG = KERNEL_SEG  # temporary loading here, replacing ROOT_DIR

# Variables Root Directory Section
ROOT_DIR_ENTRY_SIZE     = 32
ROOT_DIR_TOTAL_SIZE     = ROOT_DIR_ENTRY_SIZE * _RootEntCnt
ROOT_DIR_TOTAL_SECTORS  = (ROOT_DIR_TOTAL_SIZE + _BytsPerSec - 1) / _BytsPerSec
ROOT_DIR_SEC_OFFSET         = _RsvdSecCnt + (_NumFATs * _FATSz16) + 1
ROOT_DIR_SEG = FAT_SEG # temporary loading here,
                      # then take the information required
                      # load the fat and take the information required.
                      # then load kernel here

.include "bios/DriveReadSectors.asm"
.include "utils/LBA2CHS.asm"

# ************************************* #
# Load FAT into memory region FAT_SEG   #
# ------------------------------------- #
# Paramters:                            #
#  DL = Drive Number                    #
# Returns:                              #
#   Loaded into mem at FAT_SEG          #
# ************************************* #
.func LoadFAT
LoadFAT:
  mov ax, FAT_SEC_OFFSET
  call LBA2CHS
  # here CH, CL, DH are set
  # missing AL, ES:BX
  mov al, FAT_TOTAL_SECTORS
  mov bx, FAT_SEG
  call DriveReadSectors
  ret
.endfunc

# ************************************* #
# Load Root Directory of FAT12/16       #
# ------------------------------------- #
# Parameters:                           #
#   DL = Drive Number                   #
# Returns:                              #
#   Loaded into mem at ROOT_DIR_SEG     #
# ************************************* #
.func LoadRootDirectory
LoadRootDirectory:
  mov ax, ROOT_DIR_SEC_OFFSET
  call LBA2CHS
  # here CH, CL, DH are set
  # missing AL, ES:BX
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
#  AL       RootDir Index               #
#           not found FFFFh             #
#  BX       Num Cluster                 #
#  DX       File Size Low  16 bits      #
#  CX       File Size High 16 bits      #
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
  cmp dl, BYTE PTR [DI + 11] # FAT Attribute Byte field
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
  mov bx, WORD PTR [DI + 26] # NumCluster
  mov dx, WORD PTR [DI + 28] # FileSize lo 16 bits
  mov cx, WORD PTR [DI + 30] # FileSize hi 16 bits
  ret
.endfunc

# *************************** #
# Load File from FAT          #
# --------------------------- #
# Requirements:               #
#  FAT loaded in FAT_SEG      #
# Paramters:                  #
#  AX = ???
#  BX = NumCluster            #
#  DL = Num Drive             #
# Returns:                    #
#
# *************************** #
.func LoadFile
LoadFile:
  # Each FAT entry is 12 bits:
  # a WORD is 1.5 FAT12 entries
  # a BYTE is 0.5 FAT12 entries
  # 3 bytes are exactly 2 FAT12 entries.
  # The first 2 entries are reserved, so should be ignored.
  # If the first entry is 2, in byte is 2 * 1.5 => 3
  # 1.5N => N + (N/2) in integers
  # --------------------------------------------------------------------------
  # if N is even take the lower 12 bits of the WORD
  # if N is odd  take the upper 12 bits of the WORD
  # e.g:
  #
  # 0 1 2 3 4 5 6 <- N
  # 0   3   6   9 <- bytes
  #
  # N=2 => offset=3 => read [FAT+3] AND 0xFFF (Bytes [3, 4.5])
  # N=3 => offset=4 => read [FAT+4] SHL 4     (Bytes [4.5, 6])
  

  # Parse the FAT to get all the Clusters

## required to read the file:
  # AL = num sectors to read
  # DL = drive
  # BX = buffer to load into
  
.endfunc
.endif
