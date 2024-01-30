#include "commands.h"

int HID::save_to_internal_flash(hid_device *handle)
{
    uint8_t c[64] = {0};
    // SEQ::save_to_internal_flash_command(c);
    //  TODO several trues to do
  //  check_operation_result(FUNC::SAVE_TO_INTERNAL_FLASH, hid_write(handle, c, 64), handle);
    uint8_t r[64] = {0};
    // todo call check result write cause error
 //   check_operation_result(FUNC::READ_AS_RESPONSE, hid_read_timeout(handle, r, 64, 100), handle, r);
    return 0;
}

int HID::restore_to_factory_settings(hid_device *handle)
{
    uint8_t c[64] = {0};
    SEQ::restore_to_factory_settings_command(c);

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

bool HID::testing_connect_for_erasing_duplicates(hid_device *handle)
{
    uint8_t c[30] = {0};
    SEQ::testing_connect_for_erasing_duplicates_command(c);
    hid_write(handle, c, 30);
    uint8_t r[64] = {0};
    hid_read_timeout(handle, r, 64, 100);
    if (r[5] == 0x02 &&
        r[6] == 0x00 &&
        r[7] == 0x00 &&
        r[8] == 0x01)
    {
        return true;
    }
    return false;
}