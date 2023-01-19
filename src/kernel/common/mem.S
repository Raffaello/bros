/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text

.globl memset
memset:
  ret # TODO
.type memset, @function
# .set .L__sym_size_##memset, .-memset
.size memset, .-memset

.globl add
add:
  # return value in eax
  push ebp
  mov ebp, esp
  mov eax, [ebp + 8]  # 8, because 4 for ebp and other for 4 EIP due to the call/ret
  mov edx, [ebp + 12]
  add eax, edx
  pop ebp
  ret
.type add, @function
# .set .L__sym_size_##add, .-add
.size add, .-add