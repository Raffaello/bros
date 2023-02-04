#include <bios/boot_info.h>
#include <stddef.h>
#include <lib/conio.h>

void boot_info_sanitize(uint32_t *tot_mem, const uint8_t num_mem_map, boot_MEM_MAP_Info_Entry_t* mem_map)
{
    uint32_t tot_memMap = 0;
    if (num_mem_map == 0)
        return;

    // _mem_map_entries = mem_map;
    // uint8_t index[255];
    for(int i = 0; i < num_mem_map; ++i)
    {
        // index[i] = i;
        if(mem_map[i].type >= MEM_MAP_TYPE_BAD 
        || mem_map[i].type == MEM_MAP_TYPE_ERROR)
            mem_map[i].type = MEM_MAP_TYPE_RESERVED;

        if(mem_map[i].type == MEM_MAP_TYPE_AVAILABLE)
            tot_memMap += mem_map[i].length_lo/1024;
    }

    if(*tot_mem < tot_memMap) {
        *tot_mem = tot_memMap;
    }

    // bubble sort
    // for(int i=0; i< num_mem_map-1; ++i)
    // {
    //     boot_MEM_MAP_Info_Entry_t memi = mem_map[i];
    //     for(int j=i+1; j < num_mem_map; ++j)
    //     {
    //         boot_MEM_MAP_Info_Entry_t memj = mem_map[j];
    //         if(memi.base_addr_hi < memj.base_addr_lo) 
    //             continue; // already in order
            
    //         if(memi.base_addr_hi > memj.base_addr_hi
    //         || memi.base_addr_lo > memj.base_addr_lo) {
    //             //swap
    //             mem_map[i] = memj;
    //             mem_map[j] = memi;
    //             // reset 2 elements
    //             memi = mem_map[i];
    //             memj = mem_map[j]
    //         }
    //     }
    // }
}
