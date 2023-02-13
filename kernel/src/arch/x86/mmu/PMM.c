#include <stddef.h>
#include <arch/x86/mmu/PMM.h>
#include <lib/bitset.h>
#include <lib/conio.h>
#include <lib/string.h>
#include <sys/panic.h>

// TODO: having a block size of 4096 if i have to allocate few bytes is a lot of waste...
//       something is not ok ... it should be more fine granted
//       instead of alloc 1 block at time so a minimum of 4096 bytes
// ...
// e.g. mem_map and mem_map_info can just use 1 block in common...

// TODO need a more efficient malloc that doesnt alloc a PAGE/BLOCK at time, too much memory wasteful
//      especially for the internal kernel variables

#define PMM_MEM_MAP_BLOCKS_PER_BYTE 8
// PAGE_SIZE
#define PMM_BLOCK_SIZE      4096
#define PMM_BLOCK_ALIGN     PMM_BLOCK_SIZE


static uint8_t                      _PMM_boot_drive         = 0;
static uint32_t                     _PMM_tot_mem            = 0; // should be size_t ?
static uint32_t                     _PMM_max_blocks         = 0;
static uint32_t                     _PMM_used_blocks        = 0;
static bitset32_t                   _PMM_mem_map            = NULL;
static uint32_t                     _PMM_mem_map_size       = 0;

static PMM_mem_t*                   _PMM_mem_map_info       = NULL;
static uint32_t                     _PMM_mem_map_info_length    = 0;
static paddr_t                      _PMM_cur_paddr          = 0;

static inline void _PMM_used_blocks_panic(const char* msg)
{
    char str[128];
    CON_sprintf(str, "%s: %u (%u)", msg, _PMM_used_blocks, _PMM_max_blocks);
    KERNEL_PANIC(str);
}

static inline size_t _size2block(const size_t size)
{
    return (size / PMM_BLOCK_SIZE) + ((size % PMM_BLOCK_SIZE) ? 1 : 0);
}

void PMM_init(const uint32_t tot_mem_KB, paddr_t physical_mem_start, const uint8_t boot_drive)
{
    _PMM_boot_drive     = boot_drive;
    _PMM_tot_mem        = tot_mem_KB;
    _PMM_max_blocks     = tot_mem_KB * 1024 / PMM_BLOCK_SIZE; // there is no extra block in this case for the reminder
    _PMM_used_blocks    = _PMM_max_blocks;
    

    _PMM_mem_map_size   = _PMM_max_blocks / PMM_MEM_MAP_BLOCKS_PER_BYTE;
    _PMM_mem_map        = (bitset32_t) physical_mem_start;
    _PMM_cur_paddr      = physical_mem_start + _PMM_mem_map_size;

    // All Memory in use, as not known if it can be really used...
    memset(_PMM_mem_map, 0xF, _PMM_mem_map_size);
}

void PMM_MemMap_init(const paddr_t physical_addr, const uint32_t size)
{
    uint32_t block_addr = physical_addr / PMM_BLOCK_SIZE;
    // const uint32_t blocks = _size2block(size);
    uint32_t blocks = (size / PMM_BLOCK_SIZE); // can't have the reminder as another block ..

    if(blocks > _PMM_used_blocks)
        blocks = _PMM_used_blocks;

    for(uint32_t i = 0; i < blocks; ++i)
    {
        bitset_clear(_PMM_mem_map, block_addr++);
        _PMM_used_blocks--;
    }

    if(_PMM_used_blocks > _PMM_max_blocks)
        _PMM_used_blocks_panic("PMM_MemMap_init used blocks");
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

    if(_PMM_used_blocks > _PMM_max_blocks)
        _PMM_used_blocks_panic("PMM_MemMap_deinit too many used blocks");
}

void PMM_MemMap_deinit_kernel(const uint32_t code_start, const uint32_t code_size)
{
    // extern uint32_t __size;
    // extern uint32_t __code_start;

    // TODO: add also something for the stack!
    PMM_MemMap_deinit(code_start, code_size + _PMM_mem_map_size);
}

void PMM_store_MemMapInfo(const uint32_t num_entries, const volatile boot_MEM_MAP_Info_Entry_t* mem_map)
{
    _PMM_mem_map_info_length = num_entries;
    _PMM_mem_map_info = PMM_malloc_linear(sizeof(PMM_mem_t) * _PMM_mem_map_info_length);
    if(_PMM_mem_map_info == NULL)
        KERNEL_PANIC("PMM_store_MemMapInfo");

    for(int i = 0; i < _PMM_mem_map_info_length; i++)
    {
        _PMM_mem_map_info[i].addr = mem_map[i].base_addr_lo;
        _PMM_mem_map_info[i].length = mem_map[i].length_lo;
        _PMM_mem_map_info[i].type = mem_map[i].type;
    }
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
    if (_PMM_used_blocks < num_blocks)
        KERNEL_PANIC("PMM_free_blocks");

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

void *PMM_malloc_linear(const size_t size)
{
    // check if current block is allocated
    if(!bitset_test(_PMM_mem_map, _PMM_cur_paddr / PMM_BLOCK_SIZE))
        return PMM_malloc(size);
    
    // already allocated so reuse the same block (page)
    size_t avail = _PMM_cur_paddr % PMM_BLOCK_SIZE;
    paddr_t tmp = _PMM_cur_paddr;
    // mark next block(s) allocated
    if(size > avail && PMM_malloc(size - avail) == NULL)
        _PMM_used_blocks_panic("PMM_kmalloc no more free memory");
    
    // size here is <= avail, or if greater next block(s) are allocated
    _PMM_cur_paddr += size;
    
    return (void*) tmp;
}
