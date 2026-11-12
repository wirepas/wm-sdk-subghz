/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/*
 * uart.c
 *
 *  Created on: 14.11.2025
 *      Author: Wirepas Ltd
 */

/******************************************************************************
 * @file    uart.c
 * @brief   TI CC1354P10 UART driver - implements usart_multi.h API
 *
 * Multi-instance UART driver with DMA transfers and hardware flow control.
 * Opaque instance pointer maps to internal uart_config_t.
 *
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal_api.h"
#include "usart_multi.h"

/* TI DriverLib includes with warnings suppressed */
#include "uart_internal.h"
#include "../dma/dma.h"

/******************************************************************************
 * TYPE CASTING
 ******************************************************************************/

/* Cast between opaque API type and internal config type */
#define INSTANCE_TO_CONFIG(p) ((const uart_config_t *)(p))
#define CONFIG_TO_INSTANCE(p) ((usart_multi_instance_t *)(p))

/******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/*
 * @brief   Configure GPIO pins for UART
 * @param   p_config        UART configuration
 * @param   flow_control    Flow control mode
 *
 * NOTE: TI driverlib IOCPinTypeUart() only supports UART0 (contains ASSERT).
 */
static void configure_uart_pins(const uart_config_t *p_config,
                                uart_flow_control_e flow_control)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;
    const uart_pin_config_t *p_pins = p_config->p_pin_config;

    /* Flow control not supported in this driver version */
    (void)flow_control;

    /* Configure RX pin (input with peripheral function) */
    if (p_pins->rx_pin != IOID_UNUSED)
    {
        IOCPortConfigureSet(p_pins->rx_pin,
                           IOC_PORT_GPIO,
                           IOC_INPUT_ENABLE);
        IOCPortConfigureSet(p_pins->rx_pin,
                           p_hw->ioc_rx_port,
                           IOC_STD_INPUT);
    }

    /* Configure TX pin (output with peripheral function) */
    if (p_pins->tx_pin != IOID_UNUSED)
    {
        IOCPortConfigureSet(p_pins->tx_pin,
                           p_hw->ioc_tx_port,
                           IOC_STD_OUTPUT);
    }
}

/*
 * @brief   Enable UART peripheral and clocks
 * @param   p_config    UART configuration
 */
static void enable_uart_peripheral(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;
    const uart_state_t *p_state = p_config->p_state;

    /* Enable UART peripheral power domain */
    PRCMPowerDomainOn(p_hw->prcm_power_domain);
    while (PRCMPowerDomainsAllOn(p_hw->prcm_power_domain) != PRCM_DOMAIN_POWER_ON) {}

    /* Enable UART peripheral clock */
    PRCMPeripheralRunEnable(p_hw->prcm_periph);
    PRCMPeripheralSleepEnable(p_hw->prcm_periph);
    PRCMLoadSet();
    while (!PRCMLoadGet()) {}

    /* Disable DMA requests (bootloader may have left them enabled) */
    UARTDMADisable(p_hw->base_address, UART_DMA_TX);
    UARTDMADisable(p_hw->base_address, UART_DMA_RX);

    /* Configure UART: 8N1 format */
    UARTConfigSetExpClk(p_hw->base_address,
                        UART_CLOCK_FREQ_HZ,
                        p_state->baudrate,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    /* Configure GPIO pins for UART */
    configure_uart_pins(p_config, p_state->flow_control);

    /* Configure FIFO levels */
    UARTFIFOLevelSet(p_hw->base_address, UART_TX_FIFO_LEVEL, UART_RX_FIFO_LEVEL);

    /* Enable/disable hardware flow control */
    if (p_state->flow_control == UART_FLOW_CONTROL_HW)
    {
        UARTHwFlowControlEnable(p_hw->base_address);
    }
    else
    {
        UARTHwFlowControlDisable(p_hw->base_address);
    }

    /* Clear any stale interrupt flags before enabling UART */
    UARTIntClear(p_hw->base_address, 0xFFFFFFFF);

    /* Enable UART hardware (TX, RX, and FIFOs) */
    UARTEnable(p_hw->base_address);
}

/*
 * @brief   Disable UART peripheral and clocks
 * @param   p_config    UART configuration
 */
static void disable_uart_peripheral(const uart_config_t *p_config)
{
    const uart_hw_config_t *p_hw = p_config->p_hw_config;

    /* Disable UART peripheral */
    UARTDisable(p_hw->base_address);

    /* Disable UART peripheral clock */
    PRCMPeripheralRunDisable(p_hw->prcm_periph);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
        /* Wait for settings to take effect */
    }
}


/*
 * @brief   is_valid_baudrate
 * @param   uint32_t baudrate
 * @return  bool     true if baudrate is valid, false otherwise
 */
static bool is_valid_baudrate(uint32_t baudrate)
{
    if (baudrate != UART_BAUD_9600 &&
        baudrate != UART_BAUD_19200 &&
        baudrate != UART_BAUD_115200 &&
        baudrate != UART_BAUD_125000 &&
        baudrate != UART_BAUD_460800 &&
        baudrate != UART_BAUD_1000000)
    {
        return false;
    }
    return true;
}

/*
 * @brief   Validate instance and get config/state pointers
 * @param   p_instance      Instance pointer
 * @param   pp_config       Output: config pointer (if valid)
 * @param   pp_state        Output: state pointer (if valid)
 * @return  Status code (USART_MULTI_SUCCESS if valid)
 */
static usart_multi_status_t validate_instance(const usart_multi_instance_t *p_instance,
                                               const uart_config_t **pp_config,
                                               uart_state_t **pp_state)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;

    if (p_instance == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_config = INSTANCE_TO_CONFIG(p_instance);
    p_state = p_config->p_state;

    if (!p_state->initialized)
    {
        return USART_MULTI_ERROR_NOT_INITIALIZED;
    }

    if (pp_config != NULL)
    {
        *pp_config = p_config;
    }
    if (pp_state != NULL)
    {
        *pp_state = p_state;
    }

    return USART_MULTI_SUCCESS;
}

/*
 * @brief   Validate initialization parameters
 * @param   pp_instance     Pointer to instance pointer
 * @param   instance_id     Instance ID
 * @param   p_params        Initialization parameters
 * @param   pp_config       Output: config pointer (if valid)
 * @return  Status code (USART_MULTI_SUCCESS if valid)
 */
static usart_multi_status_t validate_init_params(usart_multi_instance_t **pp_instance,
                                                  usart_multi_instance_id_t instance_id,
                                                  const usart_multi_init_params_t *p_params,
                                                  const uart_config_t **pp_config)
{
    const uart_config_t *p_config;

    /* Validate pointers */
    if (pp_instance == NULL || p_params == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    /* Validate instance ID */
    if (instance_id >= USART_MULTI_MAX_INSTANCES)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    /* Check for unsupported features */
    if (p_params->flow_control == USART_MULTI_FLOW_CONTROL_HW)
    {
        return USART_MULTI_ERROR_NOT_SUPPORTED;
    }

    if (!p_params->dma_enabled)
    {
        return USART_MULTI_ERROR_NOT_SUPPORTED;
    }

    /* Validate baud rate */
    if (!is_valid_baudrate(p_params->baudrate))
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    /* Get and validate config */
    p_config = uart_get_config(instance_id);
    if (p_config == NULL ||
        p_config->p_hw_config == NULL ||
        p_config->p_pin_config == NULL ||
        p_config->p_state == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    /* Validate pin configuration */
    if (p_config->p_pin_config->tx_pin == IOID_UNUSED ||
        p_config->p_pin_config->rx_pin == IOID_UNUSED)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    /* Check if already initialized */
    if (p_config->p_state->initialized)
    {
        return USART_MULTI_ERROR_ALREADY_INITIALIZED;
    }

    *pp_config = p_config;
    return USART_MULTI_SUCCESS;
}
/******************************************************************************
 * PUBLIC API - usart_multi.h implementation
 ******************************************************************************/

/**
 * @brief   Initialize UART instance from driver's static pool
 * @details On success, populates *pp_instance with pointer to instance from
 *          driver's memory pool. UART starts disabled - call
 *          usart_multi_set_enabled(true) to activate.
 * @param   pp_instance Pointer to instance pointer (populated by driver)
 * @param   instance_id Instance ID to initialize (USART_MULTI_INSTANCE_0, etc.)
 * @param   p_params Initialization parameters
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Instance initialized successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer, invalid instance_id,
 *          or invalid parameters
 * @retval  USART_MULTI_ERROR_ALREADY_INITIALIZED Instance already initialized
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware initialization failed
 */
usart_multi_status_t usart_multi_init(usart_multi_instance_t **pp_instance,
                                      usart_multi_instance_id_t instance_id,
                                      const usart_multi_init_params_t *p_params)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate all parameters */
    status = validate_init_params(pp_instance, instance_id, p_params, &p_config);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    p_state = p_config->p_state;

    /* Clear state and error statistics */
    memset(p_config->p_state, 0, sizeof(uart_state_t));
    memset(p_config->p_error_stats, 0, sizeof(uart_error_stats_t));

    /* Store configuration in state */
    p_state->baudrate = p_params->baudrate;
    p_state->flow_control = UART_FLOW_CONTROL_NONE;

    /* Initialize DMA controller */
    if (!ti_dma_init())
    {
        return USART_MULTI_ERROR_HARDWARE;
    }

    /* Initialize TX double buffer */
    DoubleBuffer_init(p_state->tx_buffers);
    p_state->tx_ongoing = false;

    /* Initialize RX double buffer */
    DoubleBuffer_init(p_state->rx_buffers);
    p_state->rx_ongoing = false;

    /* Zero-initialize RX buffers */
    memset(p_state->rx_buffers.buffer_1, 0, BUFFER_SIZE);
    memset(p_state->rx_buffers.buffer_2, 0, BUFFER_SIZE);

    /* Mark as initialized but disabled */
    p_state->initialized = true;
    p_state->enabled = false;
    p_state->enable_count = 0;

    /* Register interrupt handler */
    if (!ti_uart_register_isr(p_config))
    {
        p_state->initialized = false;
        return USART_MULTI_ERROR_HARDWARE;
    }

    /* Return instance pointer to caller */
    *pp_instance = CONFIG_TO_INSTANCE(p_config);

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Deinitialize UART instance
 * @details Stops all transfers, disables peripheral, and sets *pp_instance to NULL
 * @param   pp_instance Pointer to instance pointer
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Instance deinitialized successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer or *pp_instance is NULL
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware deinitialization failed
 */
usart_multi_status_t usart_multi_deinit(usart_multi_instance_t **pp_instance)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;

    /* Validate parameters */
    if (pp_instance == NULL || *pp_instance == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_config = INSTANCE_TO_CONFIG(*pp_instance);

    if (p_config->p_state == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_state = p_config->p_state;

    /* Check if already uninitialized */
    if (!p_state->initialized)
    {
        return USART_MULTI_ERROR_NOT_INITIALIZED;
    }

    /* Stop DMA and disable peripheral — only safe while peripheral is clocked.
     * If set_enabled(false) was already called the clock is off and UART
     * registers are inaccessible. */
    if (p_state->enabled)
    {
        ti_uart_dma_stop_tx(p_config);
        ti_uart_dma_stop_rx(p_config);
        disable_uart_peripheral(p_config);
    }

    /* Unregister interrupt handler */
    ti_uart_unregister_isr(p_config);

    /* Clear state flags */
    p_state->initialized = false;
    p_state->enabled = false;
    p_state->receiver_on = false;
    p_state->tx_ongoing = false;
    p_state->enable_count = 0;
    p_state->rx_callback = NULL;
    p_state->tx_callback = NULL;
    p_state->error_callback = NULL;
    p_state->idle_callback = NULL;

    /* Clear caller's pointer */
    *pp_instance = NULL;

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Enable or disable UART peripheral
 * @details Uses reference counting - multiple enables require matching disables
 * @param   p_instance Instance pointer
 * @param   enabled True to enable, false to disable
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS UART enabled/disabled successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware enable/disable failed
 */
usart_multi_status_t usart_multi_set_enabled(usart_multi_instance_t *p_instance,
                                             bool enabled)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;

    /* Validate parameters */
    if (p_instance == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_config = INSTANCE_TO_CONFIG(p_instance);

    if (p_config->p_state == NULL || p_config->p_hw_config == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_state = p_config->p_state;

    /* Check if UART is initialized */
    if (!p_state->initialized)
    {
        return USART_MULTI_ERROR_NOT_INITIALIZED;
    }

    /* Handle enable request */
    if (enabled)
    {
        p_state->enable_count++;
        if (p_state->enable_count == 1)
        {
            /* First enable - actually enable the UART */
            p_state->enabled = true;
            enable_uart_peripheral(p_config);
        }
        return USART_MULTI_SUCCESS;
    }

    /* Handle disable request */
    if (p_state->enable_count == 0)
    {
        return USART_MULTI_SUCCESS;
    }

    p_state->enable_count--;
    if (p_state->enable_count == 0)
    {
        /* Last disable - actually disable the UART */
        p_state->enabled = false;
        disable_uart_peripheral(p_config);
    }

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Start receiver
 * @details RX callback must be registered first using
 *          usart_multi_register_rx_callback()
 * @param   p_instance Instance pointer
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Receiver started successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_NOT_ENABLED UART not enabled
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware RX enable failed
 */
usart_multi_status_t usart_multi_enable_receiver(usart_multi_instance_t *p_instance)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, &p_config, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    if (!p_state->enabled)
    {
        return USART_MULTI_ERROR_NOT_ENABLED;
    }

    /* Start RX DMA */
    if (!ti_uart_dma_start_rx(p_config))
    {
        return USART_MULTI_ERROR_HARDWARE;
    }

    p_state->receiver_on = true;
    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Abort ongoing RX transfer and stop receiver
 * @param   p_instance Instance pointer
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Reception aborted successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_NOT_SUPPORTED Abort not supported on this platform
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware abort failed
 */
usart_multi_status_t usart_multi_abort_receive(usart_multi_instance_t *p_instance)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, &p_config, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    /* Stop RX DMA */
    ti_uart_dma_stop_rx(p_config);
    p_state->receiver_on = false;

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Change flow control mode
 * @param   p_instance Instance pointer
 * @param   flow_control Desired flow control mode
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Flow control configured successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer or invalid value
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_BUSY UART is enabled (must disable first)
 * @retval  USART_MULTI_ERROR_NOT_SUPPORTED Flow control not supported on platform
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware configuration failed
 */
usart_multi_status_t usart_multi_set_flow_control(usart_multi_instance_t *p_instance,
                                                  usart_multi_flow_control_e flow_control)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, &p_config, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    /* Cannot change while enabled */
    if (p_state->enabled)
    {
        return USART_MULTI_ERROR_BUSY;
    }

    /* HW flow control not supported on this platform */
    if (flow_control == USART_MULTI_FLOW_CONTROL_HW)
    {
        return USART_MULTI_ERROR_NOT_SUPPORTED;
    }

    /* NONE is already the default, nothing to do */
    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Queue data for transmission
 * @details All-or-nothing: either all data queued (returns SUCCESS) or none.
 *          Non-blocking operation.
 * @param   p_instance Instance pointer
 * @param   p_buf Buffer to transmit (read-only)
 * @param   len Number of bytes to transmit
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS All data queued for transmission
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer or len exceeds max TX size
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_NOT_ENABLED UART not enabled
 * @retval  USART_MULTI_ERROR_BUFFER_FULL TX buffer full, no space for this data
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware error during queuing
 */
usart_multi_status_t usart_multi_send_buffer(usart_multi_instance_t *p_instance,
                                             const void *p_buf,
                                             size_t len)
{
    const uart_config_t *p_config;
    const uart_hw_config_t *p_hw;
    uart_state_t *p_state;

    /* Validate parameters */
    if (p_instance == NULL || p_buf == NULL || len == 0)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    /* Check length against max TX size */
    if (len > BUFFER_SIZE)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_config = INSTANCE_TO_CONFIG(p_instance);
    p_hw = p_config->p_hw_config;
    p_state = p_config->p_state;

    if (!p_state->initialized)
    {
        return USART_MULTI_ERROR_NOT_INITIALIZED;
    }

    if (!p_state->enabled)
    {
        return USART_MULTI_ERROR_NOT_ENABLED;
    }

    /* Check if previous transfer still ongoing */
    if (p_state->tx_ongoing)
    {
        /* DMA is currently active - use double buffering */
        uint8_t *inactive_buffer;
        uint32_t current_offset;

        current_offset = DoubleBuffer_getIndex(p_state->tx_buffers);

        /* Check if inactive buffer has room (all-or-nothing) */
        if (current_offset + len > BUFFER_SIZE)
        {
            return USART_MULTI_ERROR_BUFFER_FULL;
        }

        /* Determine which buffer is inactive */
        if (DoubleBuffer_getActive(p_state->tx_buffers) == p_state->tx_buffers.buffer_1)
        {
            inactive_buffer = p_state->tx_buffers.buffer_2;
        }
        else
        {
            inactive_buffer = p_state->tx_buffers.buffer_1;
        }

        /* Copy data to inactive buffer */
        memcpy(inactive_buffer + current_offset, p_buf, len);
        DoubleBuffer_incrIndex(p_state->tx_buffers, len);

        /* ISR will chain this buffer when current DMA completes */
        return USART_MULTI_SUCCESS;
    }

    /* No DMA ongoing - start fresh transfer */
    memcpy(DoubleBuffer_getActive(p_state->tx_buffers), p_buf, len);

    p_state->tx_ongoing = true;

    /* Enable TX */
    HWREG(p_hw->base_address + UART_O_CTL) |= UART_CTL_TXE;

    /* Start DMA transfer */
    if (!ti_uart_dma_start_tx(p_config, DoubleBuffer_getActive(p_state->tx_buffers), len))
    {
        HWREG(p_hw->base_address + UART_O_CTL) &= ~UART_CTL_TXE;
        p_state->tx_ongoing = false;
        return USART_MULTI_ERROR_HARDWARE;
    }

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Block until all pending TX data transmitted
 * @param   p_instance Instance pointer
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS All data transmitted successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_TIMEOUT Transmission did not complete in time
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware error during transmission
 */
usart_multi_status_t usart_multi_flush(usart_multi_instance_t *p_instance)
{
    const uart_config_t *p_config;
    const uart_hw_config_t *p_hw;
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, &p_config, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    if (!p_state->enabled)
    {
        return USART_MULTI_ERROR_NOT_ENABLED;
    }

    p_hw = p_config->p_hw_config;

    /* Wait for UART to finish transmitting */
    while (UARTBusy(p_hw->base_address))
    {
        /* Busy wait */
    }

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Abort ongoing TX transfer
 * @param   p_instance Instance pointer
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Transmission aborted successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 * @retval  USART_MULTI_ERROR_NOT_SUPPORTED Abort not supported on this platform
 * @retval  USART_MULTI_ERROR_HARDWARE Hardware abort failed
 */
usart_multi_status_t usart_multi_abort_transmit(usart_multi_instance_t *p_instance)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, &p_config, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    /* Stop TX DMA and clear pending data */
    ti_uart_dma_stop_tx(p_config);
    p_state->tx_ongoing = false;
    p_state->tx_buffers.current_writing_index = 0;

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Register RX data callback
 * @details Callback must be provided (cannot be NULL). Callback runs in ISR context.
 * @param   p_instance Instance pointer
 * @param   p_callback Callback function
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Callback registered successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer or NULL callback
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 */
usart_multi_status_t usart_multi_register_rx_callback(usart_multi_instance_t *p_instance,
                                                      usart_multi_rx_callback_f p_callback)
{
    const uart_config_t *p_config;
    uart_state_t *p_state;

    /* Validate parameters */
    if (p_instance == NULL || p_callback == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_config = INSTANCE_TO_CONFIG(p_instance);
    p_state = p_config->p_state;

    if (!p_state->initialized)
    {
        return USART_MULTI_ERROR_NOT_INITIALIZED;
    }

    p_state->rx_callback = p_callback;
    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Register TX complete callback
 * @details Pass NULL to disable. Callback runs in ISR context.
 * @param   p_instance Instance pointer
 * @param   p_callback Callback function (NULL to disable)
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Callback registered successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 */
usart_multi_status_t usart_multi_register_tx_callback(usart_multi_instance_t *p_instance,
                                                      usart_multi_tx_callback_f p_callback)
{
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, NULL, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    p_state->tx_callback = p_callback;  /* NULL allowed to disable */
    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Register error callback for error notification
 * @details Pass NULL to disable. Callback runs in ISR context.
 * @param   p_instance Instance pointer
 * @param   p_callback Callback function (NULL to disable)
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Callback registered successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 */
usart_multi_status_t usart_multi_register_error_callback(usart_multi_instance_t *p_instance,
                                                         usart_multi_error_callback_f p_callback)
{
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, NULL, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    p_state->error_callback = p_callback;  /* NULL allowed to disable */
    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Register idle line detection callback
 * @details Pass NULL to disable. Requires idle_detect_enabled = true during init.
 *          Callback runs in ISR context. Used for variable-length packet detection.
 * @param   p_instance Instance pointer
 * @param   p_callback Callback function (NULL to disable)
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Callback registered successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 */
usart_multi_status_t usart_multi_register_idle_callback(usart_multi_instance_t *p_instance,
                                                        usart_multi_idle_callback_f p_callback)
{
    uart_state_t *p_state;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, NULL, &p_state);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    p_state->idle_callback = p_callback;  /* NULL allowed to disable */
    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Get maximum bytes per usart_multi_send_buffer() call
 * @param   p_instance Instance pointer
 * @return  Maximum TX size in bytes, or 0 if instance is NULL or not initialized
 */
size_t usart_multi_get_max_tx_size(const usart_multi_instance_t *p_instance)
{
    const uart_config_t *p_config;

    if (p_instance == NULL)
    {
        return 0;
    }

    p_config = INSTANCE_TO_CONFIG(p_instance);

    if (!p_config->p_state->initialized)
    {
        return 0;
    }

    return BUFFER_SIZE;
}

/**
 * @brief   Get total UART instances available on platform
 * @return  Number of available UART instances
 */
uint8_t usart_multi_get_instance_count(void)
{
    return USART_MULTI_MAX_INSTANCES;
}

/**
 * @brief   Retrieve accumulated error statistics
 * @param   p_instance Instance pointer
 * @param   p_stats Statistics structure to populate
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Statistics retrieved successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 */
usart_multi_status_t usart_multi_get_error_stats(const usart_multi_instance_t *p_instance,
                                                 usart_multi_error_stats_t *p_stats)
{
    const uart_config_t *p_config;

    /* Validate parameters */
    if (p_instance == NULL || p_stats == NULL)
    {
        return USART_MULTI_ERROR_INVALID_PARAM;
    }

    p_config = INSTANCE_TO_CONFIG(p_instance);

    if (!p_config->p_state->initialized)
    {
        return USART_MULTI_ERROR_NOT_INITIALIZED;
    }

    /* Copy error stats */
    p_stats->overrun_errors = p_config->p_error_stats->overrun_errors;
    p_stats->framing_errors = p_config->p_error_stats->framing_errors;
    p_stats->parity_errors = p_config->p_error_stats->parity_errors;
    p_stats->break_errors = p_config->p_error_stats->break_errors;

    return USART_MULTI_SUCCESS;
}

/**
 * @brief   Reset all error counters to zero
 * @param   p_instance Instance pointer
 * @return  Status code
 * @retval  USART_MULTI_SUCCESS Error counters cleared successfully
 * @retval  USART_MULTI_ERROR_INVALID_PARAM NULL pointer
 * @retval  USART_MULTI_ERROR_NOT_INITIALIZED Instance not initialized
 */
usart_multi_status_t usart_multi_clear_error_stats(usart_multi_instance_t *p_instance)
{
    const uart_config_t *p_config;
    usart_multi_status_t status;

    /* Validate instance */
    status = validate_instance(p_instance, &p_config, NULL);
    if (status != USART_MULTI_SUCCESS)
    {
        return status;
    }

    /* Clear error stats */
    memset(p_config->p_error_stats, 0, sizeof(uart_error_stats_t));

    return USART_MULTI_SUCCESS;
}
