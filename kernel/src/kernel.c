#include <stdint.h>
#include <bios/vga.h>
#include <cpu/GDT_IDT.h>
#include <lib/std.h>
#include <drivers/PIC.h>
#include <drivers/PIT.h>
#include <lib/ISR_IRQ.h>
#include <bios/boot_info.h>
#include <lib/conio.h>

#include <stdnoreturn.h>
#include <stddef.h>

#include <cpu/mmu/PMM.h>
#include <cpu/mmu/VMM.h>

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

noreturn void main()            __attribute__((section(".text.main")));
noreturn void _start_entry()    __attribute__((section(".text._start_entry")));
noreturn void _start_failure()  __attribute__((section(".text._start_failure")));

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 __attribute__((force_align_arg_pointer))
__attribute__((section(".text._start_entry"))) noreturn void _start_entry()
{
    __asm__ ("cli");
    uint32_t _eax, _ebx;
    __asm__ volatile("mov %0, eax" : "=m"(_eax));
    __asm__ volatile("mov %0, ebx" : "=m"(_ebx));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
    uint32_t __BROS = (uint32_t)('BROS');
#pragma GCC diagnostic pop
    // 1. check boot sector EAX value
    // 2. point to EBX SYS_INFO struct
    // 3. if not in the Kernel aspected address...
    boot_SYS_Info_t* _sys_info = (boot_SYS_Info_t*) _ebx;
    const uint32_t* _sys_info_end_marker = SYS_INFO_END_MARKER_PTR(_sys_info);
    extern void _start();
    const uint32_t* _startPtr = (uint32_t*)&_start;
    // self-relocating kernel checks
    extern uint32_t __end;
    extern const uint32_t __size;
    // TODO: with optimization there is a difference...
    const uint32_t kernel_size = (uint32_t)&__end - (uint32_t)(&main);

    if(_eax != __BROS
        || _sys_info->begin_marker != SYS_INFO_BEGIN
        || *_sys_info_end_marker != SYS_INFO_END
        || _startPtr != KERNEL_ADDR
        || kernel_size != (uint32_t)&__size
        || (uint32_t)&main <= (uint32_t)&_start_failure
        || (uint32_t)&main <= (uint32_t)&_start)
    {
         _start_failure();
    }

    // Init VGA Console
    {
        int cur_offs = VGA_get_cursor_offset();
        CON_gotoXY(cur_offs % 80, cur_offs / 80);
        VGA_enable_cursor(0, 0);
        CON_setConsoleColor2(VGA_COLOR_BLACK, VGA_COLOR_GREEN);
        CON_puts("Console Init\n");
    }

    // boot info sanitize
    {
        uint32_t tot_mem = _sys_info->tot_mem;
        boot_info_sanitize(&tot_mem, _sys_info->num_mem_map_entries, MEM_MAP_ENTRY_PTR(_sys_info));
        _sys_info->tot_mem = tot_mem;
        CON_printf("Total Available Memory: %u MB\n", tot_mem/1024);
    }

    CON_puts("Init DTs\n");
    init_descriptor_tables();
    CON_puts("Init PIC\n");
    PIC_init();
    CON_puts("Init ISR\n");
    ISR_init();
    CON_puts("Init IRQ\n");
    IRQ_init();
    CON_puts("Init PIT\n");
    PIT_init(10);

    // PMM
    {
        CON_puts("Init PMM\n");
        PMM_init(_sys_info->tot_mem, (paddr_t)&__end);
        CON_puts("Memory Regions\n");
        con_col_t old_col = CON_getConsoleColor();
        CON_setConsoleColor2(VGA_COLOR_RED, VGA_COLOR_BRIGHT_CYAN);
        const boot_MEM_MAP_Info_Entry_t* mem_map = MEM_MAP_ENTRY_PTR(_sys_info);

        for(int i = 0; i < _sys_info->num_mem_map_entries; ++i)
        {
            const char* mem_types[] = {
                "Available",
                "Reserved",
                "ACPI recl",
                "ACPI nvs",
                // "Bad"
            };

            const boot_MEM_MAP_Info_Entry_t memi = mem_map[i];
            CON_printf(
                "Mem Map %u: start=0x%X:%X --- length=0x%X:%X --- type=%u (%s)\n",
                i,
                memi.base_addr_hi,
                memi.base_addr_lo,
                memi.length_hi,
                memi.length_lo,
                memi.type,
                mem_types[memi.type - 1]
            );

            if(memi.type == MEM_MAP_TYPE_AVAILABLE)
            {
                // Because 32 bits, the High part is always zero.
                // can't address more the 4GB after all..
                PMM_MemMap_init(memi.base_addr_lo, memi.length_lo);
            }
        }

        // reserve the kernel memory area plus the PMM Bit set
        PMM_MemMap_deinit_kernel(KERNEL_SEG);

        CON_setConsoleColor((con_col_t){.bg_col=VGA_COLOR_BLUE, .fg_col=VGA_COLOR_YELLOW});
        CON_printf("PMM Blocks: used=%u --- free=%u\n", PMM_Blocks_used(), PMM_Blocks_free());
        CON_setConsoleColor(old_col);
    }

    // VMM
    {
        CON_puts("Init VMM\n");
        if (!VMM_init())
        {
            _start_failure();
        }
    }


    // TODO: self-relocate the kernel
    // TODO to self-relocate the kernel, when? if doing it here can't drop this function,
    //      i should do at the end before calling main, so i can drop the _start* functions

    // self-relocating kernel, from main();


    // TODO: init other cpu cores...


    // // TODO: set up kernel stack, EBP,ESP ... and align it
    // __asm__ volatile("mov esp, 0x9000");
    // __asm__ volatile("mov ebp, esp");
    __asm__("sti");
    __asm__ volatile("jmp %0"::"i"(&main));
    for(;;);
}


noreturn void _start_failure()
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

    // VGA_clear();
    VGA_WriteString(20, 10, hello_msg, 15);

    VGA_update_cursor(0, 24);

// Page Fault Test
    // intptr_t* t = (intptr_t*)0xFFFFFFFF;
    // *t=0;

    while(1);
}