/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text
CR0_PG_MASK=0x80000000

.globl switch_page_directory
switch_page_directory:

  ret
.type switch_page_directory, @function
.size switch_page_directory, .-switch_page_directory

.globl enable_paging
enable_paging:
  pop eax
  mov cr3, eax

  mov eax, cr0
  or eax, CR0_PG_MASK
  mov cr0, eax
  ret
.type enable_paging, @function
.size enable_paging, .-enable_paging

.globl disable_paging
disable_paging:
  mov eax, cr0
  mov ecx, CR0_PG_MASK
#   andn eax, ecx, eax
  mov cr0, eax
  ret
.type disable_paging, @function
.size disable_paging, .-disable_paging

