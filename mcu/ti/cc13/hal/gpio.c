/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "board.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "gpio.h"
#include "mcu.h"
#include "api.h"

#include DeviceFamily_constructPath_SDK(inc/hw_memmap.h)
#include DeviceFamily_constructPath_SDK(inc/hw_types.h)
#include DeviceFamily_constructPath_SDK(inc/hw_fcfg1.h)
#include DeviceFamily_constructPath_SDK(driverlib/ioc.h)
#include DeviceFamily_constructPath_SDK(driverlib/gpio.h)
#include DeviceFamily_constructPath_SDK(driverlib/prcm.h)

/* Check that BOARD_GPIO_PIN_LIST is defined. Without it the driver cannot be
 * used. */
#if !defined(BOARD_GPIO_PIN_LIST)
#error "BOARD_GPIO_PIN_LIST not defined in board.h"
#endif  // BOARD_GPIO_PIN_LIST

/** GPIO pin map:
 *  - array index: GPIO ID
 *  - array value: GPIO pin */
static const gpio_pin_t m_id_to_pin_map[] = BOARD_GPIO_PIN_LIST;

/** Number of GPIOs in board configuration */
#define BOARD_GPIO_NUMBER (sizeof(m_id_to_pin_map) / sizeof(m_id_to_pin_map[0]))

/** Indicates whether or not the library is initialized */
static bool m_initialized;

/** GPIO event callback map:
 *  - array index: GPIO ID
 *  - array value: GPIO event callback */
static gpio_in_event_cb_f m_id_to_event_cb_map[BOARD_GPIO_NUMBER];


/**
 * \brief   Check GPIO pins from board configuration.
 * \return  True if pin configuration is valid
 */
static bool check_pins(void)
{
    uint8_t gpio_count
        = ((HWREG(FCFG1_BASE + FCFG1_O_IOCONF) & FCFG1_IOCONF_GPIO_CNT_M)
           >> FCFG1_IOCONF_GPIO_CNT_S);

    bool pins_valid = true;
    for (gpio_id_t id = 0; (pins_valid && (id < BOARD_GPIO_NUMBER)); ++id)
    {
        gpio_pin_t pin;
        gpio_res_e result = Gpio_getPin(id, NULL, &pin);
        pins_valid        = ((result == GPIO_RES_OK) && (pin < gpio_count));
    }

    return pins_valid;
}


/**
 * \brief   Get GPIO pin.
 * \param   id
 *          ID of the pin
 * \param   pin
 *          GPIO pin number that corresponds to id
 * \param   output
 *          Indicates whether the pin is input (false) or output (true)
 * \return  One of the values from \ref gpio_res_e
 */
static gpio_res_e get_pin(gpio_id_t id, gpio_pin_t * pin, bool output)
{
    if (!m_initialized)
    {
        return GPIO_RES_UNINITIALIZED;
    }

    gpio_res_e result = Gpio_getPin(id, NULL, pin);
    if (result == GPIO_RES_OK)
    {
        uint32_t output_enable
            = (output ? GPIO_OUTPUT_ENABLE : GPIO_OUTPUT_DISABLE);

        if (GPIO_getOutputEnableDio(*pin) != output_enable)
        {
            result = GPIO_RES_INVALID_DIRECTION;
        }
    }

    return result;
}


/**
 * \brief   Read GPIO pin.
 * \param   id
 *          ID of the pin
 * \param   level
 *          Logical level of the pin
 * \param   output
 *          Indicates whether the pin is input (false) or output (true)
 * \return  One of the values from \ref gpio_res_e
 */
gpio_res_e gpio_pin_read(gpio_id_t id, gpio_level_e * level, bool output)
{
    if (level == NULL)
    {
        return GPIO_RES_INVALID_PARAM;
    }

    gpio_pin_t pin;
    gpio_res_e result = get_pin(id, &pin, output);
    if (result == GPIO_RES_OK)
    {
        *level = (GPIO_readDio(pin) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    }

    return result;
}


/**
 * \brief   Interrupt handler for GPIO edge detection events.
 */
static void gpio_event_handler(void)
{
    for (gpio_id_t id = 0; id < BOARD_GPIO_NUMBER; ++id)
    {
        gpio_pin_t pin;
        if (Gpio_getPin(id, NULL, &pin) == GPIO_RES_OK)
        {
            if (IOCIntStatus(pin))
            {
                IOCIntClear(pin);

                gpio_in_event_cb_f callback = m_id_to_event_cb_map[id];
                gpio_level_e       level;
                if (callback && (Gpio_inputRead(id, &level) == GPIO_RES_OK))
                {
                    gpio_in_event_e event = ((level == GPIO_LEVEL_HIGH)
                                                 ? GPIO_IN_EVENT_RISING_EDGE
                                                 : GPIO_IN_EVENT_FALLING_EDGE);

                    callback(id, event);
                }
            }
        }
    }
}


gpio_res_e Gpio_init(void)
{
    if (m_initialized)
    {
        return GPIO_RES_OK;
    }

    if (!check_pins())
    {
        return GPIO_RES_INVALID_PIN;
    }

    /* If peripheral domain is off, power it on */
    if (PRCMPowerDomainsAllOn(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON)
    {
        PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
        while (PRCMPowerDomainsAllOn(PRCM_DOMAIN_PERIPH)
               != PRCM_DOMAIN_POWER_ON)
        {
            /* Wait until peripheral domain is powered on */
        }
    }

    /* Enable the GPIO peripheral in run and sleep modes */
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();
    while (!PRCMLoadGet())
    {
        /* Wait until modified register values are propagated to HW */
    }

    /* Set all GPIOs to use default configuration */
    for (gpio_id_t id = 0; id < BOARD_GPIO_NUMBER; ++id)
    {
        gpio_pin_t pin;
        Gpio_getPin(id, NULL, &pin);
        IOCPinTypeGpioInput(pin);
        IOCIOInputSet(pin, IOC_INPUT_DISABLE);
    }

    lib_system->enableAppIrq(true,
                             AON_GPIO_EDGE_IRQn,
                             APP_LIB_SYSTEM_IRQ_PRIO_LO,
                             gpio_event_handler);

    lib_system->clearPendingFastAppIrq(AON_GPIO_EDGE_IRQn);

    m_initialized = true;
    return GPIO_RES_OK;
}


gpio_res_e Gpio_inputSetCfg(gpio_id_t id, const gpio_in_cfg_t * in_cfg)
{
    if (!m_initialized)
    {
        return GPIO_RES_UNINITIALIZED;
    }
    else if (in_cfg == NULL)
    {
        return GPIO_RES_INVALID_PARAM;
    }

    gpio_pin_t pin;
    gpio_res_e result = Gpio_getPin(id, NULL, &pin);
    if (result == GPIO_RES_OK)
    {
        /* Set pin to use standard input configuration */
        IOCPinTypeGpioInput(pin);
        switch (in_cfg->in_mode_cfg)
        {
            case GPIO_IN_DISABLED:
                IOCIOInputSet(pin, IOC_INPUT_DISABLE);
                break;

            case GPIO_IN_PULL_NONE:
                /* No pull is already selected in the standard input
                 * configuration */
                break;

            case GPIO_IN_PULL_DOWN:
                IOCIOPortPullSet(pin, IOC_IOPULL_DOWN);
                break;

            case GPIO_IN_PULL_UP:
                IOCIOPortPullSet(pin, IOC_IOPULL_UP);
                break;

            default:
                result = GPIO_RES_INVALID_PARAM;
                break;
        }

        if (in_cfg->event_cfg == GPIO_IN_EVENT_NONE)
        {
            /* The standard input configuration disables edge detection */
        }
        else if (IS_RISING_EDGE(in_cfg->event_cfg)
                 && IS_FALLING_EDGE(in_cfg->event_cfg))
        {
            IOCIOIntSet(pin, IOC_INT_ENABLE, IOC_BOTH_EDGES);
        }
        else if (IS_RISING_EDGE(in_cfg->event_cfg))
        {
            IOCIOIntSet(pin, IOC_INT_ENABLE, IOC_RISING_EDGE);
        }
        else if (IS_FALLING_EDGE(in_cfg->event_cfg))
        {
            IOCIOIntSet(pin, IOC_INT_ENABLE, IOC_FALLING_EDGE);
        }
        else
        {
            result = GPIO_RES_INVALID_PARAM;
        }

        if (result == GPIO_RES_OK)
        {
            m_id_to_event_cb_map[id] = in_cfg->event_cb;
        }
    }

    return result;
}


gpio_res_e Gpio_inputRead(gpio_id_t id, gpio_level_e * level)
{
    return gpio_pin_read(id, level, false);
}


gpio_res_e Gpio_outputSetCfg(gpio_id_t id, const gpio_out_cfg_t * out_cfg)
{
    if (!m_initialized)
    {
        return GPIO_RES_UNINITIALIZED;
    }
    else if (out_cfg == NULL)
    {
        return GPIO_RES_INVALID_PARAM;
    }

    gpio_pin_t pin;
    gpio_res_e result = Gpio_getPin(id, NULL, &pin);
    if (result == GPIO_RES_OK)
    {
        /* Set pin to use standard output configuration */
        IOCPinTypeGpioOutput(pin);
        switch (out_cfg->out_mode_cfg)
        {
            case GPIO_OUT_MODE_PUSH_PULL:
                break;

            case GPIO_OUT_MODE_OPEN_DRAIN_WITH_PULL_UP:
                /* The default configuration sets pull to no pull -> switch to
                 * pull-up */
                IOCIOPortPullSet(pin, IOC_IOPULL_UP);

                /* Intentional fall through */
            case GPIO_OUT_MODE_OPEN_DRAIN:
                /* In the default configuration IO mode is normal -> switch to
                 * open drain */
                IOCIOModeSet(pin, IOC_IOMODE_OPEN_DRAIN_NORMAL);
                break;

            default:
                result = GPIO_RES_INVALID_PARAM;
                break;
        }

        if (result == GPIO_RES_OK)
        {
            GPIO_writeDio(pin, (out_cfg->level_default == GPIO_LEVEL_HIGH));
        }
    }

    return result;
}


gpio_res_e Gpio_outputWrite(gpio_id_t id, gpio_level_e level)
{
    gpio_pin_t pin;
    gpio_res_e result = get_pin(id, &pin, true);
    if (result == GPIO_RES_OK)
    {
        GPIO_writeDio(pin, (level == GPIO_LEVEL_HIGH));
    }

    return result;
}


gpio_res_e Gpio_outputToggle(gpio_id_t id)
{
    gpio_pin_t pin;
    gpio_res_e result = get_pin(id, &pin, true);
    if (result == GPIO_RES_OK)
    {
        GPIO_toggleDio(pin);
    }

    return result;
}


gpio_res_e Gpio_outputRead(gpio_id_t id, gpio_level_e * level)
{
    return gpio_pin_read(id, level, true);
}


gpio_res_e Gpio_getPin(gpio_id_t id, gpio_port_t * port, gpio_pin_t * pin)
{
    gpio_res_e result;
    if ((id < BOARD_GPIO_NUMBER) && pin)
    {
        result = GPIO_RES_OK;
        if (port)
        {
            /* On CC13x4 devices all information is stored into pin -> port is
             * set to 0 and can be ignored */
            *port = 0;
        }

        *pin = m_id_to_pin_map[id];
    }
    else
    {
        result = GPIO_RES_INVALID_PARAM;
    }

    return result;
}


uint8_t Gpio_getNumber(void)
{
    return BOARD_GPIO_NUMBER;
}
