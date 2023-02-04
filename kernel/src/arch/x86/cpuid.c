#include <arch/x86/cpuid.h>
#include <stddef.h>

void cpuid(CPU_Info_t *cpuid)
{
    if (cpuid == NULL)
        return;

    __asm__("cpuid");
}