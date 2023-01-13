# Print a number stored in AX
# It uses the stack to store the digits
# it will then pop from the stack to display the number correctly as a string.
# It will use the PrintString function.

.ifndef "PrintChar"
.include "utils/bios/PrintChar.asm"
.endif

.func PrintNumber
PrintNumber:
  push cx
  push dx
  xor cx, cx # storing number of digits
  mov dl, 10 # divides by 10
PrintNumber_loop:
  div dl     # AL = Quotient, AH = Remainder
  push ax
  inc cx
  or ah, ah
  jnz PrintNumber_loop
PrintNumber_loop_print:
  # 1 digits is always guaranteed: 0
  pop ax
  mov al, ah
  add al, '0'
  call PrintChar
  loop PrintNumber_loop_print

  # restore cx,dx registers
  pop dx
  pop cx
  ret
.endfunc
