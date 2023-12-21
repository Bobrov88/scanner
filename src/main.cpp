#include <iostream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include "utility.h"

int main() {
    std::cout<<"All HIDs: \n";
    std::vector<UTIL::AVAILABLE_HID> response = UTIL::get_available_hid_devices();
    for (const auto& hid : response) {
        std::cout<<hid.pid_<<" "<<hid.vid_<<" "<<hid.product_<<"\n";
    }
    int number;
    std::cout<<"Number: \n";
    std::cin>>number;
    hid_device *handle;
    handle = hid_open(response[number].pid_, response[number].vid_, response[number].serial_number_);
    if (!handle) {
        std::cout<<"Error!";
        hid_exit();
        return 1;
    }
    

}