#pragma once

#define PIC1_BASE_INT       0x20  // 1st 8 interrupts (0-7) remapped
#define PIC2_BASE_INT       0x28  // 2nd 8 interrupts (8-15) remapped

#if PIC2_BASE_INT - PIC1_BASE_INT != 8
#error "ISR_IRQ.c requires PIC2 mapped after PIC1"
#endif
