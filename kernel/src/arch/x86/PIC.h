#pragma once

#include <stdint.h>

void PIC_init();
void PIC_EOI(const uint8_t num_int); // End of Interrupt
// TODO: disable to use APIC / IO-APIC.
void PIC_disable();

uint16_t PIC_get_irr();
uint16_t PIC_get_isr();
