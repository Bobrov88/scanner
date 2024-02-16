#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <regex>
#include <hidapi.h>
#include "CppConsoleTable.hpp"
#include "utility.h"
#include "print.h"

namespace MENU {
    void PrintStartMenu();
    void PrintAttentionComToHID();
    std::string ChooseScannerToProceed();
    void PrintAvailableDevices();
    void SaveSettings();
    void WriteFromJson();
}