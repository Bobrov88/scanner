#include "logger.h"

BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, lg_type)
{
    lg_type lg;

    typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink;
    boost::shared_ptr<file_sink> f_sink(new file_sink(
        keywords::file_name = "scan_log.log",
        keywords::open_mode = std::ios_base::app,
        keywords::auto_flush = true,
        keywords::max_size = 10 * 1024 * 1024));
    f_sink->set_filter(expr::attr<int>("Severity") == 1);
    logging::formatter formatter = expr::stream << "["
                                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%d-%m-%Y %H:%M:%S.%f")
                                                << "] > "
                                                << expr::smessage;
    f_sink->set_formatter(formatter);
    logging::core::get()->add_sink(f_sink);

    boost::shared_ptr<sinks::synchronous_sink<sinks::text_ostream_backend>> c_sink = logging::add_console_log();
    c_sink->set_filter(expr::attr<int>("Severity") == 0);
    c_sink->locked_backend()->auto_flush(true);
    logging::core::get()->add_sink(c_sink);

    logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());

    return lg;
}