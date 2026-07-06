/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "mcu.h"
#include "board.h"
#include "fem_rf_switch.h"


void Board_custom_init(void)
{
    fem_rf_switch_init();
}
