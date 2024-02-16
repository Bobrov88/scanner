#include "converters.h"

std::string CONVERT::str(const std::wstring &src)
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

std::string CONVERT::str(const wchar_t *ws)
{
    std::wstring w(ws);
    return str(w);
}

std::wstring CONVERT::wstr(const std::string &src)
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

std::string CONVERT::convert_from_bytes_to_string(const std::vector<uint8_t> &from)
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

std::string CONVERT::hex_view(const unsigned short number)
{
    std::stringstream str_hex;
    str_hex << "0x" << std::setfill('0') << std::setw(4) << std::hex << number;
    return str_hex.str();
}


std::string &CONVERT::low(std::string &str)
{
    std::for_each(str.begin(),
                  str.end(),
                  [](auto &ch)
                  { ch = std::tolower(ch); });
    return str;
}

std::string CONVERT::to_hex(uint8_t *response, size_t size)
{
    std::ostringstream oss;
    for (int i = 0; i < size; i++)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(response[i]) << " ";
    return oss.str();
}

std::string CONVERT::to_hex(const std::vector<uint8_t>& bytes)
{
    std::ostringstream oss;
    for (const auto byte : bytes)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    return oss.str();
}