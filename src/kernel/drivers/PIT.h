/********************************
 * Programmable Interrupt Timer *
 * ---------------------------- *
 * Installs Timer IRQ handler   *
 *******************************/
#pragma once

#include <stdint.h>

void PIT_init(const uint32_t freq);

/**
 * freq is in Herz, if 0 no changes.
 */
void PIT_set_timer_freq(const uint32_t freq);
