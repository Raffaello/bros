// NOTE: the generated assembly seems sub-optimal..
//       looks better just implement them directly in asm

#include <lib/io.h>


uint8_t inb(const uint16_t port)
{
    register uint8_t ret;
    __asm__ volatile("in %0,%1" : "=a"(ret) : "dN"(port));

    return ret;
}

inline void outb(const uint16_t port ,const uint8_t data)
{
    __asm__ volatile("out %0,%1" : :"dN"(port), "a" (data));
}

uint16_t inw(const uint16_t port)
{
    register uint16_t ret;
    __asm__ volatile("in %0,%1" : "=a" (ret) : "dN"(port));

    return ret;
}

void outw(const uint16_t port, const uint16_t data )
{
    __asm__ volatile("out %0,%1" : :"d"(port), "a" (data));
}
