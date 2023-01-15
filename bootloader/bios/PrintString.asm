# BIOS print using interrupt 10h, ah=0Eh
# expecting \0 terminated string in ds:si as paramter
#

.ifndef PrintString

.include "bios/PrintChar.asm"

.func PrintString
PrintString:
  lodsb                  # load byte at ds:si into al (advancing si)
  or     al, al          # test if character is 0 (end)
  jz     PrintString_end # jump to end if 0.
  call   PrintChar
  jmp    PrintString     # Repeat for next character.
PrintString_end:
  ret # return
.endfunc
.endif
