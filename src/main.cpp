#include <iostream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <iomanip>
#include <bitset>
#include "utility.h"

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
    std::vector<UTIL::AVAILABLE_HID> response = UTIL::get_available_hid_devices();
    for (const auto &hid : response)
    {
        std::cout << std::hex << hid.vid_ << " " << std::hex << hid.pid_ << " " << hid.path_ << "\n";
    }
    std::cout << std::dec;

    hid_init();

    hid_device *handle2;
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
    std::cout<<"\n"<<big_json;
    
    hid_close(handle2);
    hid_exit();
    return 0;
}