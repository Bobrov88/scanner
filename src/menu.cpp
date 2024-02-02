#include "menu.h"

void MENU::PrintStartMenu()
{
    std::cout << "\nUsage\n";
    std::cout << "\t -h --help           \t\tget description of available functions\n";
    std::cout << "\t                     \t\tполучить описание доступных функций\n";
    std::cout << "\t -i --info           \t\tget information about usb-devices\n";
    std::cout << "\t                     \t\tполучить информацию о usb-устройствах\n";
    std::cout << "\t -s --save           \t\tsave scanner settings to json\n";
    std::cout << "\t                     \t\tсохранить настройки сканера в json\n";
    std::cout << "\t -w --write          \t\twrite setting into scanner from json\n";
    std::cout << "\t                     \t\tзаписать настройки в сканер из json\n";
    std::cout << "\t -f --restore-factory\t\trestore scanner to factory default\n";
    std::cout << "\t                     \t\tвосстановить заводские настройки сканера\n";
    std::cout << "\t -u --restore-user   \t\trestore scanner to user default\n";
    std::cout << "\t                     \t\tвосстановить пользовательские настройки сканера\n";
}

std::vector<UTIL::AVAILABLE_HID> MENU::PrintAvailableUSB()
{
    std::cout << "Note:         Scanners found as COM-devices will automatically be passed to HID-devices\n";
    std::cout << "Примечание:   Сканеры в режиме COM будут автоматически переведены в режим HID\n";
    // std::vector<UTIL::AVAILABLE_COM> COMS = UTIL::get_available_linux_com_ports();

    // uint8_t c[9] = {0};
    // SEQ::test_com_devices_is_scanner_command(c);
    //  std::string message_com_to_hid;

    // for (const auto &com : COMS)
    // {
    //     if (HID::testing_to_pass_HID_from_COM(com.data_, COMS.size()))
    //     {
    //         message_com_to_hid += "\n";
    //         message_com_to_hid += com.data_;
    //         message_com_to_hid += ": switched to HID";
    //     }
    // }

    // if (!message_com_to_hid.empty())
    // {
    //     std::cout << message_com_to_hid;
    // }
    auto hids = UTIL::detect_all_hid_linux_devices();
    return hids;
}

void MENU::SaveSettings()
{
    auto hids = MENU::PrintAvailableUSB();
    std::string scanner_numbers;
    // todo REGEX
    const std::regex int_number{"[0-9]+"};
    //  const std::regex vid_number{"([0]{1}[x]{1}[0-9A-Fa-f]{4}[ ]+)+"};
    std::vector<int> ints;
    std::vector<std::string> vids;
    std::vector<int> scanner_to_proceed;
    bool is_correct_cin = false;
    while (!is_correct_cin)
    {
        std::cout << "Enter scanner numbers with a space for selective saving settings.         E.g.: 1 4 5 \n";
        std::cout << "or enter VIDs with a space to save settings from scanner with these VIDs. E.g.: 0x34eb 0x53da\n";
        std::cout << "----------->: ";
        std::getline(std::cin, scanner_numbers);
        UTIL::trim(scanner_numbers);
        for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), int_number}, rEnd;
             rBegin != rEnd;
             ++rBegin)
            ints.push_back(std::stoi(rBegin->str())-1);
            // if number == 0
        std::cout << "INTS " << ints.size();
        // for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), vid_number}, rEnd;
        //      rBegin != rEnd;
        //      ++rBegin)
        //     vids.push_back(rBegin->str());
        // std::cout << "\nVIDS " << vids.size();
        if ( //! vids.empty() ||
            !ints.empty())
            is_correct_cin = true;
    }
    scanner_to_proceed.assign(ints.begin(), ints.end());
    // for (const auto &vid : vids)
    // {
    //     for (int i = 0; i < hids.size(); ++i)
    //     {
    //         if (UTIL::hex_view(hids[i].vid_) == vid)
    //         {
    //             scanner_to_proceed.push_back(i);
    //         }
    //     }
    // }
    std::sort(scanner_to_proceed.begin(), scanner_to_proceed.end());
    std::unique(scanner_to_proceed.begin(), scanner_to_proceed.end());

    std::cout << "\n\n";
    std::for_each(scanner_to_proceed.begin(), scanner_to_proceed.end(), [&hids](int n)
                  { std::cout << UTIL::hex_view(hids[n].vid_) << " "; });
}