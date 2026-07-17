#pragma once

#include <stdint.h>

/**
 * @brief sets the mode of a channel
 *
 * @param channel
 * @param mode
 */
void dma_set_mode(uint8_t channel, uint8_t mode);

/**
 * @brief prepares for generic channel read
 *
 * @param channel
 */
void dma_set_read(uint8_t channel);

/**
 * @brief prepares for generic channel write
 *
 * @param channel
 */
void dma_set_write(uint8_t channel);

/**
 * @brief sets the address of a channel
 *
 * @param channel
 * @param low
 * @param high
 */
void dma_set_address(uint8_t channel, uint8_t low, uint8_t high);

/**
 * @brief sets the counter of a channel
 *
 * @param channel
 * @param low
 * @param high
 */
void dma_set_count(uint8_t channel, uint8_t low, uint8_t high);

/**
 * @brief masks a channel
 *
 * @param channel
 */
void dma_mask_channel(uint8_t channel);

/**
 * @brief unmasks a channel
 *
 * @param channel
 */
void dma_unmask_channel(uint8_t channel);

/**
 * @brief
 *
 * @param dma 0 => DMA0 otherwise DMA1
 */
void dma_reset_flipflop(uint8_t dma);

/**
 * @brief reset the dma to defaults
 *
 * @param dma
 */
void dma_reset(uint8_t dma);

/**
 * @brief  sets an external page register
 *
 * @param reg
 * @param val
 */
void dma_set_external_page_register(uint8_t reg, uint8_t val);

/**
 * @brief unmasks all registers
 *
 * @param dma
 */
void dma_unmask_all(uint8_t dma);
