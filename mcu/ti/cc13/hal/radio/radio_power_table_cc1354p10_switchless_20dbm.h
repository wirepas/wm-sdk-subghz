/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef RADIO_POWER_TABLE_CC1354P10_SL_20DBM_H_
#define RADIO_POWER_TABLE_CC1354P10_SL_20DBM_H_

#include "radio_power_table_cc1354p10_types.h"

/**
 * Power table for switchless CC1354P10 +20dBm.
 */
const app_lib_radio_cfg_power_t power_table_cc1354p10_switchless_20dBm =
{
    .rx_current  = 54,                   // 5.4 mA RX current
    .rx_gain_db  = 0,                    // 0 dB RX gain
    .power_count = 10,                   // 10 power levels
    .powers =
    {
        {                      0x0300DB, -16, 1,  51 }, // -15.7 dBm,  5.1 mA
        {                      0x0300ED, -12, 1,  58 }, // -12.1 dBm,  5.8 mA
        {                      0x0208E0,  -8, 1,  81 }, //  -8.1 dBm,  8.1 mA
        {                      0x02169C,  -4, 1,  69 }, //  -4.1 dBm,  6.9 mA
        {                      0x012493,   0, 1,  79 }, //   0.3 dBm,  7.9 mA
        {                      0x015CAC,   4, 1, 105 }, //   4.4 dBm, 10.5 mA
        {                      0x005452,   8, 1, 130 }, //   8.1 dBm, 13.0 mA
        { HIGH_POWER_PA_MASK | 0x00380D,  13, 1, 185 }, //  13.0 dBm, 18.5 mA
        { HIGH_POWER_PA_MASK | 0x048022,  16, 1, 397 }, //  16.1 dBm, 39.7 mA
        { HIGH_POWER_PA_MASK | 0x1B8ED2,  20, 1, 696 }, //  20.0 dBm, 69.6 mA
    },
};

#define RADIO_CUSTOM_PA_CONFIG

#if defined RADIO_CUSTOM_PA_CONFIG
// PA configuration for cc1354p10 with RF switch
const cc1354p10_pa_config_t pa_config_cc1354p10_switchless =
    {
        .version = 1,
        .switchless = true,
    };
#endif

#if defined(RADIO_CUSTOM_POWER_TABLE_H)
__STATIC_INLINE const app_lib_radio_cfg_power_t * get_custom_power_table(void)
{
    return &power_table_cc1354p10_switchless_20dBm;
}
#endif

#if defined RADIO_CUSTOM_PA_CONFIG
__STATIC_INLINE const cc1354p10_pa_config_t * get_custom_pa_config(void)
{
    return &pa_config_cc1354p10_switchless;
}
#endif

#endif  // RADIO_POWER_TABLE_CC1354P10_SL_20DBM_H_
