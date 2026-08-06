/* Copyright 2022 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>
#include <stdarg.h>
#include "api.h"
#include "usart.h"
#include "wireshark.h"


static const uint8_t SLIP_END=0xC0;

/**
 * \brief send a buffer with slip escaping
 * \note no check on return code as it is assumed that system
 *       can write the messages fast enough
 */
static void slip_send_buffer(const uint8_t * buf, size_t len)
{
    const uint8_t SLIP_ESC=0xDB;
    const uint8_t SLIP_ESC_END=0xDC;
    const uint8_t SLIP_ESC_ESC=0xDD;

    while (len--) {
        uint8_t ch = *buf;
        switch(ch)
        {
            case SLIP_ESC:
                Usart_sendBuffer(&SLIP_ESC, 1);
                Usart_sendBuffer(&SLIP_ESC_ESC, 1);
                break;
            case SLIP_END:
                Usart_sendBuffer(&SLIP_ESC, 1);
                Usart_sendBuffer(&SLIP_ESC_END, 1);
                break;
            default:
                Usart_sendBuffer(buf, 1);
                break;
        }
        buf++;
    }
}

void Wireshark_print(
        uint32_t src,
        uint32_t dest,
        uint8_t qos,
        uint8_t src_ep,
        uint8_t dst_ep,
        int8_t rssi,
        uint32_t delay,
        const app_lib_data_fragment_t * fragment_info,
        const uint8_t * data,
        size_t len)
{
    uint8_t has_fragment = (fragment_info != NULL) ? 1 : 0;
    uint16_t packet_id = has_fragment ? fragment_info->packet_id : 0;
    uint16_t fragment_offset = has_fragment ? fragment_info->fragment_offset : 0;
    uint8_t last_fragment = has_fragment ? (uint8_t) fragment_info->last_fragment : 0;

    /* Print new frame char */
    Usart_sendBuffer(&SLIP_END, 1);

    /* Print meta data */
    slip_send_buffer((uint8_t *) &src, 4);
    slip_send_buffer((uint8_t *) &dest, 4);
    slip_send_buffer((uint8_t *) &qos, 1);
    slip_send_buffer((uint8_t *) &src_ep, 1);
    slip_send_buffer((uint8_t *) &dst_ep, 1);
    slip_send_buffer((uint8_t *) &rssi, 1);
    slip_send_buffer((uint8_t *) &delay, 4);
    slip_send_buffer(&has_fragment, 1);
    slip_send_buffer((uint8_t *) &packet_id, 2);
    slip_send_buffer((uint8_t *) &fragment_offset, 2);
    slip_send_buffer(&last_fragment, 1);

    slip_send_buffer(data, len);

    /* Print end frame char */
    Usart_sendBuffer(&SLIP_END, 1);
}

void Wireshark_init(void)
{
    Usart_init(WIRESHARK_UART_BAUDRATE, UART_FLOW_CONTROL_NONE);
    Usart_setEnabled(true);
}
