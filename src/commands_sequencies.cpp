#pragma once
#include "commands_sequencies.h"

std::pair<uint8_t, uint8_t> SEQ::crc_16(uint8_t *data, uint16_t len)
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
    return std::make_pair(crc16 >> 8, crc16 & 0x00FF);
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
    std::make_pair(c[10], c[11]) = crc_16(&c[5], 5);
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
    std::make_pair(c[10], c[11]) = crc_16(&c[5], 5);
}