/****************************
 * Virtual Memory Manager   *
 ****************************/

/**************************************************************************
 *** 32 bit paging only for now (PAE later?)                            ***
 *** 3 Level paging only 4KB pages                                      ***
 *** TODO:                                                              ***
 *** opimizing page size on process based with 3 level paging e.g.:     ***
 *** mix 4k, 2MB and 1GB pages. No need to use uniform page size.       ***
 *** So a process having 9MB of data can get 4 2MB pages and            ***
 *** the rest with 4k pages.                                            ***
 *** That saves paging structures, improves TLB usage                   ***
 *** and does not increase the overhead at all.                         ***
 *************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PAGE_DIR_ENTRIES    1024
#define PAGE_TABLE_ENTRIES  1024

#pragma pack(push, 1)
typedef struct PDE_t
{
    uint32_t p          :   1;  // present bit to reference a page table
    uint32_t rw         :   1;  // if 0 write may not be allowed
    uint32_t us         :   1;  // if 0 User mode not allowed to access the memory region.
    uint32_t pwt        :   1;  // Page level Write Through
    uint32_t pcd        :   1;  // Page level Cache Disabled
    uint32_t a          :   1;  // Accessed?
    uint32_t ign        :   1;  // ignored
    uint32_t zero       :   1;  // If CR4.PSE = 1, must be 0 (4KB pages)
    uint32_t ign2       :   4;  // ignored
    uint32_t page_table :   20; // Physical address of 4-KByte aligned page table referenced by this entry

} __attribute__((packed)) PDE_t;
_Static_assert(sizeof(PDE_t) == sizeof(uint32_t));

typedef struct PTE_t
{
    uint32_t p          : 1;     // Present bit (must be 1, 0 otheerwise OS can use it for swap)
    uint32_t rw         : 1;     // if 0 write may not be allowed
    uint32_t us         : 1;     // if 0 User mode not allowed to access the page referenced by this entry.
    uint32_t pwt        : 1;     // Page level Write Through
    uint32_t pcd        : 1;     // Page level Cache Disabled
    uint32_t a          : 1;     // Accessed?
    uint32_t d          : 1;     // Dirty?
    uint32_t pat        : 1;     // if PAT supported, otherwise reseverd (must be 0)
    uint32_t g          : 1;     // Global; if CR3.PGE=1 determines wheter the translation is global. Otherwise ignored
    uint32_t ignored    : 3;     // ignored
    uint32_t frame      : 20;    // Physical address of 4KB page referenced by this entry.
} __attribute__((packed)) PTE_t;
_Static_assert(sizeof(PTE_t) == sizeof(uint32_t));
#pragma pack(pop)

typedef struct page_table_t
{
    PTE_t entries[PAGE_TABLE_ENTRIES];
} page_table_t;



typedef struct page_directory_t
{
    PDE_t  entries[PAGE_DIR_ENTRIES];
    
    // page_table_t* page_tables[PAGE_TABLE_ENTRIES];
    // uint32_t page_table_physical[1024];
    // uint32_t physical_addr;
} page_directory_t;

bool VMM_init();
bool VMM_switch_page_directory(page_directory_t* page_directory);
void VMM_enable_paging();
void VMM_disable_paging();
