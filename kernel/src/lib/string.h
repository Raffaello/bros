/************************************************
 * C <string.h> alike                           *
 * the specific implementations are for e.g. in *
 * arch/x86/lib/{*}.S                           *
 ************************************************/
#pragma once

#include <stdint.h>
#include <stddef.h>

_Static_assert(sizeof(size_t) == sizeof(uint32_t));

void* memset(void *buf, int val, size_t n);
// void memcpy();
// void memcmp();

size_t strlen(const char *str);
