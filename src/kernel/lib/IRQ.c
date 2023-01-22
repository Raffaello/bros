// ===============================================
// ***             IRQ Section                 ***
// ===============================================
#include <lib/IRQ.h>
#include <defs.h>
#include <defs/IRQ.h>
#include <defs/PIC.h>
#include <drivers/PIC.h>
#include <cpu/GDT_IDT.h>

IRQ_Handler_t irq_handlers[IRQ_TOTAL];

void IRQ_UniversalHandler(IRQ_registers_t r)
{
    PIC_EOI(r.irq_no);

    if (irq_handlers[r.irq_no] != NULL)
    {
       IRQ_Handler_t handler = irq_handlers[r.irq_no];
       handler(r);
    }
}

extern void IRQ_INT_0();
extern void IRQ_INT_1();
extern void IRQ_INT_2();
extern void IRQ_INT_3();
extern void IRQ_INT_4();
extern void IRQ_INT_5();
extern void IRQ_INT_6();
extern void IRQ_INT_7();
extern void IRQ_INT_8();
extern void IRQ_INT_9();
extern void IRQ_INT_10();
extern void IRQ_INT_11();
extern void IRQ_INT_12();
extern void IRQ_INT_13();
extern void IRQ_INT_14();
extern void IRQ_INT_15();


void IRQ_init()
{
    // Irq Timer set up in PIT, shoudl be set up here instead?
    // IDT_set_gate(PIC1_BASE_INT + IRQ_TIMER, PIT_timer_interrupt_handler);
    IDT_set_gate(PIC1_BASE_INT + IRQ_TIMER, IRQ_INT_0);
    IDT_set_gate(PIC1_BASE_INT + IRQ_KEYBOARD, IRQ_INT_1);
    IDT_set_gate(PIC1_BASE_INT + IRQ_CASCADE, IRQ_INT_2);
    IDT_set_gate(PIC1_BASE_INT + IRQ_SERIAL2, IRQ_INT_3);
    IDT_set_gate(PIC1_BASE_INT + IRQ_SERIAL1, IRQ_INT_4);
    IDT_set_gate(PIC1_BASE_INT + IRQ_PARALLEL2, IRQ_INT_5);
    IDT_set_gate(PIC1_BASE_INT + IRQ_DISKETTE, IRQ_INT_6);
    IDT_set_gate(PIC1_BASE_INT + IRQ_PARALLEL1, IRQ_INT_7);

    IDT_set_gate(PIC2_BASE_INT + IRQ_CMOSTIMER, IRQ_INT_8);
    IDT_set_gate(PIC2_BASE_INT + IRQ_CGARETRACE, IRQ_INT_9);
    IDT_set_gate(PIC2_BASE_INT + IRQ_OPEN1, IRQ_INT_10);
    IDT_set_gate(PIC2_BASE_INT + IRQ_OPEN2, IRQ_INT_11);
    IDT_set_gate(PIC2_BASE_INT + IRQ_AUX, IRQ_INT_12);
    IDT_set_gate(PIC2_BASE_INT + IRQ_FPU, IRQ_INT_13);
    IDT_set_gate(PIC2_BASE_INT + IRQ_ATA_PRIMARY, IRQ_INT_14);
    IDT_set_gate(PIC2_BASE_INT + IRQ_ATA_SECONDARY, IRQ_INT_15);
}

void IRQ_register_interrupt_handler(uint8_t n, IRQ_Handler_t handler)
{
    // TODO: it can be more efficient havinga IDT_handler of 255 element instead
    //       unifying IRQ & INT in 1 file ... 
    //       do it later
    if(n >= IRQ_TOTAL)
        return;

    irq_handlers[n] = handler;
}
