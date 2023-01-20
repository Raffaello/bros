#pragma once

#include <drivers/PIC.h>

#define PIT_INT PIC1_BASE_INT + PIC_IRQ_TIMER

void PIT_init();
