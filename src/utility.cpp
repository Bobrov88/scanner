#include "utility.h"
// #include "windows.h"
// #include "tchar.h"

/*std::vector<UTIL::AVAILABLE_COM> UTIL::get_available_windows_com_ports()
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
}*/

std::vector<UTIL::AVAILABLE_COM> UTIL::get_available_linux_com_ports()
{
    namespace fs = std::filesystem;
    std::vector<UTIL::AVAILABLE_COM> com_ports;
    fs::path p("/dev/serial/by-id");
    try
    {
        if (!exists(p))
        {
            throw std::runtime_error(p.generic_string() + " does not exist");
        }
        else
        {
            for (const auto &dir : fs::directory_iterator(p))
            {
                if (is_symlink(dir.symlink_status()))
                {
                    fs::path symlink_points_at = read_symlink(dir);
                    fs::path canonical_path = fs::canonical(p / symlink_points_at);
                    com_ports.push_back({canonical_path.generic_string()});
                }
            }
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        // std::cout << ex.what() << '\n';
        throw ex;
    }
    std::sort(com_ports.begin(), com_ports.end(), [](const auto &first, const auto &second)
              { return first.data_ < second.data_; });
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
    return device;
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

uint16_t UTIL::crc_16(uint8_t *data, uint16_t len)
{
    uint16_t crc16 = 0x0000;
    while (len--)
    {
        for (uint8_t i = 0x80; i != 0; i >>= 1)
        {
            if ((crc16 & 0x8000) != 0)
            {
                crc16 = crc16 << 1;
                crc16 = crc16 ^ 0x1021;
            }
            else
            {
                crc16 = crc16 << 1;
            }
            if ((*data & i) != 0)
            {
                crc16 = crc16 ^ 0x1021; // crc16 = crc16 ^ (0x10000 ^ 0x11021)
            }
        }
        data++;
    }
    return crc16;
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

 std::string UTIL::send_command_for_json_response(hid_device *handle) {
    uint8_t ch[64] = {0};
    ch[0] = 0xfd;
    ch[1] = 0x0C;
    ch[2] = 0xff;
    ch[3] = 0x47;
    ch[4] = 0x65;
    ch[5] = 0x74;
    ch[6] = 0x43;
    ch[7] = 0x6F;
    ch[8] = 0x6E;
    ch[9] = 0x66;
    ch[10] = 0x69;
    ch[11] = 0x67;
    ch[12] = 0x30;
    ch[13] = 0x31;
    ch[14] = 0x2E;
    // TODO if result is negative, not all bytes send
    hid_write(handle, ch, 64);
    std::cout<<"GetConfig01.\n";
    std::string result1 = read_json_settings(handle);

    ch[13] = 0x32;
    hid_write(handle, ch, 64);
    std::cout<<"GetConfig02.\n";
    std::string result2 = read_json_settings(handle);
    
    ch[13] = 0x33;
    hid_write(handle, ch, 64);
    std::cout<<"GetConfig03.\n";
    std::string result3 = read_json_settings(handle);

    ch[13] = 0x34;
    hid_write(handle, ch, 64);
    std::cout<<"GetConfig04.\n";
    std::string result4 = read_json_settings(handle);

    return result1 + result2 + result3 + result4;
 }

 std::string UTIL::read_json_settings(hid_device *handle) {
    std::vector<uint8_t> result;
    while(true) {
        std::vector<uint8_t> tmp = read_json_piece(handle);
        if (tmp.empty()) break;
        result.insert(result.end(), tmp.begin(), tmp.end());
    }
    return convert_from_bytes_to_string(result);
 }