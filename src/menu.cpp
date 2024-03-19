#include "menu.h"
#include "logger.h"

boost::asio::io_service io;
boost::asio::serial_port s_port(io);

int write(char *buf, int length)
{
    return boost::asio::write(s_port, boost::asio::buffer(buf, length));
}

int read(char *buf, int length)
{
    return boost::asio::read(s_port, boost::asio::buffer(buf, length));
}

void MENU::PrintStartMenu()
{
    //  logger("PrintStartMenu");
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

    // BOOST_LOG_TRIVIAL(fatal) << "111This is a trace severity message";
}

void MENU::PrintAttentionComToHID()
{
    // logger("PrintAttentionComToHID");
    std::cout << "Note:         Scanners in COM-mode (if exist) \n";
    std::cout << "              will be automatically switched to HID-mode\n";
    std::cout << "Waiting for available scanners\n";
    //  std::cout << "Примечание:   Сканеры в режиме COM (если таковые имеются)\n";
    // std::cout << "              будут автоматически переведены в режим HID\n";
}

void MENU::PrintAvailableDevices()
{
    //  logger("PrintAvailableDevices");
    MENU::PrintAttentionComToHID();
    
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            std::cout << "\n"
                      << com.port_ << " passing failed";
        }
    }
    std::this_thread::sleep_for(1000ms);
    std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);
}

void MENU::SaveSettings()
{
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            std::cout << "\n"
                      << com.port_ << " passing failed";
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    if (hids.empty())
        return;

    if (UTIL::save_settings_to_files(hids))
        std::cout << "All scanners settings successfully saved into files\n";
    else
        std::cout << "\nNot all scanners settings saved!\n";
}

void MENU::WriteFromJson()
{
    // logger("Write from json");
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            std::cout << "\n"
                      << com.port_ << " passing failed";
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto jsons = UTIL::get_json_file_list();
    PRINT::print_all_json_files(jsons);
    if (jsons.empty()) {
        return;
    }
    int json_file = PRINT::ChooseToProceed(jsons.size());

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    if (hids.empty())
        return;

    auto settings = UTIL::convert_json_to_bits(jsons[json_file].first);
    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (-1 == UTIL::write_settings_from_json(settings, device))
        {
            std::cout << "\n Settings write fail";
            return;
        }
        if (0 == HID::save_to_internal_flash(device))
            std::cout << "Success\n";
        else
            std::cout << "Saving Failed\n";
        hid_close(device.ptr);
    }
    hid_exit();
}

void MENU::RestoreFactorySettings()
{
    // logger("Restoring factory settings");
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            std::cout << "\n"
                      << com.port_ << " passing failed";
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    if (hids.empty())
        return;

    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (0 == HID::restore_to_factory_settings(device))
        {
            std::cout << device.serial_number << " successfully restored to factory settings\n";
            // logger("Restoring to factory - Success", device.serial_number);
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
    // logger("Restoring custom settings");
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            std::cout << "\n"
                      << com.port_ << " passing failed";
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    if (hids.empty())
        return;

    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (0 == HID::restore_to_custom_settings(device))
        {
            std::cout << device.serial_number << " successfully restored to custom settings\n";
            // logger("Restoring to custom - Success", device.serial_number);
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
    // logger("Download firmware");
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            std::cout << "\n"
                      << com.port_ << " passing failed";
        }
        std::this_thread::sleep_for(1s);
    }

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_linux_devices(hids);

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    if (hids.empty())
        return;

    std::cout << std::endl;

    auto firmware_files = UTIL::get_firmware_list();
    PRINT::print_all_firmware_files(firmware_files);
    int number = PRINT::ChooseToProceed(firmware_files.size());
    if (firmware_files[number].second != 0)
    {
        std::cout << "\n Firmware file corrupted. Downloading canceled!";
        return;
    }

    for (auto &hid : hids)
    {
        auto firmware = firmware_files[number].first.data();

        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (!HID::testing_to_pass_COM_from_HID(device.ptr))
        {
            std::cout << "\n"
                      << hid.serial_number_ << " passing to COM failed!";
        }
        if (device.ptr)
            hid_close(device.ptr);
        std::this_thread::sleep_for(3000ms);
        try
        {
            coms = UTIL::get_available_linux_com_ports();
            if (coms.empty())
            {
                std::cout << device.serial_number << ": connection aborted\n";
                continue;
            }
            s_port.open(coms[0].port_);
            s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
            firmware_parse_pro(firmware);

            int fw_download_start = firmware_download_start(write, read, false);
            std::cout << device.serial_number << ": firmware downloading ";
            if (fw_download_start == 0)
                std::cout << "...\n";
            else
            {
                std::cout << " FAILED\n";
                return;
            }

            double persent = 0;
            DownloadState state;
            std::this_thread::sleep_for(500ms);
            int out_percent = 0;
            do
            {
                get_download_state(&persent, &state);
                if (out_percent != (int)(persent * 100) / 10 * 10)
                {
                    out_percent = (int)(persent * 100) / 10 * 10;
                    std::cout << out_percent << "%  ";
                    std::flush(std::cout);
                }

                if (!s_port.is_open())
                {
                    coms = UTIL::get_available_linux_com_ports();
                    if (coms.empty())
                    {
                        std::cout << "\n"
                                  << device.serial_number << ": connection aborted\n";
                        break;
                    }
                    s_port.open(coms[0].port_);
                }

                if (state == DownloadState::SUCCESS || state < DownloadState::SUCCESS)
                {
                    if (state == DownloadState::RECONNECTDEVICE)
                    {
                        if (s_port.is_open())
                        {
                            s_port.close();
                        }
                        std::this_thread::sleep_for(3000ms);
                        coms = UTIL::get_available_linux_com_ports();
                        if (coms.empty())
                        {
                            std::cout << "\n"
                                      << device.serial_number << ": connection aborted\n";
                            break;
                        }
                        s_port.open(coms[0].port_);
                        s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
                        std::this_thread::sleep_for(1000ms);
                        fw_download_start = firmware_download_start(write, read, false);
                    }
                    else
                    {
                        if (s_port.is_open())
                        {
                            std::cout << "\n"
                                      << device.serial_number;
                            if (state == DownloadState::FAIL_NONEEDUPDATE)
                                std::cout << ": already has current firmware\n";
                            else
                                std::cout << ": download SUCCESS\n";
                            s_port.close();
                        }
                        break;
                    }
                }
                std::this_thread::sleep_for(500ms);
            } while (persent < 100);
        }
        catch (boost::system::system_error &e)
        {
            boost::system::error_code ec = e.code();
            std::cerr << ec.value() << '\n';
            std::cerr << ec.category().name() << '\n';
            std::cerr << ec.message() << '\n';
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