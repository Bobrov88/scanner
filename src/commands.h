#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "hidapi.h"
#include "handler.h"
#include "utility.h"

namespace HID
{
    int save_to_internal_flash(handler &device);
    int restore_to_factory_settings(handler &device);
    int restore_to_custom_settings(handler &device);
    bool testing_connect_for_erasing_duplicates(handler &device);
    bool testing_to_pass_HID_from_COM(const std::string& com, size_t size);
    bool save_settings_to_files(const std::vector<UTIL::AVAILABLE_HID>& hids);
}