/* Copyright 2024 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/* Linker script for Wirepas Mesh bootloader on CC1354P10
 *
 * This file is for the 1024 kB Flash, 256 kB RAM variant
 *
 * Flash is split into:
 *  - Bootloader executable code (BOOTLOADER)
 *  - Bootloader config: memory areas and encryption keys (BLCONFIG)
 *  - Customer Configuration Area (CCFG) for device configuration
 *
 * RAM is split into:
 *  - RAM for bootloader variables, to support runtime services (BL_STATIC)
 *  - Rest of the RAM the bootloader can use for variables during booting (RAM)
 *
 * GPRAM region (8 kB) can be used if cache is disabled in CCFG. Curently used for Cpu cache.
 * (CCFG:SIZE_AND_DIS_FLAGS.DIS_GPRAM = 0)
 */

MEMORY
{
    /* Wirepas-specific memory regions */
    BOOTLOADER (rx)   : ORIGIN = 0x00000000,                        LENGTH = __bl_length__ - 1K
    BLCONFIG (rx)     : ORIGIN = 0x00000000 + __bl_length__ - 1K,   LENGTH = 1K
    BL_STATIC (rwx)   : ORIGIN = 0x20000000,                        LENGTH = 1K
    RAM (rwx)         : ORIGIN = 0x20000000 + 1K,                   LENGTH = 255K

    /* TI-specific memory regions */
    GPRAM (rwx)       : ORIGIN = 0x11000000,                        LENGTH = 8K
    FLASH_CCFG (rx)   : ORIGIN = 0x50000000,                        LENGTH = 2K

    INVALID (rwx)     : ORIGIN = 0xFFFFFFFF,                        LENGTH = 0
}

SECTIONS
{
    .ccfg :
    {
        KEEP(*(.ccfg))
    } > FLASH_CCFG
}

INCLUDE gcc_bl_common.ld
