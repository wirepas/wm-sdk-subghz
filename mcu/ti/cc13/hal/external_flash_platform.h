/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef EXTERNAL_FLASH_PLATFORM_H
#define EXTERNAL_FLASH_PLATFORM_H

#include <stdint.h>

#include "board.h"
#include "mcu.h"

#include DeviceFamily_constructPath_SDK(driverlib/cpu.h)
#include DeviceFamily_constructPath_SDK(driverlib/prcm.h)
#include DeviceFamily_constructPath_SDK(driverlib/ioc.h)
#include DeviceFamily_constructPath_SDK(driverlib/spi.h)
#include DeviceFamily_constructPath_SDK(driverlib/gpio.h)

/** SPI frequency for external flash */
#define SPI_EXT_FLASH_FREQUENCY (8000000UL)

/** SPI data width is one byte */
#define SPI_DATA_WIDTH_BITS (8U)

/** When defined flash initialization status is indicated with LEDs:
 *  - one LED: flash initialization was successful
 *  - two LEDs: flash initialization failed */
// #define EXT_FLASH_DRIVER_DEBUG_LED


/**
 * \brief   Active wait.
 * \param   wait_ms
 *          Number of milliseconds to wait
 */
__attribute__((__always_inline__)) static inline void active_wait_ms(uint32_t wait_ms)
{
    /** Delay in microseconds is converted to loop counter value like this:
     *  [delay in µs] * [CPU clock in MHz] / [cycles per loop]
     *
     *  The implementation expects that code is in flash, cache is enabled and
     * cache prefetching is enabled (i.e. "cycles per loop" is 4 according to
     * the DriverLib documentation). With these assumptions delay value
     * calculation becomes: wait_ms * 1000 * 48 / 4.
     */
    uint32_t cpu_delay
        = (((wait_ms * 1000UL) * ((GET_MCU_CLOCK) / 1000000UL)) >> 2);

    CPUdelay(cpu_delay);
}


/**
 * \brief   Set Chip Select (CS) active. CS is active low.
 */
__attribute__((__always_inline__)) static inline void ext_flash_set_cs_active(void)
{
    GPIO_clearDio(BOARD_SPI_EXTFLASH_CS_PIN);
}


/**
 * \brief   Set Chip Select (CS) inactive. CS is active low.
 */
__attribute__((__always_inline__)) static inline void ext_flash_set_cs_inactive(void)
{
    GPIO_setDio(BOARD_SPI_EXTFLASH_CS_PIN);
}


/**
 * \brief   Turn power on in required power domains and enable needed
 *          peripherals.
 */
__attribute__((__always_inline__)) static inline void ext_flash_init_platform(void)
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

    /* Enable the SPI peripheral in run and sleep modes */
    PRCMPeripheralRunEnable(BOARD_SPI_EXTFLASH_PERIPH);
    PRCMPeripheralSleepEnable(BOARD_SPI_EXTFLASH_PERIPH);

    /* Enable the GPIO peripheral in run and sleep modes */
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);

    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
        /* Wait until modified register values are propagated to HW */
    }

    IOCPinTypeSpiMaster(BOARD_SPI_EXTFLASH_DEV,
                        BOARD_SPI_EXTFLASH_MISO_PIN,
                        BOARD_SPI_EXTFLASH_MOSI_PIN,
                        IOID_UNUSED,
                        BOARD_SPI_EXTFLASH_SCLK_PIN);

    IOCPinTypeGpioOutput(BOARD_SPI_EXTFLASH_CS_PIN);
    ext_flash_set_cs_inactive();

    SPIConfigSetExpClk(BOARD_SPI_EXTFLASH_DEV,
                       GET_MCU_CLOCK,
                       SPI_FRF_MOTO_MODE_3,
                       SPI_MODE_CONTROLLER,
                       SPI_EXT_FLASH_FREQUENCY,
                       SPI_DATA_WIDTH_BITS);

    SPIEnable(BOARD_SPI_EXTFLASH_DEV);

    // Flush residual data from SPI
    uint32_t data;
    while (SPIDataGetNonBlocking(BOARD_SPI_EXTFLASH_DEV, &data))
    {
    }
}


/**
 * \brief   Transfer one byte over SPI.
 * \return  Byte that was received over SPI
 */
__attribute__((__always_inline__)) static inline uint8_t ext_flash_spi_transfer(uint8_t data)
{
    SPIDataPut(BOARD_SPI_EXTFLASH_DEV, data);

    uint32_t rx_data;
    SPIDataGet(BOARD_SPI_EXTFLASH_DEV, &rx_data);
    return rx_data;
}


#if defined(EXT_FLASH_DRIVER_DEBUG_LED)
static const uint8_t m_gpio_pin_map[] = BOARD_GPIO_PIN_LIST;
static const uint8_t m_led_id_map[]   = BOARD_LED_ID_LIST;


static void ext_flash_debug_init(void)
{
    // Configure LED GPIOs to output and switch LEDs off
    for (uint8_t i = 0; i < (sizeof(m_led_id_map) / sizeof(m_led_id_map[0]));
         ++i)
    {
        uint8_t pin = m_gpio_pin_map[m_led_id_map[i]];
        IOCPinTypeGpioOutput(pin);
        GPIO_clearDio(pin);
    }
}


static void ext_flash_debug_success(void)
{
    // one LED = OK
    GPIO_setDio(m_gpio_pin_map[m_led_id_map[0]]);
    GPIO_clearDio(m_gpio_pin_map[m_led_id_map[1]]);
}


static void ext_flash_debug_fail(void)
{
    // two LEDs = error
    GPIO_setDio(m_gpio_pin_map[m_led_id_map[0]]);
    GPIO_setDio(m_gpio_pin_map[m_led_id_map[1]]);
}


#else  // defined(EXT_FLASH_DRIVER_DEBUG_LED)


static void ext_flash_debug_init(void)
{
}


static void ext_flash_debug_success(void)
{
}


static void ext_flash_debug_fail(void)
{
}

#endif  // defined EXT_FLASH_DRIVER_DEBUG_LED

#endif  // EXTERNAL_FLASH_PLATFORM_H
