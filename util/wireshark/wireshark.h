/* Copyright 2022 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file wireshark.h
 *
 * Helper file to print wireshark frame to be displayed in Wireshark UI
 */
#ifndef WIRESHARK_H
#define WIRESHARK_H

#include <stdint.h>
#include <stdarg.h>
#include "api.h"

#ifndef WIRESHARK_UART_BAUDRATE
#define WIRESHARK_UART_BAUDRATE 115200
#endif

/** Direction / type of a captured frame. Extensible (e.g. network beacons). */
typedef enum
{
    WIRESHARK_TYPE_DOWNLINK = 0,   /* sent to the mesh */
    WIRESHARK_TYPE_UPLINK   = 1,   /* received from the mesh */
    /* WIRESHARK_TYPE_BEACON = 2,     future: network beacons */
} wireshark_frame_type_e;

void Wireshark_print(
        uint8_t type,
        uint32_t src,
        uint32_t dest,
        uint8_t qos,
        uint8_t src_ep,
        uint8_t dst_ep,
        int8_t rssi,
        uint32_t delay,
        uint8_t hops,       /* hop count (from reception); 0 on transmission */
        uint8_t hop_limit,  /* hop limit (from transmission); 0 on reception */
        const app_lib_data_fragment_t * fragment_info,
        const uint8_t * data,
        size_t len);

void Wireshark_init(void);

#endif // WIRESHARK_H
