/**************************************
 *** 32 bit paging only for now     ***
 * ************************************/
#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct PTE_4KB_t
{
    uint32_t p          : 1;     // Present bit (must be 1)
    uint32_t rw         : 1;     // if 0 write may not be allowed
    uint32_t us         : 1;     // if 0 User mode not allowed to access the page referenced by this entry.
    uint32_t pwt        : 1;     // Page level Write Through
    uint32_t pcd        : 1;     // Page level cached disabled
    uint32_t a          : 1;     // Accessed?
    uint32_t d          : 1;     // Dirty?
    uint32_t pat        : 1;     // if PAT supported, otherwise reseverd (must be 0)
    uint32_t g          : 1;     // Global; if CR3.PGE=1 determines wheter the translation is global. Otherwise ignored
    uint32_t ignored    : 3;     // ignored
    uint32_t frame      : 20;    // Physical address of 4KB page referenced by this entry.
} __attribute__((packed)) PTE_4KB_t;
_Static_assert(sizeof(PTE_4KB_t) == sizeof(uint32_t));

typedef struct
#pragma pack(pop)
