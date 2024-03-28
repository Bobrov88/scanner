#include "utility.h"

#ifdef __WIN__
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

std::vector<UTIL::AVAILABLE_COM> UTIL::get_available_linux_com_ports()
{
    std::vector<UTIL::AVAILABLE_COM> com_ports;

    fs::path p("/dev/serial/by-id");
    try
    {
        if (exists(p))
        {
            for (const auto &dir : fs::directory_iterator(p))
            {
                if (is_symlink(dir.symlink_status()))
                {
                    auto str = dir.path().generic_string();
                    if (str.find("NEO_FOX_SD") != std::string::npos ||
                        str.find("_040801") != std::string::npos)
                    {
                        fs::path symlink_points_at = read_symlink(dir);
                        fs::path canonical_path = fs::canonical(p / symlink_points_at);
                        com_ports.push_back({canonical_path.generic_string()});
                    }
                }
            }
            std::sort(com_ports.begin(), com_ports.end(), [](const auto &first, const auto &second)
                      { return first.port_ < second.port_; });
            //     remove_com_devices_if_not_scanner(com_ports);
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        console << ex.what();
        throw ex;
    }
    return com_ports;
}

std::vector<UTIL::AVAILABLE_HID> UTIL::list_all_hid()
{
    std::vector<UTIL::AVAILABLE_HID> devices;
    struct hid_device_info *cur_dev;
    cur_dev = hid_enumerate(0x0, 0x0);
    // todo in separate function
    while (cur_dev)
    {
        if (cur_dev->vendor_id != 0 && cur_dev->product_id != 0)
        {
            devices.push_back({cur_dev->path,
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
    return devices;
}

std::vector<UTIL::AVAILABLE_HID> UTIL::get_available_hid_devices()
{
    std::vector<UTIL::AVAILABLE_HID> device = UTIL::list_all_hid();
    if (!device.empty())
    {
        remove_dublicates_of_hid_devices(device);
    }
    return device;
}

void UTIL::remove_dublicates_of_hid_devices(std::vector<AVAILABLE_HID> &hids)
{
    std::vector<AVAILABLE_HID> unique_hids;
    unique_hids.reserve(hids.size());
    for (auto &hid : hids)
    {
        handler device{hid_open_path(hid.path_), hid.path_, CONVERT::str(hid.serial_number_)};
        if (device.ptr)
        {
            if (HID::testing_connect_for_erasing_duplicates(device))
            {
                unique_hids.emplace_back(std::move(hid));
            }
        }
        hid_close(device.ptr);
    }
    hids.clear();
    hids.assign(unique_hids.begin(), unique_hids.end());
}

std::vector<uint8_t> UTIL::read_json_piece(hid_device *handle)
{
    uint8_t response[64] = {0};
    int size_of_read_bytes = 0;
    int attempt = 10;
    while (attempt != 0)
    {
        --attempt;
        size_of_read_bytes = hid_read_timeout(handle, response, 64, 100);
        if (size_of_read_bytes <= 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    size_t size_of_json_piece = static_cast<size_t>(response[1]);
    std::vector<uint8_t> json_bytes;
    json_bytes.reserve(size_of_json_piece);
    for (size_t i = 0; i < size_of_json_piece; ++i)
    {
        json_bytes.push_back(*(&response[5] + i));
    }
    return json_bytes;
}

std::string UTIL::get_full_json_response(hid_device *handle)
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
    int attempt = 10;
    int a = 0;
    while (attempt != 0)
    {
        --attempt;
        a = hid_write(handle, ch, 64);
        if (64 != a)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return read_json_settings(handle);
}

std::string UTIL::get_read_device_info(hid_device *handle)
{
    uint8_t ch[64] = {0};
    SEQ::read_device_info_command(ch);
    int attempt = 10;
    int a = 0;
    while (attempt != 0)
    {
        --attempt;
        a = hid_write(handle, ch, 64);
        if (64 != a)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return read_json_settings(handle); // using json-reading for readDeviceInfo
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
    return CONVERT::convert_from_bytes_to_string(result);
}

std::vector<std::string> UTIL::split_for_read_device_info(const std::string &vers)
{
    std::vector<std::string> datas;
    datas.resize(5);
    datas[0] = vers.substr(vers.find("DeviceName") + 11, std::distance(vers.begin(),
                                                                       std::find(std::next(vers.begin(), vers.find("DeviceName")), vers.end(), '\n')) -
                                                             vers.find("DeviceName") - 11);
    datas[1] = vers.substr(vers.find("HwVer") + 6, std::distance(vers.begin(),
                                                                 std::find(std::next(vers.begin(), vers.find("HwVer")), vers.end(), '\n')) -
                                                       vers.find("HwVer") - 6);
    datas[2] = vers.substr(vers.find("BootVer") + 8, std::distance(vers.begin(),
                                                                   std::find(std::next(vers.begin(), vers.find("BootVer")), vers.end(), '\n')) -
                                                         vers.find("BootVer") - 8);
    datas[3] = vers.substr(vers.find("FwVer") + 6, std::distance(vers.begin(),
                                                                 std::find(std::next(vers.begin(), vers.find("FwVer")), vers.end(), '\n')) -
                                                       vers.find("FwVer") - 6);
    datas[4] = vers.substr(vers.find("LibVer") + 7, std::distance(vers.begin(),
                                                                  std::find(std::next(vers.begin(), vers.find("LibVer")), vers.end(), '\n')) -
                                                        vers.find("LibVer") - 7);
    return datas;
}

int UTIL::HID_WRITE(handler &device, uint8_t *c, int size)
{
    try
    {
#ifdef __WIN__
        logger << "Trying connect " << device.serial_number << " " << get_trimmed_long_HID_path(device.path);
#elif __DEBIAN__ || __CENTOS__
        logger << "Trying connect " << device.serial_number << " " << device.path;
#endif
        int attempt = 10; // define as a system var
        while (attempt != 0)
        {
            if (!device.ptr)
            {
                device.ptr = hid_open_path(device.path.data());
            }
            int write_result = hid_write(device.ptr, c, size);
            --attempt;
            if (write_result < size)
            {
                logger << device.serial_number << " hid error: " << hid_error(device.ptr);
                continue;
            }

            uint8_t r[64] = {0};
            int a = hid_read_timeout(device.ptr, r, 64, 100);
            if (a == -1 || r[0] == 0)
            {
                continue;
            }
            else if (r[5] == 0x02 &&
                     r[6] == 0x00 &&
                     r[7] == 0x00 &&
                     r[8] == 0x01)
            {
                logger << "Successful read with " << (10 - attempt) << " attempt";
                return 0;
            }
            logger << device.serial_number << ": read failed -> " << CONVERT::to_hex(r, 64);
            logger << "Hid error: " << hid_error(device.ptr);
        }
    }
    catch (std::exception &ex)
    {
        console << ex.what();
    }
    return -1;
}

std::vector<std::pair<uint16_t, std::vector<uint8_t>>> UTIL::convert_json_to_bits(const std::string &json)
{
    std::vector<std::pair<uint16_t, std::vector<uint8_t>>> set_of_bytes;
    try
    {
        std::ifstream file(json);
        boost::json::value str = boost::json::parse(get_string_from_source(file));
        std::vector<uint8_t> bytes;
        std::string incorrect_data;

        auto set_bytes_from_symbols = [&str, &incorrect_data](const std::string &key, size_t length)
        {
            std::vector<uint8_t> byte(length, 0);
            try
            {
                std::string tmp = str.at(key).as_string().c_str();
                if (tmp.size() > length)
                {
                    incorrect_data += key + ": " + INCORR_STRING + " "s + std::to_string(length) + " "s + SYM;
                }
                else
                {
                    for (size_t i = 0; i < tmp.size(); ++i)
                    {
                        byte[i] = static_cast<uint8_t>(tmp[i]);
                    }
                }
            }
            catch (const std::out_of_range &ex)
            {
                incorrect_data += NOT_KEY + ": "s + key + "\n";
            }
            return byte;
        };

        auto set_byte_if_key_uint_byte = [&str, &incorrect_data](uint8_t &byte, const std::string &key)
        {
            try
            {
                auto value = str.at(key).to_number<int>();
                if (value < 0 || value > 255)
                {
                    incorrect_data += key + ": " + INCORR_NUM + "\n";
                }
                else
                {
                    byte = static_cast<uint8_t>(value);
                }
            }
            catch (const std::out_of_range &ex)
            {
                incorrect_data += key + ": " + NOT_KEY + "\n";
            }
        };

        auto set_bit_if_key_bool_true = [&str, &incorrect_data](uint8_t &byte, uint8_t bit_number, const std::string &key)
        {
            try
            {
                if (str.at(key).as_bool())
                {
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
                    default:
                        incorrect_data += INCORR_BYTES + " " + V_IN + " "s + key + "\n";
                    }
                }
                else
                    incorrect_data += get_bool_possible_data(key);
            }
            catch (const std::out_of_range &ex)
            {
                incorrect_data += key + ": " + NOT_KEY + "\n";
            }
        };

        {
            // FLAG 0x0000
            uint8_t byte = 0;
            {
                const std::string key = "locateLed"s;
                std::vector<std::string> variants = {"allwaysOff"s, "getPictureOn"s, "allwaysOn"s};
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b01000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b10000000;
                else
                    incorrect_data += UTIL::get_string_possible_data(variants, key);
            }
            {
                const std::string key = "fillLed"s;
                std::vector<std::string> variants = {"allwaysOff"s, "getPictureOn"s, "allwaysOn"s};
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00010000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00100000;
                else
                    incorrect_data += UTIL::get_string_possible_data(variants, key);
            }
            {
                const std::string key = "workMode";
                std::vector<std::string> variants = {"keyTrig"s, "edgeTrig"s, "cmdTrig"s, "incuction"s, "noMoveNoRead"s, "continuous"s};
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000001;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00000010;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b00000011;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[4]))
                    byte |= 0b00000100;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[5]))
                    byte |= 0b00000101;
                else
                    incorrect_data += UTIL::get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0001
            uint8_t byte = 0;
            {
                const std::string key = "cmdTrigAck"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            {
                const std::string key = "muteEnable"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b00000001;
            }
            {
                const std::string key = "decodeLed"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "continuousSwitch"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                std::vector<std::string> variants = {"1.7KHz"s, "2.0KHz"s, "2.7KHz"s, "3.7KHz"s};
                const std::string key = "buzzerFreq"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]) || CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00000100;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b00001000;
                else
                    incorrect_data += UTIL::get_string_possible_data(variants, key);
            }
            {
                std::vector<std::string> variants = {"passive"s, "active"s};
                const std::string key = "buzzerType"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000001;
                else // third parameter "mimicry" is not supported (Chinese answer)
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        // --------from 0x0000 to 0x0001--------------------- //
        set_of_bytes.push_back({0x0000, std::move(bytes)});
        bytes.clear();
        // --------from 0x0000 to 0x0001--------------------- //
        {
            // FLAG 0x0003
            uint8_t byte = 0;
            {
                // Invalid settings (Chinese answer)
            }
            {
                const std::string key = "setCodeEnable"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                const std::string key = "printSetCode"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        // --------from 0x0003 to 0x0003--------------------- //
        set_of_bytes.push_back({0x0003, std::move(bytes)});
        bytes.clear();
        // --------from 0x0003 to 0x0003--------------------- //
        {
            // FLAG 0x0005
            uint8_t byte = 0;
            {
                const std::string key = "readInterval"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>() / 100);
                if (tmp < 0 || tmp > 255)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 25500);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0006
            uint8_t byte = 0;
            {
                const std::string key = "longestReadTime"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>() / 100);
                if (tmp < 0 || tmp > 255)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 25500);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        // --------from 0x0005 to 0x0006--------------------- //
        set_of_bytes.push_back({0x0005, std::move(bytes)});
        bytes.clear();
        // --------from 0x0005 to 0x0006--------------------- //
        {
            // FLAG 0x0009
            uint8_t byte = 0;
            {
                const std::string key = "keyboardKeyDelay"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>());
                if (tmp < 0 || tmp > 63)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 63);
                }
                else
                {
                    byte |= (tmp << 2);
                }
            }
            {
                const std::string key = "pictureTurn"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x000A
            uint8_t byte = 0;
            {
                const std::string key = "disableChinese"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            {
                std::vector<std::string> variants = {"off"s, "ctrlMode"s, "altMode"s /*, "notOut"s (Chinese answer)*/};
                const std::string key = "ctrlCharMode"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00100000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b01000000;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                const std::string key = "keypadNumLock"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                const std::string key = "keypadCalcSingal"s;
                set_bit_if_key_bool_true(byte, 3, key);
            }
            {
                // Reserved 2-1
            }
            {
                const std::string key1 = "keyboardLead(Ctrl+Shift+R)"s;
                set_bit_if_key_bool_true(byte, 0, key1);
                // const std::string key2 = "keyboardLead"s; // different names in other version
                // set_bit_if_key_bool_true(byte, 0, key2);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x000B
            uint8_t byte = 0;
            {
                const std::string key = "buzzerOnTime"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>());
                if (tmp < 0 || tmp > 255)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 255);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x000C
            uint8_t byte = 0;
            {
                // Reserved 7-3;
            }
            {
                std::vector<std::string> variants = {"normal"s, "turn"s, "allwaysOn"s, "allwaysOff"s};
                const std::string key = "capsLockMode"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000010;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00000100;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b00000110;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                std::vector<std::string> variants = {"High"s, "Low"s};
                const std::string key = "activeBuzzerWorkLevel"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000001;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        /*{
            FLAG 0x000D
           uint8_t byte = 0;
           {
                const std::string key = "invoiceModeEnable"s;
                set_bit_if_key_bool_true(byte, 7, key);
           }
            {
                const std::string key = "virtualKeyboard"s;
                set_bit_if_key_bool_true(byte, 6, key);
           }
           {
                Reserved 5
           }
           {
               const std::string key = "codeTypeAutoCheckUtf8"s;
               set_bit_if_key_bool_true(byte, 4, key);
           }
           {
                std::vector<std::string> variants = {"UART-TTL"s, "keyboard"s, "virtualCom"s, "pos"s, "composite"s};
                const std::string key = "outMode"s;
                std::string tmp = str.at(key).as_string().c_str();
                 if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                     byte |= 0b00000000;
                 else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                     byte |= 0b00000001;
                 else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                     byte |= 0b00000011;
                 else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                     byte |= 0b00000100;
                 else if (CONVERT::low(tmp) == CONVERT::low(variants[4]))
                     byte |= 0b00000101;
                 else
                     incorrect_data += get_string_possible_data(variants, key);
           }
                       bytes.push_back(byte);
        }*/
        // Writing this set of flags makes fail
        // --------from 0x0009 to 0x000D--------------------- //
        set_of_bytes.push_back({0x0009, std::move(bytes)});
        bytes.clear();
        // --------from 0x0009 to 0x000D--------------------- //
        {
            // FLAG 0x000E
            uint8_t byte = 0;
            {
                // Reserved 7-4
            }
            {
                const std::string key = "startBuzzer"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b00001000;
            }
            {
                const std::string key = "decodeSucessBuzzer"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            {
                const std::string key = "setCodeBuzzer"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b00000010;
            }
            {
                // Not supported
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x000F
            uint8_t byte = 0;
            {
                const std::string key = "sensitivity1"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>());
                if (tmp < 0 || tmp > 255)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 255);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0010
            uint8_t byte = 0;
            {
                const std::string key = "sensitivity2"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>());
                if (tmp < 0 || tmp > 255)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 255);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        // --------from 0x000E to 0x0010--------------------- //
        set_of_bytes.push_back({0x000E, std::move(bytes)});
        bytes.clear();
        // --------from 0x000E to 0x0010--------------------- //
        {
            // FLAG 0x0013
            uint8_t byte = 0;
            {
                const std::string key = "sameReadDelayEnable"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            {
                const std::string key = "sameReadDelay"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>() / 100);
                if (tmp < 0 || tmp > 127)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 12700);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0014
            uint8_t byte = 0;
            {
                const std::string key = "inductionReadDelay"s;
                const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>() / 100);
                if (tmp < 0 || tmp > 255)
                {
                    incorrect_data += get_uint8_t_possible_data(key, 0, 25500);
                }
                else
                {
                    byte |= tmp;
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0015
            uint8_t byte = 0;
            {
                // Reserved 7-4
            }
            {
                //    const std::string key = "motorEnable"s;
                //    set_bit_if_key_bool_true(byte, 3, key);
            }
            {
                std::vector<std::string> variants = {"Bracket"s, "Handheld"s};
                const std::string key = "placeMode"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000100;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000000;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                const std::string key = "decodeSucessBuzzer"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Reserved 0
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0016
            uint8_t byte = 0;
            {
                // Reserved 7-5
            }
            {
                const std::string key = "reverse"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                // Reserved 3-0
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0017
            uint8_t byte = 0;
            {
                // Reserved 7-5
            }
            {
                // Chinese answer (left to think)
            }
            {
                const std::string key = "QRLead"s;
                set_bit_if_key_bool_true(byte, 3, key);
            }
            {
                const std::string key = "code128Lead"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            {
                const std::string key = "urlCodeReadEnable"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b00000010;
            }
            {
                const std::string key = "GSExchangeEanble"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0018
            uint8_t byte = 0;
            {
                std::vector<std::string> variants = {"1D"s};
                const std::string key = "GSExchangeChar"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00011101;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        // --------from 0x0013 to 0x0018--------------------- //
        set_of_bytes.push_back({0x0013, std::move(bytes)});
        bytes.clear();
        // --------from 0x0013 to 0x0018--------------------- //
        {
            // FLAG 0x001A
            uint8_t byte = 0;
            {
                {
                    // Reserved 7-1
                }
                {
                    const std::string key = "keyContinuous"s;
                    set_bit_if_key_bool_true(byte, 0, key);
                }
            }
            bytes.push_back(byte);
        }
        // --------from 0x001A to 0x001A--------------------- //
        set_of_bytes.push_back({0x001A, std::move(bytes)});
        bytes.clear();
        // --------from 0x001A to 0x001A--------------------- //
        {
            // FLAG 0x001D
            uint8_t byte = 0;
            {
                {
                    std::vector<std::string> variants = {"High"s, "Low"s};
                    const std::string key = "ledWorkLevel"s;
                    std::string tmp = str.at(key).as_string().c_str();
                    if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                        byte |= 0b00000000;
                    else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                        byte |= 0b10000000;
                    else
                        incorrect_data += get_string_possible_data(variants, key);
                }
                {
                    const std::string key = "ledOnTime"s;
                    const uint8_t tmp = static_cast<uint8_t>(str.at(key).to_number<int>() / 100);
                    if (tmp < 0 || tmp > 255)
                    {
                        incorrect_data += get_uint8_t_possible_data(key, 0, 1270);
                    }
                    else
                    {
                        byte |= tmp;
                    }
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x001E
            uint8_t byte = 0;
            {
                {
                    // Reserved 7-3
                }
                {
                    std::vector<std::string> variants = {"GBK"s, "Unicode"s, "Raw"s, "UTF8"s, "BIG5"s};
                    const std::string key = "dataOutType"s;
                    std::string tmp = str.at(key).as_string().c_str();
                    if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                        byte |= 0b00000000;
                    else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                        byte |= 0b00000001;
                    else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                        byte |= 0b00000010;
                    else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                        byte |= 0b00000011;
                    else if (CONVERT::low(tmp) == CONVERT::low(variants[4]))
                        byte |= 0b00000100;
                    else
                        incorrect_data += get_string_possible_data(variants, key);
                }
            }
            bytes.push_back(byte);
        }
        // --------from 0x001D to 0x001E--------------------- //
        set_of_bytes.push_back({0x001D, std::move(bytes)});
        bytes.clear();
        // --------from 0x001D to 0x001E--------------------- //
        {
            // FLAG 0x002A, 0x002B
            uint16_t byte = 0;
            {
                // Reserved 15-13
            }
            {
                const std::string key = "uartBaud"s;
                std::vector<std::string> variants = {"4800"s,
                                                     "9600"s,
                                                     "14400"s,
                                                     "19200"s,
                                                     "38400"s,
                                                     "57600"s,
                                                     "115200"s,
                                                     "230400"s,
                                                     "460800"s,
                                                     "921600"s};
                const uint16_t tmp = static_cast<uint16_t>(str.at(key).to_number<int>());
                if (tmp == std::stoi(variants[0]))
                    byte = 0x7102;
                else if (tmp == std::stoi(variants[1]))
                    byte = 0x3901;
                else if (tmp == std::stoi(variants[2]))
                    byte = 0xD000;
                else if (tmp == std::stoi(variants[3]))
                    byte = 0x9C00;
                else if (tmp == std::stoi(variants[4]))
                    byte = 0x4E00;
                else if (tmp == std::stoi(variants[5]))
                    byte = 0x3400;
                else if (tmp == std::stoi(variants[6]))
                    byte = 0x1A00;
                else if (tmp == std::stoi(variants[7]))
                    byte = 0x0D00;
                else if (tmp == std::stoi(variants[8]))
                    byte = 0x0700;
                else if (tmp == std::stoi(variants[9]))
                    byte = 0x0400;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte >> 8);
            bytes.push_back(byte & 0x00FF);
        }
        {
            // FLAG 0x002C
            uint8_t byte = 0;
            {
                // Reserved 7-3
            }
            {
                std::vector<std::string> variants = {"default"s, "allDisable"s, "allEnable"s};
                const std::string key = "decodeConfig"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000010;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00000100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 0
            }
            bytes.push_back(byte);
        }
        // --------from 0x002A to 0x002C--------------------- //
        set_of_bytes.push_back({0x002A, std::move(bytes)});
        bytes.clear();
        // --------from 0x002A to 0x002C--------------------- //
        {
            // FLAG 0x002E
            //-----EAN13
            uint8_t byte = 0;
            {
                const std::string key = "EAN13Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "EAN13ParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                const std::string key = "EAN13ToISSN"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            {
                const std::string key = "EAN13ToISBN"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                const std::string key = "EAN13ExtraOut"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "EAN13Extra2Bits"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                const std::string key = "EAN13Extra5Bits"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            {
                // Reserved 0
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x002F
            //-----EAN8
            uint8_t byte = 0;
            {
                const std::string key = "EAN8Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "EAN8ParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Reseved 4-2
            }
            {
                const std::string key = "EAN8ExtraOut"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "EAN8Extra2Bits"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                const std::string key = "EAN8Extra5Bits"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0030
            //-----UPCA
            uint8_t byte = 0;
            {
                const std::string key = "UPCAEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "UPCAParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Reserved 3-2
            }
            {
                const std::string key = "UPCAToEAN13"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                const std::string key = "UPCAExtraOut"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "UPCAExtra2Bits"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                const std::string key = "UPCAExtra5Bits"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0031
            //-----UPCE0
            uint8_t byte = 0;
            {
                const std::string key = "UPCE0Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "UPCE0ParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Reserved 4-2
            }
            {
                const std::string key = "UPCE0ExtraOut"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "UPCE0Extra2Bits"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                const std::string key = "UPCE0Extra5Bits"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0032
            //-----UPCE1
            uint8_t byte = 0;
            {
                const std::string key = "UPCE1Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "UPCE1ParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Reserved 4-2
            }
            {
                const std::string key = "UPCE1ExtraOut"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "UPCE1Extra2Bits"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                const std::string key = "UPCE1Extra5Bits"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0033
            //-----code128
            uint8_t byte = 0;
            {
                const std::string key = "code128Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Reserved 5-1
            }
            {
                const std::string key = "code128GS1AIBracket"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                // Reserved 7
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0034
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
        {
            // FLAG 0x0036
            //-----code39
            uint8_t byte = 0;
            {
                const std::string key = "code39Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "code39ParityOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                const std::string key = "code39ParityProcess"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            {
                const std::string key = "code39FullASCII"s;
                set_bit_if_key_bool_true(byte, 3, key);
            }
            {
                const std::string key = "code39ToCode32"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                const std::string key = "code39ToCode32Lead"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "code39Head"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                const std::string key = "code39Tail"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0037
            uint8_t byte = 0;
            const std::string key = "code39MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0038
            uint8_t byte = 0;
            const std::string key = "code39MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0039
            //-----code93
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "code93Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
                bytes.push_back(byte);
            }
        }
        {
            // FLAG 0x003A
            uint8_t byte = 0;
            const std::string key = "code93MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x003B
            uint8_t byte = 0;
            const std::string key = "code93MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x003C
            //-----codeBar
            uint8_t byte = 0;
            {
                const std::string key = "codeBarEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "codeBarOutHead/Tail"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                std::vector<std::string> variants = {
                    "NotProcess",
                    "Mod16",
                    "Mod10",
                    "double"};
                const std::string key = "codeBarParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000100;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00001000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b00001100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 6-4
            }
            {
                const std::string key = "codeBarParityOut"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x003D
            uint8_t byte = 0;
            const std::string key = "codeBarMessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x003E
            uint8_t byte = 0;
            const std::string key = "codeBarMessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x003F
            uint8_t byte = 0;
            {
                const std::string key = "QREnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Reserved 4-1
            }
            {
                const std::string key = "QRMode1Switch"s;
                set_bit_if_key_bool_true(byte, 5, key);
            }
            {
                const std::string key = "QRGS1AIRBracketOut"s;
                set_bit_if_key_bool_true(byte, 6, key);
            }
            {
                // Reserved 7
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0040
            //-----interleaved2of5
            uint8_t byte = 0;
            {
                const std::string key = "interleaved2of5Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
                // true = Mod10
            }
            {
                const std::string key = "interleaved2of5ParityOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                std::vector<std::string> variants = {"None"s, "Mod10"s};
                const std::string key = "interleaved2of5ParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 7-3
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
        {
            // FLAG 0x0043
            //-----industrial25
            uint8_t byte = 0;
            {
                const std::string key = "industrial25Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
                // true = Mod10
            }
            {
                const std::string key = "industrial25ParityOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                std::vector<std::string> variants = {"None"s, "Mod10"s};
                const std::string key = "industrial25ParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 7-3
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0044
            uint8_t byte = 0;
            const std::string key = "industrial25MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0045
            uint8_t byte = 0;
            const std::string key = "industrial25MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0046
            //-----matrix2
            uint8_t byte = 0;
            {
                const std::string key = "matrix2of5Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
                // true = Mod10
            }
            {
                const std::string key = "matrix2of5ParityOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                std::vector<std::string> variants = {"None"s, "Mod10"s};
                const std::string key = "matrix2of5ParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 7-3
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0047
            uint8_t byte = 0;
            const std::string key = "matrix2of5MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0048
            uint8_t byte = 0;
            const std::string key = "matrix2of5MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0049
            //----code11
            uint8_t byte = 0;
            {
                const std::string key = "code11Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "code11ParityOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                std::vector<std::string> variants = {"1bit(C/K)"s, "2bits(C+K)"s};
                const std::string key = "code11ParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 7-3
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x004A
            uint8_t byte = 0;
            const std::string key = "code11MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x004B
            uint8_t byte = 0;
            const std::string key = "code11MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            //  FLAG 0x004C
            // MSI
            uint8_t byte = 0;
            {
                const std::string key = "MSIEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "MSIParityOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                std::vector<std::string> variants = {"singleMod10"s, "doubleMod10"s};
                const std::string key = "MSIParityProcess"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000100;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                // Reserved 7-3
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x004D
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
        {
            // FLAG 0x004F
            //------GS1Databar
            uint8_t byte = 0;
            {
                const std::string key = "GS1DatabarEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Chinese answer (left to think)
            }
            {
                // Reserved 6-2
            }
            {
                const std::string key = "GS1DatabarAIBracket"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0050
            uint8_t byte = 0;
            {
                const std::string key = "GS1DatabarLimitedEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Chinese answer (left to think)
            }
            {
                // Reserved 6-2
            }
            {
                const std::string key = "GS1DatabarLimitedAIBracket"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b10000000;
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0051
            uint8_t byte = 0;
            {
                const std::string key = "GS1DatabarExpansionEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Chinese answer (left to think)
            }
            {
                const std::string key = "GS1DatabarExpansionAIBracket"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b10000000;
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0052
            uint8_t byte = 0;
            const std::string key = "GS1DatabarExpansionMessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0053
            uint8_t byte = 0;
            const std::string key = "GS1DatabarExpansionMessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            //  FLAG 0x0054
            // DM
            uint8_t byte = 0;
            {
                const std::string key = "DMEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "DMMultiCodeEnable"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Reserved 5-2
            }
            {
                // const std::string key1 = "DMAIBracketOut"s;
                // set_bit_if_key_bool_true(byte, 6, key1);
                const std::string key2 = "DMBracketOut"s;
                set_bit_if_key_bool_true(byte, 6, key2); // different names in other version
            }
            {
                // Reserved 7
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0055
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "PDF417Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0056
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "HanXinEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0057
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "MicroPDF417Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0058
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "MicroQREnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0059
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "MaxiCodeEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x005A
            uint8_t byte = 0;
            {
                const std::string key = "AztecEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Reserved 5-1
            }
            {
                const std::string key = "AztecAIBracketOut"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b01000000;
            }
            {
                // Reserved 7
            }
            bytes.push_back(byte);
        }

        {
            // standart25
            //  FLAG 0x005B
            uint8_t byte = 0;
            {
                {
                    const std::string key = "standard25Enable"s;
                    set_bit_if_key_bool_true(byte, 0, key);
                }
                {
                    const std::string key = "standard25ParityOut"s;
                    set_bit_if_key_bool_true(byte, 1, key);
                }
                {
                    const std::string key = "standard25ParityMod10"s;
                    set_bit_if_key_bool_true(byte, 2, key);
                }
                {
                    // Reserved 7-3
                }
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x005C
            uint8_t byte = 0;
            const std::string key = "standard25MessageMinLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // FLAG 0x005D
            uint8_t byte = 0;
            const std::string key = "standard25MessageMaxLen"s;
            set_byte_if_key_uint_byte(byte, key);
            bytes.push_back(byte);
        }
        {
            // GS1
            // FLAG 0x005E
            uint8_t byte = 0;
            {
                {
                    const std::string key = "GS1CompositeEnable"s;
                    set_bit_if_key_bool_true(byte, 0, key);
                }
                {
                    // Reserved 5-1
                }
                {
                    const std::string key = "GS1CompositeAIBracketOut"s;
                    bool tmp = str.at(key).as_bool();
                    if (tmp)
                        byte |= 0b00000000;
                    else
                        byte |= 0b01000000;
                }
                {
                    // Reserved 7
                }
            }
            bytes.push_back(byte);
        }
        {
            // GM
            // FLAG 0x005F
            uint8_t byte = 0;
            {
                const std::string key = "GMEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                // Reserved 5-1
            }
            {
                const std::string key = "GMAIBracketOut"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b01000000;
            }
            {
                // Reserved 7
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0060
            uint8_t byte = 0;
            {
                const std::string key = "TTL&VCOMOutProtocol"s;
                set_bit_if_key_bool_true(byte, 7, key);
            }
            {
                std::vector<std::string> variants = {
                    "CR",
                    "CRLF",
                    "TAB",
                    "None"};
                const std::string key = "suffixType"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00100000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b01000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b01100000;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            {
                const std::string key = "RFEnable"s;
                set_bit_if_key_bool_true(byte, 4, key);
            }
            {
                const std::string key = "prefixEnable"s;
                set_bit_if_key_bool_true(byte, 3, key);
            }
            {
                const std::string key = "codeIDEnable"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            {
                const std::string key = "suffixEnable"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                // Not used (Chinese answer)
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0061
            uint8_t byte = 0;
            {
                std::vector<std::string> variants = {
                    "America", "Czech", "French", "German", "Hungary",
                    "Italy", "Japan", "Spanish", "Turkey_Q", "Turkey_F",
                    "Latin", "Vietnamse", "French-Switzerland", "German-Switzerland",
                    "Italy-Switzerland", "Rassia", "Russian Windows1251", "Russian KOI8-R",
                    "Finland", "Sweden", "Denmark", "Norway", "Iceland", "Arab", "Slovenia",
                    "Croatia", "Alnia", "Serbia-Latin", "Serbia-Cyrillic", "Dutch",
                    "Estonia", "Lithuania", "Irish", "Faeroese", "Portugal", "Portugal-Brazil"};
                const std::string key = "keyboardLayout"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte = 0x00;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte = 0x01;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte = 0x02;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte = 0x03;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[4]))
                    byte = 0x04;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[5]))
                    byte = 0x05;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[6]))
                    byte = 0x06; // Japan not in json, in case of emergency
                else if (CONVERT::low(tmp) == CONVERT::low(variants[7]))
                    byte = 0x07;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[8]))
                    byte = 0x08;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[9]))
                    byte = 0x09;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[10]))
                    byte = 0x0A;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[11]))
                    byte = 0x0B;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[12]))
                    byte = 0x0C;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[13]))
                    byte = 0x0D;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[14]))
                    byte = 0x0E;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[15]))
                    byte = 0x0F;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[16]))
                    byte = 0x10;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[17]))
                    byte = 0x11;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[18]))
                    byte = 0x12;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[19]))
                    byte = 0x13;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[20]))
                    byte = 0x14;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[21]))
                    byte = 0x15;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[22]))
                    byte = 0x16;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[23]))
                    byte = 0x17;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[24]))
                    byte = 0x18;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[25]))
                    byte = 0x19;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[26]))
                    byte = 0x1A;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[27]))
                    byte = 0x1B;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[28]))
                    byte = 0x1C;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[29]))
                    byte = 0x1D;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[30]))
                    byte = 0x1E;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[31]))
                    byte = 0x1F;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[32]))
                    byte = 0x20;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[33]))
                    byte = 0x21;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[34]))
                    byte = 0x22;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[35]))
                    byte = 0x23;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        // --------from 0x002E to 0x0061--------------------- //
        set_of_bytes.push_back({0x002E, std::move(bytes)});
        bytes.clear();
        // --------from 0x002E to 0x0061--------------------- //
        {
            // FLAG 0x0063-0x0071
            const std::string key = "prefix"s;
            auto byte = set_bytes_from_symbols(key, 15);
            std::copy(byte.begin(), byte.end(), std::back_inserter(bytes));
        }
        {
            // FLAG 0x0072-0x0080
            const std::string key = "suffix"s;
            auto byte = set_bytes_from_symbols(key, 15);
            std::copy(byte.begin(), byte.end(), std::back_inserter(bytes));
        }
        // --------from 0x0063 to 0x0080--------------------- //
        set_of_bytes.push_back({0x0063, std::move(bytes)});
        bytes.clear();
        // --------from 0x0063 to 0x0080--------------------- //
        {
            // FLAG 0x0082-0x0090
            const std::string key = "RFInfo"s;
            auto byte = set_bytes_from_symbols(key, 15);
            std::copy(byte.begin(), byte.end(), std::back_inserter(bytes));
        }
        {
            // FLAG 0x0082-0x0090
            const std::string key = "codeID"s;
            auto byte = set_bytes_from_symbols(key, 31);
            std::copy(byte.begin(), byte.end(), std::back_inserter(bytes));
        }
        // --------from 0x0082 to 0x00AF--------------------- //
        set_of_bytes.push_back({0x0082, std::move(bytes)});
        bytes.clear();
        // --------from 0x0082 to 0x00AF--------------------- //
        {
            // FLAG 0x00B0
            uint8_t byte = 0;
            {
                {
                    // Reserved 7-1
                }
                std::vector<std::string> variants = {
                    "SendAll",
                    "SendFirstNBytes",
                    "SendTailNBytes",
                    "SendMiddleNBytes"};
                const std::string key = "dataCutSetOut"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000001;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00000010;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b00000011;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x00B1
            uint8_t byte = 0;
            {
                const std::string key = "dataCutBeforeLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x00B2
            uint8_t byte = 0;
            {
                const std::string key = "dataCutAfterLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        //---------------------FROM 0x00B0 to 0x00B2 -----------------//
        set_of_bytes.push_back({0x00B0, std::move(bytes)});
        bytes.clear();
        //---------------------FROM 0x00B0 to 0x00B2 -----------------//
        // 0x00B3 is not set according to Chinese answer
        {
            // FLAG 0x00B4
            uint8_t byte = 0;
            {
                // Reserved 7-4
            }
            {
                const std::string key = "AIMPrefixEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                std::vector<std::string> variants = {
                    "codeID+AIM+custom",
                    "AIM+codeID+custom",
                    "AIM+custom+codeID",
                    "codeID+custom+AIM",
                    "custom+codeID+AIM",
                    "custom+AIM+codeID"};
                const std::string key = "AIMPrefixType"s;
                std::string tmp = str.at(key).as_string().c_str();
                if (CONVERT::low(tmp) == CONVERT::low(variants[0]))
                    byte |= 0b00000000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[1]))
                    byte |= 0b00000010;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[2]))
                    byte |= 0b00000100;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[3]))
                    byte |= 0b00000110;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[4]))
                    byte |= 0b00001000;
                else if (CONVERT::low(tmp) == CONVERT::low(variants[5]))
                    byte |= 0b00001010;
                else
                    incorrect_data += get_string_possible_data(variants, key);
            }
            bytes.push_back(byte);
        }
        //---------------------FROM 0x00B4 to 0x00B4 -----------------//
        set_of_bytes.push_back({0x00B4, std::move(bytes)});
        bytes.clear();
        //---------------------FROM 0x00B4 to 0x00B4 -----------------//
        {
            // FLAG 0x00BB
            uint8_t byte = 0;
            {
                // Reserved 7-3
            }
            {
                const std::string key = "plesseyEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "plesseyParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                const std::string key = "plesseyParityProcess"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x00BC
            uint8_t byte = 0;
            {
                const std::string key = "plesseyMessageMinLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x00BD
            uint8_t byte = 0;
            {
                const std::string key = "plesseyMessageMaxLen"s;
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        {
            // ChinaPost25
            //  FLAG 0x00BE
            uint8_t byte = 0;
            {
                // Reserved 7-3
            }
            {
                const std::string key = "chinaPost25Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
            }
            {
                const std::string key = "chinaPost25ParityBitOut"s;
                set_bit_if_key_bool_true(byte, 1, key);
            }
            {
                const std::string key = "chinaPost25ParityProcess"s;
                set_bit_if_key_bool_true(byte, 2, key);
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x00BF
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
        // Code16
        {
            // FLAG 0x00C1
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "code16KEnable"s;
                set_bit_if_key_bool_true(byte, 0, key);
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
        // code 49
        {
            // FLAG 0x00C4
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "code49Enable"s;
                set_bit_if_key_bool_true(byte, 0, key);
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
                set_byte_if_key_uint_byte(byte, key);
            }
            bytes.push_back(byte);
        }
        // --------from 0x00BB to 0x00C6--------------------- //
        set_of_bytes.push_back({0x00BB, std::move(bytes)});
        bytes.clear();
        // --------from 0x00BB to 0x00C6--------------------- //
        // todo add compare if max <= min
        {
            // FLAG 0x0100
            uint8_t byte = 0;
            {
                // Reserved 7-1
            }
            {
                const std::string key = "strExchangeEnable"s;
                bool tmp = str.at(key).as_bool();
                if (tmp)
                    byte |= 0b00000000;
                else
                    byte |= 0b00000001;
            }
            bytes.push_back(byte);
        }
        {
            // FLAG 0x0102-0x0121
            const std::string key = "strExchangeBefore"s;
            std::string tmp = str.at(key).as_string().c_str();
            auto byte = set_bytes_from_symbols(key, 15);
            std::copy(byte.begin(), byte.end(), std::back_inserter(bytes));
        }
        // --------from 0x0100 to 0x0121--------------------- //
        set_of_bytes.push_back({0x0100, std::move(bytes)});
        bytes.clear();
        // --------from 0x0100 to 0x0121--------------------- //
        {
            // FLAG 0x0123-0x142
            const std::string key = "strExchangeAfter"s;
            std::string tmp = str.at(key).as_string().c_str();
            auto byte = set_bytes_from_symbols(key, 15);
            std::copy(byte.begin(), byte.end(), std::back_inserter(bytes));
        }
        // --------from 0x0123 to 0x0142--------------------- //
        set_of_bytes.push_back({0x0123, std::move(bytes)});
        bytes.clear();
        // --------from 0x0123 to 0x0142--------------------- //
        if (!incorrect_data.empty())
        {
            throw incorrect_data;
        }
    }
    catch (boost::system::system_error &e)
    {
        boost::system::error_code ec = e.code();
        logger << ec.value()
               << " "
               << ec.category().name() << " " << ec.message();
        console << EXCEPTION << " "s << LOOK_TO_LOG;
    }
    catch (const std::exception &e)
    {
        console << EXCEPTION << " "s << e.what();
    }
    catch (const std::string &e)
    {
        console << INCORR_DATA << ": " << e;
        logger << INCORR_DATA << ": " << e;
        return {};
    }
    return set_of_bytes;
}

std::string UTIL::get_string_possible_data(const std::vector<std::string> &variants, const std::string &key)
{
    std::string result;
    result += POSS_VALUES;
    result += " "s;
    result += key;
    result += ": "s;
    for (const auto &str : variants)
    {
        result += str;
        result += " "s;
    }
    return result;
}

std::string UTIL::get_bool_possible_data(const std::string &key)
{
    std::string result;
    result += POSS_VALUES;
    result += " "s;
    result += key;
    result += ": true false";
    return result;
}

std::string UTIL::get_uint8_t_possible_data(const std::string &key, const uint8_t from, const int to)
{
    std::string result;
    result += POSS_VALUES;
    result += " "s;
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
        if (*it == '}' && (it + 1) != json.cend())
        {
            merged_json.back() = ',';
            merged_json.push_back('\n');
            it += 2;
        }
        else
            merged_json.push_back(*it);
    }
    json.clear();
    json = merged_json;
}

void UTIL::trim(std::string &str)
{
    const char space = ' ';
    const size_t first(str.find_first_not_of(space));
    if (std::string::npos != first)
    {
        const size_t last(str.find_last_not_of(space));
        str = str.substr(first, (last - first + 1));
    }
}
#ifdef __WIN__
std::string UTIL::get_trimmed_long_HID_path(const std::string &str)
{
    auto b = std::find(str.cbegin(), str.cend(), '#');
    ++b;
    auto e = std::find(b, str.cend(), '#');
    return str.substr(std::distance(str.begin(), b), std::distance(b, e));
}
#endif

std::vector<std::pair<std::string, std::string>> UTIL::get_json_file_list()
{
    std::vector<std::pair<std::string, std::string>> json_files;
    for (const auto &entry : fs::directory_iterator(fs::current_path()))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            std::string file = entry.path().filename().string();
            std::string parse_result = parse_json_file(file);
            json_files.push_back({std::move(file), std::move(parse_result)});
        }
    }
    return json_files;
}

std::vector<std::pair<std::string, int>> UTIL::get_firmware_list()
{
    std::vector<std::pair<std::string, int>> firmware_files;
    for (const auto &entry : fs::directory_iterator(fs::current_path()))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".sig")
        {
            std::string file = entry.path().filename().string();
            int parse_result = firmware_parse_pro(file.data());
            firmware_files.push_back({std::move(file), parse_result});
        }
    }
    return firmware_files;
}

std::string UTIL::parse_json_file(const std::string &source)
{
    boost::json::error_code ec;
    std::ifstream file(source);
    auto value = boost::json::parse(get_string_from_source(file), ec);
    if (ec)
    {
        return PARSE_FAIL + " "s + ERR + ": "s + ec.message();
    }
    else
    {
        return OK;
    }
}

int UTIL::write_settings_from_json(const std::vector<std::pair<uint16_t, std::vector<uint8_t>>> &settings, handler &device)
{
    for (const auto &[flag, bits] : settings)
    {
        uint8_t c[64] = {0};
        SEQ::create_subcommand(flag, bits, c);
        if (-1 == HID_WRITE(device, c, bits.size() + 11))
        {
            return -1;
        }
    }
    return 0;
}

bool UTIL::save_settings_to_files(const std::vector<UTIL::AVAILABLE_HID> &hids)
{
    bool ret = true;
    std::for_each(hids.begin(), hids.end(), [&ret](const auto &hid)
                  {
#ifdef __WIN__
                    logger << "Saving settings from "<<hid.serial_number_<<" "<<UTIL::get_trimmed_long_HID_path(hid.path_);
#elif __DEBIAN__ || __CENTOS__
                    logger << "Saving settings from "<<hid.serial_number_<<" "<<hid.path_;
#endif
                    hid_device *handle = hid_open_path(hid.path_);
                    std::string json = UTIL::get_full_json_response(handle);
                    std::string out_file_name = CONVERT::str(hid.serial_number_) + ".json"s;
                    hid_close(handle);
                    std::ofstream out;
                    out.open(out_file_name);
                    if (out) {
                    out << json;
                    console << out_file_name<<": "s << NEW_FILE;
                    logger << out_file_name<<": "s << NEW_FILE;
                    }                    
                    else {
                        console << NO_NEW_FILE <<": "<<out_file_name;
                        logger << NO_NEW_FILE <<": "<<out_file_name;
                        ret = false;
                    }
                    out.close(); });
    return ret;
}

std::string UTIL::get_string_from_source(std::ifstream &file)
{
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

std::vector<UTIL::AVAILABLE_HID> UTIL::get_scanners_list_by_regex(std::vector<UTIL::AVAILABLE_HID> &hids, const std::string &scanner_numbers)
{
    if (!scanner_numbers.empty())
        if (scanner_numbers[0] == '0')
            return hids;

    std::vector<int> scanner_to_proceed;
    std::vector<int> ints;
    std::vector<std::string> vids;
    const std::regex int_number{"[1-9]+"};
    const std::regex vid_number{"[0x]{1}[A-Fa-f0-9]{4}"};
    while (true)
    {
        for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), int_number}, rEnd;
             rBegin != rEnd;
             ++rBegin)
        {
            ints.push_back(std::stoi(rBegin->str()) - 1);
        }

        for (std::sregex_iterator rBegin{scanner_numbers.begin(), scanner_numbers.end(), vid_number}, rEnd;
             rBegin != rEnd;
             ++rBegin)
            vids.push_back(rBegin->str());

        if (!vids.empty() ||
            !ints.empty())
            break;
    }
    scanner_to_proceed.assign(ints.begin(), ints.end());
    for (const auto &vid : vids)
    {
        for (size_t i = 0; i < hids.size(); ++i)
        {
            if (CONVERT::hex_view(hids[i].vid_) == vid)
            {
                scanner_to_proceed.push_back(i);
            }
        }
    }
    std::sort(scanner_to_proceed.begin(), scanner_to_proceed.end());
    auto u = std::unique(scanner_to_proceed.begin(), scanner_to_proceed.end());
    scanner_to_proceed.erase(u, scanner_to_proceed.end());

    std::vector<UTIL::AVAILABLE_HID> tmp;
    for (const auto i : scanner_to_proceed)
    {
        tmp.push_back(std::move(hids[i]));
    }

    if (!tmp.empty())
    {
        hids.clear();
        hids.assign(tmp.begin(), tmp.end());
    }
    return hids;
}

std::string UTIL::get_com_port(const std::vector<AVAILABLE_COM> &not_scanners, const std::vector<AVAILABLE_COM> &all_com)
{
    auto m = std::mismatch(not_scanners.cbegin(), not_scanners.cend(), all_com.begin(), all_com.end(),
                           [](const auto &lhs, const auto &rhs)
                           {
                               if (0 == lhs.port_.compare(rhs.port_))
                                   return true;
                               else
                                   return false;
                           });
    if (m.second != all_com.cend())
    {
        return (*(m.second)).port_;
    }
    else
        return "";
}