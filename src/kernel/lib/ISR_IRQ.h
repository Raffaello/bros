/**********************************
 *** Interrupt Service Routines ***
 **********************************/
#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct ISR_registers_t
{
   uint16_t ds;                                         // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;     // Pushed by pusha.
   uint32_t int_no/*, err_code*/;                       // Interrupt number and error code
   uint32_t eip, cs, eflags, useresp, ss;               // Pushed by the processor automatically, restored by iretd.

} __attribute__((packed)) ISR_registers_t;
#pragma pack(pop)

// Enables registration of callbacks for interrupts (no IRQ).
typedef void ((*ISR_Handler_t)(ISR_registers_t));

void ISR_init();
void IRQ_init();

void ISR_register_interrupt_handler(const uint8_t n, ISR_Handler_t handler);
void IRQ_register_interrupt_handler(const uint8_t n, ISR_Handler_t handler);
