#include "reconnect.h"

#define ATTEMPT 10

hid_device *RECONNECT::hid_reconnect(const std::string &serial)
{
    auto hids = UTIL::get_available_hid_devices();
    auto found_device = std::find_if(hids.begin(), hids.end(), [&serial](const auto &hid)
                                  { return UTIL::str(hid.serial_number_) == serial; });
    int i = 0;
    hid_device *handle = NULL;
    while (i < ATTEMPT && handle == NULL)
    {
        handle = hid_open_path(found_device->path_);
        ++i;
    }
    return handle;
}