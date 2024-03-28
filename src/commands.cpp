#include "commands.h"

int HID::save_to_internal_flash(handler &device)
{
    logger << "save_to_internal_flash";
    uint8_t c1[64] = {0};
    SEQ::save_to_internal_flash_command(c1);
    if (-1 == UTIL::HID_WRITE(device, c1, 64))
    {
        return -1;
    };
    uint8_t c2[64] = {0};
    SEQ::save_as_custom_flash_command(c2);
    if (-1 == UTIL::HID_WRITE(device, c2, 64))
    {
        return -1;
    }
    return 0;
}

int HID::restore_to_custom_settings(handler &device)
{
    logger << "restore_to_custom_settings";
    uint8_t c[64] = {0};
    SEQ::restore_to_custom_settings(c);
    if (-1 == UTIL::HID_WRITE(device, c, 64))
    {
        return -1;
    };
    return 0;
}

int HID::restore_to_factory_settings(handler &device)
{
    logger << "restore_to_factory_settings";
    uint8_t c[64] = {0};
    SEQ::restore_to_factory_settings_command(c);

    if (-1 == UTIL::HID_WRITE(device, c, 64))
    {
        return -1;
    };
    return 0;
}

bool HID::testing_connect_for_erasing_duplicates(handler &device)
{
    logger << "testing_connect_for_erasing_duplicates";
    uint8_t c[64] = {0};
    SEQ::testing_connect_for_erasing_duplicates_command(c);
    if (-1 == UTIL::HID_WRITE(device, c, 64))
    {
        return false;
    };
    return true;
}

bool HID::testing_to_pass_HID_from_COM(const std::string &com)
{
    logger << "testing_to_pass_HID_from_COM: " << com;
    try
    {
        boost::asio::io_service io;
        boost::asio::serial_port s_port(io);
        s_port.open(com);
        s_port.set_option(boost::asio::serial_port_base::baud_rate(115200));
        uint8_t c[9] = {0};
        SEQ::testing_to_pass_HID_from_COM_command(c);
        boost::asio::write(s_port, boost::asio::buffer(c, 9));
        std::this_thread::sleep_for(300ms);
        char resp[1] = {0};
        boost::posix_time::time_duration m_timeout = boost::posix_time::milliseconds(100);
        boost::asio::deadline_timer m_timer(io);
        boost::system::error_code e;
        int read_result = 0;
        m_timer.expires_from_now(m_timeout);
        m_timer.async_wait([&read_result](const boost::system::error_code &error)
                           {
                if (!error && read_result == 0) read_result = 3; });
        boost::asio::async_read(s_port, boost::asio::buffer(resp, 1),
                                [&read_result](const boost::system::error_code &error, [[maybe_unused]] const size_t transferred)
                                {
                                    if (error)
                                    {
                                        if (error != boost::asio::error::operation_aborted)
                                            read_result = 2;
                                    }
                                    else
                                    {
                                        if (read_result != 0)
                                        {
                                            return;
                                        }
                                        read_result = 1;
                                    }
                                });
        read_result = 0;
        while (read_result != 2)
        {
            io.run_one();
            switch (read_result)
            {
            case 1:
                m_timer.cancel();
                io.reset();
                return true;
            case 3:
                s_port.cancel();
                io.reset();
                return false;
            case 2:
                m_timer.cancel();
                s_port.cancel();
                io.reset();
                break;
            default: // if resultInProgress remain in the loop
                break;
            }
        }
        s_port.close();
    }
    catch (boost::system::system_error &e)
    {
        boost::system::error_code ec = e.code();
        logger << ec.value()
               << " "
               << ec.category().name() << " " << ec.message();
               console << EXCEPTION << " "s << LOOK_TO_LOG;
    }
    catch (const std::exception &ex)
    {
        console << EXCEPTION <<": "<< ex.what();
        logger << EXCEPTION <<": "<< ex.what();
    }
    return false;
}

bool HID::testing_to_pass_COM_from_HID(hid_device* handle) {
    logger << "testing_to_pass_COM_from_HID";
    uint8_t c[64] = {0};
    SEQ::testing_to_pass_COM_from_HID_command(c);
    if (-1 == hid_write(handle, c, 64))
    {
        return false;
    };
    return true;
}