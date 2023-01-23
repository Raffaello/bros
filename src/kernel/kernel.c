#include <stdint.h>
#include <bios/vga.h>
#include <lib/mem.h>
#include <cpu/GDT_IDT.h>
#include <lib/std.h>
#include <drivers/PIC.h>
#include <drivers/PIT.h>
#include <lib/ISR.h>
#include <lib/IRQ.h>
#include <defs/boot_SYS_Info.h>
#include <lib/conio.h>

#ifndef KERNEL_SEG
    #error KERNEL_SEG define missing
#endif
#define KERNEL_ADDR ((uint32_t*)(KERNEL_SEG))

void main();
void start_failure();

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 __attribute__((force_align_arg_pointer))
void _start()
{
    __asm__ ("cli");

    uint32_t _eax, _ebx;
    __asm__ volatile("mov %0, eax" : "=a"(_eax));
    __asm__ volatile("mov %0, ebx" : "=a"(_ebx));
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
    // check boot sector EAX value
    if(_eax != (uint32_t)('BROS'))
#pragma GCC diagnostic pop
    {
        start_failure();
    }
    // point to EBX SYS_INFO struct
    boot_SYS_Info_t* _sys_info = (boot_SYS_Info_t*) _ebx;
    if(_sys_info->begin_marker != 0x22345678 || _sys_info->end_marker != 0x87654321)
    {
         start_failure();
    }
    
    const uint32_t* _startPtr = (uint32_t*)&_start;
    // if not in the Kernel aspected address...
    if(_startPtr != KERNEL_ADDR)
    {
        start_failure();
    }

    init_descriptor_tables();

    PIC_init();
    ISR_init();
    IRQ_init();
    PIT_init(1000);

    __asm__("sti");
    main();
}

void start_failure()
{
    const char fail_msg[] = "Kernel load failure";
    
    VGA_fill(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    VGA_WriteString(0,0, fail_msg, VGA_COLOR_WHITE);
    __asm__("hlt");
    while(1);
}

void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    VGA_clear();
    VGA_WriteString(20, 10, hello_msg, 15);

    VGA_enable_cursor(0, 0);
    VGA_update_cursor(0, 24);
    VGA_scroll_down();

// TEST int handler
    //  __asm__("int 5");
    //  __asm__("int 4");

    while(1);
}
