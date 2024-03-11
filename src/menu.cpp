#include "menu.h"

boost::asio::io_service io;
boost::asio::serial_port s_port(io);
std::string com_port;
std::string sN;

int write(char *buf, int length)
{
#if DEBUG_COMMENT == TRUE
    //   std::cout << "\nWrite = " << CONVERT::to_hex((uint8_t *)buf, length);
#endif
    if (!s_port.is_open())
    {
        //    std::cout << "Trying reconnect in write\n";
        // auto coms = UTIL::get_available_linux_com_ports();
        //      std::cout << "coms[0].port_=" << coms[0].port_ << std::endl;

        // todo shared_ptr
        s_port.open(com_port);
        std::this_thread::sleep_for(500ms);
    }
#if DEBUG_COMMENT == TRUE
    std::cout << "\n"
              << CONVERT::to_hex((uint8_t *)buf, (size_t)length);
#endif
    return boost::asio::write(s_port, boost::asio::buffer(buf, length));
}

int read(char *buf, int length)
{
    if (!s_port.is_open())
    {
        // std::cout << "Trying reconnect in write\n";
        //  auto coms = UTIL::get_available_linux_com_ports();
        //  std::cout << "coms[0].port_=" << coms[0].port_ << std::endl;
        s_port.open(com_port);
        std::this_thread::sleep_for(500ms);
    }
    //   auto size = boost::asio::read(s_port, boost::asio::buffer(buf, length));
    //  std::cout << "\nRead = " << CONVERT::to_hex((uint8_t *)buf, length);
    int a = boost::asio::read(s_port, boost::asio::buffer(buf, length));
#if DEBUG_COMMENT == TRUE
    std::cout << "\n"
              << CONVERT::to_hex((uint8_t *)buf, (size_t)length);
#endif
    return a;
}

void MENU::PrintStartMenu()
{
    //logger("PrintStartMenu");
    std::cout << "\nUsage\n";
    std::cout << "\t -h --help           \t\tget description of available functions\n";
    // std::cout << "\t                     \t\tполучить описание доступных функций\n";
    std::cout << "\t -i --info           \t\tget information about usb-devices\n";
    // std::cout << "\t                     \t\tполучить информацию о usb-устройствах\n";
    std::cout << "\t -s --save           \t\tsave scanner settings to json\n";
    // std::cout << "\t                     \t\tсохранить настройки сканера в json\n";
    std::cout << "\t -w --write          \t\twrite setting into scanner from json\n";
    // std::cout << "\t                     \t\tзаписать настройки в сканер из json\n";
    std::cout << "\t -f --restore-factory\t\trestore scanner to factory default\n";
    // std::cout << "\t                     \t\tвосстановить заводские настройки сканера\n";
    std::cout << "\t -c --restore-custom \t\trestore scanner to user default\n";
    // std::cout << "\t                     \t\tвосстановить пользовательские настройки сканера\n";
    std::cout << "\t -d --download       \t\tupdate scanner firmware\n";
    // std::cout << "\t                     \t\tобновить прошивку сканера\n";
}

void MENU::PrintAttentionComToHID()
{
    //logger("PrintAttentionComToHID");
    std::cout << "Note:         Scanners found as COM-devices (if exist) \n";
    std::cout << "              will automatically be passed into HID-devices\n";
    //  std::cout << "Примечание:   Сканеры в режиме COM (если таковые имеются)\n";
    // std::cout << "              будут автоматически переведены в режим HID\n";
}

int MENU::OfferToSaveAs()
{
    // std::cout << "Примечания:   Произведённые изменения действительны при наличии питания сканера\n";
    // std::cout << "Note:         The changes made are valid when the scanner is powered up\n";
    //  std::cout << "Сохранить во внутреннюю память?\n";
    std::cout << "Save to internal flash?\n";
    int save_as = 0;
    return save_as;
}

void MENU::PrintAvailableDevices()
{
    //logger("PrintAvailableDevices");
    std::cout << "HID-devices\n";
    // std::cout << "HID-устройства\n";
    std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);
    // pass to HID
    // revise if error
    // saving with printing lists
    std::cout << "COM-devices\n";
    //  std::cout << "COM-устройства\n";
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
    auto coms = UTIL::get_available_linux_com_ports();
    PRINT::print_all_com_linux_devices(coms);

    for (const auto &com : coms)
    {
        HID::testing_to_pass_HID_from_COM(com.port_);
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);

    // std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    // hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);
    // std::cout << "\n138 menu";
    if (UTIL::save_settings_to_files(hids))
        std::cout << "All scanners settings successfully saved into files\n";
    else
        std::cout << "\nNot all scanners settings saved!\n";
}

void MENU::WriteFromJson()
{
    //logger("Write from json");
    MENU::PrintAttentionComToHID();
    auto coms = UTIL::get_available_linux_com_ports();
    PRINT::print_all_com_linux_devices(coms);

    for (const auto &com : coms)
    {
        HID::testing_to_pass_HID_from_COM(com.port_);
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto jsons = UTIL::get_json_file_list();
    PRINT::print_all_json_files(jsons);
    int json_file = PRINT::ChooseToProceed(jsons.size());

    auto hids = UTIL::get_available_hid_devices();
    // std::cout<<"\nGood 166";
    PRINT::print_all_hid_linux_devices(hids);
    // std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    // hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    auto settings = UTIL::convert_json_to_bits(jsons[json_file].first);
    // std::cout << "\nCONVERTED OK";
    for (const auto &hid : hids)
    // for (const auto &com : coms)
    {
        // boost::asio::io_service io;
        // boost::asio::serial_port s_port(io);
        // s_port.open(com.port_);
        // if (0 == UTIL::write_settings_from_json(settings, s_port))
        // {
        //     std::cout << "Success";
        // }
        // else
        // {
        //     std::cout << "Failed!";
        // }
        // if (s_port.is_open())
        //     s_port.close();
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        //    std::cout << "\nPATH=" << std::string(hid.path_);
        if (-1 == UTIL::write_settings_from_json(settings, device)) {
            std::cout<<"\n Settings write fail";
            return;
        }
        //      std::cout << "\nWRITE OK";
        if (0 == HID::save_to_internal_flash(device))
            std::cout << "Success\n";
        else
            std::cout << "Saving Failed\n";
        // int save_as = OfferToSaveAs();
        // switch (save_as)
        // {
        // case 1:
        //     break;
        // default:
        //     break;
        // }
        hid_close(device.ptr);
    }
    hid_exit();
}

void MENU::RestoreFactorySettings()
{
    //logger("Restoring factory settings");
    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);
    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (0 == HID::restore_to_factory_settings(device))
        {
            std::cout << device.serial_number << " successfully restored to factory settings\n";
            //logger("Restoring to factory - Success", device.serial_number);
        }
        else
        {
            std::cout << device.serial_number << " failed to restored to factory settings\n";
        }
        hid_close(device.ptr);
    }
}

void MENU::RestoreCustomSettings()
{
    //logger("Restoring custom settings");
    auto hids = UTIL::get_available_hid_devices();
    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (0 == HID::restore_to_custom_settings(device))
        {
            std::cout << device.serial_number << " successfully restored to custom settings\n";
            //logger("Restoring to custom - Success", device.serial_number);
        }
        else
        {
            std::cout << device.serial_number << " failed to restored to custom settings\n";
        }
        hid_close(device.ptr);
    }
}

void MENU::DownloadFirmware()
{
    //logger("Download firmware");
    // std::cout << "HID-devices\n";
    // std::cout << "HID-устройства\n";
    // std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    // PRINT::print_all_hid_linux_devices(hids);
    // pass to HID
    // revise if error
    // saving with printing lists
    std::cout << "COM-devices\n";
    // std::cout << "COM-устройства\n";
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    // std::vector<UTIL::AVAILABLE_COM> coms;
    PRINT::print_all_com_linux_devices(coms);
    try
    {
        //logger("Port open", coms[0].port_);
        s_port.open(coms[0].port_);
        // com_port = coms[0].port_;
        // com_port = "/dev/ttyACM1"s;
        //  sN = coms[0].serial_number_;
        // sN = "F23450001"s;
        s_port.open(com_port);
        s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
        std::this_thread::sleep_for(500ms);
    }
    catch (boost::system::system_error &e)
    {
        boost::system::error_code ec = e.code();
        std::cerr << ec.value() << '\n';
        std::cerr << ec.category().name() << '\n';
        std::cerr << ec.message() << '\n';
    }

    auto firmware_files = UTIL::get_firmware_list();
    PRINT::print_all_firmware_files(firmware_files);
    int number = PRINT::ChooseToProceed(firmware_files.size());
    firmware_parse_pro(firmware_files[number].first.data());
    //  std::cout << "234 after choose\n";
    while (true)
    {
        // std::cout << "\n"
        //           << "Before writing to the port: ";
        int fw_download_start = firmware_download_start(write, read, false);
        if (fw_download_start == 0)
            std::cout << "\nFirmware downloading: ";
        else
        {
            std::cout << "\nfirmware Download FAILED\n";
            return;
        }

        double persent = 0;
        DownloadState state;
        std::this_thread::sleep_for(3000ms);

        while (persent < 100)
        {
            std::cout << persent * 100 << "%\r";
            get_download_state(&persent, &state);
            // std::cout << "\n"
            //           << persent << "% -- Status: " << state << std::endl;
            // if (!s_port.is_open())
            // {
            //     std::cout << "Trying reconnect\n";
            //     coms = UTIL::get_available_linux_com_ports();
            //     std::cout << "coms[0].port_=" << coms[0].port_ << std::endl;
            //     s_port.open(coms[0].port_);
            // }
            // else
            // {
            //     std::cout << "Port active\n";
            // }

            if (state == DownloadState::SUCCESS || state < DownloadState::SUCCESS)
            {
                if (state == DownloadState::RECONNECTDEVICE)
                {
                    if (s_port.is_open())
                    {
                        //logger("Closing...", coms[0].port_);
                        s_port.close();
                    }
                    std::cout << "  Need reconnecting...  ";
                    std::this_thread::sleep_for(3000ms);
                    try
                    {
                        coms = UTIL::get_available_linux_com_ports();
                        // com_port = RECONNECT::com_reconnect(sN);
                        s_port.open(coms[0].port_);
                        //logger("Opening...", coms[0].port_);
                        s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
                    }
                    catch (boost::system::system_error &e)
                    {
                        boost::system::error_code ec = e.code();
                        std::cerr << ec.value() << '\n';
                        std::cerr << ec.category().name() << '\n';
                        std::cerr << ec.message() << '\n';
                    }
                    std::this_thread::sleep_for(1000ms);
                    fw_download_start = firmware_download_start(write, read, false);
                }
                else
                {
                    if (s_port.is_open())
                    {
                        std::cout << " OK\n";
                        s_port.close();
                        //logger("Firmware successfully downloaded", coms[0].port_);
                    }
                    return;
                }
            }
            std::this_thread::sleep_for(500ms);
        }
    }
    return;
}

// void MENU::ReadUsbIdentifiers(std::string dev_path){
//     auto udev = udev_new();
//     if (!udev) { return; }

//     struct stat statbuf;
//     if (stat(dev_path.c_str(), &statbuf) < 0) { return; }
//     auto type =  S_ISBLK(statbuf.st_mode) ? 'b' : S_ISCHR(statbuf.st_mode) ? 'c' : 0;

//     auto opened_dev = udev_device_new_from_devnum(udev, type, statbuf.st_rdev);
//     auto dev = opened_dev;

//     while (dev != nullptr)
//     {
//         auto serial = udev_device_get_sysattr_value(dev, "serial");
//         if (nullptr == serial)
//         {
//             dev = udev_device_get_parent(dev);
//         }
//         else
//         {
//             std::cout << "VID: " <<  udev_device_get_sysattr_value(dev, "idVendor") << std::endl;
//             std::cout << "PID: " <<  udev_device_get_sysattr_value(dev, "idProduct") << std::endl;
//             std::cout << "Serial Number: " <<  serial << std::endl;
//         }
//     }
//     if (opened_dev) { udev_device_unref(opened_dev); }
//     udev_unref(udev);
// }