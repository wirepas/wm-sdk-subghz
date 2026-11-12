/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 */

/*
 * uart_hw_config.c
 *
 *  Created on: 25.11.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    uart_hw_config.c
 * @brief   Hardware configuration tables for all UART instances
 *
 * This file contains the hardware configuration for all 4 UART instances
 * on the CC1354P10
 *****************************************************************************/
// CPD-OFF
/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "uart_internal.h"
#include "board.h"

/******************************************************************************
 * STATIC DATA - Hardware Configuration (Read-Only)
 *****************************************************************************/

/*
 * External IRQ numbers (for Wirepas Sys_enableFastAppIrq API)
 * ARM Cortex-M has 16 internal exceptions, so external IRQ = INT_UARTx_COMB - 16
 */
#define UART0_EXT_IRQ  5   /* INT_UART0_COMB (21) - 16 = 5 */
#define UART1_EXT_IRQ  36  /* INT_UART1_COMB (52) - 16 = 36 */
#define UART2_EXT_IRQ  40  /* INT_UART2_COMB (56) - 16 = 40 */
#define UART3_EXT_IRQ  41  /* INT_UART3_COMB (57) - 16 = 41 */

/*
 * Hardware configuration table
 *
 * Source: CC13x4/CC26x4 IOC header file from driverlib
 * - UART0: Base 0x40001000, SERIAL domain, DMA channels 1/2
 * - UART1: Base 0x4000B000, PERIPH domain, DMA channels 5/6
 * - UART2: Base 0x4000C000, PERIPH domain, DMA channels 28/29
 * - UART3: Base 0x4000D000, PERIPH domain, DMA channels 30/31
 */
static const uart_hw_config_t m_uart_hw_configs[UART_TOTAL_INSTANCES] = {
    /* UART0 - SERIAL Power Domain */
    {
        .base_address = UART0_BASE,
        .prcm_periph = PRCM_PERIPH_UART0,
        .prcm_power_domain = PRCM_DOMAIN_SERIAL,
        .dma_rx_channel = UDMA_CHAN_UART0_RX,
        .dma_tx_channel = UDMA_CHAN_UART0_TX,
        .ioc_rx_port = IOC_PORT_MCU_UART0_RX,
        .ioc_tx_port = IOC_PORT_MCU_UART0_TX,
        .ioc_cts_port = IOC_PORT_MCU_UART0_CTS,
        .ioc_rts_port = IOC_PORT_MCU_UART0_RTS,
        .irq_number = UART0_EXT_IRQ
    },
#if USART_MULTI_MAX_INSTANCES >= 2
    /* UART1 - PERIPH Power Domain */
    {
        .base_address = UART1_BASE,
        .prcm_periph = PRCM_PERIPH_UART1,
        .prcm_power_domain = PRCM_DOMAIN_PERIPH,
        .dma_rx_channel = UDMA_CHAN_UART1_RX,
        .dma_tx_channel = UDMA_CHAN_UART1_TX,
        .ioc_rx_port = IOC_PORT_MCU_UART1_RX,
        .ioc_tx_port = IOC_PORT_MCU_UART1_TX,
        .ioc_cts_port = IOC_PORT_MCU_UART1_CTS,
        .ioc_rts_port = IOC_PORT_MCU_UART1_RTS,
        .irq_number = UART1_EXT_IRQ
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
    /* UART2 - PERIPH Power Domain */
    {
        .base_address = UART2_BASE,
        .prcm_periph = PRCM_PERIPH_UART2,
        .prcm_power_domain = PRCM_DOMAIN_PERIPH,
        .dma_rx_channel = UDMA_CHAN_UART2_RX,
        .dma_tx_channel = UDMA_CHAN_UART2_TX,
        .ioc_rx_port = IOC_PORT_MCU_UART2_RX,
        .ioc_tx_port = IOC_PORT_MCU_UART2_TX,
        .ioc_cts_port = IOC_PORT_MCU_UART2_CTS,
        .ioc_rts_port = IOC_PORT_MCU_UART2_RTS,
        .irq_number = UART2_EXT_IRQ
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
    /* UART3 - PERIPH Power Domain */
    {
        .base_address = UART3_BASE,
        .prcm_periph = PRCM_PERIPH_UART3,
        .prcm_power_domain = PRCM_DOMAIN_PERIPH,
        .dma_rx_channel = UDMA_CHAN_UART3_RX,
        .dma_tx_channel = UDMA_CHAN_UART3_TX,
        .ioc_rx_port = IOC_PORT_MCU_UART3_RX,
        .ioc_tx_port = IOC_PORT_MCU_UART3_TX,
        .ioc_cts_port = IOC_PORT_MCU_UART3_CTS,
        .ioc_rts_port = IOC_PORT_MCU_UART3_RTS,
        .irq_number = UART3_EXT_IRQ
    }
#endif
};

/******************************************************************************
 * STATIC DATA - Pin Configuration (Read-Only, Board-Specific)
 *****************************************************************************/

/*
 * Pin configuration table
 *
 * Pin assignments come from board-specific board.h file
 */
static const uart_pin_config_t m_uart_pin_configs[UART_TOTAL_INSTANCES] = {
    /* UART0 - Primary UART (pins defined in board.h) */
    {
        .tx_pin = BOARD_UART_TX_PIN,
        .rx_pin = BOARD_UART_RX_PIN,
        .rts_pin = IOID_UNUSED,
        .cts_pin = IOID_UNUSED
    },
#if USART_MULTI_MAX_INSTANCES >= 2
    /* UART1 (pins defined in board.h) */
    {
        .tx_pin = BOARD_UART1_TX_PIN,
        .rx_pin = BOARD_UART1_RX_PIN,
        .rts_pin = IOID_UNUSED,
        .cts_pin = IOID_UNUSED
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
    /* UART2 (pins defined in board.h) */
    {
        .tx_pin = BOARD_UART2_TX_PIN,
        .rx_pin = BOARD_UART2_RX_PIN,
        .rts_pin = IOID_UNUSED,
        .cts_pin = IOID_UNUSED
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
    /* UART3 (pins defined in board.h) */
    {
        .tx_pin = BOARD_UART3_TX_PIN,
        .rx_pin = BOARD_UART3_RX_PIN,
        .rts_pin = IOID_UNUSED,
        .cts_pin = IOID_UNUSED
    }
#endif
};

/******************************************************************************
 * STATIC DATA - Runtime State (Mutable, Per-Instance)
 *****************************************************************************/

/*
 * Per-instance runtime state
 *
 * Each UART instance has its own independent state.
 * Initialized to safe defaults (all zeros/false).
 */
static uart_state_t m_uart_states[UART_TOTAL_INSTANCES] = {
    /* UART0 State */
    {
        .baudrate     = 0,
        .flow_control = UART_FLOW_CONTROL_NONE,
        .rx_callback  = NULL,
        .initialized  = false,
        .enabled      = false,
        .receiver_on  = false,
        .enable_count = 0
    },
#if USART_MULTI_MAX_INSTANCES >= 2
    /* UART1 State */
    {
        .baudrate     = 0,
        .flow_control = UART_FLOW_CONTROL_NONE,
        .rx_callback  = NULL,
        .initialized  = false,
        .enabled      = false,
        .receiver_on  = false,
        .enable_count = 0
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
    /* UART2 State */
    {
        .baudrate     = 0,
        .flow_control = UART_FLOW_CONTROL_NONE,
        .rx_callback  = NULL,
        .initialized  = false,
        .enabled      = false,
        .receiver_on  = false,
        .enable_count = 0
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
    /* UART3 State */
    {
        .baudrate     = 0,
        .flow_control = UART_FLOW_CONTROL_NONE,
        .rx_callback  = NULL,
        .initialized  = false,
        .enabled      = false,
        .receiver_on  = false,
        .enable_count = 0
    }
#endif
};

/*
 * Per-instance error statistics
 *
 * Each UART instance has its own independent error counters.
 * Initialized to zero.
 */
static uart_error_stats_t m_uart_error_stats[UART_TOTAL_INSTANCES] = {
    /* UART0 Stats */
    {
        .overrun_errors = 0,
        .framing_errors = 0,
        .parity_errors  = 0,
        .break_errors   = 0
    },
#if USART_MULTI_MAX_INSTANCES >= 2
    /* UART1 Stats */
    {
        .overrun_errors = 0,
        .framing_errors = 0,
        .parity_errors  = 0,
        .break_errors   = 0
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
    /* UART2 Stats */
    {
        .overrun_errors = 0,
        .framing_errors = 0,
        .parity_errors  = 0,
        .break_errors   = 0
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
    /* UART3 Stats */
    {
        .overrun_errors = 0,
        .framing_errors = 0,
        .parity_errors  = 0,
        .break_errors   = 0
    }
#endif
};

/******************************************************************************
 * STATIC DATA - Combined Configuration (Read-Only Structure, Mutable State)
 *****************************************************************************/

/*
 * Combined configuration table
 *
 * Combines hardware config + pin config + state pointers for each instance.
 * This is what gets returned by uart_get_config().
 *
 * The structure itself is const (pointers don't change), but the state
 * data pointed to is mutable (runtime state can change).
 */
static const uart_config_t m_uart_configs[UART_TOTAL_INSTANCES] = {
    /* UART0 Configuration */
    {
        .instance      = USART_MULTI_INSTANCE_0,
        .p_hw_config   = &m_uart_hw_configs[USART_MULTI_INSTANCE_0],
        .p_pin_config  = &m_uart_pin_configs[USART_MULTI_INSTANCE_0],
        .p_state       = &m_uart_states[USART_MULTI_INSTANCE_0],
        .p_error_stats = &m_uart_error_stats[USART_MULTI_INSTANCE_0]
    },
#if USART_MULTI_MAX_INSTANCES >= 2
    /* UART1 Configuration */
    {
        .instance      = USART_MULTI_INSTANCE_1,
        .p_hw_config   = &m_uart_hw_configs[USART_MULTI_INSTANCE_1],
        .p_pin_config  = &m_uart_pin_configs[USART_MULTI_INSTANCE_1],
        .p_state       = &m_uart_states[USART_MULTI_INSTANCE_1],
        .p_error_stats = &m_uart_error_stats[USART_MULTI_INSTANCE_1]
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 3
    /* UART2 Configuration */
    {
        .instance      = USART_MULTI_INSTANCE_2,
        .p_hw_config   = &m_uart_hw_configs[USART_MULTI_INSTANCE_2],
        .p_pin_config  = &m_uart_pin_configs[USART_MULTI_INSTANCE_2],
        .p_state       = &m_uart_states[USART_MULTI_INSTANCE_2],
        .p_error_stats = &m_uart_error_stats[USART_MULTI_INSTANCE_2]
    },
#endif
#if USART_MULTI_MAX_INSTANCES >= 4
    /* UART3 Configuration */
    {
        .instance      = USART_MULTI_INSTANCE_3,
        .p_hw_config   = &m_uart_hw_configs[USART_MULTI_INSTANCE_3],
        .p_pin_config  = &m_uart_pin_configs[USART_MULTI_INSTANCE_3],
        .p_state       = &m_uart_states[USART_MULTI_INSTANCE_3],
        .p_error_stats = &m_uart_error_stats[USART_MULTI_INSTANCE_3]
    }
#endif
};

/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/

/*
 * @brief   Get complete UART configuration for instance
 *
 * Returns a pointer to the complete configuration (hardware + pins + state)
 * for the specified UART instance. The configuration structure is static
 * and remains valid for the lifetime of the program.
 *
 * @param   instance    UART instance (USART_MULTI_INSTANCE_0, etc.)
 *
 * @return  Pointer to uart_config_t, or NULL if invalid instance
 *
 * @note    The returned pointer is const, but the state data it points to
 *          is mutable (runtime state can be modified by driver functions)
 */
const uart_config_t * uart_get_config(usart_multi_instance_id_t instance)
{
    /* Validate instance number against configurable limit */
    if (instance >= USART_MULTI_MAX_INSTANCES)
    {
        return NULL;
    }

    /* Return pointer to static configuration */
    return &m_uart_configs[instance];
}

// CPD-ON
