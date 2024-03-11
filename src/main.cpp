#include "menu.h"
#include "locale.h"
#include "commands_sequencies.h"
#include <ncurses.h>

int main(int argc, char *argv[])
{
    // initscr(); // Инициализация ncurses
    // cbreak();  // Включение режима "cbreak", чтобы клавиши были доступны немедленно
    // noecho();  // Отключение отображения вводимых символов

    // int choice;
    // bool running = true;

    // while (running)
    // {
    //     clear();
    //     printw("1. Option 1\n");
    //     printw("2. Option 2\n");
    //     printw("3. Exit\n");

    //     refresh();

    //     choice = getch(); // Получение ввода пользователя

    //     switch (choice)
    //     {
    //     case '1':
    //         // Действия для опции 1
    //         break;
    //     case '2':
    //         // Действия для опции 2
    //         break;
    //     case '3':
    //         running = false;
    //         break;
    //     default:
    //         printw("Invalid choice. Press any key to continue.\n");
    //         getch();
    //         break;
    //     }
    // }

    // endwin(); // Завершение работы с ncurses
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