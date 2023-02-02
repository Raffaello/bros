#include <cpu/mmu/PMM.h>
#include <lib/bitset.h>
#include <stddef.h>
#include <lib/string.h>

#define PMM_BLOCKS_PER_BYTE  8
// PAGE_SIZE
#define PMM_BLOCK_SIZE      4096
#define PMM_BLOCK_ALIGN     PMM_BLOCK_SIZE

static uint32_t     _PMM_tot_mem        = 0; // should be size_t ?
static uint32_t     _PMM_max_blocks     = 0;
static uint32_t     _PMM_used_blocks    = 0;
static bitset32_t   _PMM_mem_map        = NULL;
static uint32_t     _PMM_mem_map_size   = 0;


void PMM_init(const uint32_t tot_mem_KB, uint32_t* physical_mem_start)
{
    _PMM_tot_mem = tot_mem_KB;
    _PMM_max_blocks = tot_mem_KB * 1024 / PMM_BLOCK_SIZE;
    _PMM_used_blocks = _PMM_max_blocks;
    _PMM_mem_map = physical_mem_start;
    _PMM_mem_map_size = _PMM_max_blocks / PMM_BLOCKS_PER_BYTE;

    // All Memory in use, as not known if it can be really used...
    memset(_PMM_mem_map, 0xF, _PMM_mem_map_size);
}

void PMM_MemMap_init(const uint32_t physical_addr, const uint32_t size)
{
    uint32_t block_addr = physical_addr / PMM_BLOCK_SIZE;
    const uint32_t blocks = size / PMM_BLOCK_SIZE;

    for(uint32_t i = 0; i < blocks; ++i)
    {
        bitset_unset(_PMM_mem_map, block_addr++);
        _PMM_used_blocks--;
    }

    // TODO assert used blocks <= max blocks (underflow)

}

void PMM_MemMap_deinit(const uint32_t physical_addr, const uint32_t size)
{
    uint32_t block_addr = physical_addr / PMM_BLOCK_SIZE;
    const uint32_t blocks = size / PMM_BLOCK_SIZE;

    for(uint32_t i = 0; i < blocks; ++i)
    {
        bitset_set(_PMM_mem_map, block_addr++);
        _PMM_used_blocks++;
    }

    // TODO assert used blocks <= max blocks
}

void PMM_MemMap_deinit_kernel(const uint32_t physical_addr, const uint32_t size)
{
    PMM_MemMap_deinit(physical_addr, size + _PMM_mem_map_size);
}

inline uint32_t PMM_Blocks_used()
{
    return _PMM_used_blocks;
}

inline uint32_t PMM_Blocks_free()
{
    return _PMM_max_blocks - _PMM_used_blocks;
}