/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 */

/*
 * uart_ints.c
 *
 *  Created on: 01.12.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    uart_ints.c
 * @brief   UART interrupt handlers
 *
 * TX: DMA completion → EOT detection
 * RX: DMA completion + timeout → buffer swap
 ******************************************************************************/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "uart_internal.h"
#include "api.h"

/* Need hw_uart.h for UART_O_* register offset defines and uart.h for UART_INT_* */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include DeviceFamily_constructPath_SDK(inc/hw_uart.h)
#include DeviceFamily_constructPath_SDK(driverlib/uart.h)
#pragma GCC diagnostic pop

/******************************************************************************
 * DEFINES
 ******************************************************************************/

/**
 * @brief   Get instance ID from configuration structure
 * @param   p_config UART configuration
 * @return  Instance ID for callback invocation
 */
#define GET_INSTANCE_ID(p_config) ((p_config)->instance)

/******************************************************************************
 * STATIC DATA
 ******************************************************************************/

/* Forward declarations for ISR wrappers */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void __attribute__((__interrupt__)) UART0_IRQHandler(void);
#if USART_MULTI_MAX_INSTANCES >= 2
static void __attribute__((__interrupt__)) UART1_IRQHandler(void);
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
static void __attribute__((__interrupt__)) UART2_IRQHandler(void);
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
static void __attribute__((__interrupt__)) UART3_IRQHandler(void);
#endif
#pragma GCC pop_options

/* ISR function pointer table indexed by UART instance */
static app_lib_system_irq_handler_f const m_uart_isr_table[USART_MULTI_MAX_INSTANCES] = {
    UART0_IRQHandler,
#if USART_MULTI_MAX_INSTANCES >= 2
    UART1_IRQHandler,
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
    UART2_IRQHandler,
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
    UART3_IRQHandler
#endif
};

/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

/*
 * @brief   Handle RX DMA completion - swap buffers and notify app
 * @param   p_config    UART configuration
 */
static void handle_rx_dma_done(const uart_config_t *p_config)
{
    uart_state_t *p_state = p_config->p_state;

    /* Ignore interrupt if receiver has been disabled (abort_receive called) */
    if (!p_state->receiver_on)
    {
        return;
    }

    uint32_t bytes_received = ti_uart_dma_stop_rx(p_config);

    /* No data (spurious interrupt), restart DMA */
    if (bytes_received == 0)
    {
        ti_uart_dma_start_rx(p_config);
        return;
    }

    /* Save current buffer pointer, swap buffers, restart DMA */
    uint8_t *p_callback_buffer = DoubleBuffer_getActive(p_state->rx_buffers);
    DoubleBuffer_swipe(p_state->rx_buffers);
    ti_uart_dma_start_rx(p_config);

    /* Notify application with stable buffer pointer */
    if (p_state->rx_callback != NULL)
    {
        p_state->rx_callback(GET_INSTANCE_ID(p_config), p_callback_buffer, bytes_received);
    }
}

/*
 * @brief   Handle RX timeout - partial buffer with buffer swap
 * @param   p_config    UART configuration
 */
static void handle_rx_timeout(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;
    uart_state_t *p_state = p_config->p_state;

    /* Ignore interrupt if receiver has been disabled (abort_receive called) */
    if (!p_state->receiver_on)
    {
        return;
    }

    uint32_t bytes_received = ti_uart_dma_stop_rx(p_config);
    uint32_t fifo_count = 0;
    uint8_t *p_callback_buffer = DoubleBuffer_getActive(p_state->rx_buffers);

    /* Drain remaining bytes from FIFO (< burst size bytes not transferred by DMA) */
    while (UARTCharsAvail(p_hw->base_address) &&
           (bytes_received + fifo_count) < BUFFER_SIZE)
    {
        int32_t ch = UARTCharGetNonBlocking(p_hw->base_address);
        if (ch == -1)
        {
            break;
        }
        p_callback_buffer[bytes_received + fifo_count] = (uint8_t)ch;
        fifo_count++;
    }

    bytes_received += fifo_count;

    /* No data (spurious timeout), restart DMA */
    if (bytes_received == 0)
    {
        ti_uart_dma_start_rx(p_config);
        return;
    }

    /* Swap buffers, restart DMA, notify application */
    DoubleBuffer_swipe(p_state->rx_buffers);
    ti_uart_dma_start_rx(p_config);

    if (p_state->rx_callback != NULL)
    {
        p_state->rx_callback(GET_INSTANCE_ID(p_config), p_callback_buffer, bytes_received);
    }
}

/*
 * @brief   Handle TX DMA completion
 * @param   p_config    UART configuration
 */
static void handle_tx_dma_done(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;
    uart_state_t *p_state = p_config->p_state;
    uint8_t *p_next_buffer;
    uint32_t next_length;

    /* Stop DMA */
    ti_uart_dma_stop_tx(p_config);
    p_state->tx_ongoing = false;

    /* Check if more data is waiting in the other buffer */
    next_length = DoubleBuffer_getIndex(p_state->tx_buffers);
    if (next_length == 0)
    {
        /* No more data - enable EOT to wait for shift register empty */
        UARTIntEnable(p_hw->base_address, UART_INT_EOT);
        return;
    }

    /* Swap buffers and chain next transfer */
    DoubleBuffer_swipe(p_state->tx_buffers);
    p_next_buffer = DoubleBuffer_getActive(p_state->tx_buffers);

    if (ti_uart_dma_start_tx(p_config, p_next_buffer, next_length))
    {
        p_state->tx_ongoing = true;
    }
    else
    {
        UARTIntEnable(p_hw->base_address, UART_INT_EOT);
    }
}

/*
 * @brief   Handle EOT (End of Transmission) interrupt
 * @param   p_config    UART configuration
 */
static void handle_tx_eot(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;
    uart_state_t *p_state = p_config->p_state;

    /* Wait for shift register to finish transmitting */
    while (UARTBusy(p_hw->base_address))
    {
        /* Wait for last byte to shift out */
    }

    /* Disable TX */
    HWREG(p_hw->base_address + UART_O_CTL) &= ~UART_CTL_TXE;

    /* Disable EOT interrupt */
    UARTIntDisable(p_hw->base_address, UART_INT_EOT);

    /* Notify application that transmission is complete */
    if (p_state->tx_callback != NULL)
    {
        p_state->tx_callback(GET_INSTANCE_ID(p_config));
    }
}

/*
 * @brief   Check for UART errors and invoke error callback
 * @param   p_config    UART configuration
 * @param   status      UART interrupt status
 */
static void handle_uart_errors(const uart_config_t *p_config, uint32_t status)
{
    uart_state_t *p_state = p_config->p_state;
    uart_error_stats_t *p_stats = p_config->p_error_stats;
    uint32_t error_flags = USART_MULTI_ERROR_FLAG_NONE;

    /* Map UART error interrupts to our error flags */
    if (status & UART_INT_OE)
    {
        p_stats->overrun_errors++;
        error_flags |= USART_MULTI_ERROR_FLAG_OVERRUN;
    }
    if (status & UART_INT_FE)
    {
        p_stats->framing_errors++;
        error_flags |= USART_MULTI_ERROR_FLAG_FRAMING;
    }
    if (status & UART_INT_PE)
    {
        p_stats->parity_errors++;
        error_flags |= USART_MULTI_ERROR_FLAG_PARITY;
    }
    if (status & UART_INT_BE)
    {
        p_stats->break_errors++;
        error_flags |= USART_MULTI_ERROR_FLAG_BREAK;
    }

    /* Notify application of errors */
    if (error_flags != 0 && p_state->error_callback != NULL)
    {
        p_state->error_callback(GET_INSTANCE_ID(p_config), error_flags);
    }
}

/*
 * @brief   UART interrupt handler - routes TX/RX/EOT/error events
 * @param   p_config    UART configuration
 */
static void uart_interrupt_handler(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;
    uart_state_t *p_state = p_config->p_state;
    uint32_t status;
    uint32_t dma_status;
    uint32_t our_dma_channels;

    /* Read masked UART interrupt status (only enabled interrupts) */
    status = UARTIntStatus(p_hw->base_address, true);

    /* Read DMA interrupt status (DMA completion fires on UART_COMB IRQ) */
    dma_status = uDMAIntStatus(UDMA0_BASE);

    /* Clear UART interrupt flags */
    UARTIntClear(p_hw->base_address, status);

    /* Clear only OUR DMA channel flags (TX and RX) */
    our_dma_channels = (1 << p_hw->dma_tx_channel) | (1 << p_hw->dma_rx_channel);
    if (dma_status & our_dma_channels)
    {
        uDMAIntClear(UDMA0_BASE, dma_status & our_dma_channels);
    }

    /* Handle UART errors (overrun, framing, parity, break) */
    if (status & (UART_INT_OE | UART_INT_FE | UART_INT_PE | UART_INT_BE))
    {
        handle_uart_errors(p_config, status);
    }

    /* Handle RX DMA completion (full buffer) */
    if (dma_status & (1 << p_hw->dma_rx_channel))
    {
        handle_rx_dma_done(p_config);
    }

    /* Handle RX timeout (partial buffer detection) */
    if (status & UART_INT_RT)
    {
        handle_rx_timeout(p_config);
    }

    /* Handle TX DMA completion */
    if ((dma_status & (1 << p_hw->dma_tx_channel)) && p_state->tx_ongoing)
    {
        handle_tx_dma_done(p_config);
    }

    /* Handle EOT (End of Transmission), shift register empty.
     * Guard: if handle_tx_dma_done() just chained a new buffer above,
     * tx_ongoing is true again, do NOT disable TX (that would stall the new DMA).
     * 
     * This race occurs when radio/SCAN ISRs delay the UART ISR long enough
     * for both DMA-done and EOT to become pending simultaneously. */
    if (status & UART_INT_EOT)
    {
        if (p_state->tx_ongoing)
        {
            UARTIntDisable(p_hw->base_address, UART_INT_EOT);
        }
        else
        {
            handle_tx_eot(p_config);
        }
    }
}

/*
 * @brief   UART0 interrupt service routine
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void __attribute__((__interrupt__)) UART0_IRQHandler(void)
{
    uart_interrupt_handler(uart_get_config(USART_MULTI_INSTANCE_0));
}
#pragma GCC pop_options

#if USART_MULTI_MAX_INSTANCES >= 2
/*
 * @brief   UART1 interrupt service routine
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void __attribute__((__interrupt__)) UART1_IRQHandler(void)
{
    uart_interrupt_handler(uart_get_config(USART_MULTI_INSTANCE_1));
}
#pragma GCC pop_options
#endif

#if USART_MULTI_MAX_INSTANCES >= 3
/*
 * @brief   UART2 interrupt service routine
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void __attribute__((__interrupt__)) UART2_IRQHandler(void)
{
    uart_interrupt_handler(uart_get_config(USART_MULTI_INSTANCE_2));
}
#pragma GCC pop_options
#endif

#if USART_MULTI_MAX_INSTANCES >= 4
/*
 * @brief   UART3 interrupt service routine
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void __attribute__((__interrupt__)) UART3_IRQHandler(void)
{
    uart_interrupt_handler(uart_get_config(USART_MULTI_INSTANCE_3));
}
#pragma GCC pop_options
#endif

/******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/*
 * @brief   Register interrupt handler in RAM vector table
 * @param   p_config    UART configuration
 * @return  true if successful, false otherwise
 */
bool ti_uart_register_isr(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw;
    usart_multi_instance_id_t instance;
    app_lib_system_irq_handler_f isr_handler;

    if (p_config == NULL || p_config->p_hw_config == NULL)
    {
        return false;
    }

    p_hw = p_config->p_hw_config;
    instance = p_config->instance;

    if (instance >= USART_MULTI_MAX_INSTANCES)
    {
        return false;
    }

    /* Select ISR wrapper from table */
    isr_handler = m_uart_isr_table[instance];

    /* Clear any pending interrupts */
    Sys_clearFastAppIrq(p_hw->irq_number);

    /* Register interrupt handler */
    if (Sys_enableFastAppIrq(p_hw->irq_number,
                             APP_LIB_SYSTEM_IRQ_PRIO_HI,
                             isr_handler) != APP_RES_OK)
    {
        return false;
    }

    return true;
}

/*
 * @brief   Unregister interrupt handler
 * @param   p_config    UART configuration
 */
void ti_uart_unregister_isr(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw;

    if (p_config == NULL || p_config->p_hw_config == NULL)
    {
        return;
    }

    p_hw = p_config->p_hw_config;

    Sys_disableAppIrq(p_hw->irq_number);
    Sys_clearFastAppIrq(p_hw->irq_number);
}
