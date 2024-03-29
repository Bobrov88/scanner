#include "print.h"

ConsoleTable PRINT::getTableInitialSetup()
{
    ConsoleTable table;

    ConsoleTable::TableChars chars;
    chars.topDownSimple = '-';
    chars.leftRightSimple = '|';
    chars.centreSeparation = '+';
    chars.topSeparation = '-';
    chars.downSeparation = '-';
    chars.leftSeparation = '|';
    chars.rightSeparation = '|';
    chars.topLeft = '*';
    chars.topRight = '*';
    chars.downLeft = '*';
    chars.downRight = '*';
    table.setTableChars(chars);

    table.setAlignment(samilton::Alignment::centre);
    table.setIndent(1, 1);
    return table;
}

void PRINT::print_all_hid_devices(const std::vector<UTIL::AVAILABLE_HID> &hids)
{
    ConsoleTable table = getTableInitialSetup();
    if (!hids.empty())
    {
        table[0][0] = "#";
        table[0][1] = "VID";
        table[0][2] = "PID";
        table[0][3] = PROD;
        table[0][4] = SN;
        table[0][5] = MODEL;
        table[0][6] = FW_VER;

        int row = 1;
        for (const auto &hid : hids)
        {
            table[row][0] = row;
            table[row][1] = CONVERT::hex_view(hid.vid_);
            table[row][2] = CONVERT::hex_view(hid.pid_);
            table[row][3] = CONVERT::str(hid.product_).c_str();
            table[row][4] = CONVERT::str(hid.serial_number_).c_str();
            hid_device *ptr = hid_open_path(hid.path_);
            try
            {
                auto resp = UTIL::get_firmware_device_name_model(ptr);
                if (!resp.empty())
                {
                    boost::json::value obj = boost::json::parse(resp);
                    table[row][5] = obj.at("deviceName").as_string().c_str();
                    table[row][6] = obj.at("FwVer").as_string().c_str();
                }
                else
                {
                    table[row][5] = table[row][5] = ERR;
                    continue;
                }
            }
            catch (const boost::system::system_error &e)
            {
                boost::system::error_code ec = e.code();
                logger << CONVERT::str(hid.serial_number_).c_str() << ": " << READ_ERROR;
                logger << ec.value()
                       << " "
                       << ec.category().name() << " " << ec.message();
                console << EXCEPTION << " "s << LOOK_TO_LOG;
                table[row][5] = table[row][5] = ERR;
            }
            catch (const std::exception &ex)
            {
                logger << CONVERT::str(hid.serial_number_).c_str() << ": " << READ_ERROR;
                logger << ex.what();
                console << EXCEPTION << " "s << LOOK_TO_LOG;
                table[row][5] = table[row][5] = ERR;
            }
            ++row;
            hid_close(ptr);
        }
    }
    else
    {
        table[0][0] = NO_SCAN;
    }

    console << table;
}

void PRINT::print_software_version(const std::vector<UTIL::AVAILABLE_HID> &hids)
{
    ConsoleTable table = getTableInitialSetup();
    if (!hids.empty())
    {
        table[0][0] = "#";
        table[0][1] = PROD;
        table[0][2] = SN;
        table[0][3] = MODEL;
        table[0][4] = HW_VER;
        table[0][5] = BOOT_VER;
        table[0][6] = FW_VER;
        table[0][7] = LIB_VER;

        int row = 1;
        for (const auto &hid : hids)
        {
            table[row][0] = row;
            table[row][1] = CONVERT::str(hid.product_).c_str();
            table[row][2] = CONVERT::str(hid.serial_number_).c_str();
            hid_device *ptr = hid_open_path(hid.path_);
            try
            {
                auto info = UTIL::get_read_device_info(ptr);
                auto vers = UTIL::split_for_read_device_info(info);
                if (!vers.empty())
                {
                    table[row][3] = vers[0];
                    table[row][4] = vers[1];
                    table[row][5] = vers[2];
                    table[row][6] = vers[3];
                    table[row][7] = vers[4];
                }
                else
                {
                    table[row][3] = table[row][4] = table[row][5] = table[row][6] = table[row][7] = ERR;
                    continue;
                }
            }
            catch (const std::exception &ex)
            {
                logger << CONVERT::str(hid.serial_number_).c_str() << ": " << READ_ERROR;
                logger << ex.what();
                console << EXCEPTION << " "s << LOOK_TO_LOG;
                table[row][3] = table[row][4] = table[row][5] = table[row][6] = table[row][7] = ERR;
            }
            ++row;
            hid_close(ptr);
        }
    }
    else
    {
        table[0][0] = NO_SCAN;
    }
    console << table;
}

void PRINT::print_all_com_linux_devices(const std::vector<UTIL::AVAILABLE_COM> &coms)
{
    ConsoleTable table = getTableInitialSetup();

    if (!coms.empty())
    {
        table[0][0] = "#";
        table[0][1] = COM;
        table[0][2] = PROD;
        table[0][3] = MODEL;
        table[0][4] = SN;
        table[0][5] = FW_VER;

        int row = 1;
        for (const auto &com : coms)
        {
            table[row][0] = row;
            table[row][1] = com.port_;
            // table[row][2] = com.product_;
            // table[row][3] = com.model_;
            // table[row][4] = com.serial_number_;
            // table[row][5] = com.firmware_;
            ++row;
        }
    }
    else
    {
        table[0][0] = NO_SCAN;
    }
    console << table;
}

void PRINT::print_all_json_files(std::vector<std::pair<std::string, std::string>> &json_list)
{
    ConsoleTable table = getTableInitialSetup();

    if (!json_list.empty())
    {
        table[0][0] = "#";
        table[0][1] = JSON_NAME;
        table[0][2] = ST;

        int row = 1;
        for (const auto &[name, status] : json_list)
        {
            table[row][0] = row;
            table[row][1] = name;
            table[row][2] = status;
            ++row;
        }
    }
    else
    {
        table[0][0] = NO_JSON;
    }
    console << table;
}

void PRINT::print_all_firmware_files(std::vector<std::pair<std::string, int>> &firmware_list)
{
    ConsoleTable table = getTableInitialSetup();

    if (!firmware_list.empty())
    {
        table[0][0] = "#";
        table[0][1] = FW_NAME;
        table[0][2] = ST;

        int row = 1;
        for (const auto &[name, status] : firmware_list)
        {
            table[row][0] = row;
            table[row][1] = name;
            (status == 0) ? table[row][2] = OK : table[row][2] = ERR + ": "s + std::to_string(status);
            ++row;
        }
    }
    else
    {
        table[0][0] = NO_FW;
    }
    console << table;
}

void PRINT::download_attention()
{
    ConsoleTable table = getTableInitialSetup();
    table[0][0] = FW_DOWNLOAD_ATTENTION;
    console << table;
}

std::string PRINT::ChooseScannerToProceed() // take this function out of MENU namespace
{
    console << CHOOSE_SCANNER;

    std::string result_string;
    do
    {
        result_string.clear();

        std::cout << "----------->: ";
        std::flush(std::cout);

        std::string input, token;
        std::getline(std::cin, input);

        std::istringstream iss(input);

        while (iss >> token)
        {
            result_string += token;
            result_string += ' ';
        }

    } while (result_string.empty());

    return result_string;
}

int PRINT::ChooseToProceed(size_t amount) // take this function out of MENU namespace
{
    int number;
    while (true)
    {
        std::cout << CHOOSE << ": ";
        std::flush(std::cout);
        if (std::cin >> number)
        {
            if (number > amount || number < 1)
                continue;
            else
                break;
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return number - 1;
}