/* -*-Asm-*- */

.code32
.intel_syntax noprefix
.text
.globl PIT_timer_interrupt_handler
PIT_timer_interrupt_handler:
# TODO requires the PIC_EOI
  iretd
.type PIT_timer_interrupt_handler, @function
.size PIT_timer_interrupt_handler, .-PIT_timer_interrupt_handler