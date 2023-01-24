# ***************************************************************** #
# GetMemoryMap BIOS Call                                            #
#                                                                   #
# BOCHS & QEMU don't support this function                          #
# So this function is optional for the bootloader...                #
# ----------------------------------------------------------------- #
# Parameters:                                                       #
#   ES:DI Destination Buffer, where to store results                #
#   each entry is 24 bytes long                                     #
# Returns:                                                          #
#   BP = num entries, 0 error/not supported                         #
#   struct entry                                                    #
#       uint64_t baseAddress                                        #
#       uint64_t length                                             #
#       uint32_t type                                               #
#       uint32_t acpi (might be zeros)                              #
# Destroys:                                                         #
#   EAX, EBX, ECX, EDX                                              #
# ***************************************************************** #

.ifndef GetMemoryMap

.include "bios/PrintNumber.inc"
.include "bios/PrintNewLine.inc"

SMAP = 0x534D4150
ENTRY_SIZE = 24

# GetMemoryMap BIOS Call external Call
# returns a 20 bytes or 24 bytes struct, so use 24 bytes
.func GetMemoryMap
GetMemoryMap:
  xor bp, bp            # counter
  xor ebx, ebx          # offset
GetMemoryMap_loop:
  mov eax, 0x8E20
  mov ecx, ENTRY_SIZE   # struct size
  mov edx, SMAP         # 'SMAP'
  int 0x15
  jc GetMemoryMap_error
  cmp eax, SMAP
  jne GetMemoryMap_error
  # Here there are the results
  # ebx, 0 done otherwise next entry
  # ecx struct length in size (20 or 24)
  jcxz GetMemoryMap_skip_entry  # cx == 0 ?
  # check if the returned entry has a valid length
#   cmp DWORD PTR [DI + 8], 0x0
#   jz GetMemoryMap_skip_entry

  add di, ENTRY_SIZE
  inc bp
GetMemoryMap_skip_entry:
  cmp ebx, 0
  je  GetMemoryMap_done
  jmp GetMemoryMap_loop
GetMemoryMap_done:
  ret
GetMemoryMap_error:
  # AH contains error code
  xor  al, al
  xchg ah, al
  call PrintNumber
  call PrintNewLine
  xor bp, bp
  ret
.endfunc
.endif
