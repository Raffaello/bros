#include <lib/bitset.h>

#define BITSET_SIZE         (sizeof(bitset32_t) * 8)
#define BITSET_INDEX(x)     (x / BITSET_SIZE)
#define BITSET_OFFSET(x)    (x % BITSET_SIZE)

_Static_assert(BITSET_SIZE == 32);

inline void bitset_set(bitset32_t bitset, const unsigned int bit)
{
    bitset[BITSET_INDEX(bit)] |= (1 << (BITSET_OFFSET(bit)));
}

inline void bitset_clear(bitset32_t bitset, const unsigned int bit)
{
    bitset[BITSET_INDEX(bit)] &= ~ (1 << (BITSET_OFFSET(bit)));
}

inline bool bitset_test(bitset32_t bitset, const unsigned int bit)
{
    return bitset[BITSET_INDEX(bit)] & (1 << BITSET_OFFSET(bit));
}

// find contiguos memory
bool bitset_find(const bitset32_t bitset, const uint32_t bitset_size, const size_t size, unsigned int* pos)
{
    // it looks some error somewhere ... 
    // it returned 0 after some malloc, no free.. 
    // maybe a wrong init? 
    // missing test at the moment..
    // should move to test.
    if(size == 0)
        return false;

    const uint32_t bss = bitset_size / BITSET_SIZE;
    for(uint32_t i = 0; i < bss; i++)
    {
        if(bitset[i] == 0xFFFFFFFF)
            continue;
        uint32_t free = 0;
        // for each bit of the bitset32_t (uint32_t)
        for(int j = 0; j < BITSET_SIZE; j++)
        {
            const unsigned int bit = 1 << j;
            if((bitset[i] & bit) == 0)
                free++;
            else 
                free = 0;

            if(free == size)
            {
                // startPos - bit + j;
                *pos = i * BITSET_SIZE + j;
                return true;
            }
        }
    }

    return false;
}
