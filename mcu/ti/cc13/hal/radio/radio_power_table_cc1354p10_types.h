/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef RADIO_POWER_TABLE_CC1354P10_TYPES_H_
#define RADIO_POWER_TABLE_CC1354P10_TYPES_H_

/** \brief Supported TX output power levels */
enum radio_tx_power_e
{
    RADIO_TXPOWER_Neg20dBm,  //< -20 dBm
    RADIO_TXPOWER_Neg16dBm,  //< -16 dBm
    RADIO_TXPOWER_Neg12dBm,  //< -12 dBm
    RADIO_TXPOWER_Neg8dBm,   //<  -8 dBm
    RADIO_TXPOWER_Neg4dBm,   //<  -4 dBm
    RADIO_TXPOWER_0dBm,      //<   0 dBm
    RADIO_TXPOWER_4dBm,      //<   4 dBm
    RADIO_TXPOWER_8dBm,      //<   8 dBm
    RADIO_TXPOWER_12dBm,     //<  12 dBm
    RADIO_TXPOWER_16dBm,     //<  16 dBm
    RADIO_TXPOWER_20dBm,     //<  20 dBm
    RADIO_TXPOWER_COUNT      //< Number of TX power levels
};

#endif  // RADIO_POWER_TABLE_CC1354P10_TYPES_H_
