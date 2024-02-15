#pragma once

#include <vector>
#include <boost/json.hpp>
#include <boost/system/error_code.hpp>
#include <algorithm>
#include <string>
#include <filesystem>
#include <fstream>
#include <utf8.h>
#include "hidapi.h"
#include <iostream>
#include <fstream>
#include "CppConsoleTable.hpp"
#include "commands.h"
#include "commands_sequencies.h"
#include "reconnect.h"
#include <thread>
#include <chrono>
#include "handler.h"

using ConsoleTable = samilton::ConsoleTable;
using namespace std::string_literals;
namespace fs = std::filesystem;

namespace UTIL
{
    struct AVAILABLE_COM
    {
        std::string data_ = "";
        // todo from JSON straight to here
        AVAILABLE_COM(std::string data) : data_(data) {}
    };

    struct AVAILABLE_HID
    {
        char *path_;
        unsigned short vid_ = 0x0000u;
        unsigned short pid_ = 0x0000u;
        wchar_t *serial_number_;
        wchar_t *manufacturer_;
        wchar_t *product_;
        unsigned short release_number_ = 0;
        int interface_ = 0;
        unsigned short usage_ = 0;
        unsigned short usage_page_ = 0;

        AVAILABLE_HID(char *path,
                      unsigned short vid,
                      unsigned short pid,
                      wchar_t *serial_number,
                      wchar_t *manufacturer,
                      wchar_t *product,
                      unsigned short release_number,
                      int interface,
                      unsigned short usage,
                      unsigned short usage_page) : path_(path),
                                                   vid_(vid),
                                                   pid_(pid),
                                                   serial_number_(serial_number),
                                                   manufacturer_(manufacturer),
                                                   product_(product),
                                                   release_number_(release_number),
                                                   interface_(interface),
                                                   usage_(usage),
                                                   usage_page_(usage_page)
        {
        }
    };

    ConsoleTable getTableInitialSetup();

    int HID_WRITE(handler &device, uint8_t *c);
    int HID_SAVE(hid_device *handle);
    int HID_RESTORE(hid_device *handle);
    std::vector<uint8_t> HID_READ(hid_device *handle);

    std::vector<AVAILABLE_COM> get_available_windows_com_ports();
    std::vector<AVAILABLE_COM> get_available_linux_com_ports();
    std::vector<AVAILABLE_HID> get_available_hid_devices();
    std::vector<UTIL::AVAILABLE_HID> list_all_hid();
    void print_all_hid_linux_devices(const std::vector<UTIL::AVAILABLE_HID> &hids);
    void print_all_com_linux_devices(const std::vector<UTIL::AVAILABLE_COM> &coms);
    void print_all_json_files(std::vector<std::pair<std::string, std::string>> &json_list);
    void remove_dublicates_of_hid_devices(std::vector<AVAILABLE_HID> &hids);
    void remove_com_devices_if_not_scanner(std::vector<AVAILABLE_COM> &coms);
    int write_settings_from_json(const std::map<uint16_t, std::vector<uint8_t>>& settings, handler &device);

    std::wstring wstr(const std::string &src);
    std::string str(const std::wstring &src);
    std::string str(const wchar_t *ws);
    std::string hex_view(const unsigned short number);

    std::string convert_from_bytes_to_string(std::vector<uint8_t> &from);
    std::vector<uint8_t> read_json_piece(hid_device *handle);
    std::string read_json_settings(hid_device *handle);
    std::string get_json_responce_for_com_detection(const std::string &com);

    std::string get_firmware_device_name_model(hid_device *handle);
    std::string get_full_json_response(hid_device *handle);

    std::map<uint16_t, std::vector<uint8_t>> convert_json_to_bits(const std::string &json);
    std::string &low(std::string &str);
    std::string get_string_possible_data(const std::vector<std::string> &variants, const std::string &key);
    std::string get_bool_possible_data(const std::string &key);
    std::string get_uint8_t_possible_data(const std::string &key, const uint8_t from, const int to);
    void merge_json(std::string &json);
    void trim(std::string &str);

    std::vector<std::pair<std::string, std::string>> get_json_file_list();
    std::string parse_json_file(const std::string &source);
    std::string read_device_info();
    std::string get_string_from_source(std::ifstream& file);
};