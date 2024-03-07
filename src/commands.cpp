#include "commands.h"

int HID::save_to_internal_flash(handler &device)
{
    uint8_t c1[64] = {0};
    SEQ::save_to_internal_flash_command(c1);
    logger("Saving to internal flash", device.serial_number);
    if (-1 == UTIL::HID_WRITE(device, c1, 64))
    {
        logger(CONVERT::str(hid_error(device.ptr)), device.serial_number);
        return -1;
    };
    logger("Success");

    uint8_t c2[64] = {0};
    SEQ::save_as_custom_flash_command(c2);
    logger("Saving as custom settings", device.serial_number);
    if (-1 == UTIL::HID_WRITE(device, c2, 64))
    {
        logger(CONVERT::str(hid_error(device.ptr)), device.serial_number);
        return -1;
    }
    logger("Success");
    return 0;
}

int HID::restore_to_custom_settings(handler &device)
{
    uint8_t c[64] = {0};
    SEQ::restore_to_custom_settings(c);
    if (-1 == UTIL::HID_WRITE(device, c, 64))
    {
        return -1;
    };
    return 0;
}

int HID::restore_to_factory_settings(handler &device)
{
    uint8_t c[64] = {0};
    SEQ::restore_to_factory_settings_command(c);

    if (-1 == UTIL::HID_WRITE(device, c, 64))
    {
        logger(CONVERT::str(hid_error(device.ptr)), device.serial_number);
        return -1;
    };
    return 0;

    // if (write_result == -1)
    // {
    //     std::cout << "Save to internal flash failed. Reason: ";
    //     std::cout << CONVERT::str(hid_error(device.ptr));
    //     return -1;
    // }
    // if (write_result < 64)
    // {
    //     std::cout << "The device received fewer bytes than it sent.";
    //     std::cout << CONVERT::str(hid_error(device.ptr));
    //     return 1;
    // }

    // uint8_t r[64] = {0};
    // int read_result = hid_read_timeout(device.ptr, r, 64, 100);
    // if (read_result == -1)
    // {
    //     std::cout << "Reading result of saving to internal flash failed. Reason: ";
    //     std::cout << CONVERT::str(hid_error(device.ptr));
    //     return -1;
    // }
    // if (read_result == 0)
    // {
    //     std::cout << "No available packets to read. Reason: ";
    //     std::cout << CONVERT::str(hid_error(device.ptr));
    //     return 0;
    // }
    // if (read_result < 64)
    // {
    //     std::cout << "Response is not complete. Reason: ";
    //     std::cout << CONVERT::str(hid_error(device.ptr));
    //     return 2;
    // }
    // // todo
    // // add check_crc
    // if (r[5] == 0x02 &&
    //     r[6] == 0x00 &&
    //     r[7] == 0x00 &&
    //     r[8] == 0x01 &&
    //     r[9] == 0x00 &&
    //     r[10] == 0x33 &&
    //     r[11] == 0x31)
    // {
    //     std::cout << "Save to internal flash: Success";
    //     return 0;
    // }
    // else
    // {
    //     std::cout << "Unknown error";
    //     return 3;
    // }
}

bool HID::testing_connect_for_erasing_duplicates(handler &device)
{
    uint8_t c[64] = {0};
    SEQ::testing_connect_for_erasing_duplicates_command(c);
    if (-1 == UTIL::HID_WRITE(device, c, 64))
    {
        return false;
    };
    return true;
}

void HID::testing_to_pass_HID_from_COM(const std::string &com)
{
    logger("testing_to_pass_HID_from_COM", com);
    uint8_t c[9] = {0};
    SEQ::test_com_devices_is_scanner_command(c);
    boost::asio::io_service io;
    boost::asio::serial_port s_port(io, com);
    s_port.set_option(boost::asio::serial_port::baud_rate(9600));
    boost::asio::write(s_port, boost::asio::buffer(c, 9));
    s_port.close();
    // add error ec
}