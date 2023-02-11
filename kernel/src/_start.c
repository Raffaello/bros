/*
 * the code generated from the functions in this file are "disposable"
 * used only once as entry point, afterwards the code could be reclaimed.
 *
*/

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <bios/boot_info.h>
#include <bios/vga.h>
#include <lib/conio.h>
#include <arch/x86/PIC.h>
#include <arch/x86/PIT.h>
#include <arch/x86/ISR_IRQ.h>
#include <arch/x86/descriptors.h>
#include <arch/x86/mmu/PMM.h>
#include <arch/x86/mmu/VMM.h>


noreturn void  _start()         __attribute__((section(".text._start"), naked, weak));

noreturn void _start_init()     __attribute__((section(".text._start_init"), force_align_arg_pointer, weak));

noreturn void _start_failure()  __attribute__((section(".text._start_failure"), weak));

void _start_VGA_init()          __attribute__((section(".text._start_VGA_init"), weak));

void _start_boot_info(boot_SYS_Info_t* _sys_info)
                                __attribute__((section(".text._start_boot_info"), weak));

void _start_PMM_init(boot_SYS_Info_t* _sys_info, const paddr_t kernel_end, const uint32_t kernel_size)
                                __attribute__((section(".text._start_PMM_init"), weak));

/*
 * TODO:
 *  conforming freestanding implementation is only required to provide certain library facilities:
 *  those in <float.h>, <limits.h>, <stdarg.h>, and <stddef.h>; since AMD1, also those in <iso646.h>;
 *  since C99, also those in <stdbool.h> and <stdint.h>; and since C11, also those in <stdalign.h> 
 * and <stdnoreturn.h>. In addition, complex types, added in C99, 
 * are not required for freestanding implementations.
*/

#if (!defined(DEBUG) && !defined(NDEBUG))
#error "DEBUG or NDEBUG is not defined"
#endif


#ifndef KERNEL_SEG
    #error KERNEL_SEG define missing
#endif
#define KERNEL_ADDR ((uint32_t*)(KERNEL_SEG))

noreturn void  _start()
{
    // extern uint32_t __stack_end;

    __asm__ ("cli");


    // TODO relocate the kernel to where?
    //      need to have the memory map info before
    //      moving it around ... right?

    // TODO so the same is for setting up the stack.
    //      also the stack could benefit of its own selector


    // TODO move the minimum requirement here before jumping
    //      somewhere else.

    // __asm__ volatile("mov esp, %0"::"i"(&__stack_end));
    __asm__ volatile("jmp %0"::"i"(&_start_init));
}

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
 
noreturn void _start_init()
{
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
    extern const uint32_t __end;
    // extern const uint32_t __size;
    // NOTE: with optimization there is a difference about alignment main is 16 bytes aligned
    extern void main();
    const uint32_t kernel_size = (uint32_t)&__end - (uint32_t)(&main);

    if(_eax != __BROS
        || _sys_info->begin_marker != SYS_INFO_BEGIN
        || *_sys_info_end_marker != SYS_INFO_END
        || _startPtr != KERNEL_ADDR
    ) {
         _start_failure();
    }

    // Init VGA Console
    _start_VGA_init();

    /* before paging or interrupt can be done after paging too? */
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
    // boot info sanitize
    _start_boot_info(_sys_info);
    // PMM
    _start_PMM_init(_sys_info, (paddr_t)&__end, kernel_size);
    // VMM
    CON_puts("Init VMM\n");
    if (!VMM_init())
        _start_failure();


    // TODO: self-relocate the kernel
    // TODO to self-relocate the kernel, when? if doing it here can't drop this function,
    //      i should do at the end before calling main, so i can drop the _start* functions
    // TODO to do optimally all the instruction and functions called here to do init,
    //      eventually could be reclaimed as free memory as that code won't be used anymore i guess

    // self-relocating kernel, from main();


    // TODO: init other cpu cores...


    // // TODO: set up kernel stack, EBP,ESP ... and align it
    __asm__ volatile ("sti");
    __asm__ volatile ("jmp %0"::"i"(&main));
    for(;;);
}


// TODO create a kernel panic
noreturn void _start_failure()
{
    const char fail_msg[] = "Kernel load failure";
    
    VGA_fill(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    VGA_WriteString(0,0, fail_msg, VGA_COLOR_WHITE);
    __asm__("hlt");
    for(;;);
}

void _start_VGA_init()
{
    const int cur_offs = VGA_get_cursor_offset();
    CON_gotoXY(cur_offs % 80, cur_offs / 80);
    VGA_enable_cursor(0, 0);
    CON_setConsoleColor2(VGA_COLOR_BLACK, VGA_COLOR_GREEN);
#ifdef DEBUG
    CON_puts("Console Init (DEBUG)\n");
#else
    CON_puts("Console Init\n");
#endif
}

void _start_boot_info(boot_SYS_Info_t* _sys_info)
{
    uint32_t tot_mem = _sys_info->tot_mem;
    boot_info_sanitize(&tot_mem, _sys_info->num_mem_map_entries, MEM_MAP_ENTRY_PTR(_sys_info));
    _sys_info->tot_mem = tot_mem;
    CON_printf("Total Available Memory: %u MB\n", tot_mem/1024);
    if (_sys_info->num_mem_map_entries < 1)
        _start_failure();
}

void _start_PMM_init(boot_SYS_Info_t* _sys_info, const paddr_t kernel_end, const uint32_t kernel_size)
{
    CON_puts("Init PMM\n");
    PMM_init(_sys_info->tot_mem, kernel_end);
    CON_puts("Memory Regions\n");
    con_col_t old_col = CON_getConsoleColor();
    CON_setConsoleColor2(VGA_COLOR_RED, VGA_COLOR_BRIGHT_CYAN);
    volatile boot_MEM_MAP_Info_Entry_t* mem_map = MEM_MAP_ENTRY_PTR(_sys_info);
    for(int i = 0, tot = _sys_info->num_mem_map_entries; i < tot; i++)
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
    PMM_MemMap_deinit_kernel(KERNEL_SEG, kernel_size);

    CON_setConsoleColor((con_col_t){.bg_col=VGA_COLOR_BLUE, .fg_col=VGA_COLOR_YELLOW});
    CON_printf("PMM Blocks: used=%u --- free=%u\n", PMM_Blocks_used(), PMM_Blocks_free());
    CON_setConsoleColor(old_col);
}
