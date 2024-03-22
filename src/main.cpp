#include "menu.h"
#include "locale.h"
#include "commands_sequencies.h"
#include "logger.h"

using namespace std::string_literals;

int main(int argc, char *argv[])
{
    // if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    // {
    //     MENU::PrintStartMenu();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0))
    // {
    //     MENU::PrintAvailableDevices();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0))
    // {
    //     MENU::SaveSettings();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "--write") == 0))
    // {
    //     MENU::WriteFromJson();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--restore-factory") == 0))
    // {
    //     MENU::RestoreFactorySettings();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--restore-custom") == 0))
    // {
    //     MENU::RestoreCustomSettings();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--download") == 0))
    // {
    //     MENU::DownloadFirmware();
    // }
    // else
    // {
    //     console << UNKNOWN;
    // }
    // console << PRESS_KEY;

    for (int i = 0; i < 255; ++i)
    {
        console << static_cast<char>(i);
    }
    for (int i = 0; i < 255; ++i)
    {
        std::cout << static_cast<char>(i);
    }
    MENU::PrintAvailableDevices();

    char c = getchar();
    return 0;
}