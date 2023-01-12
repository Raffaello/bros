# BIOS print using interrupt 10h, ah=0Eh
# expecting \0 terminated string in ds:si as paramter
#

.func PrintString
PrintString:
  lodsb                  # load byte at ds:si into al (advancing si)
  or     al, al          # test if character is 0 (end)
  jz     PrintString_end # jump to end if 0.
  mov    ah, 0xE         # Subfunction 0xe of int 10h (video teletype output)
  mov    bx, 0x9         # Set bh (page nr) to 0, and bl (attribute) to white (9)
  int    0x10            # call BIOS interrupt.
  jmp    PrintString     # Repeat for next character.
PrintString_end:
  ret # return
.endfunc
