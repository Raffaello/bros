/***************************
 * Physical Memory Manager *
 **************************/
#pragma once

#include <stdint.h>

void PMM_init(const uint32_t tot_mem_KB, uint32_t* physical_mem_start);