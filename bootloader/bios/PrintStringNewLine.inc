# Print BIOS a string and a new line
# ds:si string to be printed zero terminated

.ifndef PrintStringNewLine

.include "bios/PrintString.inc"
.include "bios/PrintNewLine.inc"

.func PrintStringNewLine
PrintStringNewLine:
  call PrintString
  call PrintNewLine
  ret
.endfunc
.endif
