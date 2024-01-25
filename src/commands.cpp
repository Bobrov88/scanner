#include "commands.h"

int HID::save_to_internal_flash(hid_device *handle)
{
    uint8_t c[64] = {0};
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
    uint16_t crc = UTIL::crc_16(&c[5], 5);
    c[10] = crc >> 8;
    c[11] = crc & 0x00FF;

    int write_result = hid_write(handle, c, 64);

    if (write_result == -1)
    {
        std::cout << "Save to internal flash failed. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return -1;
    }
    if (write_result < 64)
    {
        std::cout << "The device received fewer bytes than it sent.";
        std::cout << UTIL::str(hid_error(handle));
        return 1;
    }

    uint8_t r[64] = {0};
    int read_result = hid_read_timeout(handle, r, 64, 100);
    if (read_result == -1)
    {
        std::cout << "Reading result of saving to internal flash failed. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return -1;
    }
    if (read_result == 0)
    {
        std::cout << "No available packets to read. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return 1;
    }
    if (read_result < 64)
    {
        std::cout << "Response is not complete. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return 2;
    }
    // todo
    // add check_crc
    if (r[5] == 0x02 &&
        r[6] == 0x00 &&
        r[7] == 0x00 &&
        r[8] == 0x01 &&
        r[9] == 0x00 &&
        r[10] == 0x33 &&
        r[11] == 0x31)
    {
        std::cout << "Save to internal flash: Success";
        return 0;
    }
    else
    {
        std::cout << "Unknown error";
        return 3;
    }
}

int HID::restore_to_factory_settings(hid_device *handle)
{
    uint8_t c[64] = {0};
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
    uint16_t crc = UTIL::crc_16(&c[5], 5);
    c[10] = crc >> 8;
    c[11] = crc & 0x00FF;

    int write_result = hid_write(handle, c, 64);

    if (write_result == -1)
    {
        std::cout << "Save to internal flash failed. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return -1;
    }
    if (write_result < 64)
    {
        std::cout << "The device received fewer bytes than it sent.";
        std::cout << UTIL::str(hid_error(handle));
        return 1;
    }

    uint8_t r[64] = {0};
    int read_result = hid_read_timeout(handle, r, 64, 100);
    if (read_result == -1)
    {
        std::cout << "Reading result of saving to internal flash failed. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return -1;
    }
    if (read_result == 0)
    {
        std::cout << "No available packets to read. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return 1;
    }
    if (read_result < 64)
    {
        std::cout << "Response is not complete. Reason: ";
        std::cout << UTIL::str(hid_error(handle));
        return 2;
    }
    // todo
    // add check_crc
    if (r[5] == 0x02 &&
        r[6] == 0x00 &&
        r[7] == 0x00 &&
        r[8] == 0x01 &&
        r[9] == 0x00 &&
        r[10] == 0x33 &&
        r[11] == 0x31)
    {
        std::cout << "Save to internal flash: Success";
        return 0;
    }
    else
    {
        std::cout << "Unknown error";
        return 3;
    }
}