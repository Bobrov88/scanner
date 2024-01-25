#include "hidapi.h"
#include "utility.h"

namespace HID
{
    int save_to_internal_flash(hid_device *handle);
    int restore_to_factory_settings(hid_device *handle);
}