#pragma once

#include <stdint.h>

// int atoi(const char * str);

// TODO generalize in itoa taking a base 
char* itoa10(uint16_t value, char* str);
char* itoa16(uint16_t value, char* str);

char* itoa(unsigned int value, char* str, const uint8_t base);
