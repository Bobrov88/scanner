#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <regex>
#include <hidapi.h>
#include "CppConsoleTable.hpp"
#include "utility.h"

namespace MENU {
    void PrintStartMenu();
    std::vector<UTIL::AVAILABLE_HID> PrintAvailableUSB();
    void SaveSettings();
}