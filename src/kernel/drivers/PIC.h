#pragma once

#include <stdint.h>

#define PIC1_BASE_INT 0x20  // 1st 8 interrupts (0-7)
#define PIC2_BASE_INT 0x28  // 2nd 8 interrupts (8-15)

void PIC_init();
void PIC_EOI(const uint8_t num_int); // End of Interrupt
