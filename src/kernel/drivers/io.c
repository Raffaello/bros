// NOTE: the generated assembly seems sub-optimal..
//       looks better just implement them directly in asm

#include <drivers/io.h>


uint8_t inb(const uint16_t port)
{
    uint8_t ret;
    __asm__("in %0,%1" : "=a"(ret) : "Nd"(port));

    return ret;
}

void outb(const uint16_t port ,const uint8_t data)
{
    __asm__("out %1,%0" : :"a" (data), "d"(port));
}

uint16_t inw(const uint16_t port)
{
    uint16_t result=0;
    __asm__("in %0,%1" : "=a" (result) : "Nd"(port));

    return result;
}

void outw(const uint16_t port, const uint16_t data )
{
    __asm__("out %1,%0" : :"a" (data), "d"(port));
}
