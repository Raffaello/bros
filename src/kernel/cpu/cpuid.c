#include <cpu/cpuid.h>
#include <defs.h>

void cpuid(CPU_Info_t *cpuid)
{
    if (cpuid == NULL)
        return;

    __asm__("cpuid");
}