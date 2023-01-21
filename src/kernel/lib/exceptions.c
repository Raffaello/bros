#include <lib/exceptions.h>
#include <cpu/GDT_IDT.h>
#include <lib/interrupts.h>
#include <bios/vga.h>
#include <drivers/PIC.h>

static void printError(const char* str)
{
    VGA_clear();
    VGA_WriteString(0,1, str, 15); // this is not working ...
}

__attribute__((naked)) void divide_error()
{
    printError("Division by Zero");
    PIC_EOI(INT_Divide_Error); // Not sure this is required...
    __asm__("iretd");
}


void init_exception_handlers()
{
    IDT_set_gate(INT_Divide_Error, divide_error);
}