#include <drivers/io.h>

uint8_t inb(const uint16_t port)
{
    uint8_t result;
    asm("in al,dx" : "=a" (result) : "d" (port));

    return result;
}

void outb(const uint16_t port ,const uint8_t data)
{
    asm ("out dx,al" : :"a" (data), "d" (port));
}

uint16_t inw(const uint16_t port)
{
    uint16_t result;
    asm("in ax,dx" : "=a" (result) : "d" (port));

    return result;
}

void outw(const uint16_t port, const uint16_t data )
{
    asm("out dx,ax" : :"a" (data), "d" (port));
}
