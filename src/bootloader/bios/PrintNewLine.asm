# BIOS Print a New Line: CR LF
.ifndef PrintNewLine
.func PrintNewLine
PrintNewLine:
    mov al, '\r'
    call PrintChar
  # mov    ah, 0xE         # Subfunction 0xe of int 10h (video teletype output)
  # mov    bx, 0x9         # Set bh (page nr) to 0, and bl (attribute) to white (9)
  # int    0x10            # call BIOS interrupt.
    mov al, '\n'
    call PrintChar
    # int    0x10            # call BIOS interrupt.
    ret
.endfunc
.endif
