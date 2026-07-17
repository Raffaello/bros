#pragma once

#include <sys/panic.h>

#ifdef NDEBUG
#define assert(condition, msg) ((void) 0)
#else
#define assert(condition, msg) \
    do                         \
    {                          \
        if (!(condition))      \
            KERNEL_PANIC(msg); \
    }                          \
    while (0);
#endif
