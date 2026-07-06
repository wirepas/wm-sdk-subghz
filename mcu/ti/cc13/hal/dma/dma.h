/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 */

/*
 * dma.h
 *
 *  Created on: 28.11.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    dma.h
 * @brief   System-level DMA controller initialization
 *
 * Manages the shared DMA control table used by all peripherals (UART, SPI, etc.).
 * Call ti_dma_init() before configuring any DMA channels.
 *****************************************************************************/

#ifndef DMA_H_
#define DMA_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * PUBLIC API
 *****************************************************************************/

/*
 * @brief   Initialize the uDMA controller
 *
 * Enables the DMA peripheral and sets up the control table.
 * Safe to call multiple times (idempotent).
 *
 * @return  true if successful, false otherwise
 */
bool ti_dma_init(void);

/*
 * @brief   Deinitialize the uDMA controller
 *
 * Disables DMA peripheral. Stop all channels before calling.
 *
 * @return  None
 */
void ti_dma_deinit(void);

#endif /* DMA_H_ */
