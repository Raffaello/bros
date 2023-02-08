#include <arch/x86/mmu/PMM.h>
#include <lib/bitset.h>
#include <stddef.h>
#include <lib/string.h>

#define PMM_BLOCKS_PER_BYTE 8
// PAGE_SIZE
#define PMM_BLOCK_SIZE      4096
#define PMM_BLOCK_ALIGN     PMM_BLOCK_SIZE

static uint32_t     _PMM_tot_mem        = 0; // should be size_t ?
static uint32_t     _PMM_max_blocks     = 0;
static uint32_t     _PMM_used_blocks    = 0;
static bitset32_t   _PMM_mem_map        = NULL;
static uint32_t     _PMM_mem_map_size   = 0;

static inline size_t _size2block(const size_t size)
{
    return (size / PMM_BLOCK_SIZE) + ((size % PMM_BLOCK_SIZE) ? 1 : 0);
}

void PMM_init(const uint32_t tot_mem_KB, paddr_t physical_mem_start)
{
    _PMM_tot_mem = tot_mem_KB;
    _PMM_max_blocks = tot_mem_KB * 1024 / PMM_BLOCK_SIZE;
    _PMM_used_blocks = _PMM_max_blocks;
    _PMM_mem_map = (bitset32_t) physical_mem_start;
    _PMM_mem_map_size = _PMM_max_blocks / PMM_BLOCKS_PER_BYTE;

    // All Memory in use, as not known if it can be really used...
    memset(_PMM_mem_map, 0xF, _PMM_mem_map_size);
}

void PMM_MemMap_init(const paddr_t physical_addr, const uint32_t size)
{
    uint32_t block_addr = physical_addr / PMM_BLOCK_SIZE;
    const uint32_t blocks = _size2block(size);

    for(uint32_t i = 0; i < blocks; ++i)
    {
        bitset_clear(_PMM_mem_map, block_addr++);
        _PMM_used_blocks--;
    }

    // TODO assert used blocks <= max blocks (underflow)
}

void PMM_MemMap_deinit(const paddr_t physical_addr, const uint32_t size)
{
    uint32_t block_addr = physical_addr / PMM_BLOCK_SIZE;
    const uint32_t blocks = _size2block(size);

    for(uint32_t i = 0; i < blocks; ++i)
    {
        bitset_set(_PMM_mem_map, block_addr++);
        _PMM_used_blocks++;
    }

    // TODO assert used blocks <= max blocks
}

void PMM_MemMap_deinit_kernel(const uint32_t code_start, const uint32_t code_size)
{
    // extern uint32_t __size;
    // extern uint32_t __code_start;

    // TODO: add also something for the stack!
    PMM_MemMap_deinit(code_start, code_size + _PMM_mem_map_size);
}

inline int PMM_Blocks_used()
{
    return _PMM_used_blocks;
}

inline int PMM_Blocks_free()
{
    return _PMM_max_blocks - _PMM_used_blocks;
}

void *PMM_malloc_blocks(const size_t num_blocks)
{
    if (PMM_Blocks_free() < num_blocks)
        return NULL;

    unsigned int pos;
    if(!bitset_find(_PMM_mem_map, _PMM_mem_map_size, num_blocks, &pos))
        return NULL;
    
    for(size_t i = 0; i < num_blocks; ++i)
        bitset_set(_PMM_mem_map, pos + i);

    _PMM_used_blocks += num_blocks;

    return (void*) (pos * PMM_BLOCK_SIZE);
}

void PMM_free_blocks(void* ptr, const size_t num_blocks)
{
    // TODO assert used blocks > num_blocks
    // if (_PMM_used_blocks < num_blocks)
    //     return;

    unsigned int pos = ((unsigned int) ptr) / PMM_BLOCK_SIZE;
    for(size_t i = 0; i < num_blocks; i++)
        bitset_clear(_PMM_mem_map, pos + i);
    _PMM_used_blocks -= num_blocks;
}

void *PMM_malloc(const size_t size)
{
    return PMM_malloc_blocks(_size2block(size));
}

void PMM_free(void* ptr, const size_t size)
{
    PMM_free_blocks(ptr, _size2block(size));
}
