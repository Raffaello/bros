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

bool bitset_find(const bitset32_t bitset, const uint32_t bitset_size, const size_t size, unsigned int* pos)
{
    if(size == 0)
        return false;

    const uint32_t bss = bitset_size / BITSET_SIZE;
    for(uint32_t i = 0; i < bss; i++)
    {
        // for each bit of the bitset32_t (uint32_t)
        for(int j = 0; j < BITSET_SIZE; j++)
        {
            const unsigned int bit = 1 << j;
            if ((bitset[i] & bit) == 0)
            {
                const uint32_t startPos = (i * BITSET_SIZE) + bit;
                if(startPos + size > bitset_size)
                    return false; // not enough space

                uint32_t free = 0;
                for(uint32_t k = 0; k <= size; k++ )
                {
                    if(bitset_test(bitset, startPos + k) == 0)
                        free++;
                    if(free == size)
                    {
                        // startPos - bit + j;
                        *pos = i * BITSET_SIZE + j;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}
