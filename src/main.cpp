#include "menu.h"
#include "locale.h"
#include "commands_sequencies.h"
#include "logger.h"

using namespace std::string_literals;

int main(int argc, char *argv[])
{
#ifdef __WIN__
    system("chcp 65001 > nul");
#endif
    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    {
        MENU::PrintStartMenu();
        console << PRESS_KEY;
        char c = getchar();
    }
    else if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0))
    {
        MENU::PrintAvailableDevices();
    }
    else if (argc == 2 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0))
    {
        MENU::SaveSettings();
    }
    else if (argc == 2 && (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "--write") == 0))
    {
        MENU::WriteFromJson();
    }
    else if (argc == 2 && (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--restore-factory") == 0))
    {
        MENU::RestoreFactorySettings();
    }
    else if (argc == 2 && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--restore-custom") == 0))
    {
        MENU::RestoreCustomSettings();
    }
    else if (argc == 2 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--download") == 0))
    {
        MENU::DownloadFirmware();
    }
    else
    {
        console << UNKNOWN;
    }
    return 0;
}