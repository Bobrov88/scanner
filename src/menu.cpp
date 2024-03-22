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
    console << USAGE;
    console << GET_DESC;
    console << GET_INFO;
    console << GET_SAVE;
    console << GET_WRITE;
    console << GET_REST_FACT;
    console << GET_REST_CUST;
}

void MENU::PrintAttentionComToHID()
{
    console << COM2HID;
    console << WAIT4SCAN;
}

void MENU::PrintAvailableDevices()
{
    logger << "PrintAvailableDevices";
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms;
    #ifdef __WIN__
    coms = UTIL::get_available_windows_com_ports();
    #elif (__DEBIAN__ || __CENTOS__)
    coms = UTIL::get_available_linux_com_ports();
    #endif
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            console << com.port_ << " " << PASS_FAIL;
        }
    }
    std::this_thread::sleep_for(3000ms);
    std::vector<UTIL::AVAILABLE_HID> hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_devices(hids);
}

void MENU::SaveSettings()
{
    logger << "SaveSettings";
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            console << com.port_ << " " << PASS_FAIL;
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_devices(hids);

    if (hids.empty())
        return;

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    if (UTIL::save_settings_to_files(hids))
    {
        console << SAVE_OK;
        logger << SAVE_OK;
    }
    else
    {
        console << SAVE_FAIL;
        logger << SAVE_FAIL;
    }
}

void MENU::WriteFromJson()
{
    logger << "WriteFromJson";
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            console << com.port_ << " " << PASS_FAIL;
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto jsons = UTIL::get_json_file_list();
    PRINT::print_all_json_files(jsons);
    if (jsons.empty())
    {
        return;
    }
    int json_file = PRINT::ChooseToProceed(jsons.size());

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_devices(hids);

    if (hids.empty())
        return;

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    logger << "Json file: " << jsons[json_file].first;

    auto settings = UTIL::convert_json_to_bits(jsons[json_file].first);
    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (-1 == UTIL::write_settings_from_json(settings, device))
        {
            console << WRITE_FAIL;
            logger << device.serial_number << ": " << WRITE_FAIL;
            hid_close(device.ptr);
            continue;
        }
        if (0 == HID::save_to_internal_flash(device))
            console << _SUCCESS_;
        else
        {
            console << WRITE_CUS_FAIL;
            logger << device.serial_number << ": " << WRITE_CUS_FAIL;
        }
            hid_close(device.ptr);
        hid_exit();
    }
}

void MENU::RestoreFactorySettings()
{
    logger << "RestoreFactorySettings";
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            console << com.port_ << " " << PASS_FAIL;
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_devices(hids);

    if (hids.empty())
        return;

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (0 == HID::restore_to_factory_settings(device))
        {
            console << device.serial_number << " " << REST_OK;
            logger << device.serial_number << ": " << REST_OK;
        }
        else
        {
            console << device.serial_number << " " << REST_FAIL;
            logger << device.serial_number << ": " << REST_FAIL;
        }
        hid_close(device.ptr);
    }
}

void MENU::RestoreCustomSettings()
{
    logger << "RestoreCustomSettings";
    MENU::PrintAttentionComToHID();
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            console << com.port_ << " " << PASS_FAIL;
        }
        std::this_thread::sleep_for(300ms);
    }
    std::this_thread::sleep_for(3000ms); // delay for reconnecting

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_devices(hids);

    if (hids.empty())
        return;

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    for (const auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (0 == HID::restore_to_custom_settings(device))
        {
            console << device.serial_number << " " << CUST_OK;
            logger << device.serial_number << ": " << CUST_OK;
        }
        else
        {
            console << device.serial_number << " " << CUST_FAIL;
            logger << device.serial_number << ": " << CUST_FAIL;
        }
        hid_close(device.ptr);
    }
}

void MENU::DownloadFirmware()
{
    std::vector<UTIL::AVAILABLE_COM> coms = UTIL::get_available_linux_com_ports();
    for (const auto &com : coms)
    {
        if (!HID::testing_to_pass_HID_from_COM(com.port_))
        {
            console << com.port_ << " " << PASS_FAIL;
        }
        std::this_thread::sleep_for(1s);
    }

    auto hids = UTIL::get_available_hid_devices();
    PRINT::print_all_hid_devices(hids);

    if (hids.empty())
        return;

    std::string scanner_numbers = PRINT::ChooseScannerToProceed();
    hids = UTIL::get_scanners_list_by_regex(hids, scanner_numbers);

    auto firmware_files = UTIL::get_firmware_list();
    PRINT::print_all_firmware_files(firmware_files);
    int number = PRINT::ChooseToProceed(firmware_files.size());
    if (firmware_files[number].second != 0)
    {
        console << FW_CORRUPT;
        logger << FW_NAME << " " << FW_CORRUPT;
        return;
    }
    else
        logger << FW_NAME << ": " << OK;

    for (auto &hid : hids)
    {
        auto firmware = firmware_files[number].first.data();

        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (!HID::testing_to_pass_COM_from_HID(device.ptr))
        {
            console << hid.serial_number_ << HID2COM_FAIL;
            logger << hid.serial_number_ << HID2COM_FAIL;
        }
        if (device.ptr)
            hid_close(device.ptr);
        std::this_thread::sleep_for(3000ms);
        try
        {
            coms = UTIL::get_available_linux_com_ports();
            if (coms.empty())
            {
                console << device.serial_number << ": " << CONN_ABORT;
                logger << device.serial_number << ": " << CONN_ABORT;
                continue;
            }
            s_port.open(coms[0].port_);
            s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
            firmware_parse_pro(firmware);

            int fw_download_start = firmware_download_start(write, read, false);
            if (fw_download_start == 0)
                console << device.serial_number << ": " << FWDL << "...";
            else
            {
                console << device.serial_number << ": " << FWDL << " " << _FAIL_;
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
                }

                if (!s_port.is_open())
                {
                    coms = UTIL::get_available_linux_com_ports();
                    if (coms.empty())
                    {
                        console << device.serial_number << ": " << CONN_ABORT;
                        logger << device.serial_number << ": " << CONN_ABORT;
                        break;
                    }
                    s_port.open(coms[0].port_);
                }

                if (state == DownloadState::SUCCESS || state < DownloadState::SUCCESS)
                {
                    if (state == DownloadState::RECONNECTDEVICE)
                    {
                        logger << "Need reconnect: " << coms[0].port_;
                        if (s_port.is_open())
                        {
                            s_port.close();
                        }
                        std::this_thread::sleep_for(3000ms);
                        coms = UTIL::get_available_linux_com_ports();
                        if (coms.empty())
                        {
                            console << device.serial_number << ": " << CONN_ABORT;
                            logger << device.serial_number << ": " << CONN_ABORT;
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
                            if (state == DownloadState::FAIL_NONEEDUPDATE)
                            {
                                console << device.serial_number << " " << HAS_FW;
                                logger << device.serial_number << " " << HAS_FW;
                            }
                            else
                            {
                                console << device.serial_number << ": " << FW_OK;
                                logger << device.serial_number << ": " << FW_OK;
                            }
                            s_port.close();
                        }
                        break;
                    }
                }
                std::this_thread::sleep_for(500ms);
            } while (persent < 100);
            logger << "Last downloading status: " << state;
        }
        catch (boost::system::system_error &e)
        {
            boost::system::error_code ec = e.code();
            logger << ec.value()
                   << " "
                   << ec.category().name() << " " << ec.message();
            console << EXCEPTION << " For detailed information look into log";
        }
    }
    return;
}