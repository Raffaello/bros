#include <cpu/mmu/VMM.h>
#include <cpu/mmu/PMM.h>
#include <lib/string.h>
#include <defs/interrupts.h>
#include <lib/ISR_IRQ.h>

#define PAGE_SIZE   4096

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDR(x) (*x & ~0xFFF)

#define CR0_PG_MASK 0x80000000

static page_directory_t* _kernel_directory  = NULL;
static page_directory_t* _current_directory = NULL;

void page_fault_handler(ISR_registers_t regs)
{
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    uint32_t faulting_address;
    __asm__ volatile("mov %0, cr2" : "=r" (faulting_address));

    // TODO 
    
    // The error code gives us details of what happened.
    // int present   = !(regs.err_code & 0x1); // Page not present
    // int rw = regs.err_code & 0x2;           // Write operation?
    // int us = regs.err_code & 0x4;           // Processor was in user-mode?
    // int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    // int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

    // // Output an error message.
    // monitor_write("Page fault! ( ");
    // if (present) {monitor_write("present ");}
    // if (rw) {monitor_write("read-only ");}
    // if (us) {monitor_write("user-mode ");}
    // if (reserved) {monitor_write("reserved ");}
    // monitor_write(") at 0x");
    // monitor_write_hex(faulting_address);
    // monitor_write("\n");
    // PANIC("Page fault");
}

bool VMM_init()
{
    // TODO pass MemMap informations

    _kernel_directory = PMM_malloc(sizeof(page_directory_t));
    
    page_table_t*       page_table  = PMM_malloc(sizeof(page_table_t));

    if(page_table == NULL || _kernel_directory == NULL)
        return false;

    memset(_kernel_directory, 0, sizeof(page_directory_t));
    memset(page_table, 0, sizeof(page_table_t));
 
    // TODO: forgot to allocate some space for the stack ... just inside the linker at the moment

    // first 1MB
    for (uint32_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
    {
        PTE_t* page = &page_table->entries[i];
        page->p     = 1;
        page->rw    = 1;
        page->frame = i; // * PAGE_SIZE (4096) , physical addr

        // page_table->entries[i] = (PTE_t){((i << 12) | 3)};
    }

    PDE_t* entry = &_kernel_directory->entries[0];
    entry->p = 1;
    entry->rw = 1;
    entry->page_table = ((uint32_t)page_table >> 12);
    // _kernel_directory->entries[0] = ((PDE_t)page_table) | 3;

    ISR_register_interrupt_handler(INT_Page_Fault, page_fault_handler);
    VMM_switch_page_directory(_kernel_directory);
    VMM_enable_paging();

    return true;
}

bool VMM_switch_page_directory(page_directory_t* page_directory)
{
    if(page_directory == NULL)
        return false;

    _current_directory = page_directory;
    __asm__ volatile("mov cr3, %0": : "a"(page_directory));

    return true;
}
