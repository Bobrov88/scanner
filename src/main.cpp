#include <iostream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include "utility.h"

int main() {
    std::cout<<"All HIDs: \n";
    for (const auto& hid : UTIL::get_available_hid_devices()) {
        std::cout<<hid.pid_<<" "<<hid.vid_<<" "<<hid.device_info<<"\n";
    }
    int number;
    std::cout<<"Number: \n";
    std::cin>>number;
    hid_device *handle;
    handle = hid_open(hid[number].pid_, hid[number].vid_, hid[number].serial_number_);
    if (!handle) {
        std::cout<<"Error!";
        hid_exit();
        return 1;
    }
    

}