#pragma once
#include "hidapi.h"
#include <algorithm>
#include "utility.h"
#include "converters.h"
#include <thread>
#include <chrono>

namespace RECONNECT {
    using namespace std::chrono_literals;

    hid_device* hid_reconnect(const std::string& serial_number);
    std::string com_reconnect(const std::string &serial);
}