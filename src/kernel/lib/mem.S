/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text

.globl memset
memset:
  # This could be done with SIMD instruction
  push ebp
  mov ebp, esp
  mov di, [ebp + 8]   # buf
  mov eax, [ebp + 12] # val
  mov ecx, [ebp + 16] # n
  rep stosb
  pop ebp
  ret
.type memset, @function
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
.size add, .-add