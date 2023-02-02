#include <lib/std.h>

// int atoi (const char * str)
// {
//     // TODO: do it in ASM
//     int res = 0;
//     char* s = str;
    
//     while(*s)
//     {
//         res *= 10;
//         res += *str - '0';
//         ++s;
//     }

//     return res;
// }

char* itoa(unsigned int value, char * str, const uint8_t base)
{
    // if (base!=10 || base !=16)
    // {
    //     str[0]=0;
    //     return str;
    // }

    register int r = 0;
    register int i = 0;
    uint8_t buf[12];
    const char *alphadigit = "0123456789ABCDEF";
    do
    {
        r = value % base;
        value /= base;
        buf[i++] = alphadigit[r];
    } while(value > 0);
    // strcpy reversed
    buf[i--] = 0;
    int k = 0;
    for(; k <= i; ++k)
    {
        str[k]=buf[i-k];
    }
    str[k] = 0;

    return str;
}

// 1000, 100, 0
// 100, 10, 0
// 10, 1, 0
// 1, 0, 1