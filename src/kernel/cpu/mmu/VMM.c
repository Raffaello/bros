#include <cpu/mmu/VMM.h>
#include <cpu/mmu/PMM.h>
#include <lib/string.h>

#define PAGE_SIZE   4096

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDR(x) (*x & ~0xFFF)

#define CR0_PG_MASK 0x80000000

// static page_directory_t* _kernel_directory  = NULL;
static page_directory_t* _current_directory = NULL;

bool VMM_init()
{
    // TODO pass MemMap informations

    page_table_t*       page_table  = PMM_malloc(sizeof(page_table_t));
    page_table_t*       page_table2 = PMM_malloc(sizeof(page_table_t));
    page_directory_t*   page_dir    = PMM_malloc(sizeof(page_directory_t));

    if(page_table == NULL || page_table2 == NULL || page_dir == NULL)
        return false;

    memset(page_table, 0, sizeof(page_table_t));
    memset(page_table2, 0, sizeof(page_table_t));
    // TODO: this overwrite the actual stack at 0x9000 ...
    // TODO: forgot to allocate some space for the stack ...
    memset(page_dir, 0, sizeof(page_directory_t));

    for (int i = 0, frame = 0, virt = 0; i < PAGE_TABLE_ENTRIES; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
    {
        PTE_t page = {0};
        page.p = 1;
        page.frame = frame;
        page_table2->entries[PAGE_TABLE_INDEX(virt)] = page;
    }

    for (int i = 0, frame = 0x100000, virt = 0xc0000000; i<PAGE_TABLE_ENTRIES; i++, frame+=PAGE_SIZE, virt+=PAGE_SIZE)
    {
        PTE_t page = {0};
        page.p = 1;
        page.frame = frame;
        page_table->entries[PAGE_TABLE_INDEX(virt)] = page;
   }

    //! get first entry in dir table and set it up to point to our table
    PDE_t* entry = &page_dir->entries[PAGE_DIR_INDEX(0xc0000000)];
    entry->p = 1;
    entry->rw = 1;
    entry->page_table = (uint32_t)page_table;

    PDE_t* entry2 = &page_dir->entries[PAGE_DIR_INDEX(0)];
    entry2->p = 1;
    entry2->rw = 1;
    entry2->page_table = (uint32_t)page_table2;

    VMM_switch_page_directory(page_dir);
    VMM_enable_paging();

    return true;
}

bool VMM_switch_page_directory(page_directory_t* page_directory)
{
    if(page_directory == NULL)
        return false;

    _current_directory = page_directory;
    __asm__ volatile("mov cr3, %0": : "a"(&page_directory->entries));

    return true;
}

inline void VMM_enable_paging()
{
    __asm__ volatile("mov ecx, cr0");
    __asm__ volatile("or ecx, %0"::"a"(CR0_PG_MASK));
    __asm__ volatile("mov cr0, ecx");
}

inline void VMM_disable_paging()
{
    __asm__ volatile("mov ecx, cr0");
    __asm__ volatile("or ecx, %0":: "a"(~CR0_PG_MASK));
    __asm__ volatile("mov cr0, ecx");
}
