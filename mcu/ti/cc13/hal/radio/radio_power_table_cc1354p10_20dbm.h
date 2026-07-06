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
        { RADIO_TXPOWER_Neg20dBm,  -20, 1,  54 }, // -19.90 dBm,  5.4 mA
        { RADIO_TXPOWER_Neg16dBm,  -16, 1,  51 }, // -16.20 dBm,  5.1 mA
        { RADIO_TXPOWER_Neg12dBm,  -12, 1,  58 }, // -12.10 dBm,  5.8 mA
        { RADIO_TXPOWER_Neg8dBm,    -8, 1,  81 }, //  -8.10 dBm,  8.1 mA
        { RADIO_TXPOWER_Neg4dBm,    -4, 1,  69 }, //  -4.10 dBm,  6.9 mA
        { RADIO_TXPOWER_0dBm,        0, 1,  79 }, //   0.10 dBm,  7.9 mA
        { RADIO_TXPOWER_4dBm,        4, 1, 105 }, //   3.90 dBm, 10.5 mA
        { RADIO_TXPOWER_8dBm,        8, 1, 130 }, //   8.10 dBm, 13.0 mA
        { RADIO_TXPOWER_12dBm,      12, 1, 185 }, //  12.20 dBm, 18.5 mA
        { RADIO_TXPOWER_16dBm,      16, 1, 397 }, //  15.30 dBm, 39.7 mA
        { RADIO_TXPOWER_20dBm,      20, 1, 696 }, //  19.60 dBm, 69.6 mA
    },
};


#if defined(RADIO_CUSTOM_POWER_TABLE_H)
__STATIC_INLINE const app_lib_radio_cfg_power_t * get_custom_power_table(void)
{
    return &power_table_cc1354p10_20dBm;
}
#endif

#endif  // RADIO_POWER_TABLE_CC1354P10_20DBM_H_
