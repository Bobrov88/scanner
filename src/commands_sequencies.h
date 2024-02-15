#pragma once
#include <iostream>
#include <iomanip>
#include <vector>

namespace SEQ
{
    uint16_t crc_16(uint8_t *data, uint16_t len);
    std::string to_hex(uint8_t *response, size_t size);
    std::string to_hex(const std::vector<uint8_t>& bytes);
    void save_to_internal_flash_command(uint8_t *c);
    void restore_to_factory_settings_command(uint8_t *c);
    void testing_connect_for_erasing_duplicates_command(uint8_t *c);
    void testing_com_connect_for_erasing_duplicates_command(uint8_t *c);
    void get_config_command(uint8_t *c, const uint8_t suffix);
    void test_com_devices_is_scanner_command(uint8_t *c);
    void get_config02_command(uint8_t *c);
    void create_subcommand(const uint16_t flag,const std::vector<uint8_t>& bits,uint8_t *c);
    void read_device_info_command(uint8_t *c);
}