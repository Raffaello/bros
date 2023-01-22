/********************************
 * Programmable Interrupt Timer *
 * ---------------------------- *
 * Installs Timer IRQ handler   *
 *******************************/
#pragma once

#include <defs.h>

void PIT_init(const uint32_t freq);
// freq is in Herz, 1 Hz = 1s
void PIT_set_timer_freq(const uint32_t freq);
