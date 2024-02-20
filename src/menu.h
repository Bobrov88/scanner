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

namespace MENU
{
    void PrintStartMenu();
    void PrintAttentionComToHID();
    void PrintAvailableDevices();
    std::string ChooseScannerToProceed();
    void SaveSettings();
    void WriteFromJson();
    int OfferToSaveAs();
    void RestoreFactorySettings();
    void RestoreCustomSettings();
}