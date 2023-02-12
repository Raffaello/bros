#pragma once

#include <sys/panic.h>

#ifdef NDEBUG
#define assert(condition) ((void)0)
#else
#define assert(condition, msg) if(!condition) KERNEL_PANIC(msg)
#endif
