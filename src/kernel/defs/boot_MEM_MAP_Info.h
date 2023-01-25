#pragma once

#include <stdint.h>

#define MEM_MAP_TYPE_AVAILABLE      1
#define MEM_MAP_TYPE_RESERVED       2
#define MEM_MAP_TYPE_ACPI_RECLAIM   3
#define MEM_MAP_TYPE_ACPI_NVS       4

#pragma pack(push, 1)
typedef struct boot_MEM_MAP_Info_Entry_t
{
    uint32_t base_addr_lo;
    uint32_t base_addr_hi;
    uint32_t length_lo;
    uint32_t length_hi;
    uint32_t type;
    uint32_t acpi;

} __attribute__((packed)) boot_MEM_MAP_Info_Entry_t;
_Static_assert(sizeof(boot_MEM_MAP_Info_Entry_t) == 24);
#pragma pack(pop)

// boot_MEM_MAP_Info_Entry_t   *__mem_map_info; // dynamic array
// int32_t                     __mem_map_info_size = 0;
