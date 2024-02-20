#include <iostream>
#include "utility.h"
#include "commands.h"
#include "menu.h"
#include "spdlog/spdlog.h"
#include "fwdlinterface.h"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

boost::asio::io_service io;
std::string com = "/dev/ttyACM1"s;
boost::asio::serial_port s_port(io, com);

int pSend(char *buf, int len)
{
    return boost::asio::write(s_port, boost::asio::buffer(buf, len));
}

int pReceive(char *buf, int len)
{
    return boost::asio::read(s_port, boost::asio::buffer(buf, len));
}

int main(int argc, char *argv[])
{
    // if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    // {
    //     MENU::PrintStartMenu();
    //     return 0;
    // }
    // if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0))
    // {
    //     MENU::PrintAvailableDevices();
    //     return 0;
    // }
    // if (argc == 2 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0))
    // {
    //     MENU::SaveSettings();
    //     return 0;
    // }
    // if (argc == 2 && (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "--write") == 0))
    // {
    //     MENU::WriteFromJson();
    //     return 0;
    // }
    // std::cout << UTIL::read_device_info();
    // uint8_t c[9] = {0};
    // c[0] = 0x7e;
    // c[1] = 0x00;
    // c[2] = 0x08;
    // c[3] = 0x01;
    // c[4] = 0x00;
    // c[5] = 0x00;
    // c[6] = 0xA0;
    // uint16_t crc = SEQ::crc_16(&c[2], 5);
    // c[7] = crc >> 8;
    // c[8] = crc & 0x00ff;
    // try
    // {
    //     boost::asio::serial_port s_port(io, com);
    //     s_port.set_option(boost::asio::serial_port::baud_rate(9600));
    //     boost::asio::write(s_port, boost::asio::buffer(c, 9));
    //     uint8_t res[64] = {0};
    //     auto length = s_port.read_some(boost::asio::buffer(res));
    //     std::cout << CONVERT::to_hex(res, length);
    // }
    // catch (boost::system::system_error &e)
    // {
    //     boost::system::error_code ec = e.code();
    //     std::cerr << ec.value() << '\n';
    //     std::cerr << ec.category().name() << '\n';
    //     std::cerr << ec.message() << '\n';
    // }

    char firmware[50] = "QR_APP_003_BGA65_HModuleFC_QM3503_V1.2.0 copy.sig";
    int fw_parse_result = firmware_parse_pro(firmware);
    if (fw_parse_result == 0)
    {
        std::cout << "Parsing SUCCESS\n";
        while (true)
        {
            int fw_download_start = firmware_download_start(pSend, pReceive, false);
            if (fw_download_start == 0)  std::cout << "Download SUCCESS\n";
            else  {
                std::cout << "Download FAILED\n";
                return 0;
            }

            double persent;
            DownloadState state;

            while (persent < 100)
            {
                get_download_state(&persent, &state);
                std::cout << "\r" << persent << "% -- Status: " << state;
                if (state == DownloadState::SUCCESS || state < DownloadState::SUCCESS)
                {
                    if (state == DownloadState::RECONNECTDEVICE)
                    {
                        //std::cout << "\n Reconnecting";
                        s_port.close();
                        //std::cout << "\n Closed";
                        std::this_thread::sleep_for(500ms);
                        //std::cout << "\n Sleeped";
                        com = RECONNECT::com_reconnect("F23450001"s);
                        s_port.open(com);
                        //std::cout << "\n Reconnected";
                        break;
                    }
                    else {
                        s_port.close();
                        return 0;
                    }
                }
            }
        }
    }
    else
        std::cout << "Parsing FAILED\n";
    return 0;
}