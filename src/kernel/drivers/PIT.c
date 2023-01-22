// #include <drivers/PIT.h>
// #include <drivers/PIC.h>
// #include <cpu/GDT_IDT.h>

// #define PIT_TIMER PIC1_BASE_INT + IRQ_TIMER

// extern void PIT_timer_interrupt_handler();

void PIT_init()
{
    // Timer
    // IDT_set_gate(PIT_TIMER, PIT_timer_interrupt_handler);
}
