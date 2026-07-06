/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "power.h"
#include "mcu.h"
#include "board.h"

#include DeviceFamily_constructPath_SDK(driverlib/pwr_ctrl.h)


void Power_enableDCDC()
{
#if BOARD_HW_DCDC
#error "DCDC is not supported on this device"
#else
    /* DCDC is disabled -> use global LDO */
    PowerCtrlSourceSet(PWRCTRL_PWRSRC_GLDO);
#endif  // BOARD_HW_DCDC
}
