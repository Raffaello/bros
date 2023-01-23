#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct boot_SYS_Info_t
{
    uint32_t begin_marker;
    uint32_t tot_mem;
    uint8_t  boot_drive;
    uint32_t end_marker;

} __attribute__((packed)) boot_SYS_Info_t;
_Static_assert(sizeof(boot_SYS_Info_t) == 13);
#pragma pack(pop)
