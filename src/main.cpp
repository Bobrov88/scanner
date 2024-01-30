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
    std::string res = UTIL::send_command_for_json_response(handle);
    std::cout<<"\n\n\n"<<res<<"\n\n\n";
    boost::json::value str = boost::json::parse(res).as_object();
    auto tmp = str.at("reverse").as_bool();
    if (tmp) std::cout<<"TRUE11";
    else std::cout<<"FALSE11";

    hid_close(handle);
    hid_exit();

    return 0;
}