#include "menu.h"

boost::asio::io_service io;
boost::asio::serial_port s_port(io);

int write(char *buf, int length)
{
    std::cout << "\nWrite = " << CONVERT::to_hex((uint8_t *)buf, length);
    if (!s_port.is_open())
    {
        std::cout << "Trying reconnect in write\n";
        auto coms = UTIL::get_available_linux_com_ports();
        std::cout << "coms[0].port_=" << coms[0].port_ << std::endl;
        s_port.open(coms[0].port_);
        std::this_thread::sleep_for(1000ms);
    }
    return boost::asio::write(s_port, boost::asio::buffer(buf, length));
}

int read(char *buf, int length)
{
    if (!s_port.is_open())
    {
        std::cout << "Trying reconnect in write\n";
        auto coms = UTIL::get_available_linux_com_ports();
        std::cout << "coms[0].port_=" << coms[0].port_ << std::endl;
        s_port.open(coms[0].port_);
        std::this_thread::sleep_for(1000ms);
    }
    auto size = boost::asio::read(s_port, boost::asio::buffer(buf, length));
    std::cout << "\nRead = " << CONVERT::to_hex((uint8_t *)buf, length);
    return size;
}

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
    std::cout << "\t -u --restore-custom \t\trestore scanner to user default\n";
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

int MENU::OfferToSaveAs()
{
    std::cout << "Примечания:   Произведённые изменения действительны при наличии питания сканера\n";
    std::cout << "Note:         The changes made are valid when the scanner is powered up\n";
    std::cout << "Сохранить во внутреннюю память?\n";
    std::cout << "Save to internal flash?\n";
    int save_as = 0;
    return save_as;
}

void MENU::PrintAvailableDevices()
{
    std::cout << "HID-devices\n";
    std::cout << "HID-устройства\n";
    std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);
    // pass to HID
    // revise if error
    // saving with printing lists
    std::cout << "COM-devices\n";
    std::cout << "COM-устройства\n";
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    PRINT::print_all_com_linux_devices(coms);
    // uint8_t c[9] = {0};
    // SEQ::test_com_devices_is_scanner_command(c);
    //  std::string message_com_to_hid;

    // for (const auto &com : COMS)
    // {
    //     if (HID::testing_to_pass_HID_from_COM(com.port_, COMS.size()))
    //     {
    //         message_com_to_hid += "\n";
    //         message_com_to_hid += com.port_;
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
    PRINT::print_all_hid_linux_devices(hids);
    std::string scanner_numbers = ChooseScannerToProceed();
    const std::regex int_number{"[1-9]+"};
    const std::regex vid_number{"[0x]{1}[A-Fa-f0-9]{4}"};
    std::vector<int> ints;
    std::vector<std::string> vids;
    std::vector<int> scanner_to_proceed;
    bool is_correct_cin = false;
    while (!is_correct_cin)
    {
        UTIL::trim(scanner_numbers);
        for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), int_number}, rEnd;
             rBegin != rEnd;
             ++rBegin)
        {
            if (rBegin->str() != "0")
                ints.push_back(std::stoi(rBegin->str()) - 1);
        }

        for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), vid_number}, rEnd;
             rBegin != rEnd;
             ++rBegin)
            vids.push_back(rBegin->str());
        if (!vids.empty() ||
            !ints.empty())
            is_correct_cin = true;
    }
    scanner_to_proceed.assign(ints.begin(), ints.end());
    for (const auto &vid : vids)
    {
        for (int i = 0; i < hids.size(); ++i)
        {
            if (CONVERT::hex_view(hids[i].vid_) == vid)
            {
                scanner_to_proceed.push_back(i);
            }
        }
    }
    std::sort(scanner_to_proceed.begin(), scanner_to_proceed.end());
    std::unique(scanner_to_proceed.begin(), scanner_to_proceed.end());

    std::cout << "\n\n";
    std::for_each(scanner_to_proceed.begin(), scanner_to_proceed.end(), [&hids](const auto n)
                  {
                    hid_device *handle = hid_open_path(hids[n].path_);
                    std::string json = UTIL::get_full_json_response(handle);
                    std::string out_file_name = CONVERT::str(hids[n].serial_number_) + ".json"s;
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
    PRINT::print_all_json_files(jsons);
    auto hids = UTIL::get_available_hid_devices();
    // todo choose scanners, temporarily we chose the single one
    // todo choose a file, temporarily we chose single one
    auto settings = UTIL::convert_json_to_bits("F23450001.json");
    handler device{hid_open_path(hids[0].path_), hids[0].path_, CONVERT::str(hids[0].serial_number_)};
    int write_result = UTIL::write_settings_from_json(settings, device);
    if (write_result == 0)
        std::cout << "Success\n";
    else
        std::cout << "Failed\n";
    int save_as = OfferToSaveAs();
    switch (save_as)
    {
    case 1:
        HID::save_to_internal_flash(device);
        break;
    default:
        break;
    }
}

void MENU::RestoreFactorySettings()
{
    auto hids = UTIL::get_available_hid_devices();
    handler device{hid_open_path(hids[0].path_), hids[0].path_, CONVERT::str(hids[0].serial_number_)};
    if (-1 == HID::restore_to_factory_settings(device))
        std::cout << "Success\n";
    else
        std::cout << "Failed\n";
}

void MENU::RestoreCustomSettings()
{
    auto hids = UTIL::get_available_hid_devices();
    handler device{hid_open_path(hids[0].path_), hids[0].path_, CONVERT::str(hids[0].serial_number_)};
    if (-1 == HID::restore_to_custom_settings(device))
        std::cout << "Success\n";
    else
        std::cout << "Failed\n";
}

void MENU::DownloadFirmware()
{
    // std::cout << "HID-devices\n";
    // std::cout << "HID-устройства\n";
    // std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    // PRINT::print_all_hid_linux_devices(hids);
    // pass to HID
    // revise if error
    // saving with printing lists
    std::cout << "COM-devices\n";
    std::cout << "COM-устройства\n";
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    //   PRINT::print_all_com_linux_devices(coms);
    // Serial::serial comport;
    try
    {
        //  s_port.open(coms[0].port_);
        s_port.open(coms[0].port_);
        s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
        std::this_thread::sleep_for(3000ms);
    }
    catch (boost::system::system_error &e)
    {
        boost::system::error_code ec = e.code();
        std::cerr << ec.value() << '\n';
        std::cerr << ec.category().name() << '\n';
        std::cerr << ec.message() << '\n';
    }
    char firmware[64] = "QR_APP_003_BGA65_HModuleFC_QM3503_V1.2.1.sig";
    int fw_parse_result = firmware_parse_pro(firmware);
    if (fw_parse_result == 0)
    {
        std::cout << "\n"
                  << "Parsing SUCCESS\n";
        while (true)
        {
            std::cout << "\n"
                      << "Before writing to the port: ";
            int fw_download_start = firmware_download_start(write, read, false);
            if (fw_download_start == 0)
                std::cout << "\nfirmware Download SUCCESS\n";
            else
            {
                std::cout << "\nfirmware Download FAILED\n";
                return;
            }

            double persent;
            DownloadState state;
            std::this_thread::sleep_for(3000ms);
            while (persent < 100)
            {
                get_download_state(&persent, &state);
                std::cout << "\n"
                          << persent << "% -- Status: " << state << std::endl;
                if (!s_port.is_open())
                {
                    std::cout << "Trying reconnect\n";
                    coms = UTIL::get_available_linux_com_ports();
                    std::cout << "coms[0].port_=" << coms[0].port_ << std::endl;
                    s_port.open(coms[0].port_);
                    // fw_download_start = firmware_download_start(write, read, false);
                    // if (fw_download_start == 0)
                    //     std::cout << "\nfirmware Download SUCCESS\n";
                    // else
                    // {
                    //     std::cout << "\nfirmware Download FAILED\n";
                    // }
                }
                else
                {
                    std::cout << "Port active\n";
                }

                if (state == DownloadState::SUCCESS || state < DownloadState::SUCCESS)
                {
                    if (state == DownloadState::RECONNECTDEVICE)
                    {
                        std::cout << "\n"
                                  << "Reconnecting\n";
                        if (s_port.is_open())
                        {
                            std::cout << "Is open\n";
                            s_port.close();
                        }
                        std::cout << "Close\n";
                        std::this_thread::sleep_for(3000ms);
                        //  com = RECONNECT::com_reconnect("F23450001"s);
                        //  std::cout << "New port is: " << com << std::endl;
                        //   std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
                        //      std::cout << "Coms size = " << coms.size() << "\n";
                        //    PRINT::print_all_com_linux_devices(coms);
                        std::cout << "Print\n";
                        try
                        {
                            std::cout << "Openning\n";
                            coms = UTIL::get_available_linux_com_ports();
                            s_port.open(coms[0].port_);
                            std::cout << "Opened\n";
                            s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
                            std::cout << "Set option\n";
                        }
                        catch (boost::system::system_error &e)
                        {
                            boost::system::error_code ec = e.code();
                            std::cerr << ec.value() << '\n';
                            std::cerr << ec.category().name() << '\n';
                            std::cerr << ec.message() << '\n';
                        }

                        std::cout << "\nOpening\n";
                        std::cout << "\nAfter writing to the port: ";
                        fw_download_start = firmware_download_start(write, read, false);
                        if (fw_download_start == 0)
                            std::cout << "\nfirmware Download SUCCESS\n";
                        else
                        {
                            std::cout << "\nfirmware Download FAILED\n";
                        }
                        break;
                    }
                    else
                    {
                        if (s_port.is_open())
                        {
                            std::cout << "Success\n";
                            s_port.close();
                        }
                        return;
                    }
                }
                std::this_thread::sleep_for(500ms);
            }
        }
    }
    else
        std::cout << "\nParsing FAILED\n";
    return;
}