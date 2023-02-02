#include <lib/bitset.h>

#define BITSET_SIZE         (sizeof(bitset32_t) * 8)
#define BITSET_INDEX(x)     (x / BITSET_SIZE)
#define BITSET_OFFSET(x)    (x % BITSET_SIZE)

_Static_assert(BITSET_SIZE == 32);

// inline void bitset_assign(bitset32_t, unsigned int bit, const bool value)
// {

// }

inline void bitset_set(bitset32_t bitset, unsigned int bit)
{
    bitset[BITSET_INDEX(bit)] |= (1 << (BITSET_OFFSET(bit)));
}

inline void bitset_unset(bitset32_t bitset, unsigned int bit)
{
    bitset[BITSET_INDEX(bit)] &= ~ (1 << (BITSET_OFFSET(bit)));
}

inline bool bitset_test(bitset32_t bitset, unsigned int bit)
{
    return bitset[BITSET_INDEX(bit)] & (1 << BITSET_OFFSET(bit));
}
