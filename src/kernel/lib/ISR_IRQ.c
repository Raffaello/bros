#include <lib/ISR_IRQ.h>
#include <stddef.h>
#include <cpu/GDT_IDT.h>
#include <defs/interrupts.h>
#include <defs/IRQ.h>
#include <defs/PIC.h>
#include <drivers/PIC.h>

#include <bios/vga.h>
#include <lib/std.h>

static ISR_Handler_t isr_handlers[INT_TOTAL];

void ISR_UniversalHandler(ISR_registers_t r)
{
    char buf[10];
    
    VGA_clear();

    // itoa10(r.ds, buf);
    // VGA_WriteString(1,1,buf, 15);
    itoa10(r.int_no, buf);
    VGA_WriteString(1,2,buf, 15);
    // itoa10(r.err_code, buf);
    // VGA_WriteString(1,3,buf, 15);

    if (isr_handlers[r.int_no] != NULL)
    {
       ISR_Handler_t handler = isr_handlers[r.int_no];
       handler(r);
    }
}

extern void ISR_INT_0();
extern void ISR_INT_1();
extern void ISR_INT_2();
extern void ISR_INT_3();
extern void ISR_INT_4();
extern void ISR_INT_5();
extern void ISR_INT_6();
extern void ISR_INT_7();
extern void ISR_INT_8();
extern void ISR_INT_9();
extern void ISR_INT_10();
extern void ISR_INT_11();
extern void ISR_INT_12();
extern void ISR_INT_13();
extern void ISR_INT_14();
extern void ISR_INT_15();
extern void ISR_INT_16();
extern void ISR_INT_17();
extern void ISR_INT_18();
extern void ISR_INT_19();
extern void ISR_INT_20();
extern void ISR_INT_21();


void ISR_init()
{
    IDT_set_gate(INT_Divide_Error,                  ISR_INT_0);
    IDT_set_gate(INT_Debug_Exception,               ISR_INT_1);
    IDT_set_gate(INT_NMI_Interrupt,                 ISR_INT_2);
    IDT_set_gate(INT_Breakpoint,                    ISR_INT_3);
    IDT_set_gate(INT_Overflow,                      ISR_INT_4);
    IDT_set_gate(INT_Bbounds_Range_Exceeded,        ISR_INT_5);
    IDT_set_gate(INT_Invalid_Opcode,                ISR_INT_6);
    IDT_set_gate(INT_Device_Not_Available,          ISR_INT_7);
    IDT_set_gate(INT_Double_Falt,                   ISR_INT_8);
    IDT_set_gate(INT_Coprocessor_Segment_Overrun,   ISR_INT_9);
    IDT_set_gate(INT_Invalid_TSS,                   ISR_INT_10);
    IDT_set_gate(INT_Segment_Not_Present,           ISR_INT_11);
    IDT_set_gate(INT_Stack_Segment_Fault,           ISR_INT_12);
    IDT_set_gate(INT_General_Protection,            ISR_INT_13);
    IDT_set_gate(INT_Page_Fault,                    ISR_INT_14);
    IDT_set_gate(INT_Intel_Reserved,                ISR_INT_15);
    IDT_set_gate(INT_FPU_Error,                     ISR_INT_16);
    IDT_set_gate(INT_Alignment_Check,               ISR_INT_17);
    IDT_set_gate(INT_Machine_Check,                 ISR_INT_18);
    IDT_set_gate(INT_SIMD_Floating_Point_Exception, ISR_INT_19);
    IDT_set_gate(INT_Virtualization_Exception,      ISR_INT_20);
    IDT_set_gate(INT_Control_Protection_Exception,  ISR_INT_21);
}

void ISR_register_interrupt_handler(const uint8_t n, ISR_Handler_t handler)
{
    // TODO:
    // Having an unique ISR/IRQ handler is simpler to deal with.
    // also haing a 255 handlers would be simpler to debug.
    if (n >= INT_TOTAL)
        return;

    isr_handlers[n] = handler;
}


// ===============================================
// ***             IRQ Section                 ***
// ===============================================
void IRQ_UniversalHandler(ISR_registers_t r)
{
    PIC_EOI(r.int_no);
    r.int_no += PIC1_BASE_INT; // PIC2_BASE is following the first 8 IRQs

    // ISR_UniversalHandler(r);

    // TODO: can call ISR uni handler, but need to be cleaned up first.
    //       so just calling custom handlers directly here
    if (isr_handlers[r.int_no] != NULL)
    {
       ISR_Handler_t handler = isr_handlers[r.int_no];
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
    // PIC 1
    IDT_set_gate(PIC1_BASE_INT + IRQ_TIMER,         IRQ_INT_0);
    IDT_set_gate(PIC1_BASE_INT + IRQ_KEYBOARD,      IRQ_INT_1);
    IDT_set_gate(PIC1_BASE_INT + IRQ_CASCADE,       IRQ_INT_2);
    IDT_set_gate(PIC1_BASE_INT + IRQ_SERIAL2,       IRQ_INT_3);
    IDT_set_gate(PIC1_BASE_INT + IRQ_SERIAL1,       IRQ_INT_4);
    IDT_set_gate(PIC1_BASE_INT + IRQ_PARALLEL2,     IRQ_INT_5);
    IDT_set_gate(PIC1_BASE_INT + IRQ_DISKETTE,      IRQ_INT_6);
    IDT_set_gate(PIC1_BASE_INT + IRQ_PARALLEL1,     IRQ_INT_7);
    // PIC 2
    IDT_set_gate(PIC2_BASE_INT + IRQ_CMOSTIMER,     IRQ_INT_8);
    IDT_set_gate(PIC2_BASE_INT + IRQ_CGARETRACE,    IRQ_INT_9);
    IDT_set_gate(PIC2_BASE_INT + IRQ_OPEN1,         IRQ_INT_10);
    IDT_set_gate(PIC2_BASE_INT + IRQ_OPEN2,         IRQ_INT_11);
    IDT_set_gate(PIC2_BASE_INT + IRQ_AUX,           IRQ_INT_12);
    IDT_set_gate(PIC2_BASE_INT + IRQ_FPU,           IRQ_INT_13);
    IDT_set_gate(PIC2_BASE_INT + IRQ_ATA_PRIMARY,   IRQ_INT_14);
    IDT_set_gate(PIC2_BASE_INT + IRQ_ATA_SECONDARY, IRQ_INT_15);
}


void IRQ_register_interrupt_handler(const uint8_t n, ISR_Handler_t handler)
{
    // PIC2_BASE_INT is just following
    ISR_register_interrupt_handler(PIC1_BASE_INT + n, handler);
}
