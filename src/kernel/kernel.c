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
// #include <defs/boot_MEM_MAP_Info.h>
#include <lib/conio.h>

#include <stdnoreturn.h>
#include <stddef.h>   // this could replace defs.h, but wasn't compiling last time when i tried.

/*
 * TODO:
 *  conforming freestanding implementation is only required to provide certain library facilities:
 *  those in <float.h>, <limits.h>, <stdarg.h>, and <stddef.h>; since AMD1, also those in <iso646.h>;
 *  since C99, also those in <stdbool.h> and <stdint.h>; and since C11, also those in <stdalign.h> 
 * and <stdnoreturn.h>. In addition, complex types, added in C99, 
 * are not required for freestanding implementations.


*/


#ifndef KERNEL_SEG
    #error KERNEL_SEG define missing
#endif
#define KERNEL_ADDR ((uint32_t*)(KERNEL_SEG))

void main();
void start_failure();

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 __attribute__((force_align_arg_pointer))
noreturn void _start()
{
    __asm__ ("cli");

    uint32_t _eax, _ebx, _ecx, _edx;
    __asm__ volatile("mov %0, eax" : "=m"(_eax));
    __asm__ volatile("mov %0, ebx" : "=m"(_ebx));
    // TODO: ecx register is overwritten at the begining of the function.
    __asm__ volatile("mov %0, ecx" : "=m"(_ecx)); 
    __asm__ volatile("mov %0, edx" : "=m"(_edx));
    // TODO: set up kernel stack, EBP,ESP ...

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
    uint32_t __BROS = (uint32_t)('BROS');
#pragma GCC diagnostic pop
    // check boot sector EAX value
    // point to EBX SYS_INFO struct
    // if not in the Kernel aspected address...
    boot_SYS_Info_t* _sys_info = (boot_SYS_Info_t*) _ebx;
    const uint32_t* _startPtr = (uint32_t*)&_start;
    if(_eax != __BROS 
        || _sys_info->begin_marker != SYS_INFO_BEGIN
        || _sys_info->end_marker != SYS_INFO_END
        || _startPtr != KERNEL_ADDR)
    {
         start_failure();
    }

    // TODO MEM_MAP_Info related redo it later.. it is a contiguos of _ecx entries
    // boot_MEM_MAP_Info_Entry_t* _mem_map_info_entry =  (boot_MEM_MAP_Info_Entry_t*) _edx;
    // uint32_t _mem_map_info_size = _ecx;

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

noreturn void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    VGA_clear();
    VGA_WriteString(20, 10, hello_msg, 15);

    VGA_enable_cursor(0, 0);
    VGA_update_cursor(0, 24);

// TEST int handler
    //  __asm__("int 5");
    //  __asm__("int 4");

    while(1);
}
