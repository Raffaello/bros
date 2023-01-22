#include <stdint.h>
#include <bios/vga.h>
#include <lib/mem.h>
#include <cpu/GDT_IDT.h>
#include <lib/std.h>
#include <drivers/PIC.h>
#include <drivers/PIT.h>
#include <lib/ISR.h>

#ifndef KERNEL_SEG
    #error KERNEL_SEG define missing
#endif
#define KERNEL_ADDR ((uint32_t*)(KERNEL_SEG))

void main();
// TODO: this is a basic int handlers, all ring0 for now
// void init_interrupt_handlers();
// void init_exception_handlers();

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 __attribute__((force_align_arg_pointer))
void _start()
{
    __asm__ ("cli");
    const uint32_t* _startPtr = (uint32_t*)&_start;
    // if not in the Kernel aspected address...
    if(_startPtr != KERNEL_ADDR)
     {
         // TODO display an error message
        VGA_WriteChar(20,20,'Z',15);
        __asm__("hlt");
    }

    init_descriptor_tables();
    PIC_init();
    PIT_init();
    ISR_init();
    __asm__("sti");
    main();
}

void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    VGA_clear();
    VGA_WriteString(20, 10, hello_msg, 15);

    VGA_enable_cursor(0, 0);
    VGA_update_cursor(0, 24);

// TEST int handler
    //  __asm__("int 5");
     __asm__("int 4");

    while(1);
}
