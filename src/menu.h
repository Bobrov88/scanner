#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <regex>
#include <hidapi.h>
#include <future>
#include "CppConsoleTable.hpp"
#include "fwdlinterface.h"
#include <thread>
#include "utility.h"
#include "print.h"
#include "commands.h"
#include "reconnect.h"
#ifdef __WIN__
#include "windows.h"
#include "SetupAPI.h"
#endif

typedef int (*pSendData)(char *psendbuf, int sendlen);
typedef int (*pReceiveData)(char *precbuf, int reclen);

using namespace std::chrono_literals;
namespace MENU
{
    void PrintStartMenu();
    void PrintAttentionComToHID();
    void PrintAvailableDevices();
    void SaveSettings();
    void WriteFromJson();
    void RestoreFactorySettings();
    void RestoreCustomSettings();
    void DownloadFirmware();
    // void ReadUsbIdentifiers(std::string dev_path);
}