/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text

.globl memset
memset:
  # This could be done with 4,2,1 bytes stos
  push ebp
  mov ebp, esp
  mov di, [ebp + 8]   # buf
  mov eax, [ebp + 12] # val
  mov ecx, [ebp + 16] # n
  rep stosb           # store AL into ES:EDI
  pop ebp
  ret
.type memset, @function
.size memset, .-memset
