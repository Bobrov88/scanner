#pragma once
#include "hidapi.h"

struct handler {
    hid_device *ptr;
    std::string path;
    std::string serial_number;
};