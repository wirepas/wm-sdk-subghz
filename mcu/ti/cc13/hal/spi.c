/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>
#include <stddef.h>

#include "board.h"
#include "mcu.h"
#include "spi.h"

#include DeviceFamily_constructPath_SDK(inc/hw_memmap.h)
#include DeviceFamily_constructPath_SDK(inc/hw_types.h)
#include DeviceFamily_constructPath_SDK(driverlib/prcm.h)
#include DeviceFamily_constructPath_SDK(driverlib/ioc.h)
#include DeviceFamily_constructPath_SDK(driverlib/spi.h)

#if defined(USE_SPI0)
#define SPI_DEV    SPI0_BASE
#define SPI_PERIPH PRCM_PERIPH_SPI0
#elif defined(USE_SPI1)
#define SPI_DEV    SPI1_BASE
#define SPI_PERIPH PRCM_PERIPH_SPI1
#elif defined(USE_SPI2)
#define SPI_DEV    SPI2_BASE
#define SPI_PERIPH PRCM_PERIPH_SPI2
#elif defined(USE_SPI3)
#define SPI_DEV    SPI3_BASE
#define SPI_PERIPH PRCM_PERIPH_SPI3
#else
#error You must specify either USE_SPI0, USE_SPI1, USE_SPI2 or USE_SPI3 in your board.h
#endif

/** According to the CC1354P10 data sheet SPI supports master/slave operation up
 *  to 12 MHz */
#define SPI_MAX_BIT_RATE (12000000UL)

/** SPI data width is one byte */
#define SPI_DATA_WIDTH_BITS (8U)

/** Overread character that is sent when read_size > write_size and there is no
 *  more data to send */
#define SPI_ORC (0xFF)

/** Is SPI module initialized */
static bool m_initialized;

/** Power domains that were powered on by the SPI module */
static uint32_t m_power_domains;


/**
 * \brief   Get SPI mode for CC13x4
 * \param   mode
 *          SPI mode of operation
 * \param   ti_mode
 *          SPI mode for CC13x4
 * \return  True if CC13x4 mode was found, false otherwise
 */
static bool get_spi_mode(spi_mode_e mode, uint32_t * ti_mode)
{
    bool result = true;
    switch (mode)
    {
        case SPI_MODE_LOW_FIRST:
            *ti_mode = SPI_FRF_MOTO_MODE_2;
            break;

        case SPI_MODE_LOW_SECOND:
            *ti_mode = SPI_FRF_MOTO_MODE_3;
            break;

        case SPI_MODE_HIGH_FIRST:
            *ti_mode = SPI_FRF_MOTO_MODE_0;
            break;

        case SPI_MODE_HIGH_SECOND:
            *ti_mode = SPI_FRF_MOTO_MODE_1;
            break;

        default:
            /* Invalid SPI mode */
            result = false;
    }

    return result;
}


/**
 * \brief   Release a GPIO pin that was used by the SPI module
 * \param   pin
 *          GPIO pin to release
 */
static void release_gpio(uint32_t pin)
{
    IOCPinTypeGpioInput(pin);
    IOCIOInputSet(pin, IOC_INPUT_DISABLE);
}


/**
 * \brief   Check if SPI transfer parameters are valid
 * \param   xfer_p
 *          SPI transfer
 * \param   cb
 *          Transfer callback
 * \return  True if transfer is valid, false otherwise
 */
static bool is_valid_transfer(const spi_xfer_t *        xfer_p,
                              spi_on_transfer_done_cb_f cb)
{
    bool result = false;

    /* Only synchronous transfers are supported */
    if (cb == NULL)
    {
        if (xfer_p && (xfer_p->write_ptr || (xfer_p->write_size == 0))
            && (xfer_p->read_ptr || (xfer_p->read_size == 0)))
        {
            result = true;
        }
    }

    return result;
}


/**
 * \brief   Transfer a byte over SPI
 * \param   spi_base
 *          SPI instance
 * \param   tx_data
 *          Data to transfer
 * \return  Data that was received from SPI
 */
static uint8_t transfer(uint32_t spi_base, uint8_t tx_data)
{
    SPIDataPut(spi_base, tx_data);

    uint32_t rx_data;
    SPIDataGet(spi_base, &rx_data);
    return rx_data;
}


spi_res_e SPI_init(spi_conf_t * conf_p)
{
    if (m_initialized)
    {
        return SPI_RES_ALREADY_INITIALIZED;
    }
    else if (conf_p == NULL)
    {
        return SPI_RES_INVALID_CONFIG;
    }
    else if ((conf_p->clock == 0) || (conf_p->clock > SPI_MAX_BIT_RATE))
    {
        return SPI_RES_INVALID_CONFIG;
    }
    else if ((conf_p->bit_order != SPI_ORDER_MSB)
             && (conf_p->bit_order != SPI_ORDER_LSB))
    {
        return SPI_RES_INVALID_CONFIG;
    }

    uint32_t ti_protocol;
    if (!get_spi_mode(conf_p->mode, &ti_protocol))
    {
        return SPI_RES_INVALID_CONFIG;
    }

    m_power_domains = 0;
    if (PRCMPowerDomainsAllOn(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON)
    {
        m_power_domains |= PRCM_DOMAIN_PERIPH;
    }

    if (PRCMPowerDomainsAllOn(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON)
    {
        m_power_domains |= PRCM_DOMAIN_SERIAL;
    }

    if (m_power_domains)
    {
        PRCMPowerDomainOn(m_power_domains);
        while (PRCMPowerDomainsAllOn(m_power_domains) != PRCM_DOMAIN_POWER_ON)
        {
            /* Wait until all domains are powered on */
        }
    }

    /* Enable the SPI peripheral in run and sleep modes */
    PRCMPeripheralRunEnable(SPI_PERIPH);
    PRCMPeripheralSleepEnable(SPI_PERIPH);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
        /* Wait until modified register values are propagated to HW */
    }

    IOCPinTypeSpiMaster(SPI_DEV,
                        BOARD_SPI_MISO_PIN,
                        BOARD_SPI_MOSI_PIN,
                        IOID_UNUSED,
                        BOARD_SPI_SCK_PIN);

    SPIConfigSetExpClk(SPI_DEV,
                       GET_MCU_CLOCK,
                       ti_protocol,
                       SPI_MODE_CONTROLLER,
                       conf_p->clock,
                       SPI_DATA_WIDTH_BITS);

    /* SPIConfigSetExpClk sets bit order to MSB -> switch to LSB if requested */
    if (conf_p->bit_order == SPI_ORDER_LSB)
    {
        HWREG(SPI_DEV + SPI_O_CTL1) &= ~(SPI_CTL1_MSB_M);
    }

    SPIEnable(SPI_DEV);

    /* Flush residual data from SPI */
    uint32_t data;
    while (SPIDataGetNonBlocking(SPI_DEV, &data))
    {
    }

    m_initialized = true;
    return SPI_RES_OK;
}


spi_res_e SPI_close()
{
    if (!m_initialized)
    {
        return SPI_RES_NOT_INITIALIZED;
    }

    SPIDisable(SPI_DEV);

    release_gpio(BOARD_SPI_MISO_PIN);
    release_gpio(BOARD_SPI_MOSI_PIN);
    release_gpio(BOARD_SPI_SCK_PIN);

    /* Disable the SPI peripheral in run and sleep modes */
    PRCMPeripheralRunDisable(SPI_PERIPH);
    PRCMPeripheralSleepDisable(SPI_PERIPH);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
        /* Wait until modified register values are propagated to HW */
    }

    if (m_power_domains)
    {
        /* Power off domains that were powered on by the SPI driver */
        PRCMPowerDomainOff(m_power_domains);
    }

    m_initialized = false;
    return SPI_RES_OK;
}


spi_res_e SPI_transfer(spi_xfer_t * xfer_p, spi_on_transfer_done_cb_f cb)
{
    if (!m_initialized)
    {
        return SPI_RES_NOT_INITIALIZED;
    }
    else if (!is_valid_transfer(xfer_p, cb))
    {
        return SPI_RES_INVALID_XFER;
    }
    else if (SPIBusy(SPI_DEV))
    {
        /* A transfer is already ongoing */
        return SPI_RES_BUSY;
    }

    /* Send data over SPI */
    for (size_t i = 0; i < xfer_p->write_size; ++i)
    {
        uint8_t rx_data = transfer(SPI_DEV, xfer_p->write_ptr[i]);
        if (i < xfer_p->read_size)
        {
            xfer_p->read_ptr[i] = rx_data;
        }
    }

    /* Receive more data if read_size is larger than write_size */
    for (size_t j = xfer_p->write_size; j < xfer_p->read_size; ++j)
    {
        xfer_p->read_ptr[j] = transfer(SPI_DEV, SPI_ORC);
    }

    return SPI_RES_OK;
}
