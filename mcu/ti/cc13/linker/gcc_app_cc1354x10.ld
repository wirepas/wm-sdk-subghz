/* Copyright 2024 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/* Linker script for Wirepas Mesh application on CC1354P10
 *
 * This file is for the 1024 kB Flash, 256 kB RAM variant
 *
 * Flash memory areas:
 *  - Application executable code (FLASH)
 *      - By default, 256 kB of Flash is allocated for the application
 *      - Remaining space (~496 kB) is available for scratchpad (OTAP)
 *      - Increasing FLASH LENGTH reduces scratchpad space
 *      - See OTAP documentation for details
 *
 * RAM memory areas:
 *  - RAM for application static variables (RAM)
 *      - By default, 128 kB of RAM is allocated
 *      - Can be adjusted between 1 kB and 208 kB
 *      - Example for 104 kB:
 *        RAM (rwx) : ORIGIN = 0x20000000 + 256K - 104K, LENGTH = 104K
 *      - Reducing app RAM gives more heap to the mesh stack
 *      - Note: No 8-byte reservation at RAM end
 */

MEMORY
{
  /* Application starts after bootloader (32K) and stack firmware (240K) */
  FLASH (rx)      : ORIGIN = 0x00044000, LENGTH = 256K

  /* Application RAM at end of 256K RAM space */
  RAM (rwx)       : ORIGIN = 0x20000000 + 256K - 128K, LENGTH = 128K

  INVALID (rwx)   : ORIGIN = 0xFFFFFFFF, LENGTH = 0     /* Sanity check */
}

/* Sanity check in case RAM reservation is modified */
ASSERT(ORIGIN(RAM) >= (0x2000C000), "RAM origin is too low")
ASSERT(LENGTH(RAM) <= 208K, "RAM area is too big")
ASSERT((ORIGIN(RAM) + LENGTH(RAM)) == (0x20000000 + 256K), "RAM end is wrong")

INCLUDE gcc_app_common.ld
