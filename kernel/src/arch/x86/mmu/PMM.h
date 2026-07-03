/************************************
 * Physical Memory Manager          *
 *   continuos mem areas            *
 * susceptible to fragmentation     *
 ************************************/
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <bios/boot_info.h>

typedef uint32_t paddr_t;    // physical address type

typedef struct PMM_mem_t
{
    paddr_t  addr;
    uint32_t length;
    uint32_t type;
} PMM_mem_t;

void PMM_init(uint32_t num_entries, const volatile boot_MEM_MAP_Info_Entry_t* mem_map, const paddr_t physical_mem_start, const uint32_t kernel_start, const uint32_t kernel_size);
int  PMM_frames_used();
int  PMM_frames_free();

/**
 * Aligned page-size mem alloc from bytes.
 * */
void* PMM_malloc(const size_t size);
void  PMM_free(void* ptr, const size_t size);

/**
 * Alloc continuously without align nor page size, can't be free.
 * TODO: use a heap, this won't work as soon as free is used...
 * */
void* PMM_malloc_linear(const size_t size);
