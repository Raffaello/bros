#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t* bitset32_t;
_Static_assert(sizeof(bitset32_t) == 4);

void bitset_assign(bitset32_t, unsigned int bit, const bool value);
void bitset_set(bitset32_t bitset, unsigned int bit);
void bitset_unset(bitset32_t bitset, unsigned int bit);
bool bitset_test(bitset32_t bitset, unsigned int bit);
