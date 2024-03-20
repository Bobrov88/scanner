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