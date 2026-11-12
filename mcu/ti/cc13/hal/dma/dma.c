/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 */

/*
 * dma.c
 *
 *  Created on: 28.11.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    dma.c
 * @brief   uDMA controller initialization and control table
 *
 * Manages the global DMA control table shared by all peripherals.
 * The control table holds transfer descriptors for 32 DMA channels,
 * each with PRIMARY and ALTERNATE structures for ping-pong mode.
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "hal_api.h"
#include "dma.h"

/* TI DriverLib includes */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include DeviceFamily_constructPath_SDK(driverlib/udma.h)
#include DeviceFamily_constructPath_SDK(driverlib/prcm.h)
#include DeviceFamily_constructPath_SDK(inc/hw_memmap.h)

#pragma GCC diagnostic pop

/******************************************************************************
 * DEFINES
 *****************************************************************************/
/* DMA control table size: 32 channels * 2 (primary + alternate) = 64 entries */
#define DMA_CONTROL_TABLE_SIZE 64

/******************************************************************************
 * STATIC DATA
 *****************************************************************************/

/*
 * DMA control table (1024-byte aligned, required by hardware)
 *
 * This table lives in RAM and holds transfer descriptors that the DMA
 * controller reads to perform autonomous data transfers. Each channel
 * has two entries:
 * - PRIMARY [0-31]: Main descriptor for each channel
 * - ALTERNATE [32-63]: Secondary descriptor for ping-pong mode
 *
 * The hardware automatically reads from this table when DMA channels
 * are triggered, moving data without CPU intervention.
 */
static tDMAControlTable m_dma_control_table[DMA_CONTROL_TABLE_SIZE]
    __attribute__((aligned(1024)));

/* Tracks initialization state */
static bool m_dma_initialized = false;

/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/

/*
 * @brief   Initialize the uDMA controller
 *
 * Enables the DMA peripheral and sets up the control table.
 * Safe to call multiple times (idempotent).
 *
 * @return  true if successful, false otherwise
 */
bool ti_dma_init(void)
{
    uint32_t i;

    if (m_dma_initialized)
    {
        return true;
    }

    /* Enable peripheral clock */
    PRCMPeripheralRunEnable(PRCM_PERIPH_UDMA);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_UDMA);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
    }

    /* Disable all DMA channels */
    for (i = 0; i < 32; i++)
    {
        uDMAChannelDisable(UDMA0_BASE, i);
    }

    /* Zero out entire DMA control table (64 entries: 32 PRIMARY + 32 ALTERNATE) */
    for (i = 0; i < DMA_CONTROL_TABLE_SIZE; i++)
    {
        m_dma_control_table[i].pvSrcEndAddr = NULL;
        m_dma_control_table[i].pvDstEndAddr = NULL;
        m_dma_control_table[i].ui32Control = 0;
        m_dma_control_table[i].ui32Spare = 0;
    }

    /* Enable controller and set control table address */
    uDMAEnable(UDMA0_BASE);
    uDMAControlBaseSet(UDMA0_BASE, m_dma_control_table);

    /* Clear any stale DMA completion flags */
    uDMAIntClear(UDMA0_BASE, 0xFFFFFFFF);

    m_dma_initialized = true;
    return true;
}

/*
 * @brief   Deinitialize the uDMA controller
 *
 * Disables DMA peripheral. Stop all channels before calling.
 *
 * @return  None
 */
void ti_dma_deinit(void)
{
    if (!m_dma_initialized)
    {
        return;
    }

    /* Disable controller and peripheral clock */
    uDMADisable(UDMA0_BASE);
    PRCMPeripheralRunDisable(PRCM_PERIPH_UDMA);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
    }

    m_dma_initialized = false;
}
