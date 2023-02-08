# Print a number stored in AX
# It uses the stack to store the digits
# it will then pop from the stack to display the number correctly as a string.
# ***********************************

# the code can generate DIV ERROR, (>=2560 for e.g)
# but with interrupts disable should keep going and returning a wrong result (?)

# Upgrade to div by 16 bits instead of 8 bits

.ifndef PrintNumber
.include "bios/PrintChar.inc"

.func PrintNumber
PrintNumber:
  push cx
  push dx
  push bx

  xor cx, cx # storing number of digits
  mov bx, 10 # divides by 10
PrintNumber_loop:
  xor dx, dx # number high part
  div bx    # DX:AX / 10 => AX = Quotient, DX= Remainder
  push dx
  inc cx
  and ax, 0xFF
  jnz PrintNumber_loop
PrintNumber_loop_print:
  # 1 digits is always guaranteed: 0
  pop ax
  add ax, '0'
  call PrintChar
  loop PrintNumber_loop_print

  # restore cx,dx,bx registers
  pop bx
  pop dx
  pop cx
  ret
.endfunc

# 2560 / 10, 256, 0 (#DE exception 256 greater than 255 can fit a 8 bit register)

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
