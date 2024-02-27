#include "reconnect.h"

#define ATTEMPT 10 // define as a system var

hid_device *RECONNECT::hid_reconnect(const std::string &serial_number)
{
    std::this_thread::sleep_for(100ms);
    auto hids = UTIL::get_available_hid_devices();
    auto found_device = std::find_if(hids.begin(), hids.end(), [&serial_number](const auto &hid)
                                     { return CONVERT::str(hid.serial_number_) == serial_number; });
    int i = 0;
    hid_device *handle = NULL;
    while (i < ATTEMPT && handle == NULL)
    {
        handle = hid_open_path(found_device->path_);
        ++i;
    }
    return handle;
}

std::string RECONNECT::com_reconnect(const std::string &serial)
{
    auto coms = UTIL::get_available_linux_com_ports();
    auto found_device = std::find_if(coms.begin(), coms.end(), [&serial](const auto &com)
                                     { return strcmp(com.serial_number_.data(), serial.data()) == 0; });
    if (found_device != coms.end())
    {   std::cout<<"\n Found new port: "<<found_device->port_;
        return found_device->port_;
    }
    return "";
}