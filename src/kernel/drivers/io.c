#include <drivers/io.h>

uint8_t inb(const uint16_t port)
{
    uint8_t ret;
    asm("in %0,%1" : "=a"(ret) : "Nd"(port));

    return ret;
}

void outb(const uint16_t port ,const uint8_t data)
{
    asm ("out %1,%0" : :"a" (data), "d"(port));
}

uint16_t inw(const uint16_t port)
{
    uint16_t result=0;
    asm("in %0,%1" : "=a" (result) : "Nd"(port));

    return result;
}

void outw(const uint16_t port, const uint16_t data )
{
    asm("out %1,%0" : :"a" (data), "d"(port));
}
