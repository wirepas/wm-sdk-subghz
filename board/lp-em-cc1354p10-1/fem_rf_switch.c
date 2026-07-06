/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "mcu.h"
#include "api.h"
#include "board.h"
#include "radio_power_table_cc1354p10_types.h"
#include "fem_rf_switch.h"

#include DeviceFamily_constructPath_SDK(driverlib/ioc.h)
#include DeviceFamily_constructPath_SDK(driverlib/gpio.h)


/** The first power level where high-power PA is needed */
#define RF_SWITCH_HIGH_PA_MIN_LEVEL (RADIO_TXPOWER_16dBm)

/** Is high-power PA needed */
static bool m_high_power_pa;


/**
 * \brief   Callback for FEM control commands.
 * \param   femcmd
 *          FEM command from firmware
 */
static void rf_switch_cmd_cb(app_lib_radio_cfg_femcmd_e femcmd)
{
    switch (femcmd)
    {
        case APP_LIB_RADIO_CFG_FEM_PWR_OFF:
            // Intentional fall through

        case APP_LIB_RADIO_CFG_FEM_STANDBY:
            GPIO_clearDio(BOARD_RF_SWITCH_SUB_GHZ);
            GPIO_clearDio(BOARD_RF_SWITCH_20_DBM_PA);
            break;

        case APP_LIB_RADIO_CFG_FEM_RX_ON:
            GPIO_clearDio(BOARD_RF_SWITCH_20_DBM_PA);
            GPIO_setDio(BOARD_RF_SWITCH_SUB_GHZ);
            break;

        case APP_LIB_RADIO_CFG_FEM_TX_ON:
            if (m_high_power_pa)
            {
                GPIO_clearDio(BOARD_RF_SWITCH_SUB_GHZ);
                GPIO_setDio(BOARD_RF_SWITCH_20_DBM_PA);
            }
            else
            {
                GPIO_clearDio(BOARD_RF_SWITCH_20_DBM_PA);
                GPIO_setDio(BOARD_RF_SWITCH_SUB_GHZ);
            }

            break;

        default:
            // Do nothing
            break;
    }
}


/**
 * \brief   Callback for setting current radio power.
 * \param   power
 *          Power level that was set in firmware
 */
static void rf_switch_set_power_cb(uint8_t power)
{
    m_high_power_pa = (power >= RF_SWITCH_HIGH_PA_MIN_LEVEL);
}


void fem_rf_switch_init(void)
{
    IOCPinTypeGpioOutput(BOARD_RF_SWITCH_2_4_GHZ);
    IOCIODrvStrengthSet(BOARD_RF_SWITCH_2_4_GHZ,
                        IOC_CURRENT_8MA,
                        IOC_STRENGTH_AUTO);

    GPIO_clearDio(BOARD_RF_SWITCH_2_4_GHZ);

    IOCPinTypeGpioOutput(BOARD_RF_SWITCH_SUB_GHZ);
    IOCIODrvStrengthSet(BOARD_RF_SWITCH_SUB_GHZ,
                        IOC_CURRENT_8MA,
                        IOC_STRENGTH_AUTO);

    GPIO_clearDio(BOARD_RF_SWITCH_SUB_GHZ);

    IOCPinTypeGpioOutput(BOARD_RF_SWITCH_20_DBM_PA);
    IOCIODrvStrengthSet(BOARD_RF_SWITCH_20_DBM_PA,
                        IOC_CURRENT_8MA,
                        IOC_STRENGTH_AUTO);

    GPIO_clearDio(BOARD_RF_SWITCH_20_DBM_PA);

    app_lib_radio_cfg_fem_t fem_cfg =
    {
        .setPower = rf_switch_set_power_cb,
        .femCmd = rf_switch_cmd_cb,
        .femTimings =
        {
            .delay_values_set = false,
            .pd_to_sby = 0,
            .sby_to_tx = 0,
            .sby_to_rx = 0,
        },
    };

    app_res_e status = lib_radio_cfg->femSetup(&fem_cfg);
    if (status != APP_RES_OK)
    {
        while (true)
        {
        }
    }
}
