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
    std::cout << "\t -d --download       \t\tupdate scanner firmware\n";
    std::cout << "\t                     \t\tобновить прошивку сканера\n";
}

void MENU::PrintAttentionComToHID()
{
    std::cout << "Note:         Scanners found as COM-devices (if exist) will automatically be put into to HID-devices\n";
    std::cout << "Примечание:   Сканеры в режиме COM (если таковые имеются) будут автоматически переведены в режим HID\n";
}

std::string MENU::ChooseScannerToProceed() // take this function out of MENU namespace
{
    std::cout << "Enter scanner numbers with a space for selective saving settings.         E.g.: 1 4 5 \n";
    std::cout << "or enter VIDs with a space to save settings from scanner with these VIDs. E.g.: 0x34eb 0x53da\n";
    std::cout << "----------->: ";
    std::string scanner_numbers;
    std::getline(std::cin, scanner_numbers);
    return scanner_numbers;
}

void MENU::PrintAvailableDevices()
{
    std::cout << "HID-devices\n";
    std::cout << "HID-устройства\n";
    std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    UTIL::print_all_hid_linux_devices(hids);
    // pass to HID
    // revise if error
    // saving with printing lists
    std::cout << "COM-devices\n";
    std::cout << "COM-устройства\n";
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    UTIL::print_all_com_linux_devices(coms);
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
}

void MENU::SaveSettings()
{
    MENU::PrintAttentionComToHID();

    std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    UTIL::print_all_hid_linux_devices(hids);
    std::string scanner_numbers = ChooseScannerToProceed();
    // todo REGEX
    const std::regex int_number{"[0-9]+"};
    //  const std::regex vid_number{"([0]{1}[x]{1}[0-9A-Fa-f]{4}[ ]+)+"};
    std::vector<int> ints;
    std::vector<std::string> vids;
    std::vector<int> scanner_to_proceed;
    bool is_correct_cin = false;
    while (!is_correct_cin)
    {
        // std::string scanners_string = Choose
        UTIL::trim(scanner_numbers);
        for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), int_number}, rEnd;
             rBegin != rEnd;
             ++rBegin)
            ints.push_back(std::stoi(rBegin->str()) - 1);
        // if number == 0
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
    std::for_each(scanner_to_proceed.begin(), scanner_to_proceed.end(), [&hids](const auto n)
                  {
                    hid_device *handle = hid_open_path(hids[n].path_);
                    std::string json = UTIL::get_full_json_response(handle);
                    std::string out_file_name = UTIL::str(hids[n].serial_number_) + ".json"s;
                    hid_close(handle);
                    std::ofstream out;
                    out.open(out_file_name);
                    if (out)
                    out << json;
                    else std::cerr<<"Unable to create file: "<<out_file_name<<"\n";
                    out.close(); });
}

void MENU::WriteFromJson()
{
    auto jsons = UTIL::get_json_file_list();
    UTIL::print_all_json_files(jsons);
    auto hids = UTIL::get_available_hid_devices();
    // todo choose scanners, temporarily we chose the single one
    // todo choose a file, temporarily we chose single one
    auto settings = UTIL::convert_json_to_bits("F23450001.json");
    hid_device *handle = hid_open_path(hids[0].path_);
    int write_result = write_settings_from_json(settings, handle);
}
