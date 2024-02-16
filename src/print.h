#pragma once
#include <string>
#include <boost/json.hpp>
#include <boost/system/error_code.hpp>
#include "CppConsoleTable.hpp"
#include "utility.h"

using samilton::ConsoleTable;

namespace PRINT
{
    ConsoleTable getTableInitialSetup();
    void print_all_hid_linux_devices(const std::vector<UTIL::AVAILABLE_HID> &hids);
    void print_all_com_linux_devices(const std::vector<UTIL::AVAILABLE_COM> &coms);
    void print_all_json_files(std::vector<std::pair<std::string, std::string>> &json_list);
}