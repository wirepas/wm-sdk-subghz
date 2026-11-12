/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef RADIO_POWER_TABLE_CC1354P10_TYPES_H_
#define RADIO_POWER_TABLE_CC1354P10_TYPES_H_

#define HIGH_POWER_PA_MASK   0x80000000
#define POWER_RAW_VALUE_MASK 0x003FFFFF

typedef struct
{
    /** Version of cc1354p10_pa_config_t. */
    uint8_t version;
    /** Is the board swichless (true), or does it have RF switch (false). */
    bool    switchless;
} cc1354p10_pa_config_t;

#endif  // RADIO_POWER_TABLE_CC1354P10_TYPES_H_
