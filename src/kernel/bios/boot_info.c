#include <bios/boot_info.h>
#include <stddef.h>

// todo these values might not be required as global...
static uint32_t __tot_mem;
static uint8_t __num_mem_map;
static boot_MEM_MAP_Info_Entry_t* __mem_map_entries = NULL;


void boot_info_init(const uint32_t tot_mem, const uint8_t num_mem_map, boot_MEM_MAP_Info_Entry_t* mem_map)
{
    __tot_mem = tot_mem;
    __num_mem_map = num_mem_map;
    if(num_mem_map > 0) {
        __mem_map_entries = mem_map;
    }

    // todo check mem_map infos and allocate memory.

}