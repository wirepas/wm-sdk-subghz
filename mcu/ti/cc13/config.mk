# This mcu has a bootloader (enough memory)
HAS_BOOTLOADER=yes

# This MCU is supported starting from bootloader version 14
MIN_BOOTLOADER_VERSION := 14

# MCU is ARM Cortex-M33
CM33 := yes
ARCH = armv8-m.main
CFLAGS += -DARM_MATH_ARMV8MML
CFLAGS += -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mcmse

ifeq ($(MCU_SUB), 54)
    # Hardware magic used for this architecture
    HW_MAGIC=14
    HW_VARIANT_ID=30

    CFLAGS += -DCC13X4_PLATFORM -DDeviceFamily_CC13X4

    # Sanity checks for board configuration
    ifneq ($(board_hw_crystal_32k),no)
        $(error "32 kHz crystal not supported on cc1354 board")
    endif

    ifneq ($(board_hw_dcdc),no)
        $(error "DCDC not supported on cc1354 board")
    endif
else
    $(error "Invalid MCU_SUB for cc13! Only '54' is supported.")
endif

# Base address of non-volatile memory (flash)
FLASH_BASE_ADDR=0x00000000

# Different bootloader sizes available: size=mem_variant_byte,...
BOOTLOADER_SIZES="32k=0x01"
