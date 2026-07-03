/************************************
 * Physical Memory Manager          *
 *   continuos mem areas            *
 * susceptible to fragmentation     *
 ************************************/
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <bios/boot_info.h>

#define PMM_FRAME_SIZE 4096

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
 * @brief Aligned page-size mem alloc from bytes.
 * */
void* PMM_malloc(const uint32_t size);
void  PMM_free(void* ptr, const uint32_t size);
