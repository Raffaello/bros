#include <lib/string.h>

// TODO do in ASM, now need a printf quickly...
size_t strlen(const char *str)
{
    size_t len = 0;
    while(str[len] != 0)
        len++;

    return len;
}

void inline *memcpy(void *dst, const void *src, size_t n)
{
    // TODO do in ASM
    for(size_t i = 0; i<n; ++i)
        dst[i] = src[i];

    return dst;
}

size_t strnlen(const char *str, const size_t max_length)
{
    size_t len = 0;
    while(str[len] != 0 && len < max_length)
        len++;

    return (str[len] == 0) ? len : 0;
}
