#pragma once
#include "hidapi.h"
#include <algorithm>
#include "utility.h"

namespace RECONNECT {
    hid_device* hid_reconnect(const std::string& serial);
}