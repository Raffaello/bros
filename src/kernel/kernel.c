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

noreturn void main() __attribute__((section(".text.main")));
noreturn void _start_failure() __attribute__((section(".text._start_failure")));

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
// __attribute__((force_align_arg_pointer))
__attribute__((section(".text._start"))) noreturn void _start()
{
    __asm__ ("cli");

    uint32_t _eax, _ebx;
    __asm__ volatile("mov %0, eax" : "=m"(_eax));
    __asm__ volatile("mov %0, ebx" : "=m"(_ebx));

// TODO to self-relocate the kernel, when? if doing here can't drop this function,
//      i should do at the end before calling main, so i can drop the _start* functions

    // self-relocating kernel, from main();
    extern const uint32_t __end;
    const uint32_t kernel_size = (uint32_t)&__end - (uint32_t)(&main);
    extern const uint32_t __size;
    if(kernel_size == 0) {
        _start_failure();
    }

if(kernel_size != (uint32_t)&__size) {
        _start_failure();
    }


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
    uint32_t __BROS = (uint32_t)('BROS');
#pragma GCC diagnostic pop
    // 1. check boot sector EAX value
    // 2. point to EBX SYS_INFO struct
    // 3. if not in the Kernel aspected address...
    boot_SYS_Info_t* _sys_info = (boot_SYS_Info_t*) _ebx;
    const uint32_t* _sys_info_end_marker = SYS_INFO_END_MARKER_PTR(_sys_info);
    const uint32_t* _startPtr = (uint32_t*)&_start;
    if(_eax != __BROS 
        || _sys_info->begin_marker != SYS_INFO_BEGIN
        || *_sys_info_end_marker != SYS_INFO_END
        || _startPtr != KERNEL_ADDR)
    {
         _start_failure();
    }

    // TODO: set up paging...


    boot_info_init(_sys_info->tot_mem, _sys_info->num_mem_map_entries, MEM_MAP_ENTRY_PTR(_sys_info));

    // TODO MEM_MAP_Info related
    // TODO remove this block later on...
    if (_sys_info->num_mem_map_entries > 0) {
      VGA_clear();
      con_col_t cc;
      cc.fg_col=VGA_COLOR_BRIGHT_CYAN;
      cc.bg_col=VGA_COLOR_RED;
      CON_setConsoleColor(cc);
      CON_puts("TEST: ");
      const boot_MEM_MAP_Info_Entry_t* mem_map = MEM_MAP_ENTRY_PTR(_sys_info);
      for(int i = 0; i < _sys_info->num_mem_map_entries; ++i) {
          const char mem_type_msg[] = "mem type: ";
          const char a_msg[] = "available";
          const char r_msg[] = "reserved";
          const char c_msg[] = "ACPI recl";
          const char n_msg[] = "ACPI nvs";
          const char e_msg[] = "error";

          CON_puts(mem_type_msg);
          switch(mem_map[i].type)
          {
              case MEM_MAP_TYPE_AVAILABLE: 
                CON_puts(a_msg);
                break;
            case MEM_MAP_TYPE_RESERVED: 
                CON_puts(r_msg);
                break;
            case MEM_MAP_TYPE_ACPI_RECLAIM:
                CON_puts(c_msg);
                break;
            case MEM_MAP_TYPE_ACPI_NVS:
                CON_puts(n_msg);
                break;
            default:
                CON_puts(e_msg);
                break;
          }
          CON_newline();
      }
    }

    // TODO: self-relocate the kernel

    // TODO: init other cores...


    init_descriptor_tables();

    PIC_init();
    ISR_init();
    IRQ_init();
    PIT_init(10);

    // TODO: set up kernel stack, EBP,ESP ... and align it
    __asm__ volatile("mov esp, 0x9000");
    __asm__ volatile("mov ebp, esp");
    __asm__("sti");
    main();
}


noreturn void _start_failure()
{
    const char fail_msg[] = "Kernel load failure";
    
    VGA_fill(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    VGA_WriteString(0,0, fail_msg, VGA_COLOR_WHITE);
    __asm__("hlt");
    while(1);
}

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
// TODO remove later when manually aligned in _start
 __attribute__((force_align_arg_pointer))
noreturn void main()
{
    const char hello_msg[] = "*** HELLO FROM BROSKRNL.SYS ***";

    // VGA_clear();
    VGA_WriteString(20, 10, hello_msg, 15);

    VGA_enable_cursor(0, 0);
    VGA_update_cursor(0, 24);

// TEST int handler
    //  __asm__("int 5");
    //  __asm__("int 4");

    while(1);
}
