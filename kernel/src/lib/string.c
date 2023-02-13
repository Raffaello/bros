#include <lib/string.h>

// TODO do in ASM, now need a printf quickly...
size_t strlen(const char *str)
{
    size_t len = 0;
    while(str[len] != 0)
        len++;

    return len;
}

size_t strnlen(const char *str, const size_t max_length)
{
    size_t len = 0;
    while(str[len] != 0 && len < max_length)
        len++;

    return len;
}
