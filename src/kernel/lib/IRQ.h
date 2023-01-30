#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct IRQ_registers_t
{
   uint16_t ds;                                         // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;     // Pushed by pusha.
   uint32_t irq_no/*, irq_map*/;                            // IRQ
   uint32_t eip, cs, eflags, useresp, ss;               // Pushed by the processor automatically, restored by iretd.

} __attribute__((packed)) IRQ_registers_t;
#pragma pack(pop)

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void ((*IRQ_Handler_t)(IRQ_registers_t));

void IRQ_init();
void IRQ_register_interrupt_handler(uint8_t n, IRQ_Handler_t handler);
