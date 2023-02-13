/* -*-Asm-*- */
/* x86 specific code */

#include <arch/x86/defs/asm.h>

.code32
.intel_syntax noprefix
.text

ASM_BEGIN(memset)
  # This could be done with 4,2,1 bytes stos
#   push ebp
#   mov ebp, esp
  mov edi, [esp + 4]  # buf
  mov eax, [esp + 8] # val
  mov ecx, [esp + 12] # n
  rep stosb           # store AL into ES:EDI
  mov eax, [esp + 4]
#   pop ebp
  ret
ASM_END(memset)


ASM_BEGIN(memcpy)
  # This could be done with 4,2,1 bytes stos
#   push ebp
#   mov ebp, esp
  mov eax, [esp + 4]   # dst
  mov edi, eax
  mov esi, [esp + 8] # src
  mov ecx, [esp + 12] # n
  rep movsb           # move DS:ESI into ES:EDI
#   pop ebp
  ret
ASM_END(memcpy)
