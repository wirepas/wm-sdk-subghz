/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>

#include "mcu.h"
#include "board.h"
#include "usart_bl_test.h"

#include DeviceFamily_constructPath_SDK(driverlib/prcm.h)
#include DeviceFamily_constructPath_SDK(driverlib/ioc.h)
#include DeviceFamily_constructPath_SDK(driverlib/uart.h)

/** Address of UART device */
#define UART_DEV UART0_BASE

/** Peripheral ID of UART device */
#define UART_PERIPH PRCM_PERIPH_UART0


void Usart_init(uint32_t baudrate)
{
    uint32_t power_domains = 0;
    if (PRCMPowerDomainsAllOn(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON)
    {
        power_domains |= PRCM_DOMAIN_PERIPH;
    }

    if (PRCMPowerDomainsAllOn(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON)
    {
        power_domains |= PRCM_DOMAIN_SERIAL;
    }

    if (power_domains)
    {
        PRCMPowerDomainOn(power_domains);
        while (PRCMPowerDomainsAllOn(power_domains) != PRCM_DOMAIN_POWER_ON)
        {
            /* Wait until all domains are powered on */
        }
    }

    /* Enable the UART peripheral */
    PRCMPeripheralRunEnable(UART_PERIPH);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
        /* Wait until modified register values are propagated to HW */
    }

    IOCPinTypeUart(UART_DEV,
                   BOARD_UART_RX_PIN,
                   BOARD_UART_TX_PIN,
                   IOID_UNUSED,
                   IOID_UNUSED);

    UARTConfigSetExpClk(
        UART_DEV,
        GET_MCU_CLOCK,
        baudrate,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTDMADisable(UART_DEV, (UART_DMA_TX | UART_DMA_RX));
    UARTFIFODisable(UART_DEV);
    UARTHwFlowControlDisable(UART_DEV);
    UARTEnable(UART_DEV);
}


uint32_t Usart_sendBuffer(const void * buf, uint32_t len)
{
    const uint8_t * data = buf;
    for (uint32_t i = 0; i < len; ++i)
    {
        UARTCharPut(UART_DEV, data[i]);
    }

    return len;
}
