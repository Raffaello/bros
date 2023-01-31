#include <cpu/mmu/paging.h>
#include <lib/string.h>

#define PAGE_SIZE 4096

// Bitsets
static uint32_t* frames;
static uint32_t  nframes;

void paging_init()
{
    // TODO pass memory information from boot info
    // hardcoding now to understand how it works, 16MB ?
    uint32_t mem_end_page = 0x1000000;
    nframes = mem_end_page / PAGE_SIZE; // if not evenly divided what to do with the remaining mem?

    // alloc all frames

    frames = (u32int*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

      // Let's make a page directory.
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    current_directory = kernel_directory;
}
