/****************************************
 * BitSet                               *
 *   O(n) for searching, poor           *
 ***************************************/
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t* bitset32_t;
_Static_assert(sizeof(bitset32_t) == 4);

void bitset_set(bitset32_t bitset, const unsigned int bit);
void bitset_clear(bitset32_t bitset, const unsigned int bit);
bool bitset_test(bitset32_t bitset, const unsigned int bit);
bool bitset_find(const bitset32_t bitset, const uint32_t bitset_size, const size_t size, unsigned int* pos);
