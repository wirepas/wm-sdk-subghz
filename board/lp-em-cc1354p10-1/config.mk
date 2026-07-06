# Mcu of the board
MCU_FAMILY=ti
MCU=cc13
MCU_SUB=54
MCU_MEM_VAR=x10

# Which radio to use
radio=cc1354p10
# Radio Configuration india865
radio_config=india865nw
mac_profile=subg

# Hardware capabilities of the board
## Is 32kHz crystal mounted on the board.
board_hw_crystal_32k=no
## Is DCDC used on this board.
board_hw_dcdc=no

## Delta value for HFXO internal capacitors.
## This is a signed 8-bit offset that is added to the default value in FCFG.
## If missing or set to 127 the delta value will be not be set in firmware.
##
## More information about crystal tuning can be found from TI documentation:
## https://www.ti.com/lit/pdf/swra640
board_hw_hfxo_cap_arr_delta=127

# Custom power table
# if set, the custom power table will be set during application startup,
# and the stack will use that instead of the default one.
# For convenience, +12dBm and +20dBm power tables are provided as examples.
# - mcu/ti/cc13/hal/radio/radio_power_table_cc1354p10_12dbm.h
# - mcu/ti/cc13/hal/radio/radio_power_table_cc1354p10_20dbm.h
# If predefined custom power table shall be used,
# provide radio_power_table=12/20 as build parameter.
ifneq ("$(radio_power_table)", "")
    RADIO_CUSTOM_POWER_TABLE=mcu/ti/cc13/hal/radio/radio_power_table_cc1354p10_$(radio_power_table)dbm.h
endif

