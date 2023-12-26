#include <iostream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <iomanip>
#include "utility.h"

int main() {
    std::cout<<"All HIDs: \n";
    std::vector<UTIL::AVAILABLE_HID> response = UTIL::get_available_hid_devices();
    for (const auto& hid : response) {
        std::cout<<std::hex<<hid.pid_<<" "<<std::hex<<hid.vid_<<" "<<hid.product_<<"\n";
    }
    int number;
    std::cout<<"Number: \n";
    std::cin>>number;
    hid_device *handle;
    handle = hid_open(response[number].vid_, response[number].pid_, response[number].serial_number_);
    if (!handle) {
        std::cout<<"Error!!!";
        hid_exit();
        return 1;
    } else {
        std::cout<<"Success!\n";
        int result = 0;
        uint8_t ch[9];
        uint8_t buf[255];
        ch[0] = 0x7Eu;
        ch[1] = 0x00u;
        ch[2] = 0x07u;
        ch[3] = 0x01u;
        ch[4] = 0x00u;
        ch[5] = 0x08u;
        ch[6] = 0x09u;
        uint16_t crc16 = UTIL::crc_16(&ch[2], 5);
        ch[7] = crc16 >> 8;
        ch[8] = crc16 & 0x00FF;
        hid_write(handle, &ch[0], 9);

        uint8_t head2[2];
        hid_read(handle, &head2[0], 2);
        uint8_t types[1];
        hid_read(handle, types, 1);
        uint8_t length[1];
        hid_read(handle, length, 1);
        uint8_t data[256];
        hid_read(handle, data, length[0]);
        std::cout<<std::hex<<head2[0]<<" "<<head2[1]<<" "<<types<<" "<<length<<"\n";
        for (int i=0;i<length[0];++i) std::cout<<std::hex<<data[i]<<" ";
        std::cout<<"\n";
    }
    hid_exit();
    return 0;
}