/************************************
 * Physical Memory Manager          *
 *   continuos mem areas            *
 * susceptible to fragmentation     *
 ************************************/
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <bios/boot_info.h>

typedef uint32_t paddr_t; // physical address type

typedef struct PMM_mem_t
{
    paddr_t  addr;
    uint32_t length;
    uint32_t type;
} PMM_mem_t;

void PMM_init(const uint32_t tot_mem_KB, paddr_t physical_mem_start, const uint8_t boot_drive);
void PMM_MemMap_init(const paddr_t physical_addr, const uint32_t size);
void PMM_MemMap_deinit(const paddr_t physical_addr, const uint32_t size);

/*
 * Mark the kernel memory as in use
 */
void PMM_MemMap_deinit_kernel(const uint32_t code_start, const uint32_t code_size);

void PMM_store_MemMapInfo(const uint32_t num_entries, const volatile boot_MEM_MAP_Info_Entry_t* mem_map);

int PMM_Blocks_used();
int PMM_Blocks_free();
/**
 * Aligned page-size memory alloc
 * */
void *PMM_malloc_blocks(const size_t num_blocks);
void PMM_free_blocks(void* ptr, const size_t num_blocks);

/**
 * compute ho many blocks to alloc from bytes.
 * */
void *PMM_malloc(const size_t size);
void PMM_free(void* ptr, const size_t size);

/**
 * Alloc continuosly without align nor page size, can't be free.
 * TODO: use a heap
 * */
void *PMM_malloc_linear(const size_t size);
