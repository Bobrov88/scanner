#include "utility.h"

#ifdef __WINDOWS__
#include "windows.h"
#include "tchar.h"

std::vector<UTIL::AVAILABLE_COM> UTIL::get_available_windows_com_ports()
{
    std::vector<UTIL::AVAILABLE_COM> com_list;
    int r = 0;
    HKEY hkey = NULL;

    r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"), 0, KEY_READ, &hkey);
    if (r != ERROR_SUCCESS)
        return com_list;

    unsigned long CountValues = 0, MaxValueNameLen = 0, MaxValueLen = 0;
    RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &CountValues, &MaxValueNameLen, &MaxValueLen, NULL, NULL);
    ++MaxValueNameLen;
    TCHAR *bufferName = NULL, *bufferData = NULL;
    bufferName = (TCHAR *)malloc(MaxValueNameLen * sizeof(TCHAR));

    if (!bufferName)
    {
        RegCloseKey(hkey);
        return com_list;
    }

    bufferData = (TCHAR *)malloc((MaxValueLen + 1) * sizeof(TCHAR));
    if (!bufferData)
    {
        free(bufferName);
        RegCloseKey(hkey);
        return com_list;
    }

    unsigned long NameLen, type, DataLen;
    for (unsigned int i = 0; i < CountValues; i++)
    {
        NameLen = MaxValueNameLen;
        DataLen = MaxValueLen;
        r = RegEnumValue(hkey, i, bufferName, &NameLen, NULL, &type, (LPBYTE)bufferData, &DataLen);
        if ((r != ERROR_SUCCESS) || (type != REG_SZ))
            continue;
        com_list.push_back({std::string(bufferData)});
    }
    free(bufferName);
    free(bufferData);
    RegCloseKey(hkey);

    return com_list;
}
#endif

ConsoleTable UTIL::getTableInitialSetup()
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

std::vector<UTIL::AVAILABLE_COM> UTIL::get_available_linux_com_ports()
{
    std::vector<UTIL::AVAILABLE_COM> com_ports;

    // namespace fs = std::filesystem;
    // fs::path p("/dev/tty");
    // try
    // {
    //     if (!exists(p))
    //     {
    //         throw std::runtime_error(p.generic_string() + " does not exist");
    //     }
    //     else
    //     {
    //         for (const auto &dir : fs::directory_iterator(p))
    //         {
    //             if (is_symlink(dir.symlink_status()))
    //             {
    //                 fs::path symlink_points_at = read_symlink(dir);
    //                 fs::path canonical_path = fs::canonical(p / symlink_points_at);
    //                 com_ports.push_back({canonical_path.generic_string()});
    //             }
    //         }
    //     }
    // }
    // catch (const fs::filesystem_error &ex)
    // {
    //     std::cout << ex.what() << '\n';
    //     throw ex;
    // }
    // std::sort(com_ports.begin(), com_ports.end(), [](const auto &first, const auto &second)
    //           { return first.data_ < second.data_; });
    return com_ports;
}

std::vector<UTIL::AVAILABLE_HID> UTIL::get_available_hid_devices()
{
    std::vector<UTIL::AVAILABLE_HID> device;
    struct hid_device_info *cur_dev;
    cur_dev = hid_enumerate(0x0, 0x0);
    while (cur_dev)
    {
        if (cur_dev->vendor_id != 0 && cur_dev->product_id != 0)
        {
            device.push_back({cur_dev->path,
                              cur_dev->vendor_id,
                              cur_dev->product_id,
                              cur_dev->serial_number,
                              cur_dev->manufacturer_string,
                              cur_dev->product_string,
                              cur_dev->release_number,
                              cur_dev->interface_number,
                              cur_dev->usage,
                              cur_dev->usage_page});
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(cur_dev);
    if (!device.empty())
    {
        remove_dublicates_of_hid_devices(device);
    }
    return device;
}

void UTIL::remove_dublicates_of_hid_devices(std::vector<AVAILABLE_HID> &hids)
{
    // todo maybe not connected with first try
    // I delete all being connected with failure
    std::vector<AVAILABLE_HID> unique_hids;
    unique_hids.reserve(hids.size());
    for (auto &hid : hids)
    {
        hid_device *handle = hid_open_path(hid.path_);
        if (handle)
        {
            if (HID::testing_connect_for_erasing_duplicates(handle))
            {
                unique_hids.emplace_back(std::move(hid));
            }
        }
        hid_close(handle);
    }
    hids.clear();
    hids.assign(unique_hids.begin(), unique_hids.end());
}

std::vector<UTIL::AVAILABLE_HID> UTIL::detect_all_hid_linux_devices()
{
    std::vector<UTIL::AVAILABLE_HID> hids = get_available_hid_devices();
    ConsoleTable table = getTableInitialSetup();

    if (!hids.empty())
    {
        table[0][0] = "#";
        table[0][1] = "VID";
        table[0][2] = "PID";
        table[0][3] = "Product";
        table[0][4] = "Serial Number";
        table[0][5] = "Model";
        table[0][6] = "FirmwareVersion";
        int row = 1;
        for (const auto &hid : hids)
        {
            table[row][0] = row;
            table[row][1] = hex_view(hid.vid_);
            table[row][2] = hex_view(hid.pid_);
            table[row][3] = str(hid.product_);
            table[row][4] = str(hid.serial_number_);
            boost::json::value obj = boost::json::parse(get_firmware_device_name_model(hid_open_path(hid.path_)));
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
    return hids;
}

void UTIL::detect_all_com_linux_devices()
{
    std::vector<UTIL::AVAILABLE_COM> coms = get_available_linux_com_ports();

    ConsoleTable table = getTableInitialSetup();
    table[0][0] = "#";
    table[0][1] = "COM";

    if (coms.empty())
    {
        table[1][0] = "HID Devices not found\n";
        return;
    }

    int row = 1;
    for (const auto &com : coms)
    {
        table[row][0] = row;
        table[row][1] = com.data_;
        ++row;
    }

    std::cout << table;
}

std::string UTIL::str(const std::wstring &src)
{
    if (src.empty())
        return "";

    std::vector<char> dest;

    try
    {
        for (size_t i = 0; i < src.length(); ++i)
            utf8::append((uint32_t)src[i], std::back_inserter(dest));
    }
    catch (...)
    {
    }

    if (dest.empty())
        return "";
    return std::string(&dest[0], dest.size());
}

std::wstring UTIL::wstr(const std::string &src)
{
    std::wstring dest;
    size_t i = 0;

    if ((src.length() > 3) && (src[0] == (char)0xEF) && (src[1] == (char)0xBB) && (src[2] == (char)0xBF))
        i = 3;

    std::vector<char> srcV(src.length() - i);
    memcpy(&srcV[0], src.c_str() + i, srcV.size());
    std::vector<char>::iterator current = srcV.begin();
    try
    {
        while (current != srcV.end())
            dest.push_back(utf8::next(current, srcV.end()));
    }
    catch (...)
    {
    }
    return dest;
}

std::string UTIL::str(const wchar_t *ws)
{
    std::wstring w(ws);
    return str(w);
}

std::string UTIL::hex_view(const unsigned short number)
{
    std::stringstream str_hex;
    str_hex << "0x" << std::setfill('0') << std::setw(4) << std::hex << number;
    return str_hex.str();
}

std::string UTIL::convert_from_bytes_to_string(std::vector<uint8_t> from)
{
    std::string str;
    size_t size = from.size();
    str.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        str += static_cast<char>(from[i]);
    }
    return str;
}

std::vector<uint8_t> UTIL::read_json_piece(hid_device *handle)
{
    uint8_t response[64] = {0};
    int size_of_read_bytes = hid_read_timeout(handle, response, 64, 100);
    if (size_of_read_bytes <= 0)
    {
        // TODO if result is negative
        return {};
    }
    size_t size_of_json_piece = static_cast<size_t>(response[1]);
    std::vector<uint8_t> json_bytes;
    json_bytes.reserve(size_of_json_piece);
    for (int i = 0; i < size_of_json_piece; ++i)
    {
        json_bytes.push_back(*(&response[5] + i));
    }
    return json_bytes;
}

std::string UTIL::send_command_for_json_response(hid_device *handle)
{
    std::string result;
    uint8_t ch[64] = {0};
    for (int i = 1; i <= 4; ++i)
    {
        SEQ::get_config_command(ch, i);
        hid_write(handle, ch, 64);
        result += read_json_settings(handle);
    }
    merge_json(result);
    return result;
}

std::string UTIL::get_firmware_device_name_model(hid_device *handle)
{
    uint8_t ch[64] = {0};
    SEQ::get_config_command(ch, 2);
    hid_write(handle, ch, 64);
    return read_json_settings(handle);
}

std::string UTIL::read_json_settings(hid_device *handle)
{
    std::vector<uint8_t> result;
    while (true)
    {
        std::vector<uint8_t> tmp = read_json_piece(handle);
        if (tmp.empty())
            break;
        result.insert(result.end(), tmp.begin(), tmp.end());
    }
    return convert_from_bytes_to_string(result);
}

int HID_WRITE(hid_device *handle, uint8_t *c, int size)
{
    int write_result = hid_write(handle, c, size);
    // write to log
    // bytes
    // result with error
    uint8_t r[64] = {0};
    int read_result = hid_read_timeout(handle, r, 64, 100);
    // write to log
    // bytes
    // result with error
    // TODO
    return 0;
}

void UTIL::convert_json_to_bits(const std::string &json)
{
    boost::json::value str = boost::json::parse(json);
    std::vector<uint8_t> bytes;
    std::vector<std::vector<uint8_t>> set_of_bytes;
    bytes.reserve(60);
    std::string incorrect_data;

//     try
//     {
//         {
//             // FLAG 0x0000
//             uint8_t byte = 0;
//             {
//                 const std::string key = "locateLed"s;
//                 std::vector<std::string> variants = {"allwaysOff"s, "getPictureOn"s, "allwaysOn"s};
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b01000000;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b10000000;
//                 else
//                     incorrect_data += UTIL::get_string_possible_data(variants, key);
//             }
//             {
//                 const std::string key = "fillLed"s;
//                 std::vector<std::string> variants = {"allwaysOff"s, "getPictureOn"s, "allwaysOn"s};
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00010000;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b00100000;
//                 else
//                     incorrect_data += UTIL::get_string_possible_data(variants, key);
//             }
//             {
//                 const std::string key = "workMode";
//                 std::vector<std::string> variants = {"keyTrig"s, "edgeTrig"s, "cmdTrig"s, "incuction"s, "noMoveNoRead"s, "continuous"s};
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00000001;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b00000010;
//                 else if (low(tmp) == low(variants[3]))
//                     byte |= 0b00000011;
//                 else if (low(tmp) == low(variants[4]))
//                     byte |= 0b00000100;
//                 else if (low(tmp) == low(variants[5]))
//                     byte |= 0b00000101;
//                 else
//                     incorrect_data += UTIL::get_string_possible_data(variants, key);
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0001
//             uint8_t byte = 0;
//             {
//                 const std::string key = "cmdTrigAck"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b10000000;
//             }
//             {
//                 const std::string key = "muteEnable"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b01000000;
//             }
//             {
//                 const std::string key = "decodeLed"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00100000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "continuousSwitch"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b00010000;
//             }
//             // {
//             // to Chinese
//             //     const std::string key = "BIT 3-2 ???"s;
//             //     bool tmp = str.at(key).as_bool();
//             //     if (tmp)
//             //         byte |= 0b11100000;
//             //     else
//             //         byte |= 0b11110000;
//             // }
//             {
//                 std::vector<std::string> variants = {"passive"s, "active"s, "mimicry"s};
//                 const std::string key = "buzzerType"s;
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00000001;
//                 else if (low(tmp) == low(variants[2]))
//                     // byte |= 0b11111101; //to Chinese
//                     byte |= 0b00000011;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0002
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0003
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             {
//                 const std::string key = "setCodeEnable"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000010;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "printSetCode"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b00000001;
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0004
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0005
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0006
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         // --------from 0x0000 to 0x0006--------------------- //
//         set_of_bytes.push_back(std::move(bytes));
//         bytes.clear();
//         // --------from 0x0000 to 0x0006--------------------- //
//         {
//             // FLAG 0x0009
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x000A
//             uint8_t byte = 0;
//             {
//                 const std::string key = "disableChinese"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b10000000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 std::vector<std::string> variants = {"off"s, "ctrlMode"s, "altMode"s, "notOut"s};
//                 const std::string key = "ctrlCharMode"s;
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00100000;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b01000000;
//                 else if (low(tmp) == low(variants[3]))
//                     // to Chinese
//                     byte |= 0b01111111;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             {
//                 const std::string key = "keypadNumLock"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00010000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "keypadCalcSingal"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00001000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "keyboardLead(Ctrl+Shift+R)"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000001;
//                 else
//                     byte |= 0b00000000;
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x000B
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x000C
//             uint8_t byte = 0;
//             {
//                 byte |= 0b00000000;
//             }
//             {
//                 std::vector<std::string> variants = {"normal"s, "turn"s, "allwaysOn"s, "allwaysOff"s};
//                 const std::string key = "capsLockMode"s;
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00000010;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b00000100;
//                 else if (low(tmp) == low(variants[3]))
//                     byte |= 0b00000110;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             {
//                 std::vector<std::string> variants = {"High"s, "Low"s};
//                 const std::string key = "activeBuzzerWorkLevel"s;
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00000001;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x000D
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             {
//                 const std::string key = "virtualKeyboard"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b01000000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 std::vector<std::string> variants = {"GBK"s, "Unicode"s, "Raw"s, "UTF8"s, "BIG5"s};
//                 const std::string key = "dataOutType"s;
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00010000;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b00100000;
//                 else if (low(tmp) == low(variants[3]))
//                     byte |= 0b00110000;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             {
//                 std::vector<std::string> variants = {"UART-TTL"s, "keyboard"s, "virtualCom"s, "pos"s, "composite"s, "UART-TTL&keyboard"s};
//                 const std::string key = "outMode"s;
//                 std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00000001;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b00000011;
//                 else if (low(tmp) == low(variants[3]))
//                     byte |= 0b00000100;
//                 else if (low(tmp) == low(variants[4]))
//                     byte |= 0b00000101;
//                 else if (low(tmp) == low(variants[5]))
//                     byte |= 0b00000110;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x000E
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "startBuzzer"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b00001000;
//             }
//             {
//                 const std::string key = "decodeSucessBuzzer"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000100;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "setCodeBuzzer"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b00000010;
//             }
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x000F
//             uint8_t byte = 0;
//             {
//                 const std::string key = "sensitivity1"s;
//                 const uint8_t tmp = static_cast<uint8_t>(str.at(key).as_int64());
//                 if (tmp < 0 || tmp > 255)
//                 {
//                     incorrect_data += get_uint8_t_possible_data(key, 0, 255);
//                 }
//                 else
//                 {
//                     byte |= tmp;
//                 }
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0010
//             uint8_t byte = 0;
//             {
//                 const std::string key = "sensitivity2"s;
//                 const uint8_t tmp = static_cast<uint8_t>(str.at(key).as_int64());
//                 if (tmp < 0 || tmp > 255)
//                 {
//                     incorrect_data += get_uint8_t_possible_data(key, 0, 255);
//                 }
//                 else
//                 {
//                     byte |= tmp;
//                 }
//             }
//             bytes.push_back(byte);
//         }
//         // --------from 0x0009 to 0x0010--------------------- //
//         set_of_bytes.push_back(std::move(bytes));
//         bytes.clear();
//         // --------from 0x0009 to 0x0010--------------------- //
//         {
//             // FLAG 0-x0013
//             uint8_t byte = 0;
//             {
//                 const std::string key = "sameReadDelayState"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b10000000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "sameReadDelay"s;
//                 const uint8_t tmp = static_cast<uint8_t>(str.at(key).as_int64() / 100);
//                 if (tmp < 0 || tmp > 127)
//                 {
//                     incorrect_data += get_uint8_t_possible_data(key, 0, 127);
//                 }
//                 else
//                 {
//                     byte |= tmp;
//                 }
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0014
//             uint8_t byte = 0;
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0015
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 bytes |= 0b00000000;
//             }
//             {
//                 const std::string key = "motorEnabled"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000100;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 // to Chinese
//             }
//             {
//                 // Reserved
//                 bytes |= 0b00000000;
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0016
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "reflectProcess"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00010000;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "reinforcedRead"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000001;
//                 else
//                     byte |= 0b00000000;
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0017
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 // to Chinese
//             }
//             {
//                 const std::string key = "code128Lead"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000100;
//                 else
//                     byte |= 0b00000000;
//             }
//             {
//                 const std::string key = "urlCodeReadEnable"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000000;
//                 else
//                     byte |= 0b00000010;
//             }
//             {
//                 const std::string key = "GSExchangeEanble"s;
//                 bool tmp = str.at(key).as_bool();
//                 if (tmp)
//                     byte |= 0b00000001;
//                 else
//                     byte |= 0b00000000;
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x0018
//             uint8_t byte = 0;
//             {
//                 std::vector<std::string> variants = {"1D"s};
//                 const std::string key = "GSExchangeChar"s;
//                 const std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00011101;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             bytes.push_back(byte);
//         }
//         // --------from 0x0013 to 0x0018--------------------- //
//         set_of_bytes.push_back(std::move(bytes));
//         bytes.clear();
//         // --------from 0x0013 to 0x0018--------------------- //
//         {
//             // FLAG 0x002A, 0x002B
//             // Ask Chinese about order of bytes
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 // to Chinese
//             }
//             {
//                 // to Chinese
//             }
//             bytes.push_back(byte);
//         }
//         {
//             // FLAG 0x002C
//             uint8_t byte = 0;
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             {
//                 std::vector<std::string> variants = {"default"s, "allDisable"s, "allEnable"s};
//                 const std::string key = "decodeConfig"s;
//                 const std::string tmp = str.at(key).as_string().c_str();
//                 if (low(tmp) == low(variants[0]))
//                     byte |= 0b00000000;
//                 else if (low(tmp) == low(variants[1]))
//                     byte |= 0b00000010;
//                 else if (low(tmp) == low(variants[2]))
//                     byte |= 0b00000100;
//                 else
//                     incorrect_data += get_string_possible_data(variants, key);
//             }
//             {
//                 // Reserved
//                 byte |= 0b00000000;
//             }
//             bytes.push_back(byte);
//         }
        auto set_byte_if_key_uint_byte = [&str](uint8_t& byte, const std::string& key)  {
            auto value = str.at(key).as_uint64();
            if(value < 0 || value > 255)    {
                throw std::string{key + "incorrect value, must be between 0 and 255"};
            }
            else    {
                byte =  std::static_cast<uint8_t> ( value );
            }
        };

        auto set_bit_if_key_bool_true = [&str](uint8_t& byte, uint8_t bit_number, const std::string& key){
            if( str.at(key).as_bool() ) {
                switch (bit_number)
                {
                    case 0:
                        byte |= 0b00000001;
                        break;
                    case 1:
                        byte |= 0b00000010;
                        break;
                    case 2:
                        byte |= 0b00000100;
                        break;
                    case 3:
                        byte |= 0b00001000;
                        break;
                    case 4:
                        byte |= 0b00010000;
                        break;
                    case 5:
                        byte |= 0b00100000;
                        break;
                    case 6:
                        byte |= 0b01000000;
                        break;
                    case 7:
                        byte |= 0b10000000;
                        break;
                }
            }
            else byte |= 0b00000000;
        };


        //---------------------FROM 0x0060 to 0x00AE -----------------//
        //-----EAN13
        // FLAG 0x002E
        {
            uint8_t byte = 0;
            {
                const std::string key = "EAN13Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "EAN13ParityBitOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "EAN13ToISSN"s;
                set_bit_if_key_bool_true(byte,2,key);
            }
            {
                const std::string key = "EAN13ToISBN"s;
                set_bit_if_key_bool_true(byte,4,key);
            }
            {
                const std::string key = "EAN13ExtraOut"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "EAN13Extra2Bits"s;
                set_bit_if_key_bool_true(byte,6,key);
            }           
            {
                const std::string key = "EAN13Extra5Bits"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }

        //-----EAN8
        // FLAG 0x002F
        {
            uint8_t byte = 0;
            {
                const std::string key = "EAN8Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "EAN8ParityBitOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "EAN8ExtraOut"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "EAN8Extra2Bits"s;
                set_bit_if_key_bool_true(byte,6,key);
            }           
            {
                const std::string key = "EAN8Extra5Bits"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }

        //-----UPCA
        // FLAG 0x0030
        {
            uint8_t byte = 0;
            {
                const std::string key = "UPCAEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "UPCAParityBitOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "UPCAToEAN13"s;
                set_bit_if_key_bool_true(byte,4,key);
            }
            {
                const std::string key = "UPCAExtraOut"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "UPCAExtra2Bits"s;
                set_bit_if_key_bool_true(byte,6,key);
            }           
            {
                const std::string key = "UPCAExtra5Bits"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }

        //-----UPCE0
        // FLAG 0x0031
        {
            uint8_t byte = 0;
            {
                const std::string key = "UPCE0Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "UPCE0ParityBitOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "UPCE0ExtraOut"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "UPCE0Extra2Bits"s;
                set_bit_if_key_bool_true(byte,6,key);
            }           
            {
                const std::string key = "UPCE0Extra5Bits"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }

        //-----UPCE1
        // FLAG 0x0032
        {
            uint8_t byte = 0;
            {
                const std::string key = "UPCE1Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "UPCE1ParityBitOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "UPCE1ExtraOut"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "UPCE1Extra2Bits"s;
                set_bit_if_key_bool_true(byte,6,key);
            }           
            {
                const std::string key = "UPCE1Extra5Bits"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }

        //-----code128
        // FLAG 0x0033
        {
            uint8_t byte = 0;
            {
                const std::string key = "code128Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "code128GS1AIBracket"s;
                set_bit_if_key_bool_true(byte,6,key);
                 
            }
            bytes.push_back(byte);
        }   
        // FLAG 0x0034
        {
            uint8_t byte = 0;
            const std::string key = "code128MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x0035
        {
            uint8_t byte = 0;
            const std::string key = "code128MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //-----code39
        // FLAG 0x0036
        {
            uint8_t byte = 0;
            {
                const std::string key = "code39Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "code39ParityOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "code39ParityProcess"s;
                set_bit_if_key_bool_true(byte,2,key);
            }           
            {
                const std::string key = "code39FullASCII"s;
                set_bit_if_key_bool_true(byte,3,key);
            }
            {
                const std::string key = "code39ToCode32"s;
                set_bit_if_key_bool_true(byte,4,key);
            }
            {
                const std::string key = "code39ToCode32Lead"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "code39Head"s;
                set_bit_if_key_bool_true(byte,6,key);
            }           
            {
                const std::string key = "code39Tail"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }  
        // FLAG 0x0037
        {
            uint8_t byte = 0;
            const std::string key = "code39MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x0038
        {
            uint8_t byte = 0;
            const std::string key = "code39MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //-----code93
        // FLAG 0x0039
        {
            uint8_t byte = 0;
            const std::string key = "code93Enable"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);             
        }   
        // FLAG 0x003A
        {
            uint8_t byte = 0;
            const std::string key = "code93MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x003B
        {
            uint8_t byte = 0;
            const std::string key = "code93MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //-----codeBar
        // FLAG 0x003C
        {
            uint8_t byte = 0;
            {
                const std::string key = "codeBarEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "codeBarOutHead/Tail"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                std::vector<std::string> variants = {
                    "NotProcess",
                    "Mod16",
                    "Mod10",
                    "double"    };
                const std::string key = "codeBarParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (low(tmp) == low(variants[0]))
                    byte |= 0b00000000;
                else if (low(tmp) == low(variants[1]))
                    byte |= 0b00000100;
                else if (low(tmp) == low(variants[2]))
                    byte |= 0b00001000;
                else if (low(tmp) == low(variants[3]))
                    byte |= 0b00001100;
                else
                    incorrect_data += get_string_possible_data(variants, key);                
            }            
            {
                const std::string key = "codeBarParityOut"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }   
        // FLAG 0x003D
        {
            uint8_t byte = 0;
            const std::string key = "codeBarMessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x003E
        {
            uint8_t byte = 0;
            const std::string key = "codeBarMessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        // FLAG 0x003F
        {
            uint8_t byte = 0;
            {
                const std::string key = "QREnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "QRMode1Switch"s;
                set_bit_if_key_bool_true(byte,5,key);
            }
            {
                const std::string key = "QRGS1AIRBracketOut"s;
                set_bit_if_key_bool_true(byte,6,key);
            }
            bytes.push_back(byte);             
        }

        //-----interleaved2of5
        // FLAG 0x0040
        {
            uint8_t byte = 0;
            {
                const std::string key = "interleaved2of5Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "interleaved2of5ParityOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "interleaved2of5ParityProcess"s;
                set_bit_if_key_bool_true(byte,2,key);
            }
            bytes.push_back(byte);             
        }   
        // FLAG 0x0041
        {
            uint8_t byte = 0;
            const std::string key = "interleaved2of5MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x0042
        {
            uint8_t byte = 0;
            const std::string key = "interleaved2of5MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }           
        //-----industrial25
        // FLAG 0x0043
        {
            uint8_t byte = 0;
            {
                const std::string key = "industrial25Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "industrial25ParityOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "industrial25ParityProcess"s;
                set_bit_if_key_bool_true(byte,2,key);
            }
            bytes.push_back(byte);             
        }   
        // FLAG 0x0044
        {
            uint8_t byte = 0;
            const std::string key = "industrial25MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x0045
        {
            uint8_t byte = 0;
            const std::string key = "industrial25MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }         

        //-----matrix2
        // FLAG 0x0046
        {
            uint8_t byte = 0;
            {
                const std::string key = "matrix2of5Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "matrix2of5ParityOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "matrix2of5ParityProcess"s;
                set_bit_if_key_bool_true(byte,2,key);
            }
            bytes.push_back(byte);             
        }   
        // FLAG 0x0047
        {
            uint8_t byte = 0;
            const std::string key = "matrix2of5MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x0048
        {
            uint8_t byte = 0;
            const std::string key = "matrix2of5MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //----code11
        // FLAG 0x0049
        {
            uint8_t byte = 0;
            {
                const std::string key = "code11Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "code11ParityOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "code11ParityProcess"s;
                set_bit_if_key_bool_true(byte,2,key);
            }
            bytes.push_back(byte);             
        }   
        // FLAG 0x004A
        {
            uint8_t byte = 0;
            const std::string key = "code11MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x004B
        {
            uint8_t byte = 0;
            const std::string key = "code11MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //MSI
        // FLAG 0x004C
        {
            uint8_t byte = 0;
            {
                const std::string key = "MSIEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "MSIParityOut"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "MSIParityProcess"s;
                set_bit_if_key_bool_true(byte,2,key);
            }
            bytes.push_back(byte);             
        }         
        // FLAG 0x004D
        {
            uint8_t byte = 0;
            const std::string key = "MSIMessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x004E
        {
            uint8_t byte = 0;
            const std::string key = "MSIMessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //------GS1Databar
        // FLAG 0x004F
        {
            uint8_t byte = 0;
            {
                const std::string key = "GS1DatabarEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            // {
            //     const std::string key = "???????"s;
            //     set_bit_if_key_bool_true(byte,1,key);
            // }
            {
                const std::string key = "GS1DatabarAIBracket"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }   
        // FLAG 0x0050
        {
            uint8_t byte = 0;
            {
                const std::string key = "GS1DatabarLimitedEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            // {
            //     const std::string key = "???????"s;
            //     set_bit_if_key_bool_true(byte,1,key);
            // }
            {
                const std::string key = "GS1DatabarLimitedAIBracket"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }        
        // FLAG 0x0051
        {
            uint8_t byte = 0;
            {
                const std::string key = "GS1DatabarExpansionEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            // {
            //     const std::string key = "???????"s;
            //     set_bit_if_key_bool_true(byte,1,key);
            // }
            {
                const std::string key = "GS1DatabarExpansionAIBracket"s;
                set_bit_if_key_bool_true(byte,7,key);
            }
            bytes.push_back(byte);             
        }
        // FLAG 0x0052
        {
            uint8_t byte = 0;
            const std::string key = "GS1DatabarExpansionMessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x0053
        {
            uint8_t byte = 0;
            const std::string key = "GS1DatabarExpansionMessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //DM
        // FLAG 0x0054
        {
            uint8_t byte = 0;
            {
                const std::string key = "DMEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "DMMultiCodeEnable"s;
                set_bit_if_key_bool_true(byte,1,key);
            }
            {
                const std::string key = "DMAIBracketOut"s;
                set_bit_if_key_bool_true(byte,6,key);
            }
            bytes.push_back(byte);             
        }
        // FLAG 0x0055
        {
            uint8_t byte = 0;
            const std::string key = "PDF417Enable"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);            
        }
        // FLAG 0x0056
        {
            uint8_t byte = 0;
            const std::string key = "HanXinEnable"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);            
        }        
        // FLAG 0x0057
        {
            uint8_t byte = 0;
            const std::string key = "MicroPDF417Enable"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);            
        }
        // FLAG 0x0058
        {
            uint8_t byte = 0;
            const std::string key = "MicroQREnable"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);            
        }
        // FLAG 0x0059
        {
            uint8_t byte = 0;
            const std::string key = "MaxiCodeEnable"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);            
        }
        // FLAG 0x005A
        {
            uint8_t byte = 0;
            {
                const std::string key = "AztecEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "AztecAIBracketOut"s;
                set_bit_if_key_bool_true(byte,6,key);
            }
            bytes.push_back(byte);            
        }

        //standart25
        // FLAG 0x005B
        {
            uint8_t byte = 0;
            {
                {
                    const std::string key = "standard25Enable"s;
                    set_bit_if_key_bool_true(byte,0,key);
                }
                {
                    const std::string key = "standard25ParityOut"s;
                    set_bit_if_key_bool_true(byte,1,key);
                }
                {
                    const std::string key = "standard25ParityMod10"s;
                    set_bit_if_key_bool_true(byte,2,key);
                }
            }
            bytes.push_back(byte);            
        }
        // FLAG 0x005C
        {
            uint8_t byte = 0;
            const std::string key = "standard25MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        // FLAG 0x005D
        {
            uint8_t byte = 0;
            const std::string key = "standard25MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }

        //GS1
        //FLAG 0x005E
        {
            uint8_t byte = 0;
            {
                {
                    const std::string key = "GS1CompositeEnable"s;
                    set_bit_if_key_bool_true(byte,0,key);
                }
                {
                    const std::string key = "GS1CompositeAIBracketOut"s;
                    set_bit_if_key_bool_true(byte,6,key);
                }
            }
            bytes.push_back(byte);            
        }

        //GM
        //FLAG 0x005F
        {
            uint8_t byte = 0;
            {
                const std::string key = "GMEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                const std::string key = "GMAIBracketOut"s;
                set_bit_if_key_bool_true(byte,6,key);   
            }
            bytes.push_back(byte);
        }
                        
        // FLAG 0x0060
        {
            uint8_t byte = 0;
            const std::string key = "TTL&VCOMOutProtocol"s;
            set_bit_if_key_bool_true(byte,0,key);
            bytes.push_back(byte);
        }
        // FLAG 0x0061
        {   
            uint8_t byte = 0;
            {
                std::vector<std::string> variants = {
                    "U.S.", "Czech Republic", "France", "Germany", "Hungary",
                    "Italy", "Japan", "Spain", "Turkey Q", "Turkey F",
                    "Latin America (Mexico)", "Vietnam", "France (Switzerland)", "Germany (Switzerland)",
                    "Italy (Switzerland)", "Russian UTF8", "Russian Windows1251", "Russian KOI8-R",
                    "Finland", "Sweden", "Denmark", "Written Norway", "Iceland", "Arab", "Slovenia",
                    "Croatia", "Albania", "Serbian (Latin)", "Serbian (Cyril)", "Netherlands",
                    "Estonia", "Lithuania", "Ireland", "Faroese", "Portuguese", "Portugal (Brazil)"
                };
                const std::string key = "keyboardLayout"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (low(tmp) == low(variants[0]))
                    byte = 0x00;
                else if (low(tmp) == low(variants[1]))
                    byte = 0x01;
                else if (low(tmp) == low(variants[2]))
                    byte = 0x02;
                else if (low(tmp) == low(variants[3]))
                    byte = 0x03;
                else if (low(tmp) == low(variants[4]))
                    byte = 0x04;
                else if (low(tmp) == low(variants[5]))
                    byte = 0x05;
                else if (low(tmp) == low(variants[6]))
                    byte = 0x06; 
                else if (low(tmp) == low(variants[7]))
                    byte = 0x07;
                else if (low(tmp) == low(variants[8]))
                    byte = 0x08; 
                else if (low(tmp) == low(variants[9]))
                    byte = 0x09;
                else if (low(tmp) == low(variants[10]))
                    byte = 0x0A; 
                else if (low(tmp) == low(variants[11]))
                    byte = 0x0B;
                else if (low(tmp) == low(variants[12]))
                    byte = 0x0C;                
                else if (low(tmp) == low(variants[13]))
                    byte = 0x0D;
                else if (low(tmp) == low(variants[14]))
                    byte = 0x0E;
                else if (low(tmp) == low(variants[15]))
                    byte = 0x0F;
                else if (low(tmp) == low(variants[16]))
                    byte = 0x10;
                else if (low(tmp) == low(variants[17]))
                    byte = 0x11;
                else if (low(tmp) == low(variants[18]))
                    byte = 0x12;
                else if (low(tmp) == low(variants[19]))
                    byte = 0x13;
                else if (low(tmp) == low(variants[20]))
                    byte = 0x14;
                else if (low(tmp) == low(variants[21]))
                    byte = 0x15;
                else if (low(tmp) == low(variants[22]))
                    byte = 0x16;
                else if (low(tmp) == low(variants[23]))
                    byte = 0x17;
                else if (low(tmp) == low(variants[24]))
                    byte = 0x18;
                else if (low(tmp) == low(variants[25]))
                    byte = 0x19;
                else if (low(tmp) == low(variants[26]))
                    byte = 0x1A;
                else if (low(tmp) == low(variants[27]))
                    byte = 0x1B;
                else if (low(tmp) == low(variants[28]))
                    byte = 0x1C;
                else if (low(tmp) == low(variants[29]))
                    byte = 0x1D;
                else if (low(tmp) == low(variants[30]))
                    byte = 0x1E;
                else if (low(tmp) == low(variants[31]))
                    byte = 0x1F;
                else if (low(tmp) == low(variants[32]))
                    byte = 0x20;
                else if (low(tmp) == low(variants[33]))
                    byte = 0x21;                                       
                else if (low(tmp) == low(variants[34]))
                    byte = 0x22; 
                else if (low(tmp) == low(variants[35]))
                    byte = 0x23;                                                                                                   
                else
                    incorrect_data += get_string_possible_data(variants, key);                
            }
            bytes.push_back(byte);
        }

        //---------------------FROM 0x0060 to 0x00AE -----------------//


        //---------------------FROM 0x00B0 to 0x00B4 -----------------//
        // FLAG 0x00B0
        {   
            uint8_t byte = 0;
            {
                std::vector<std::string> variants = {
                    "SendAll",
                    "SendFirstNBytes",
                    "SendTailNBytes",
                    "SendMiddleNBytes"};
                const std::string key = "dataCutSetOut"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (low(tmp) == low(variants[0]))
                    byte |= 0b00000000;
                else if (low(tmp) == low(variants[1]))
                    byte |= 0b00000001;
                else if (low(tmp) == low(variants[2]))
                    byte |= 0b00000010;
                else if (low(tmp) == low(variants[3]))
                    byte |= 0b00000011;
                else
                    incorrect_data += get_string_possible_data(variants, key);                
            }
            bytes.push_back(byte);
        } 
        }
        // FLAG 0x00B1
        {
            uint8_t byte = 0;
            {
                const std::string key = "dataCutBeforeLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        // FLAG 0x00B2
        {
            uint8_t byte = 0;
            {
                const std::string key = "dataCutAfterLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }

        // FLAG 0x00B3
        {
            uint8_t byte = 0;
            {
                const std::string key = "dataCutCenterLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);    
        }

        // FLAG 0x00B4
        {
            uint8_t byte = 0;
            {
                const std::string key = "AIMPrefixEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            {
                std::vector<std::string> variants = {
                    "codeID+AIM+custom",
                    "AIM+codeID+custom",
                    "AIM+custom+codeID",
                    "codeID+custom+AIM",
                    "custom+codeID+AIM",
                    "custom+AIM+codeID" };
                const std::string key = "AIMPrefixType"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (low(tmp) == low(variants[0]))
                    byte |= 0b00000000;
                else if (low(tmp) == low(variants[1]))
                    byte |= 0b00000010;
                else if (low(tmp) == low(variants[2]))
                    byte |= 0b00000100;
                else if (low(tmp) == low(variants[3]))
                    byte |= 0b00000110;
                else if (low(tmp) == low(variants[4]))
                    byte |= 0b00001000;
                else if (low(tmp) == low(variants[5]))
                    byte |= 0b00001010;
                else
                    incorrect_data += get_string_possible_data(variants, key);                
            }
            bytes.push_back(byte); 
        }
        set_of_bytes.push_back(std::move(bytes));
        bytes.clear();
        //---------------------FROM 0x00B0 to 0x00B4 -----------------//


        //---------------------FROM 0x00BB to 0x00C6 -----------------// 
        // FLAG 0x00BB
        {
            uint8_t byte = 0;
            {
                {
                    const std::string key = "plesseyEnable"s;
                    set_bit_if_key_bool_true(byte,0,key);
                }
                {
                    const std::string key = "plesseyOutParityBit"s;
                    set_bit_if_key_bool_true(byte,1,key);
                }
                {
                    const std::string key =  "plesseyParityProcess"s;
                    set_bit_if_key_bool_true(byte,2,key);
                }          
            }
            bytes.push_back(byte);
        }
        // FLAG 0x00BC
        {
            uint8_t byte = 0;
            {
                const std::string key = "plesseyMessageMinLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        // FLAG 0x00BD
        {
            uint8_t byte = 0;
            {
                const std::string key = "plesseyMessageMaxLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        //ChinaPost25
        // FLAG 0x00BE
        {
            uint8_t byte = 0;
            {
                {
                    const std::string key = "chinaPost25Enable"s;
                    set_bit_if_key_bool_true(byte,0,key);
                }
                {
                    const std::string key = "chinaPost25OutParityBit"s;
                    set_bit_if_key_bool_true(byte,1,key);
                }
                {
                    const std::string key =  "chinaPost25ParityProcess"s;
                    set_bit_if_key_bool_true(byte,2,key);
                }          
            }
            bytes.push_back(byte);
        }
        // FLAG 0x00BF
        {
            uint8_t byte = 0;
            {
                const std::string key = "chinaPost25MessageMinLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        {
        // FLAG 0x00C0
            uint8_t byte = 0;
            {
                const std::string key = "chinaPost25MessageMaxLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        //Code16
        {
        // FLAG 0x00C1
            uint8_t byte = 0;
            {
                const std::string key = "code16KEnable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            bytes.push_back(byte);
        }        
        {
        // FLAG 0x00C2
            uint8_t byte = 0;
            {
                const std::string key = "code16KMessageMinLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        {
        // FLAG 0x00C3
            uint8_t byte = 0;
            {
                const std::string key = "code16KMessageMaxLen"s;
                set_byte_if_key_uint_byte(byte, key); 
            }
            bytes.push_back(byte);
        }
        
        
        //code 49
        {
        // FLAG 0x00C4
            uint8_t byte = 0;
            {
                const std::string key = "code49Enable"s;
                set_bit_if_key_bool_true(byte,0,key);
            }
            bytes.push_back(byte);
        }        
        {
        // FLAG 0x00C5
            uint8_t byte = 0;
            {
                const std::string key = "code49MessageMinLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        {
        // FLAG 0x00C6
            uint8_t byte = 0;
            {
                const std::string key = "code49MessageMaxLen"s;
                set_byte_if_key_uint_byte
            }
            bytes.push_back(byte);
        }

    }
    catch (...)
    {
        return;
    }
    catch (const std::string& error_message)
    {
        std::cout << error_message << "\n";
    }
    return;
}
}

std::string &UTIL::low(std::string &str)
{
    std::for_each(str.begin(),
                  str.end(),
                  [](auto &ch)
                  { std::tolower(ch); });
    return str;
}

std::string UTIL::get_string_possible_data(const std::vector<std::string> &variants, const std::string &key)
{
    std::string result;
    result += "\nPossible values for ";
    result += key;
    result += ": ";
    for (const auto &str : variants)
    {
        result += str;
        result += " ";
    }
    return result;
}

std::string UTIL::get_bool_possible_data(const std::string &key)
{
    std::string result;
    result += "\nPossible values for ";
    result += key;
    result += ": true false";
    return result;
}

std::string UTIL::get_uint8_t_possible_data(const std::string &key, const uint8_t from, const uint8_t to)
{
    std::string result;
    result += "\nPossible values for ";
    result += key;
    result += ": ";
    result += from;
    result += " - ";
    result += to;
    return result;
}

void UTIL::merge_json(std::string &json)
{
    std::string merged_json;
    merged_json.reserve(json.size());
    for (auto it = json.cbegin(); it != json.cend(); ++it)
    {
        if (*it == '}' && *(it + 1) == '{')
        {
            merged_json.push_back(',');
            it += 2;
        }
        else
            merged_json.push_back(*it);
    }
    json.clear();
    json = merged_json;
}