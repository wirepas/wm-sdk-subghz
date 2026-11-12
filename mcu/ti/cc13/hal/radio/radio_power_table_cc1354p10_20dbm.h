/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef RADIO_POWER_TABLE_CC1354P10_20DBM_H_
#define RADIO_POWER_TABLE_CC1354P10_20DBM_H_

#include "radio_power_table_cc1354p10_types.h"

/**
 * Power table for CC1354P10 +20dBm.
 */
const app_lib_radio_cfg_power_t power_table_cc1354p10_20dBm =
{
    .rx_current  = 54,                   // 5.4 mA RX current
    .rx_gain_db  = 0,                    // 0 dB RX gain
    .power_count = 11,                   // 11 power levels
    .powers =
    {
        {                      0x0300D5, -20, 1,  54 }, // -19.90 dBm,  5.4 mA
        {                      0x0208CF, -16, 1,  51 }, // -16.20 dBm,  5.1 mA
        {                      0x020ED9, -12, 1,  58 }, // -12.10 dBm,  5.8 mA
        {                      0x030EB8,  -8, 1,  81 }, //  -8.10 dBm,  8.1 mA
        {                      0x012ADD,  -4, 1,  69 }, //  -4.10 dBm,  6.9 mA
        {                      0x013297,   0, 1,  79 }, //   0.10 dBm,  7.9 mA
        {                      0x0166B1,   4, 1, 105 }, //   3.90 dBm, 10.5 mA
        {                      0x017065,   8, 1, 130 }, //   8.10 dBm, 13.0 mA
        {                      0x00BE33,  12, 1, 185 }, //  12.20 dBm, 18.5 mA
        { HIGH_POWER_PA_MASK | 0x005618,  16, 1, 397 }, //  15.30 dBm, 39.7 mA
        { HIGH_POWER_PA_MASK | 0x1B8ED2,  20, 1, 696 }, //  19.60 dBm, 69.6 mA
    },
};

#define RADIO_CUSTOM_PA_CONFIG

#if defined RADIO_CUSTOM_PA_CONFIG
// PA configuration for cc1354p10 with RF switch
const cc1354p10_pa_config_t pa_config_cc1354p10_with_switch =
    {
        .version = 1,
        .switchless = false,
    };
#endif

#if defined(RADIO_CUSTOM_POWER_TABLE_H)
__STATIC_INLINE const app_lib_radio_cfg_power_t * get_custom_power_table(void)
{
    return &power_table_cc1354p10_20dBm;
}
#endif

#if defined RADIO_CUSTOM_PA_CONFIG
__STATIC_INLINE const cc1354p10_pa_config_t * get_custom_pa_config(void)
{
    return &pa_config_cc1354p10_with_switch;
}
#endif

#endif  // RADIO_POWER_TABLE_CC1354P10_20DBM_H_
