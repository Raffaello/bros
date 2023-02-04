/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text
CR0_PG_MASK=0x80000000

.globl VMM_enable_paging
VMM_enable_paging:
  mov eax, cr0
  or eax, CR0_PG_MASK
  mov cr0, eax
  ret
.type VMM_enable_paging, @function
.size VMM_enable_paging, .-VMM_enable_paging

.globl VMM_disable_paging
VMM_disable_paging:
  mov eax, cr0
  or eax, ~CR0_PG_MASK
  mov cr0, eax
  ret
.type VMM_disable_paging, @function
.size VMM_disable_paging, .-VMM_disable_paging
