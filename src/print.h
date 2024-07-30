#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <boost/json.hpp>
#include <boost/system/error_code.hpp>
#include "CppConsoleTable.hpp"
#include "utility.h"
#include "converters.h"
#ifdef __WIN__
#include <windows.h>
#endif

using samilton::ConsoleTable;

namespace PRINT
{
    using namespace std::string_literals;

    ConsoleTable getTableInitialSetup();
    void print_all_hid_devices(const std::vector<UTIL::AVAILABLE_HID> &hids);
    void print_all_com_linux_devices(const std::vector<UTIL::AVAILABLE_COM> &coms);
    void print_all_json_files(std::vector<std::pair<std::string, std::string>> &json_list);
    void print_all_firmware_files(std::vector<std::pair<std::string, int>> &firmware_list);
    void print_software_version(const std::vector<UTIL::AVAILABLE_HID> &hids);
    void download_attention();
    //std::string ChooseScannerToProceed();
    int ChooseToProceed(size_t amount);
}