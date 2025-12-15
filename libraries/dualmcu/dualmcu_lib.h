/* Copyright 2021 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * @file dualmcu_lib.h
 *
 */

#ifndef _DUALMCU_LIB_H_
#define _DUALMCU_LIB_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * \brief   List of return code
 */
typedef enum
{
    /** Operation is successful */
    DUALMCU_LIB_RES_OK = 0,
    /** Something went wrong */
    DUALMCU_LIB_RES_INTERNAL_ERROR = 1,
} dualmcu_lib_res_e;

/**
 * \brief Callback used to receive proprietary message
 * \param buffer
 *        The buffer received
 * \param len
 *        The len of the buffer
 * \param response
 *        The response for the request
 * \param resp_max_len
 *        Max len of response buffer
 * \return Size of the response written
 */
typedef uint8_t (*Dualmcu_lib_prop_cb)(const uint8_t * const buffer, size_t len, uint8_t * resp, size_t resp_max_len);

/**
 * \brief   Initialize Dualmcu_lib
 * \param   baudrate
 *          Baudrate for the uart
 * \param   flow_ctrl
 *          Is hardware flow control enabled
 * \param   prop_cb
 *          Proprietary Callback. If not Null, this callback will
 *          be called when a cmd with proprietary id is received
 * \return  Return code of the operation
 */
dualmcu_lib_res_e Dualmcu_lib_init(uint32_t baudrate, bool flow_ctrl, Dualmcu_lib_prop_cb prop_cb);

/**
 * \brief Send a proprietary command over dualmcu protocol
 * \param buffer
 *        The buffer conataining the proprietary message
 * \param len
 *        The len of the buffer
 * \return Return code of the operation
 */
dualmcu_lib_res_e Dualmcu_lib_send_proprietary_indication(uint8_t * buffer, size_t len);


#endif //_DUALMCU_LIB_H_
