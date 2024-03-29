#pragma once
#include "hidapi.h"
#include <algorithm>
#include "utility.h"
#include "converters.h"

namespace RECONNECT {
    using namespace std::chrono_literals;

    hid_device* hid_reconnect(const std::string& serial_number);
}