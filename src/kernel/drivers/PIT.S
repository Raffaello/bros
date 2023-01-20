/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text
.globl PIT_interrupt_handler
PIT_interrupt_handler:
  iret
.type PIT_interrupt_handler, @function
.size PIT_interrupt_handler, .-PIT_interrupt_handler