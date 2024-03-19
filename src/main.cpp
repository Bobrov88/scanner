#include "menu.h"
#include "locale.h"
#include "commands_sequencies.h"
#include "logger.h"

using namespace std::string_literals;
src::severity_logger<int> slg;
#define console BOOST_LOG_SEV(slg, 0)
#define logger BOOST_LOG_SEV(slg, 1)

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
    // setlocale(LC_ALL, "rus");
    //  setlocale(LC_CTYPE, "rus");
    // endwin(); // Завершение работы с ncurses
    logger_init();

    console << "Это для консоли";
    logger << "Это для файла";


    // if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    // {
    //     MENU::PrintStartMenu();
    // }
    // else if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0))
    // {
         MENU::PrintAvailableDevices();
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
    //     std::cout << "Unknown parameter";
    // std::cout << "\nPress any key to exit...";
    // char c = getchar();
    return 0;
}

// while (true) {
//      PRINT::ChooseScannerToProceed();
//  }
//  BOOST::init_logging();
//  BOOST_LOG_TRIVIAL(warning) << "Русское предупреждение";
//  BOOST_LOG_TRIVIAL(warning) << "This is a debug severity message";
//  BOOST_LOG_TRIVIAL(info) << "This is an informational severity message";
//  BOOST_LOG_TRIVIAL(warning) << "This is a warning severity message";
//  BOOST_LOG_TRIVIAL(error) << "This is an error severity message";
//  BOOST_LOG_TRIVIAL(fatal) << "and this is a fatal severity message";

// BOOST_LOG_TRIVIAL(fatal) << "Русские символы"s;

// MENU::PrintStartMenu();

// logging::add_common_attributes();

// std::locale loc("ru_RU.UTF-8");
// std::locale::global(loc);

// BOOST::init_console_logging();
// BOOST::init_file_logging();

// src::severity_logger<logging::trivial::severity_level> console_logger;
// src::severity_logger<logging::trivial::severity_level> file_logger;

// BOOST_LOG_SEV(console_logger, logging::trivial::trace) << "Это сообщение для консоли";
// BOOST_LOG_SEV(file_logger, logging::trivial::info) << "Это сообщение для файла";

//   return 0;
//}