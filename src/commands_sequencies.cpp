#include "commands_sequencies.h"

uint16_t SEQ::crc_16(uint8_t *data, uint16_t len)
{
    uint16_t crc16 = 0x0000;
    while (len--)
    {
        for (uint8_t i = 0x80; i != 0; i >>= 1)
        {
            if ((crc16 & 0x8000) != 0)
            {
                crc16 = crc16 << 1;
                crc16 = crc16 ^ 0x1021;
            }
            else
            {
                crc16 = crc16 << 1;
            }
            if ((*data & i) != 0)
            {
                crc16 = crc16 ^ 0x1021; // crc16 = crc16 ^ (0x10000 ^ 0x11021)
            }
        }
        data++;
    }
    return crc16;
}

void SEQ::save_to_internal_flash_command(uint8_t *c)
{
    c[0] = 0xfd;
    c[1] = 0x09;
    c[2] = 0xff;
    c[3] = 0x7e;
    c[4] = 0x00;
    c[5] = 0x09;
    c[6] = 0x01;
    c[7] = 0x00;
    c[8] = 0x00;
    c[9] = 0x00;
    uint16_t crc = crc_16(&c[5], 5);
    c[10] = crc >> 8;
    c[11] = crc & 0x00ff;
}

void SEQ::restore_to_factory_settings_command(uint8_t *c)
{
    c[0] = 0xfd;
    c[1] = 0x09;
    c[2] = 0xff;
    c[3] = 0x7e;
    c[4] = 0x00;
    c[5] = 0x09;
    c[6] = 0x01;
    c[7] = 0x00;
    c[8] = 0x00;
    c[9] = 0xFF;
    uint16_t crc = crc_16(&c[5], 5);
    c[10] = crc >> 8;
    c[11] = crc & 0x00ff;
}

void SEQ::testing_connect_for_erasing_duplicates_command(uint8_t *c)
{
    c[0] = 0xfd;
    c[1] = 0x09;
    c[2] = 0xff;
    c[3] = 0x7e;
    c[4] = 0x00;
    c[5] = 0x07;
    c[6] = 0x01;
    c[7] = 0x00;
    c[8] = 0xE1;
    c[9] = 0x01;
    uint16_t crc = crc_16(&c[5], 5);
    c[10] = crc >> 8;
    c[11] = crc & 0x00ff;
}

void SEQ::testing_com_connect_for_erasing_duplicates_command(uint8_t *c)
{
    c[0] = 0x7e;
    c[1] = 0x00;
    c[2] = 0x07;
    c[3] = 0x01;
    c[4] = 0x00;
    c[5] = 0xE1;
    c[6] = 0x01;
    uint16_t crc = crc_16(&c[2], 5);
    c[7] = crc >> 8;
    c[8] = crc & 0x00ff;
}

void SEQ::test_com_devices_is_scanner_command(uint8_t *c)
{
    c[0] = 0x7e;
    c[1] = 0x00;
    c[2] = 0x08;
    c[3] = 0x01;
    c[4] = 0x00;
    c[5] = 0x0D;
    c[6] = 0x84;
    uint16_t crc = crc_16(&c[2], 5);
    c[7] = crc >> 8;
    c[8] = crc & 0x00ff;
}

void SEQ::get_config_command(uint8_t *c, const uint8_t suffix)
{
    c[0] = 0xfd;
    c[1] = 0x0C;
    c[2] = 0xff;
    c[3] = 0x47;
    c[4] = 0x65;
    c[5] = 0x74;
    c[6] = 0x43;
    c[7] = 0x6F;
    c[8] = 0x6E;
    c[9] = 0x66;
    c[10] = 0x69;
    c[11] = 0x67;
    c[12] = 0x30;
    c[13] = 0x30 + suffix;
    c[14] = 0x2E;
}

void SEQ::get_config02_command(uint8_t *c)
{
    c[0] = 0x47;
    c[1] = 0x65;
    c[2] = 0x74;
    c[3] = 0x43;
    c[4] = 0x6F;
    c[5] = 0x6E;
    c[6] = 0x66;
    c[7] = 0x69;
    c[8] = 0x67;
    c[9] = 0x30;
    c[10] = 0x32;
    c[11] = 0x2E;
}

void SEQ::read_device_info_command(uint8_t *c)
{
    c[0] = 0xfd;
    c[1] = 0x0f;
    c[2] = 0xff;
    c[3] = 0x52;
    c[4] = 0x65;
    c[5] = 0x61;
    c[6] = 0x64;
    c[7] = 0x44;
    c[8] = 0x65;
    c[9] = 0x76;
    c[10] = 0x69;
    c[11] = 0x63;
    c[12] = 0x65;
    c[13] = 0x49;
    c[14] = 0x6E;
    c[15] = 0x66;
    c[16] = 0x6F;
    c[17] = 0x2E;
}

void SEQ::create_subcommand(const uint16_t flag, const std::vector<uint8_t> &bits, uint8_t *c)
{
    c[0] = 0xfd;
    c[1] = static_cast<uint8_t>(bits.size() + 8); // 0x7e 0x00 0x08 0xbits.size() 0xbyte of flag 0xbyte of flag
    c[2] = 0xff;
    c[3] = 0x7e;
    c[4] = 0x00;
    c[5] = 0x08;
    c[6] = (uint8_t)bits.size();
    c[7] = flag >> 8;
    c[8] = flag & 0x00ff;
    
    int i = 0;
    for (; i < bits.size(); ++i)
    {
        c[i + 9] = bits[i];
    }
    uint16_t crc = crc_16(&c[5], bits.size() + 4);

    c[9+i++] = crc >> 8;
    c[9+i] = crc & 0x00ff;
}