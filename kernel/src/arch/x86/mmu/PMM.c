#include <stddef.h>
#include <arch/x86/mmu/PMM.h>
#include <lib/bitset.h>
#include <lib/conio.h>
#include <lib/string.h>
#include <sys/panic.h>

#define PMM_MEM_MAP_FRAMES_PER_BYTE 8
#define PMM_FRAME_SIZE              4096
#define PMM_FRAME_ALIGN             PMM_FRAME_SIZE


// static uint32_t   g_PMM_tot_mem_KB   = 0;    // should be size_t ?
static uint32_t   g_PMM_total_frames = 0;
static uint32_t   g_PMM_used_frames  = 0;
static bitset32_t g_PMM_mem_map      = NULL;
static uint32_t   g_PMM_mem_map_size = 0;
static paddr_t    g_PMM_cur_paddr    = 0;

static uint64_t align_up(uint64_t value, uint64_t align)
{
    return (value + align - 1) & ~(align - 1);
}

static inline void _PMM_used_frames_panic(const char* msg)
{
    char str[128];
    CON_sprintf(str, "%s: %u (%u)", msg, g_PMM_used_frames, g_PMM_total_frames);
    KERNEL_PANIC(str);
}

static inline size_t _size2frame(const size_t size)
{
    return (size / PMM_FRAME_SIZE) + ((size % PMM_FRAME_SIZE) ? 1 : 0);
}

static paddr_t _PMM_malloc_frames(const size_t num_blocks)
{
    if (PMM_frames_free() < num_blocks)
        return 0;

    uint32_t pos;
    if (!bitset_find(g_PMM_mem_map, g_PMM_mem_map_size, num_blocks, &pos))
        return 0;

    for (size_t i = 0; i < num_blocks; ++i)
        bitset_set(g_PMM_mem_map, pos + i);

    g_PMM_used_frames += num_blocks;

    return pos * PMM_FRAME_SIZE;
}

static void _PMM_free_frames(paddr_t ptr, const size_t num_frames)
{
    if (g_PMM_used_frames < num_frames)
        _PMM_used_frames_panic("PMM_free_frames");

    unsigned int pos = ptr / PMM_FRAME_SIZE;
    for (size_t i = 0; i < num_frames; i++)
        bitset_clear(g_PMM_mem_map, pos + i);
    g_PMM_used_frames -= num_frames;
}

void PMM_init(uint32_t num_entries, const volatile boot_MEM_MAP_Info_Entry_t* mem_map, const paddr_t physical_mem_start, const uint32_t kernel_start, const uint32_t kernel_size)
{
    uint64_t high_end = 0;
    _Static_assert(sizeof(uint64_t) == 2 * sizeof(uint32_t));
    for (uint32_t i = 0; i < num_entries; ++i)
    {
        if (mem_map[i].length_hi != 0 || mem_map[i].base_addr_hi != 0)
            KERNEL_PANIC("unable to address more then 4GB");

        const uint64_t end = ((uint64_t) mem_map[i].base_addr_lo) + mem_map[i].length_lo;
        if (end > high_end)
            high_end = end;
    }

    g_PMM_total_frames = align_up(high_end, PMM_FRAME_ALIGN) / PMM_FRAME_SIZE;
    g_PMM_mem_map_size = (g_PMM_total_frames + PMM_MEM_MAP_FRAMES_PER_BYTE - 1) / PMM_MEM_MAP_FRAMES_PER_BYTE;

    g_PMM_mem_map = (bitset32_t) (uint32_t) align_up(physical_mem_start, PMM_MEM_MAP_FRAMES_PER_BYTE);
    memset(g_PMM_mem_map, 0, g_PMM_mem_map_size);
    g_PMM_used_frames = 0;

    for (uint32_t i = 0; i < num_entries; ++i)
    {
        if (mem_map[i].type == MEM_MAP_TYPE_AVAILABLE)
            continue;

        const uint32_t start_frame = mem_map[i].base_addr_lo / PMM_FRAME_SIZE;
        const uint32_t end_frame   = align_up(((uint64_t) mem_map[i].base_addr_lo) + mem_map[i].length_lo, PMM_FRAME_ALIGN) / PMM_FRAME_SIZE;
        if (end_frame > g_PMM_total_frames)
            KERNEL_PANIC("end frame > total frames");

        for (uint32_t f = start_frame; f < end_frame; ++f)
        {
            if (!bitset_test(g_PMM_mem_map, f))
            {
                bitset_set(g_PMM_mem_map, f);
                ++g_PMM_used_frames;
            }
        }
    }

    // mark the kernel part
    {
        const uint32_t start_frame = kernel_start / PMM_FRAME_SIZE;
        const uint32_t end_frame   = align_up(kernel_start + kernel_size, PMM_FRAME_ALIGN) / PMM_FRAME_SIZE;
        for (uint32_t f = start_frame; f < end_frame; ++f)
        {
            if (!bitset_test(g_PMM_mem_map, f))
            {
                bitset_set(g_PMM_mem_map, f);
                ++g_PMM_used_frames;
            }
        }
    }
}

inline int PMM_frames_used()
{
    return g_PMM_used_frames;
}

inline int PMM_frames_free()
{
    return g_PMM_total_frames - g_PMM_used_frames;
}

void* PMM_malloc(const size_t size)
{
    paddr_t ptr     = _PMM_malloc_frames(_size2frame(size));
    g_PMM_cur_paddr = ptr + size;
    return (void*) ptr;
}

void PMM_free(void* ptr, const size_t size)
{
    _PMM_free_frames((paddr_t) ptr, _size2frame(size));
}

void* PMM_malloc_linear(const size_t size)
{
    // check if current block is allocated
    if (!bitset_test(g_PMM_mem_map, g_PMM_cur_paddr / PMM_FRAME_SIZE))
        return PMM_malloc(size);

    // already allocated so reuse the same block (page)
    size_t  avail = g_PMM_cur_paddr % PMM_FRAME_SIZE;
    paddr_t tmp   = g_PMM_cur_paddr;
    // mark next block(s) allocated
    if (size > avail && PMM_malloc(size - avail) == NULL)
        _PMM_used_frames_panic("PMM_malloc no more free memory");

    // size here is <= avail, or if greater next block(s) are allocated
    g_PMM_cur_paddr += size;

    return (void*) tmp;
}
