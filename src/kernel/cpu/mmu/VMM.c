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

//////// TEST ///////////
// uint32_t* page_directory;
// uint32_t* first_page_table;


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
    // page_table_t*       page_table2 = PMM_malloc(sizeof(page_table_t));

    if(page_table == NULL /*|| page_table2 == NULL */|| _kernel_directory == NULL)
        return false;

    memset(_kernel_directory, 0, sizeof(page_directory_t));
    memset(page_table, 0, sizeof(page_table_t));
    // memset(page_table2, 0, sizeof(page_table_t));
 
    // TODO: forgot to allocate some space for the stack ... just inside the linker at the moment

    // first 1MB
    for (uint32_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
    {
        // PTE_t* page = &page_table->entries[i];
        // page->p     = 1;
        // page->rw    = 1;
        // page->frame = (i << 12); // * PAGE_SIZE (4096) , physical addr

        page_table->entries[i] = (PTE_t){((i << 12) | 3)};

    }

//     for (int i = 0, frame = 0x100000, virt = 0xc0000000; i<PAGE_TABLE_ENTRIES; i++, frame+=PAGE_SIZE, virt+=PAGE_SIZE)
//     {
//         PTE_t page = {0};
//         page.p = 1;
//         page.frame = frame;
//         page_table->entries[PAGE_TABLE_INDEX(virt)] = page;
//    }

    // PDE_t* entry = &_kernel_directory->entries[0];
    // entry->p = 1;
    // entry->rw = 1;
    // entry->page_table = (uint32_t)page_table;
    _kernel_directory->entries[0] = (PDE_t)(page_table) | 3;

    // PDE_t* entry2 = &page_dir->entries[PAGE_DIR_INDEX(0)];
    // entry2->p = 1;
    // entry2->rw = 1;
    // // entry2->page_table = (uint32_t)page_table2;
    // entry2->page_table = (uint32_t) page_table;

    ISR_register_interrupt_handler(INT_Page_Fault, page_fault_handler);
    VMM_switch_page_directory(_kernel_directory);
    VMM_enable_paging();

    return true;

 ///// TEST //////
// page_directory = PMM_malloc(1024);
// memset(page_directory, 0,1024);
// first_page_table = PMM_malloc(1024);
// memset(first_page_table, 0,1024);


// for(int i = 0; i < 1024; i++)
// {
//     // This sets the following flags to the pages:
//     //   Supervisor: Only kernel-mode can access them
//     //   Write Enabled: It can be both read from and written to
//     //   Not Present: The page table is not present
//     page_directory[i] = 0x00000002;
// }

// // holds the physical address where we want to start mapping these pages to.
// // in this case, we want to map these pages to the very beginning of memory.
 
// //we will fill all 1024 entries in the table, mapping 4 megabytes
// for(unsigned int i = 0; i < 1024; i++)
// {
//     // As the address is page aligned, it will always leave 12 bits zeroed.
//     // Those bits are used by the attributes ;)
//     first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
// }

// // attributes: supervisor level, read/write, present
// page_directory[0] = ((unsigned int)first_page_table) | 3;
//  __asm__ volatile("mov cr3, %0": : "a"(page_directory));

// VMM_enable_paging();

// return true;

}

bool VMM_switch_page_directory(page_directory_t* page_directory)
{
    if(page_directory == NULL)
        return false;

    _current_directory = page_directory;
    __asm__ volatile("mov cr3, %0": : "a"(page_directory));

    return true;
}

inline void VMM_enable_paging()
{
    __asm__ volatile("mov eax, cr0");
    __asm__ volatile("or eax, %0"::"i"(CR0_PG_MASK));
    __asm__ volatile("mov cr0, eax");
}

inline void VMM_disable_paging()
{
    __asm__ volatile("mov eax, cr0");
    __asm__ volatile("or eax, %0":: "i"(~CR0_PG_MASK));
    __asm__ volatile("mov cr0, eax");
}
