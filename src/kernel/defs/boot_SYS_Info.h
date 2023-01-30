#pragma once

#include <stdint.h>

#define SYS_INFO_BEGIN  0x12345678
#define SYS_INFO_END    0x87654321

#define MEM_MAP_TYPE_AVAILABLE      1
#define MEM_MAP_TYPE_RESERVED       2
#define MEM_MAP_TYPE_ACPI_RECLAIM   3
#define MEM_MAP_TYPE_ACPI_NVS       4

#define SYS_INFO_END_MARKER_PTR(x) (uint32_t*) (((uint8_t*) x) + sizeof(boot_SYS_Info_t) + (x->num_mem_map_entries * sizeof(boot_MEM_MAP_Info_Entry_t)))

#pragma pack(push, 1)
typedef struct boot_SYS_Info_t
{
    uint32_t begin_marker;
    uint32_t tot_mem;
    uint8_t  boot_drive;
    uint8_t  num_mem_map_entries;
    // memMap_entries[num_mem_map_entries] // 24 bytes
    // uint32_t end_marker;

} __attribute__((packed)) boot_SYS_Info_t;
_Static_assert(sizeof(boot_SYS_Info_t) == 10);

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
