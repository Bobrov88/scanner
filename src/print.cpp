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

// for (const auto &hid : hids)
//         {
// std::cout<<"\n"<< CONVERT::hex_view(hid.vid_)<<" "<<CONVERT::hex_view(hid.pid_)<<" "<<CONVERT::str(hid.product_)<<" "<<CONVERT::str(hid.serial_number_);
//         }


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
            hid_device* ptr = hid_open_path(hid.path_);
      //      std::cout<<"\n 49";
            boost::json::value obj = boost::json::parse(UTIL::get_firmware_device_name_model(ptr));
     //       std::cout<<"\n 51";
            table[row][5] = obj.at("deviceName").as_string().c_str();
            table[row][6] = obj.at("FwVer").as_string().c_str();
            ++row;
            hid_close(ptr);
        }
    }
    else
    {
        table[0][0] = "Scanners not found";
    }
    std::ostringstream oss;
    oss << table;
    //logger(oss.str());
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
    std::ostringstream oss;
    oss << table;
    //logger(oss.str());
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

void PRINT::print_all_firmware_files(std::vector<std::pair<std::string, int>> &firmware_list)
{
    ConsoleTable table = getTableInitialSetup();

    if (!firmware_list.empty())
    {
        table[0][0] = "#";
        table[0][1] = "Firmware file name";
        table[0][2] = "Status";

        int row = 1;
        for (const auto &[name, status] : firmware_list)
        {
            table[row][0] = row;
            table[row][1] = name;
            (status == 0) ? table[row][2] = "OK"s : table[row][2] = "ERROR: " + std::to_string(status);
            ++row;
        }
    }
    else
    {
        table[0][0] = "Firmware files not found";
    }

    std::cout << table;
}

std::string PRINT::ChooseScannerToProceed() // take this function out of MENU namespace
{
    std::cout << "Enter scanner numbers with a space for selective saving settings.         E.g.: 1 4 5 \n";
    std::cout << "or enter VIDs with a space to save settings from scanner with these VIDs. E.g.: 0x34eb 0x53da\n";
    std::cout << "----------->: ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (std::string s; getline(std::cin>>std::ws, s, ',');) {
        if (s.empty()) { break;}
        std::cout<<s;
    }
    return "";
}

int PRINT::ChooseToProceed(size_t amount) // take this function out of MENU namespace
{
    size_t number;
    while (true)
    {
        std::cout << "Choose file by its number:  ";
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