/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <stdint.h>

#include "mcu.h"

#include DeviceFamily_constructPath_SDK(inc/hw_memmap.h)
#include DeviceFamily_constructPath_SDK(inc/hw_types.h)
#include DeviceFamily_constructPath_SDK(inc/hw_fcfg1.h)


/**
 * \brief   Get an ID for the device that is unique within network.
 * \return  Locally unique ID
 */
static inline uint32_t getUniqueId()
{
    /* The last 32 bits of the 64-bit IEEE 802.15.4 MAC address.
     *
     * MAC address is in little-endian format in factory configuration so the
     * "first 32 bits" register needs to be used here. */
    return HWREG(FCFG1_BASE + FCFG1_O_MAC_15_4_0);
}

#endif  // UNIQUE_ID_H
