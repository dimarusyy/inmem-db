#pragma once

#define FMT_HEADER_ONLY 1
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logger
{
    struct auto_logger_init
    {
        auto_logger_init()
        {
            spdlog::set_level(spdlog::level::trace);

            auto console = spdlog::stdout_color_st("inmem-db");
            console->set_level(spdlog::level::trace);
        }
    };

    inline auto_logger_init init{};
}