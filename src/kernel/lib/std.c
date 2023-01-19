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

// char* itoa(int value, char * str, const int base)
// {
//     // do in asm
//     int reminder = 0;
//     int i=0;
//     do
//     {
//         reminder = value %10;
//         value /= 10;
//         str[i]=reminder;

//     } while(reminder > 0)
// }

// 1000, 100, 0
// 100, 10, 0
// 10, 1, 0
// 1, 0, 1