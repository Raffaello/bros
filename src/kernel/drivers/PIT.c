#include <drivers/PIT.h>
#include <cpu/GDT_IDT.h>

extern void PIT_interrupt_handler();

void PIT_init()
{
    // Install interrupt handler
    IDT_set_IRQ(PIT_INT, PIT_interrupt_handler);
}
