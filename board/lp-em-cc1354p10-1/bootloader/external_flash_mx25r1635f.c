/* Copyright 2026 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include "external_flash_cfi.h"

static const struct ext_flash_id_t m_flash_id = { .flash_id     = MX25R1635F,
                                                  .man_id       = 0xC2,
                                                  .mem_type     = 0x28,
                                                  .mem_capacity = 0x15 };


static const flash_info_t m_flash_info = {
    .flash_size = 1024UL * 1024UL * 2UL,  // 16 M-bit flash
    .write_page_size = 256,
    .erase_sector_size = 32UL * 1024UL,   // 32 kB
    .write_alignment = 1,
    .byte_write_time = 100,               // Typical 32, Max 100
    .page_write_time = 4000,              // Typical 850, Max 4000
    .sector_erase_time = 1500000,         // Typical 240 000, Max 1 500 000.
    .byte_write_call_time = 50,           // isBusy time (3b) + write_enable (7b) + 4+1 bytes transfered on SPI bus + code exec time.
    .page_write_call_time = 610,          // isBusy time (3b) + write_enable (7b) + 4+256 bytes transfered on SPI bus + code exec time.
    .sector_erase_call_time = 50,         // isBusy time (3b) + write_enable (7b) + 4 bytes transfered on SPI bus + code exec time.
    .is_busy_call_time = 12               // 3 bytes transfered on SPI bus (6.45 us measured from CS down to CS up)
};


const struct ext_flash_id_t * ext_flash_get_id(void)
{
    return &m_flash_id;
}


const flash_info_t * ext_flash_get_info(void)
{
    return &m_flash_info;
}
