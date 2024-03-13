#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <memory>
#include <chrono>

using namespace std::string_literals;
using namespace std::chrono_literals;

static std::shared_ptr<spdlog::logger> _logger;

static void logger(const std::string &message, const std::string &scanner = "-")
{
    // todo make it like class
    auto check_logger = spdlog::get("scan_log");
    if (_logger == nullptr && !check_logger)
    {
        char *home = getenv("XDG_CONFIG_HOME");
        if (!home)
        {
            home = getenv("HOME");
        }
        std::string filename = home + "/"s + std::string{getenv("USER")} + "_log"s;
        auto max_size = 1048576 * 2;
        auto max_files = 1;
        _logger = spdlog::rotating_logger_st(scanner, filename, max_size, max_files);
        _logger->set_level(spdlog::level::debug);
        _logger->flush_on(spdlog::level::debug);
        spdlog::flush_every(1s);
    }
    else
    {
        _logger = check_logger;
    }
    _logger->debug(message);
}