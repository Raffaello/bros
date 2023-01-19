#include <common/GDT.h>
// #include <mem.h>

void gdt_load(const GDT_descriptor_t* gdtd)
{
    __asm__("lgdt [%0]" : : "a"(gdtd));
}

void gdt_set_descriptor()
{
    int buf[10];
    // memset(buf,0,1);
}
