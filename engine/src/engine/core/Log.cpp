//
// Created by simon on 23/03/2026.
//

#include "engine/Log.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

NAMESPACE
{

    std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
    std::shared_ptr<spdlog::logger> Log::s_AppLogger;

    void Log::Init()
    {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Nitronic.log"));

        sinks[0]->set_pattern("%^[%T] %n: %v%$");
        sinks[1]->set_pattern("[%T] [%l] %n: %v");

        s_EngineLogger = std::make_shared<spdlog::logger>("Nitronic", begin(sinks), end(sinks));
        spdlog::register_logger(s_EngineLogger);
        s_EngineLogger->set_level(spdlog::level::trace);
        s_EngineLogger->flush_on(spdlog::level::trace);

        s_AppLogger = std::make_shared<spdlog::logger>("App", begin(sinks), end(sinks));
        spdlog::register_logger(s_AppLogger);
        s_AppLogger->set_level(spdlog::level::trace);
        s_AppLogger->flush_on(spdlog::level::trace);
    }
}
