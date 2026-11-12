/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/*
 * uart_internal.h
 *
 *  Created on: 14.11.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    uart_internal.h
 * @brief   TI UART driver internal header for CC1354P10
 *
 * This file contains internal definitions, hardware configuration structures,
 * and function prototypes for the TI-specific UART driver implementation.
 *
 * All TI DriverLib includes are contained in this header with compiler
 * warnings suppressed to avoid issues with vendor code.
 *
 *****************************************************************************/

#ifndef UART_INTERNAL_H_
#define UART_INTERNAL_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "hal_api.h"
#include "usart_multi.h"  /* Multi-UART HAL API types and callbacks */

/* TI DriverLib - suppress warnings for vendor headers with inline functions */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include DeviceFamily_constructPath_SDK(driverlib/uart.h)
#include DeviceFamily_constructPath_SDK(driverlib/udma.h)
#include DeviceFamily_constructPath_SDK(driverlib/ioc.h)
#include DeviceFamily_constructPath_SDK(driverlib/prcm.h)
#include DeviceFamily_constructPath_SDK(inc/hw_memmap.h)
#include DeviceFamily_constructPath_SDK(inc/hw_ints.h)
#include DeviceFamily_constructPath_SDK(inc/hw_uart.h)

#pragma GCC diagnostic pop
/* End of vendor header includes */

/******************************************************************************
 * HARDWARE CONFIGURATION
 *****************************************************************************/

/*
 * UART clock frequency (48 MHz)
 *
 * The CC13x4 system is configured to run at 48 MHz from XOSC_HF.
 *
 * UART peripheral derives its clock from the system HF clock (48 MHz)
 */
#define UART_CLOCK_FREQ_HZ 48000000

/* Supported baud rates */
#define UART_BAUD_9600      9600    /* SmartMeter communication */
#define UART_BAUD_19200     19200   /* SmartMeter communication */
#define UART_BAUD_115200    115200  /* For testing */
#define UART_BAUD_125000    125000  /* Dual Comm NIC (sink-gateway) */
#define UART_BAUD_460800    460800  /* AMIv2 debug */
#define UART_BAUD_1000000   1000000 /* Generic (1 Mbps) */

/* FIFO and DMA arbitration configuration
 * RX FIFO trigger > burst size leaves bytes in FIFO for RT timeout
 */
#define UART_RX_FIFO_LEVEL UART_FIFO_RX2_8      /* 8 bytes */
#define UART_DMA_RX_BURST_SIZE UDMA_ARB_4       /* 4 bytes */

#define UART_TX_FIFO_LEVEL UART_FIFO_TX2_8      /* 8 bytes */
#define UART_DMA_TX_BURST_SIZE UDMA_ARB_4

/*
 * Buffer size configuration
 *
 * Must be defined via config.mk:
 *   usart_multi_tx_buffer_size = 256 (or 512)
 *   usart_multi_rx_buffer_size = 256 (or 512)
 *
 * Both sizes must be equal and must be 256 or 512 (doublebuffer.h limitation).
 */
#ifndef USART_MULTI_TX_BUFFER_SIZE
#error "USART_MULTI_TX_BUFFER_SIZE must be defined in config.mk"
#endif

#ifndef USART_MULTI_RX_BUFFER_SIZE
#error "USART_MULTI_RX_BUFFER_SIZE must be defined in config.mk"
#endif

#if USART_MULTI_TX_BUFFER_SIZE != USART_MULTI_RX_BUFFER_SIZE
#error "USART_MULTI_TX_BUFFER_SIZE and USART_MULTI_RX_BUFFER_SIZE must be equal"
#endif

#if (USART_MULTI_TX_BUFFER_SIZE != 256) && (USART_MULTI_TX_BUFFER_SIZE != 512)
// cppcheck-suppress preprocessorErrorDirective
#error "Buffer size must be 256 or 512 (doublebuffer.h limitation)"
#endif

/* Set BUFFER_SIZE for doublebuffer.h */
#define BUFFER_SIZE               USART_MULTI_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE_BYTES BUFFER_SIZE
#define UART_RX_BUFFER_SIZE_BYTES BUFFER_SIZE

/* Include double buffer management utilities */
#include "doublebuffer.h"

/******************************************************************************
 * CONFIGURATION TYPE DEFINITIONS
 *****************************************************************************/

/*
 * Configurable number of UART instances
 *
 * Must be defined via config.mk: usart_multi_max_instances = N
 * which translates to -DUSART_MULTI_MAX_INSTANCES=N
 *
 * Valid range: 1-4 (CC1354P10 has 4 UART peripherals)
 */
#ifndef USART_MULTI_MAX_INSTANCES
#error "USART_MULTI_MAX_INSTANCES must be defined in config.mk"
#endif

#if USART_MULTI_MAX_INSTANCES > 4
#error "CC1354P10 supports maximum 4 UART instances (UART0-UART3)"
#endif

#if USART_MULTI_MAX_INSTANCES < 1
#error "USART_MULTI_MAX_INSTANCES must be at least 1"
#endif

/* Internal alias for array sizing */
#define UART_TOTAL_INSTANCES USART_MULTI_MAX_INSTANCES

/**
 * @brief   Pin configuration for one UART instance
 *
 * Board-specific pin assignments for TX, RX, RTS, CTS signals.
 * Use IOID_UNUSED for pins that are not connected on the board.
 */
typedef struct
{
    uint32_t tx_pin;  /**< TX pin (IOID_x) */
    uint32_t rx_pin;  /**< RX pin (IOID_x) */
    uint32_t rts_pin; /**< RTS pin (IOID_x or IOID_UNUSED) */
    uint32_t cts_pin; /**< CTS pin (IOID_x or IOID_UNUSED) */
} uart_pin_config_t;

/**
 * @brief   Hardware configuration for one UART instance
 *
 * Contains all instance-specific hardware details from CC1354P10 datasheet.
 * These values are fixed by the chip architecture and never change.
 *
 * Reference: CC13x4/CC26x4 Technical Reference Manual
 * - Section: UART Module
 * - Section: μDMA Controller
 * - Section: I/O Controller (IOC)
 * - Section: Power, Reset, and Clock Management (PRCM)
 */
typedef struct
{
    uint32_t base_address;      /**< UART base address (UART0_BASE, etc.) */
    uint32_t prcm_periph;       /**< PRCM peripheral ID (PRCM_PERIPH_UART0, etc.) */
    uint32_t prcm_power_domain; /**< Power domain (PRCM_DOMAIN_SERIAL or PERIPH) */
    uint32_t dma_rx_channel;    /**< DMA RX channel (UDMA_CHAN_UART0_RX, etc.) */
    uint32_t dma_tx_channel;    /**< DMA TX channel (UDMA_CHAN_UART0_TX, etc.) */
    uint32_t ioc_rx_port;       /**< IOC RX port ID (IOC_PORT_MCU_UART0_RX, etc.) */
    uint32_t ioc_tx_port;       /**< IOC TX port ID (IOC_PORT_MCU_UART0_TX, etc.) */
    uint32_t ioc_cts_port;      /**< IOC CTS port ID (IOC_PORT_MCU_UART0_CTS, etc.) */
    uint32_t ioc_rts_port;      /**< IOC RTS port ID (IOC_PORT_MCU_UART0_RTS, etc.) */
    uint32_t irq_number;        /**< Interrupt number (INT_UART0_COMB, etc.) */
} uart_hw_config_t;

/**
 * @brief   UART driver state for one instance
 *
 * Tracks runtime state for a single UART instance.
 * Each UART has its own independent state.
 */
typedef struct
{
    uint32_t              baudrate;     /**< Current baud rate */
    uart_flow_control_e   flow_control; /**< Current flow control mode */
    bool                  initialized;  /**< Initialization flag */
    bool                  enabled;      /**< Enable flag */
    bool                  receiver_on;  /**< Receiver state */
    uint8_t               enable_count; /**< Reference count for enable/disable */

    /* TX double buffer (software-managed buffer swapping) */
    double_buffer_t       tx_buffers;   /**< TX double buffer structure */
    volatile bool         tx_ongoing;   /**< TX DMA transfer in progress flag */

    /* RX double buffer (software-managed ping-pong for continuous reception) */
    double_buffer_t       rx_buffers;   /**< RX double buffer structure */
    volatile bool         rx_ongoing;   /**< RX DMA transfer in progress flag */

    /*
     * Callbacks for usart_multi.h API
     *
     * All callbacks receive instance_id as first parameter and run in ISR context.
     * Set to NULL to disable (except rx_callback which is required for RX).
     */
    usart_multi_rx_callback_f    rx_callback;    /**< RX data callback (required for RX) */
    usart_multi_tx_callback_f    tx_callback;    /**< TX complete callback (optional) */
    usart_multi_error_callback_f error_callback; /**< Error notification callback (optional) */
    usart_multi_idle_callback_f  idle_callback;  /**< Idle line detection callback (optional) */
} uart_state_t;

/**
 * @brief   UART error statistics for one instance
 *
 * Tracks error counts for debugging and diagnostics.
 * Each UART has its own independent error counters.
 */
typedef struct
{
    uint32_t overrun_errors; /**< Overrun error count */
    uint32_t framing_errors; /**< Framing error count */
    uint32_t parity_errors;  /**< Parity error count */
    uint32_t break_errors;   /**< Break error count */
} uart_error_stats_t;

/**
 * @brief   Complete UART configuration (HW + Pins + State)
 *
 * Combines hardware configuration with board-specific pin assignments
 * and runtime state. This is the main configuration structure passed
 * to all TI UART driver functions.
 *
 * The configuration is obtained via uart_get_config() and remains valid
 * throughout the lifetime of the driver.
 *
 * @note    Hardware and pin configs are const (read-only)
 * @note    State pointers point to per-instance mutable state
 */
typedef struct
{
    usart_multi_instance_id_t  instance;       /**< UART instance ID */
    const uart_hw_config_t *   p_hw_config;    /**< Pointer to hardware configuration (const) */
    const uart_pin_config_t *  p_pin_config;   /**< Pointer to pin configuration (const) */
    uart_state_t *             p_state;        /**< Pointer to runtime state (mutable) */
    uart_error_stats_t *       p_error_stats;  /**< Pointer to error statistics (mutable) */
} uart_config_t;

/******************************************************************************
 * CONFIGURATION API
 *****************************************************************************/
/**
 * @brief   Get complete UART configuration for instance
 *
 * Returns a pointer to the complete configuration (hardware + pins + state)
 * for the specified UART instance. The configuration is static and remains
 * valid for the lifetime of the program.
 *
 * @param   instance    UART instance (USART_MULTI_INSTANCE_0, etc.)
 *
 * @return  Pointer to uart_config_t, or NULL if invalid instance
 */
const uart_config_t * uart_get_config(usart_multi_instance_id_t instance);

/******************************************************************************
 * INTERNAL API - uart_ints.c
 *****************************************************************************/

/*
 * @brief   Register UART interrupt handler
 * @param   p_config    Pointer to UART configuration
 * @return  true if successful, false otherwise
 */
bool ti_uart_register_isr(const uart_config_t *p_config);

/*
 * @brief   Unregister UART interrupt handler
 * @param   p_config    Pointer to UART configuration
 */
void ti_uart_unregister_isr(const uart_config_t *p_config);

/******************************************************************************
 * INTERNAL API - uart_dma.c
 *****************************************************************************/

/*
 * @brief   Start TX DMA transfer
 *
 * @param   p_config    Pointer to UART configuration
 * @param   p_data      Pointer to data buffer
 * @param   length      Number of bytes
 *
 * @return  true if started, false otherwise
 */
bool ti_uart_dma_start_tx(const uart_config_t *p_config,
                          const uint8_t *p_data,
                          uint32_t length);

/*
 * @brief   Configure and start RX DMA in ping-pong mode
 *          Called from ISR when UART_INT_RX fires (data in FIFO)
 *
 * @param   p_config    Pointer to UART configuration
 *
 * @return  true if started, false otherwise
 */
bool ti_uart_dma_configure_and_start_rx(const uart_config_t *p_config);

/*
 * @brief   Stop TX DMA transfer
 *
 * @param   p_config    Pointer to UART configuration
 *
 * @return  None
 */
uint32_t ti_uart_dma_stop_tx(const uart_config_t *p_config);

/*
 * @brief   Start RX DMA in ping-pong mode
 *
 * @param   p_config    Pointer to UART configuration
 *
 * @return  true if started, false otherwise
 */
bool ti_uart_dma_start_rx(const uart_config_t *p_config);

/*
 * @brief   Stop RX DMA and return bytes received
 *
 * @param   p_config    Pointer to UART configuration
 *
 * @return  Number of bytes received (for partial buffer handling)
 */
uint32_t ti_uart_dma_stop_rx(const uart_config_t *p_config);

#endif /* UART_INTERNAL_H_ */
