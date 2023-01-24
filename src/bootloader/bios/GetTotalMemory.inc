# *******************************************************************
# GetTotalMemorySize use BIOS to return 32bits total memory         *
# ----------------------------------------------------------------- *
# Parameters:                                                       *
#                                                                   *
# Returns:                                                          *
#   EAX = Total Memory in KB                                        *
# Destroys:                                                         *
#   EDX, ECX, BX                                                    *
# *******************************************************************
.ifndef GetTotalMemorySize
.include "utils/BootFailure.inc"
.func GetTotalMemorySize
GetTotalMemorySize:
  xor edx, edx
  xor ecx, ecx
  mov ax, 0xE801
  int 0x15
  jc GetTotalMemorySize_error
  cmp ah, 0x86                  # unsupported function
  je GetTotalMemorySize_error
  cmp ah, 0x80                  # invalid command
  je GetTotalMemorySize_error
  cmp cx,ax
  jne GetTotalMemorySize_error
  cmp bx,dx
  jne GetTotalMemorySize_error
#   mov eax, 1024                 # in KB (1MB)
#   add eax, ecx                  # up to 16MB   (max value expected 0x4000) (cx=0x3c00, ax=0x400)
  mov eax,ecx                   # 1 MB less to avoid ecx overflow
  shl edx, 6                    # times 64
  add eax, edx
  
  


  ret

GetTotalMemorySize_error:
    call BootFailure
.endif