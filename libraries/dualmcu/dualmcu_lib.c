#include "dualmcu_lib.h"
#include "waps.h"

dualmcu_lib_res_e Dualmcu_lib_init(uint32_t baudrate, bool flow_ctrl, Dualmcu_lib_prop_cb prop_cb)
{
    // Initialize the Dual-MCU API protocol
    if (Waps_init(baudrate, flow_ctrl, prop_cb))
    {
        return DUALMCU_LIB_RES_OK;
    }
    else
    {
        return DUALMCU_LIB_RES_INTERNAL_ERROR;
    }
}

dualmcu_lib_res_e Dualmcu_lib_send_proprietary_indication(uint8_t * buffer, size_t len)
{
    if (Waps_send_proprietary_indication(buffer, len))
    {
        return DUALMCU_LIB_RES_OK;
    }
    else
    {
        return DUALMCU_LIB_RES_INTERNAL_ERROR;
    }
}
