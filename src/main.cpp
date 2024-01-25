#include <iostream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <iomanip>
#include <bitset>
#include "utility.h"
#include "commands.h"

std::string to_hex(char *response, size_t size)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < size; i++)
        oss << std::setw(2) << static_cast<unsigned int>(response[i]) << "\n";
    return oss.str();
}

int main()
{
    if (hid_init() != 0) {
        std::cout<<"HIDAPI library cannot be initialized! Reason: ";
        std::cout<<UTIL::str(hid_error(NULL));
        return -1;
    }
    UTIL::detect_all_hid_linux_devices();
   // UTIL::detect_all_com_linux_devices(); https://scmax.ru/questions/16070737/
    hid_device* handle = hid_open_path("/dev/hidraw2");
    HID::save_to_internal_flash(handle);
    HID::restore_to_factory_settings(handle);
  /*  hid_device *handle2;
    handle2 = hid_open_path(response[2].path_);
    if (!handle2)
    {
        std::cout << "Error handle2!!!\t";
        std::cout << hid_error(handle2);
        hid_close(handle2);
        hid_exit();
        return 1;
    }

    std::string big_json = UTIL::send_command_for_json_response(handle2);
   // std::cout<<"\n"<<big_json;
    */
    hid_close(handle);
    hid_exit();
    return 0;
}