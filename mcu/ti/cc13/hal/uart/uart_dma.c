/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 */

/*
 * uart_dma.c
 *
 *  Created on: 14.11.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    uart_dma.c
 * @brief   UART DMA transfer operations
 *
 * TX: Double buffer with manual switch
 * RX: Double buffer with manual switch
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "uart_internal.h"
#include "../dma/dma.h"

/* Note: All TI DriverLib headers are included via uart_internal.h */

/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/

/*
 * @brief   Start TX DMA transfer
 * @param   p_config    UART configuration
 * @param   p_data      Data buffer to send
 * @param   length      Number of bytes
 * @return  true if started, false otherwise
 */
bool ti_uart_dma_start_tx(const uart_config_t *p_config,
                          const uint8_t *p_data,
                          uint32_t length)
{
    const uart_hw_config_t *p_hw;

    if (p_config == NULL || p_data == NULL || length == 0)
    {
        return false;
    }

    p_hw = p_config->p_hw_config;

    /* Disable EOT interrupt first to prevent spurious triggers */
    UARTIntDisable(p_hw->base_address, UART_INT_EOT);

    /* Disable UART DMA requests for clean reconfiguration */
    UARTDMADisable(p_hw->base_address, UART_DMA_TX);

    /* Disable DMA channel */
    uDMAChannelDisable(UDMA0_BASE, p_hw->dma_tx_channel);

    /* Configure DMA control: 8-bit, source increments, destination fixed */
    uDMAChannelControlSet(
        UDMA0_BASE,
        p_hw->dma_tx_channel | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UART_DMA_TX_BURST_SIZE
    );

    /* Configure transfer: BASIC mode, buffer to UART DR register */
    uDMAChannelTransferSet(
        UDMA0_BASE,
        p_hw->dma_tx_channel | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC,
        (void *)p_data,
        (void *)(p_hw->base_address + UART_O_DR),
        length
    );

    /* Enable DMA channel before UART DMA request */
    uDMAChannelEnable(UDMA0_BASE, p_hw->dma_tx_channel);

    /* Enable UART DMA request generation */
    UARTDMAEnable(p_hw->base_address, UART_DMA_TX);

    return true;
}

/*
 * @brief   Start RX DMA in BASIC mode
 * @param   p_config    UART configuration
 * @return  true if started, false otherwise
 */
bool ti_uart_dma_start_rx(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw;
    uart_state_t *p_state;

    if (p_config == NULL ||
        p_config->p_hw_config == NULL ||
        p_config->p_state == NULL)
    {
        return false;
    }

    p_hw = p_config->p_hw_config;
    p_state = p_config->p_state;

    /* Disable DMA and clear flags to prevent spurious interrupts */
    UARTDMADisable(p_hw->base_address, UART_DMA_RX);
    uDMAChannelDisable(UDMA0_BASE, p_hw->dma_rx_channel);
    uDMAIntClear(UDMA0_BASE, (1 << p_hw->dma_rx_channel));
    __DSB();

    /* Disable ALTERNATE descriptor (BASIC mode uses PRIMARY only) */
    uDMAChannelAttributeDisable(UDMA0_BASE, p_hw->dma_rx_channel, UDMA_ATTR_ALTSELECT);

    /* Configure PRIMARY descriptor for BASIC mode */
    uDMAChannelControlSet(
        UDMA0_BASE,                                 /* DMA controller base */
        p_hw->dma_rx_channel | UDMA_PRI_SELECT,     /* RX channel, primary descriptor */
        UDMA_SIZE_8 |                               /* 8-bit transfers */
        UDMA_SRC_INC_NONE |                         /* Source fixed (UART DR) */
        UDMA_DST_INC_8 |                            /* Destination increments */
        UART_DMA_RX_BURST_SIZE                      /* Burst size */
    );

    uDMAChannelTransferSet(
        UDMA0_BASE,                                 /* DMA controller base */
        p_hw->dma_rx_channel | UDMA_PRI_SELECT,     /* RX channel, primary descriptor */
        UDMA_MODE_BASIC,                            /* BASIC mode (manual restart) */
        (void *)(p_hw->base_address + UART_O_DR),  /* Source: UART data register */
        (void *)DoubleBuffer_getActive(p_state->rx_buffers), /* Dest: active buffer */
        BUFFER_SIZE                                 /* Transfer size in bytes */
    );

    /* Enable burst mode: DMA triggers at FIFO watermark (4 bytes), timeout handles residual */
    uDMAChannelAttributeEnable(UDMA0_BASE, p_hw->dma_rx_channel, UDMA_ATTR_USEBURST);

    /* Enable timeout interrupt for partial buffer detection */
    UARTIntEnable(p_hw->base_address, UART_INT_RT);

    /* Enable DMA channel */
    uDMAChannelEnable(UDMA0_BASE, p_hw->dma_rx_channel);

    /* Enable UART RX DMA requests */
    UARTDMAEnable(p_hw->base_address, UART_DMA_RX);

    /* Mark RX DMA as active */
    p_state->rx_ongoing = true;

    return true;
}

/*
 * @brief   Stop TX DMA transfer
 * @param   p_config    UART configuration
 * @return  Number of bytes remaining
 */
uint32_t ti_uart_dma_stop_tx(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw;
    uint32_t bytes_remaining = 0;

    if (p_config == NULL || p_config->p_hw_config == NULL)
    {
        return 0;
    }

    p_hw = p_config->p_hw_config;

    /* Disable UART DMA request first */
    UARTDMADisable(p_hw->base_address, UART_DMA_TX);

    /* Get remaining bytes before disabling channel */
    bytes_remaining = uDMAChannelSizeGet(UDMA0_BASE, p_hw->dma_tx_channel);

    /* Disable DMA channel */
    uDMAChannelDisable(UDMA0_BASE, p_hw->dma_tx_channel);

    /* Clear completion flag */
    uDMAIntClear(UDMA0_BASE, (1 << p_hw->dma_tx_channel));

    return bytes_remaining;
}

/*
 * @brief   Stop RX DMA and calculate bytes received
 * @param   p_config    UART configuration
 * @return  Number of bytes received (for partial buffer handling)
 */
uint32_t ti_uart_dma_stop_rx(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw;
    uart_state_t *p_state;
    uint32_t bytes_remaining = 0;
    uint32_t bytes_received = 0;

    if (p_config == NULL || p_config->p_hw_config == NULL || p_config->p_state == NULL)
    {
        return 0;
    }

    p_hw = p_config->p_hw_config;
    p_state = p_config->p_state;

    /* Guard: return 0 if RX DMA was never started (prevents spurious data) */
    if (!p_state->rx_ongoing)
    {
        return 0;
    }

    /* Disable UART RX DMA request first */
    UARTDMADisable(p_hw->base_address, UART_DMA_RX);

    /* Get remaining bytes before disabling channel */
    bytes_remaining = uDMAChannelSizeGet(UDMA0_BASE, p_hw->dma_rx_channel);
    bytes_received = BUFFER_SIZE - bytes_remaining;

    /* Disable DMA channel */
    uDMAChannelDisable(UDMA0_BASE, p_hw->dma_rx_channel);

    /* Clear DMA completion flag */
    uDMAIntClear(UDMA0_BASE, (1 << p_hw->dma_rx_channel));

    /* Disable RX timeout interrupt (will be re-enabled on restart) */
    UARTIntDisable(p_hw->base_address, UART_INT_RT);

    /* Mark RX DMA as stopped */
    p_state->rx_ongoing = false;

    return bytes_received;
}
