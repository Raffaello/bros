#include <stdint.h>
#include <bios/vga.h>
#include <lib/mem.h>
#include <cpu/GDT_IDT.h>
#include <lib/std.h>
#include <drivers/PIC.h>

#ifndef KERNEL_SEG
    #error KERNEL_SEG define missing
#endif
#define KERNEL_ADDR ((uint32_t*)(KERNEL_SEG))

void main();

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
        writeVGAChar(20,20,'Z',15);
        __asm__("hlt");
    }

    GDT_init();
    IDT_init();
    PIC_init();
    // __asm__("sti"); // something must be set up before enabling it
    main();
}


void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    clearVGA();

    for (int i=0; i<sizeof(hello_msg); i++)
    {
        writeVGAChar(20 + i, 10, hello_msg[i], 15);
    }

    enable_cursor(0, 0);
    update_cursor(0, 24);

// TEST int handler
    // __asm__("int 5");

// Test div by zero, if not catch will reboot! :)
// need to disable some compiling switching for that
// Test for IDT
    // int a = 10/0; // div 0 error!:)
    while(1);
}
