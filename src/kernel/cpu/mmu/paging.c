#include <cpu/mmu/paging.h>
#include <lib/string.h>
#include <lib/ISR.h>

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
    // Before we enable paging, we must register our page fault handler.
    register_interrupt_handler(14, page_fault);

    // Now, enable paging!
    // switch_page_directory(kernel_directory);
}
