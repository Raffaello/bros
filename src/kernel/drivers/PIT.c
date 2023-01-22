#include <drivers/PIT.h>
// #include <drivers/PIC.h>
// #include <cpu/GDT_IDT.h>
#include <lib/IRQ.h>

// #define PIT_TIMER PIC1_BASE_INT + IRQ_TIMER

// extern void PIT_timer_interrupt_handler();

uint32_t ticks = 0;

void PIT_init()
{
    // Timer
    
}
