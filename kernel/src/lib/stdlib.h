/************************************************
 * C <stdlib.h> alike                           *
 * the specific implementations are for e.g. in *
 * arch/x86/lib/{*}.S                           *
 ************************************************/
#pragma once

#include <stdint.h>

int atoi(const char * str);

char* utoa(unsigned int value, char* str, const uint8_t base);
char* itoa(int value, char* str, const uint8_t base);
char* ltoa(long value, char* str, const uint8_t base);
