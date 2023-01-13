# BIOS print using interrupt 10h, ah=0Eh
# expecting the char to be printed in AL

.func PrintChar
PrintChar:
  mov    ah, 0xE         # Subfunction 0xe of int 10h (video teletype output)
  mov    bx, 0x9         # Set bh (page nr) to 0, and bl (attribute) to white (9)
  int    0x10            # call BIOS interrupt.
  ret
.endfunc
