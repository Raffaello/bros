#include <cpu/mmu/paging.h>
#include <lib/string.h>
#include <lib/ISR_IRQ.h>
#include <defs/interrupts.h>

#define PAGE_SIZE   4096
#define CR0_PG_MASK 0x80000000

// Bitsets
static uint32_t* _frames;
static uint32_t  _nframes;

page_directory_t* _kernel_directory  = NULL;
page_directory_t* _current_directory = NULL;

void page_fault_handler(ISR_registers_t regs);
extern void switch_page_directory(page_directory_t *dir);
extern void enable_paging(page_directory_t* page_dir);

void init_paging()
{
    // TODO pass memory information from boot info
    // hardcoding now to understand how it works, 16MB ?
    extern uint32_t __end;
    const uint32_t mem_end_page = 0x1000000;
    const uint32_t mem_start_page = (uint32_t)&__end;
    _nframes = mem_end_page / PAGE_SIZE; // if not evenly divided what to do with the remaining mem?
    _frames = &__end;
    // alloc all frames

    // TODO

    // frames = (u32int*)kmalloc(INDEX_FROM_BIT(nframes));
    // memset(frames, 0, INDEX_FROM_BIT(nframes));

      // Let's make a page directory.
    // kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    // current_directory = kernel_directory;

    // must be 4KB aligned
    _kernel_directory = (page_directory_t*)&__end;
    _current_directory = _kernel_directory;
    for(int i = mem_start_page; i < mem_end_page; i+=PAGE_SIZE)
    {

    }

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    // int i = 0;
    // while (i < placement_address)
    // {
    //     // Kernel code is readable but not writeable from userspace.
    //     alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
    //     i += 0x1000;
    // }

    ISR_register_interrupt_handler(INT_Page_Fault, page_fault_handler);
    // enable_paging(_kernel_directory);
    // switch_page_directory(_kernel_directory);
}

// void switch_page_directory(page_directory_t *dir)
// {
// //    current_directory = dir;
// //    __asm__ volatile("mov cr3, %0":: "r"(&dir->page_table_physical));
//    uint32_t _cr0;
//    __asm__ volatile("mov %0, cr0": "=r"(_cr0));
//    _cr0 |= CR0_PG_MASK; // Enable paging
// //    __asm__ volatile("mov cr0, %0":: "r"(_cr0));
// }

// void page_fault_handler(ISR_registers_t regs)
// {
//     // A page fault has occurred.
//     // The faulting address is stored in the CR2 register.
//     uint32_t faulting_address;
//     __asm__ volatile("mov %0, cr2" : "=r" (faulting_address));

//     // TODO 
    
//     // The error code gives us details of what happened.
//     // int present   = !(regs.err_code & 0x1); // Page not present
//     // int rw = regs.err_code & 0x2;           // Write operation?
//     // int us = regs.err_code & 0x4;           // Processor was in user-mode?
//     // int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
//     // int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

//     // // Output an error message.
//     // monitor_write("Page fault! ( ");
//     // if (present) {monitor_write("present ");}
//     // if (rw) {monitor_write("read-only ");}
//     // if (us) {monitor_write("user-mode ");}
//     // if (reserved) {monitor_write("reserved ");}
//     // monitor_write(") at 0x");
//     // monitor_write_hex(faulting_address);
//     // monitor_write("\n");
//     // PANIC("Page fault");
// }