# Print BIOS a string and a new line
# ds:si string to be printed zero terminated

.ifndef PrintStringNewLine

.include "bootloader/bios/PrintString.asm"
.include "bootloader/bios/PrintNewLine.asm"

.func PrintStringNewLine
PrintStringNewLine:
  call PrintString
  call PrintNewLine
  ret
.endfunc
.endif
