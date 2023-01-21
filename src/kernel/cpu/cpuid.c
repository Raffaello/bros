#include <cpu/cpuid.h>
#include <lib/def.h>

void cpuid(CPU_Info_t *cpuid)
{
    if (cpuid == NULL)
        return;

    __asm__("cpuid");
}