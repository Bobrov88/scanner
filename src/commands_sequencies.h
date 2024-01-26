#pragma once
#include <iostream>

namespace SEQ
{
    std::pair<uint8_t, uint8_t> crc_16(uint8_t *data, uint16_t len);
    void save_to_internal_flash_command(uint8_t *c);
    void restore_to_factory_settings_command(uint8_t *c);
}