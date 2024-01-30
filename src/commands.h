#pragma once
#include <iostream>
#include "hidapi.h"
#include "utility.h"
#include "commands_sequencies.h"

namespace HID
{
    int save_to_internal_flash(hid_device *handle);
    int restore_to_factory_settings(hid_device *handle);
    bool testing_connect_for_erasing_duplicates(hid_device *handle);
    
}