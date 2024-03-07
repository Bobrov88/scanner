#include "menu.h"
#include "locale.h"
#include "commands_sequencies.h"

int main(int argc, char *argv[])
{
    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    {
        MENU::PrintStartMenu();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0))
    {
        MENU::PrintAvailableDevices();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0))
    {
        MENU::SaveSettings();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "--write") == 0))
    {
        MENU::WriteFromJson();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--restore-factory") == 0))
    {
        MENU::RestoreFactorySettings();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--restore-custom") == 0))
    {
        MENU::RestoreCustomSettings();
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--download") == 0))
    {
        MENU::DownloadFirmware();
        return 0;
    }
    return 0;
}