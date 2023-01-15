# Print a number stored in AX
# It uses the stack to store the digits
# it will then pop from the stack to display the number correctly as a string.
# It will use the PrintString function.

.ifndef PrintNumber
.include "utils/bios/PrintChar.asm"

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
#   or al, al
  and ax, 0xFF
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

# 1000 / 10, 100, 0
# 100 / 10, 10, 0
# 10/ 10, 1, 0
# 1/ 10, 0, 1

# 1234 / 10, 123, 4
# 123 / 10, 12, 3
# 12 / 10, 1, 2
# 1 / 10, 0, 1
# 65535 / 10, 6553, 5
.endif
