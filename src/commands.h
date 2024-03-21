#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "hidapi.h"
#include "handler.h"
#include "utility.h"
#include "phrases.h"
#include "logger.h"

namespace HID
{
    using namespace std::chrono_literals;

    int save_to_internal_flash(handler &device);
    int restore_to_factory_settings(handler &device);
    int restore_to_custom_settings(handler &device);
    bool testing_connect_for_erasing_duplicates(handler &device);
    bool testing_to_pass_HID_from_COM(const std::string &com);
    bool testing_to_pass_COM_from_HID(hid_device* handle);
}