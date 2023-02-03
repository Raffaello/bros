/************************************
 * Physical Memory Manager          *
 *   continuos mem areas            *
 * susceptible to fragmentation     *
 ************************************/
#pragma once

#include <stdint.h>
#include <stddef.h>

// typedef PMM_mem_t
// {
//     size_t size;
//     void* ptr;
// } PMM_mem_t;

void PMM_init(const uint32_t tot_mem_KB, uint32_t* physical_mem_start);

void PMM_MemMap_init(const uint32_t physical_addr, const uint32_t size);
void PMM_MemMap_deinit(const uint32_t physical_addr, const uint32_t size);

void PMM_MemMap_deinit_kernel(const uint32_t paddr_start, const uint32_t paddr_end);
// readonly
uint32_t PMM_Blocks_used();
uint32_t PMM_Blocks_free();

void *PMM_malloc_blocks(const size_t num_blocks);
void PMM_free_blocks(void* ptr, const size_t num_blocks);

void *PMM_malloc(const size_t size);
void PMM_free(void* ptr, const size_t size);
