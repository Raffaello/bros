/* -*-Asm-*- */

#include <arch/x86/defs/asm.h>

.code32
.intel_syntax noprefix
.text
CR0_PG_MASK=0x80000000

ASM_BEGIN(VMM_enable_paging)
  mov eax, cr0
  or eax, CR0_PG_MASK
  mov cr0, eax
  ret
ASM_END(VMM_enable_paging)

ASM_BEGIN(VMM_disable_paging)
  mov eax, cr0
  or eax, ~CR0_PG_MASK
  mov cr0, eax
  ret
ASM_END(VMM_disable_paging)
