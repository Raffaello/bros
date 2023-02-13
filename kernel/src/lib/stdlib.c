#include <lib/stdlib.h>
#include <lib/ctype.h>
#include <stddef.h>

int atoi (const char * str)
{
    register int res = 0;
 
    for(size_t i = 0; isdigit(str[i]); ++i)
        res = res * 10 + str[i] - '0';

    return res;
}

/* Reverse strcpy
 * src must be 0 terminated
 * dst must be at least long as src
 * size is src length - 1
 */
static inline void strcpy_r(const char* src, char* dst, long size)
{
    // size--;
    register long k = 0;
    for(; k <= size; ++k)
        dst[k] = src[size - k];
    dst[k] = 0;
}

char* itoa(unsigned int value, char * str, const uint8_t base)
{
    // if (base!=10 || base !=16)
    // {
    //     str[0]=0;
    //     return str;
    // }

    register int r = 0;
    register int i = 0;
    char buf[12];
    const char *alphadigit = "0123456789ABCDEF";
    do
    {
        r = value % base;
        value /= base;
        buf[i++] = alphadigit[r];
    } while(value > 0);
    // strcpy reversed
    buf[i--] = 0;
    strcpy_r(buf, str, i);
    // int k = 0;
    // for(; k <= i; ++k)
    //     str[k]=buf[i-k];
    // str[k] = 0;

    return str;
}

// 1000, 100, 0
// 100, 10, 0
// 10, 1, 0
// 1, 0, 1

char* ltoa(long value, char* str, const uint8_t base)
{
    register long r = 0;
    register long i = 0;
    char buf[21]; //  9223372036854775807
    const char *alphadigit = "0123456789ABCDEF";
    
    do
    {
        r = value % base;
        value /= base;
        buf[i++] = alphadigit[r];
    } while(value > 0);
    // strcpy reversed
    buf[i--] = 0;
    strcpy_r(buf, str, i);
    // int k = 0;
    // for(; k <= i; ++k)
    //     str[k]=buf[i-k];
    // str[k] = 0;
    
    return str;
}
