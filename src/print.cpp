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

void PRINT::print_all_hid_linux_devices(const std::vector<UTIL::AVAILABLE_HID> &hids)
{
    ConsoleTable table = getTableInitialSetup();

    if (!hids.empty())
    {
        table[0][0] = "#";
        table[0][1] = "VID";
        table[0][2] = "PID";
        table[0][3] = "Product";
        table[0][4] = "Serial Number";
        table[0][5] = "Model";
        table[0][6] = "Firmware Version";

        int row = 1;
        for (const auto &hid : hids)
        {
            table[row][0] = row;
            table[row][1] = CONVERT::hex_view(hid.vid_);
            table[row][2] = CONVERT::hex_view(hid.pid_);
            table[row][3] = CONVERT::str(hid.product_);
            table[row][4] = CONVERT::str(hid.serial_number_);
            boost::json::value obj = boost::json::parse(UTIL::get_firmware_device_name_model(hid_open_path(hid.path_)));
            table[row][5] = obj.at("deviceName").as_string().c_str();
            table[row][6] = obj.at("FwVer").as_string().c_str();
            ++row;
        }
    }
    else
    {
        table[0][0] = "Scanners not found";
    }

    std::cout << table;
}

void PRINT::print_all_com_linux_devices(const std::vector<UTIL::AVAILABLE_COM> &coms)
{
    ConsoleTable table = getTableInitialSetup();

    if (!coms.empty())
    {
        table[0][0] = "#";
        table[0][1] = "COM";
        table[0][2] = "Product";
        table[0][3] = "Model";
        table[0][4] = "Serial number";
        table[0][5] = "Firmware Version";

        int row = 1;
        for (const auto &com : coms)
        {
            table[row][0] = row;
            table[row][1] = com.port_;
            table[row][2] = com.product_;
            table[row][3] = com.model_;
            table[row][4] = com.serial_number_;
            table[row][5] = com.firmware_;
            ++row;
        }
    }
    else
    {
        table[0][0] = "Scanners not found";
    }

    std::cout << table;
}

void PRINT::print_all_json_files(std::vector<std::pair<std::string, std::string>> &json_list)
{
    ConsoleTable table = getTableInitialSetup();

    if (!json_list.empty())
    {
        table[0][0] = "#";
        table[0][1] = "Json file name";
        table[0][2] = "Status";

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
        table[0][0] = "Json files not found";
    }

    std::cout << table;
}