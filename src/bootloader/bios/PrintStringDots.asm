# Print BIOS a string and 3 dots
# ds:si string to be printed zero terminated

.ifndef PrintStringDots

.include "bios/PrintString.asm"

.func PrintStringDots
PrintStringDots:
  call PrintString
  lea si, dots_msg
  call PrintString
  ret
.endfunc

dots_msg:           .asciz "..."

.endif
