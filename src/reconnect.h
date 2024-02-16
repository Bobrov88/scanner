#pragma once
#include "hidapi.h"
#include <algorithm>
#include "utility.h"
#include "converters.h"

namespace RECONNECT {
    hid_device* hid_reconnect(const std::string& serial_number);
    std::string com_reconnect(const std::string &serial);
}