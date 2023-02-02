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


void PMM_init(const uint32_t tot_mem_KB, uint32_t* physical_mem_start)
{
    _PMM_tot_mem = tot_mem_KB;
    _PMM_max_blocks = tot_mem_KB * 1024 / PMM_BLOCK_SIZE;
    _PMM_used_blocks = _PMM_max_blocks;
    _PMM_mem_map = physical_mem_start;

    memset(_PMM_mem_map, 0xF, _PMM_max_blocks / PMM_BLOCKS_PER_BYTE);
}
