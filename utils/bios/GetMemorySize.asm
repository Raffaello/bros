# BIOS Get Memory Size int 12h
# Returns:
#  AX = amount of KB of memory
.ifndef GetMemorySize
.func GetMemorySize
GetMemorySize:
    xor ax,ax
    int 0x12
    ret
.endfunc
.endif
