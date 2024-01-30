#include <iostream>
#include "utility.h"
#include "commands.h"

int main()
{
    if (hid_init() != 0)
    {
        std::cout << "HIDAPI library cannot be initialized! Reason: ";
        std::cout << UTIL::str(hid_error(NULL));
        return -1;
    }
    auto hids = UTIL::detect_all_hid_linux_devices();
    hid_device *handle = hid_open_path(hids[0].path_);
   

    hid_exit();

    return 0;
}