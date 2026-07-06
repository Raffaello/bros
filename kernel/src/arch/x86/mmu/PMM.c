#include <stddef.h>
#include <arch/x86/mmu/PMM.h>
#include <lib/bitset.h>
#include <lib/conio.h>
#include <lib/string.h>
#include <sys/panic.h>

#define PMM_MEM_MAP_FRAMES_PER_BYTE 8
#define PMM_FRAME_ALIGN             PMM_FRAME_SIZE


static uint32_t   g_PMM_total_frames = 0;
static uint32_t   g_PMM_used_frames  = 0;
static bitset32_t g_PMM_frames_map   = NULL;

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

    g_PMM_used_frames           = 0;
    g_PMM_total_frames          = align_up(high_end, PMM_FRAME_ALIGN) / PMM_FRAME_SIZE;
    const uint32_t mem_map_size = (g_PMM_total_frames + PMM_MEM_MAP_FRAMES_PER_BYTE - 1) / PMM_MEM_MAP_FRAMES_PER_BYTE;
    const paddr_t  bitmap_paddr = (paddr_t) align_up(physical_mem_start, PMM_MEM_MAP_FRAMES_PER_BYTE);
    g_PMM_frames_map            = (bitset32_t) (uint32_t) bitmap_paddr;
    memset(g_PMM_frames_map, 0, mem_map_size);
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
            if (!bitset_test(g_PMM_frames_map, f))
            {
                bitset_set(g_PMM_frames_map, f);
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
            if (!bitset_test(g_PMM_frames_map, f))
            {
                bitset_set(g_PMM_frames_map, f);
                ++g_PMM_used_frames;
            }
        }
    }

    // mark the bitmap's own backing frames as used
    {
        const uint32_t start_frame = (uint32_t) bitmap_paddr / PMM_FRAME_SIZE;
        const uint32_t end_frame   = align_up(bitmap_paddr + mem_map_size, PMM_FRAME_ALIGN) / PMM_FRAME_SIZE;
        for (uint32_t f = start_frame; f < end_frame; ++f)
        {
            if (!bitset_test(g_PMM_frames_map, f))
            {
                bitset_set(g_PMM_frames_map, f);
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

void* PMM_malloc(const uint32_t size)
{
    const uint32_t num_frames = _size2frame(size);
    if (PMM_frames_free() < num_frames)
        return NULL;

    uint32_t pos;
    if (!bitset_find(g_PMM_frames_map, g_PMM_total_frames, num_frames, &pos))
        return NULL;

    for (size_t i = 0; i < num_frames; ++i)
        bitset_set(g_PMM_frames_map, pos + i);

    g_PMM_used_frames += num_frames;

    paddr_t ptr = pos * PMM_FRAME_SIZE;

    return (void*) ptr;
}

void PMM_free(void* ptr, const uint32_t size)
{
    const uint32_t num_frames = _size2frame(size);
    if (g_PMM_used_frames < num_frames)
        _PMM_used_frames_panic("PMM_free_frames");

    unsigned int pos = (paddr_t) ptr / PMM_FRAME_SIZE;
    for (size_t i = 0; i < num_frames; i++)
        bitset_clear(g_PMM_frames_map, pos + i);
    g_PMM_used_frames -= num_frames;
}
