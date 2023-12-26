#include <vector>
#include <algorithm>
#include <string>
#include <hidapi.h>
#include <filesystem>
#include <utf8.h>

namespace UTIL
{
    struct AVAILABLE_COM
    {
        std::string data_ = "";
        AVAILABLE_COM(std::string data) : data_(data) {}
    };

    struct AVAILABLE_HID
    {
        unsigned short pid_ = 0x0000;
        unsigned short vid_ = 0x0000;
        char *path_;
        wchar_t *serial_number_;
        wchar_t *manufacturer_;
        wchar_t *product_;
        unsigned short release_number_ = 0;
        int interface_ = 0;
        unsigned short usage_ = 0;
        unsigned short usage_page_ = 0;

        AVAILABLE_HID(unsigned short pid,
                      unsigned short vid,
                      char *path,
                      wchar_t *serial_number,
                      wchar_t *manufacturer,
                      wchar_t *product,
                      unsigned short release_number,
                      int interface,
                      unsigned short usage,
                      unsigned short usage_page) : pid_(pid),
                                                   vid_(vid),
                                                   path_(path),
                                                   serial_number_(serial_number),
                                                   manufacturer_(manufacturer),
                                                   product_(product),
                                                   release_number_(release_number),
                                                   interface_(interface),
                                                   usage_(usage),
                                                   usage_page_(usage_page) {}
    };

    std::vector<AVAILABLE_COM> get_available_windows_com_ports();
    std::vector<AVAILABLE_COM> get_available_linux_com_ports();
    std::vector<AVAILABLE_HID> get_available_hid_devices();

    inline std::wstring wstr(const std::string &src);
    inline std::string str(const std::wstring &src);
    uint16_t crc_16(uint8_t *data, uint16_t len);
};
