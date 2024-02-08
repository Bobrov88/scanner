#include <iostream>
#include "utility.h"
#include "commands.h"
#include "menu.h"
#include "spdlog/spdlog.h"
#include "fwdlinterface.h"

int main(int argc, char *argv[])
{
    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    {
        MENU::PrintStartMenu();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0))
    {
        MENU::PrintAvailableUSB();
        return 0;
    }
        if (argc == 2 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0))
    {
        MENU::SaveSettings();
        return 0;
    }
    spdlog::warn("Message");
    stop_download();
    
    // if (hid_init() != 0)
    // {
    //     std::cout << "HIDAPI library cannot be initialized! Reason: ";
    //     std::cout << UTIL::str(hid_error(NULL));
    //     return -1;
    // }
    // auto hids = UTIL::detect_all_hid_linux_devices();
    // hid_device *handle = hid_open_path(hids[0].path_);
    // std::string res = UTIL::get_full_son_response(handle);
    // std::cout<<"\n\n\n"<<res<<"\n\n\n";
    // hid_close(handle);
    
    hid_exit();
    return 0;
}