#include <iostream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <iomanip>
#include <bitset>
#include "utility.h"

std::string to_hex(char *response, size_t size)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < size; i++)
        oss << std::setw(2) << static_cast<unsigned int>(response[i]) << "\n";
    return oss.str();
}

int main()
{

    // libusb_init(NULL);

    // std::cout<<"Init\n";

    // libusb_device_handle* handle = libusb_open_device_with_vid_pid(NULL, 0x34EB, 0x1505);
    // uint8_t ch[64] = {0};
    // ch[0] = 0xfd;
    // ch[1] = 0x09;
    // ch[2] = 0xff;
    // ch[3] = 0x7e;
    // ch[4] = 0x00;
    // ch[5] = 0x08;
    // ch[6] = 0x01;
    // ch[7] = 0x00;
    // ch[8] = 0x00;
    // ch[9] = 0xA0;
    // uint16_t crc = UTIL::crc_16(&ch[5], 5);
    // ch[10] = crc >> 8;
    // ch[11] = crc & 0x00FF;
    // int transferred = 0;
    // int result = libusb_bulk_transfer(handle, 0x82, ch, 64, &transferred, 0u);
    // switch(result) {
    //     case 0: std::cout<<"Success\n"; break;
    //     case LIBUSB_ERROR_TIMEOUT: std::cout<<"LIBUSB_ERROR_TIMEOUT\n";break;
    //     case LIBUSB_ERROR_PIPE: std::cout<<"LIBUSB_ERROR_PIPE\n";break;
    //     case LIBUSB_ERROR_OVERFLOW: std::cout<<"LIBUSB_ERROR_OVERFLOW\n";break;
    //     case LIBUSB_ERROR_NO_DEVICE: std::cout<<"LIBUSB_ERROR_NO_DEVICE\n";break;
    //     case LIBUSB_ERROR_BUSY: std::cout<<"LIBUSB_ERROR_BUSY\n";break;
    //     case LIBUSB_ERROR_INVALID_PARAM: std::cout<<"LIBUSB_ERROR_INVALID_PARAM\n";break;
    //     default: std::cout<<"Something other\n"; break;
    // }
    // std::cout<<"Transferred "<<transferred<<"\n";
    // libusb_exit(NULL);
    // std::cout<<"Deinit\n";

    // boost::asio::io_service io;
    // boost::asio::serial_port serial(io);
    // serial.open("/dev/hidraw2");
    // serial.set_option(boost::asio::serial_port_base::baud_rate(57600));
    // {
    //     std::vector<uint8_t> data = {0x7e, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0xAB, 0xCD};
    //     boost::asio::write(serial, boost::asio::buffer(data, data.size()));
    //     char response[7] = {0};
    //     int size = boost::asio::read(serial, boost::asio::buffer(response, 7));
    //     std::cout << "\nRead: " << size << "\n";
    //     std::cout << to_hex(response, size);
    // }
    // serial.close();

    std::vector<UTIL::AVAILABLE_HID> response = UTIL::get_available_hid_devices();
    for (const auto &hid : response)
    {
        std::cout << std::hex << hid.vid_ << " " << std::hex << hid.pid_ << " " << hid.path_ << "\n";
    }
    // std::vector<UTIL::AVAILABLE_COM> com_response = UTIL::get_available_linux_com_ports();
    // for (const auto &com : com_response)
    // {
    // std::cout << std::hex << com.data_ << "\n";
    // }
    // int number;
    // std::cout << "Number: \n";
    // std::cin >> number;
    // int a;
    // std::cin >> a;
    std::cout << std::dec;
    // hid_device *handle1;
    // handle1 = hid_open_path(response[1].path_);
    // if (!handle1)
    // {
    //     std::cout << "Error handle1!!!\t";
    //     std::cout << hid_error(handle1);
    //     hid_close(handle1);
    //     hid_exit();
    //     return 1;
    // }
    // std::cin >> a;
    // std::cout << 103 << std::endl;
    // wchar_t *wresponce;
    // hid_get_manufacturer_string(handle1, wresponce, 255);
    // printf("Manufacturer String: %ls\n", wresponce);
    // std::cout << 107 << std::endl;
    // hid_get_product_string(handle1, wresponce, 255);
    // printf("Product String: %ls\n", wresponce);
    // std::cout << 110 << std::endl;
    // hid_get_serial_number_string(handle1, wresponce, 255);
    // printf("Serial Number String: (%d) %ls\n", wresponce[0], wresponce);
    // std::cin >> a;
    // hid_get_indexed_string(handle1, 1, wresponce, 255);
    // printf("Indexed String 1: %ls\n", wresponce);
    // hid_close(handle1);
    // hid_exit();
    // std::cin >> a;

    hid_init();

    hid_device *handle2;
    handle2 = hid_open_path(response[2].path_);
    if (!handle2)
    {
        std::cout << "Error handle2!!!\t";
        std::cout << hid_error(handle2);
        hid_close(handle2);
        hid_exit();
        return 1;
    }

    // wchar_t *wresponce;
    // hid_get_manufacturer_string(handle2, wresponce, 255);
    // printf("Manufacturer String: %ls\n", wresponce);
    // hid_get_product_string(handle2, wresponce, 255);
    // printf("Product String: %ls\n", wresponce);
    // hid_get_serial_number_string(handle2, wresponce, 255);
    // printf("Serial Number String: (%d) %ls\n", wresponce[0], wresponce);
    // hid_get_indexed_string(handle2, 1, wresponce, 255);
    // printf("Indexed String 1: %ls\n", wresponce);

    int result = 0;
    uint8_t ch[64] = {0};
    ch[0] = 0xfd;
    ch[1] = 0x0C;
    ch[2] = 0xff;
    ch[3] = 0x47;
    ch[4] = 0x65;
    ch[5] = 0x74;
    ch[6] = 0x43;
    ch[7] = 0x6F;
    ch[8] = 0x6E;
    ch[9] = 0x66;
    ch[10] = 0x69;
    ch[11] = 0x67;
    ch[12] = 0x30;
    ch[13] = 0x31;
    ch[14] = 0x2E;
    std::cout << "Result of hid_write: ";
    printf("%d \n", hid_write(handle2, ch, 64));
    std::cout << "Error: " << hid_error(handle2) << std::endl;
    uint8_t from_scanner[64] = {0};
    int f = hid_read(handle2, from_scanner, 64);
    std::cout << "\n Read " << f;
    std::cout << "\n";
    for (int i = 0; i < 64; ++i)
    {
        std::cout << std::bitset<8>(from_scanner[i]) << " ";
    }
    hid_close(handle2);
    hid_exit();
    return 0;
}