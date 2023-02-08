/************************************************
 * C <stdlib.h> alike                           *
 * the specific implementations are for e.g. in *
 * arch/x86/lib/{*}.S                           *
 ************************************************/
#pragma once

#include <stdint.h>

// int atoi(const char * str);

// TODO generalize in itoa taking a base
// char* itoa10(uint16_t value, char* str);
// char* itoa16(uint16_t value, char* str);

char* itoa(unsigned int value, char* str, const uint8_t base);
