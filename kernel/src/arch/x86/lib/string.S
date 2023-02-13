/* -*-Asm-*- */
/* x86 specific code */

#include <arch/x86/defs/asm.h>

.code32
.intel_syntax noprefix
.text

ASM_BEGIN(memset)
  # This could be done with 4,2,1 bytes stos
  push edi
  mov edi, [esp + 8]  # buf
  mov eax, [esp + 12] # val
  mov ecx, [esp + 16] # n
  rep stosb           # store AL into ES:EDI
  pop edi
  mov eax, [esp + 4]
  ret
ASM_END(memset)


ASM_BEGIN(memcpy)
  # This could be done with 4,2,1 bytes stos
  push esi
  push edi
  mov eax, [esp + 12]   # dst
  mov edi, eax
  mov esi, [esp + 16] # src
  mov ecx, [esp + 20] # n
  rep movsb           # move DS:ESI into ES:EDI
  pop edi
  pop esi
  ret
ASM_END(memcpy)
