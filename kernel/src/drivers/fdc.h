#pragma once

/*******************************************************
 * @brief Floppy Disk Controller based on Intel 82077A *
 *
 *******************************************************/

#include <stdint.h>

void fdc_init();
void fdc_set_dma_addr(uint8_t* pBuf);

/**
 * @brief read a sector using DMA at the defined DMA address given default(0x1000)
 *
 * @param drive
 * @param sectorLBA
 * @return uint8_t*
 */
uint8_t* fdc_read_sector(uint8_t drive, int sectorLBA);
