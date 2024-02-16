#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <utf8.h>
#include <algorithm>
#include <iomanip>

using namespace std::string_literals;

namespace CONVERT
{
    std::string str(const std::wstring &src);
    std::string str(const wchar_t *ws);
    std::wstring wstr(const std::string &src);
    std::string convert_from_bytes_to_string(const std::vector<uint8_t> &from);
    std::string hex_view(const unsigned short number);
    std::string &low(std::string &str);
    std::string to_hex(uint8_t *response, size_t size);
    std::string to_hex(const std::vector<uint8_t> &bytes);
}