#include <lib/ISR.h>
#include <defs/interrupts.h>
#include <cpu/GDT_IDT.h>

#include <bios/vga.h>
#include <lib/std.h>

#pragma pack(push, 1)
typedef struct ISR_registers_t
{
   uint16_t ds;                                         // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;     // Pushed by pusha.
   uint32_t int_no, err_code;                           // Interrupt number and error code
   uint32_t eip, cs, eflags, useresp, ss;               // Pushed by the processor automatically, restored by iretd.

} __attribute__((packed)) ISR_registers_t;
#pragma pack(pop)

void ISR_UniversalHandler(ISR_registers_t r)
{
    char buf[10];
    
    VGA_clear();

    itoa10(r.ds, buf);
    VGA_WriteString(1,1,buf, 15);
    itoa10(r.int_no, buf);
    VGA_WriteString(1,2,buf, 15);
    itoa10(r.err_code, buf);
    VGA_WriteString(1,3,buf, 15);
}

// TODO do it with a macro?
// #define ISR_INT_MAX 21

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
    IDT_set_gate(INT_Divide_Error, ISR_INT_0);
    IDT_set_gate(INT_Debug_Exception, ISR_INT_1);
    IDT_set_gate(INT_NMI_Interrupt, ISR_INT_2);
    IDT_set_gate(INT_Breakpoint, ISR_INT_3);
    IDT_set_gate(INT_Overflow, ISR_INT_4);
    IDT_set_gate(INT_Bbounds_Range_Exceeded, ISR_INT_5);
    IDT_set_gate(INT_Invalid_Opcode, ISR_INT_6);
    IDT_set_gate(INT_Device_Not_Available, ISR_INT_7);
    IDT_set_gate(INT_Double_Falt, ISR_INT_8);
    IDT_set_gate(INT_Coprocessor_Segment_Overrun, ISR_INT_9);
    IDT_set_gate(INT_Invalid_TSS, ISR_INT_10);
    IDT_set_gate(INT_Segment_Not_Present, ISR_INT_11);
    IDT_set_gate(INT_Stack_Segment_Fault, ISR_INT_12);
    IDT_set_gate(INT_General_Protection, ISR_INT_13);
    IDT_set_gate(INT_Page_Fault, ISR_INT_14);
    IDT_set_gate(INT_Intel_Reserved, ISR_INT_15);
    IDT_set_gate(INT_FPU_Error, ISR_INT_16);
    IDT_set_gate(INT_Alignment_Check, ISR_INT_17);
    IDT_set_gate(INT_Machine_Check, ISR_INT_18);
    IDT_set_gate(INT_SIMD_Floating_Point_Exception, ISR_INT_19);
    IDT_set_gate(INT_Virtualization_Exception, ISR_INT_20);
    IDT_set_gate(INT_Control_Protection_Exception, ISR_INT_21);
}


