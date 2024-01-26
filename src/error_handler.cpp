#include "error_handler.h"

int check_operation_result(FUNC func, const int err_code, hid_device *handle, uint8_t *r)
{
    if (func == FUNC::SAVE_TO_INTERNAL_FLASH)
    {
        if (err_code == -1)
        {
            std::cout << "\nSave to internal flash failed. ";
            std::cout << UTIL::str(hid_error(handle));
            return -1;
        }
        if (err_code == 64)
        {
            std::cout << "\nSave to internal flash: OK! ";
            return 0;
        }
        if (err_code < 64)
        {
            std::cout << "\nThe device received fewer bytes than it sent. ";
            if (std::string s = UTIL::str(hid_error(handle)); s.empty())
            {
                std::cout << "No errors";
                return 0;
            }
            else
            {
                std::cout << s;
                return -1;
            }
        }
        else
        {
            std::cout << "\nUnknown error! ";
            std::cout << UTIL::str(hid_error(handle));
            return -1;
        }
    }
    if (func == FUNC::READ_AS_RESPONSE)
    {
        if (err_code == -1)
        {
            std::cout << "\nReading result failed. ";
            std::cout << UTIL::str(hid_error(handle));
            return -1;
        }
        if (err_code == 0)
        {
            std::cout << "\nNo available packets to read. ";
            if (std::string s = UTIL::str(hid_error(handle)); s.empty())
            {
                std::cout << "No errors";
                return 0;
            }
            else
            {
                std::cout << s;
                return -1;
            }
        }
        if (err_code < 64)
        {
            std::cout << "\nResponse is not complete. ";
            if (std::string s = UTIL::str(hid_error(handle)); s.empty())
            {
                std::cout << "No errors";
                return 0;
            }
            else
            {
                std::cout << s;
                return -1;
            }
        }
        if (err_code == 64)
        {
            if (r[5] == 0x02 &&
                r[6] == 0x00 &&
                r[7] == 0x00 &&
                r[8] == 0x01 &&
                r[9] == 0x00 &&
                r[10] == 0x33 &&
                r[11] == 0x31)
            {
                return 0;
            }
            else
            {
                std::cout << "\nIncorrect response bytes";
                return -1;
            }
        }
        else
        {
            std::cout << "\nUnknown error! ";
            std::cout << UTIL::str(hid_error(handle));
            return -1;
        }
    }
    return -1;
}