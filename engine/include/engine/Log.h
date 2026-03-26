//
// Created by simon on 23/03/2026.
//

#ifndef NITRONIC_LOG_H
#define NITRONIC_LOG_H

#include <filesystem>
#include <platform_folders.h>
#include <spdlog/spdlog.h>

#include "core/Macros.h"

NAMESPACE {

    class Log
    {
    public:
        static void Init();

        static std::filesystem::path GetLogDirectory() {
            auto logDir = std::filesystem::path(sago::getDataHome()) / "Nitronic" / "Logs";
            std::filesystem::create_directories(logDir);
            return logDir;
        }

        [[nodiscard]] static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
        [[nodiscard]] static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;
        static std::shared_ptr<spdlog::logger> s_AppLogger;
    };

}

#define ENGINE_TRACE(...) ::NAMESPACE_NAME::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) ::NAMESPACE_NAME::Log::GetEngineLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::NAMESPACE_NAME::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::NAMESPACE_NAME::Log::GetEngineLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) ::NAMESPACE_NAME::Log::GetEngineLogger()->critical(__VA_ARGS__)

#define ENGINE_ABORT(...)                                    \
    do {                                                     \
        ENGINE_CRITICAL(__VA_ARGS__);                        \
        ::NAMESPACE_NAME::Log::GetEngineLogger()->flush();   \
        std::abort();                                        \
    } while(0)

#define ENGINE_ASSERT(cond, ...)                                 \
    do {                                                         \
        if (!(cond)) {                                           \
            ENGINE_CRITICAL("Assertion '{}' failed ({}:{}): {}", \
                #cond, __FILE__, __LINE__,                       \
                fmt::format(__VA_ARGS__));                       \
            ::NAMESPACE_NAME::Log::GetEngineLogger()->flush();   \
            std::abort();                                        \
        }                                                        \
    } while(0)

#define APP_TRACE(...) ::NAMESPACE_NAME::Log::GetAppLogger()->trace(__VA_ARGS__)
#define APP_INFO(...) ::NAMESPACE_NAME::Log::GetAppLogger()->info(__VA_ARGS__)
#define APP_WARN(...) ::NAMESPACE_NAME::Log::GetAppLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...) ::NAMESPACE_NAME::Log::GetAppLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...) ::NAMESPACE_NAME::Log::GetAppLogger()->critical(__VA_ARGS__)

#define APP_ABORT(...)                                       \
    do {                                                     \
        APP_CRITICAL(__VA_ARGS__);                           \
        ::NAMESPACE_NAME::Log::GetAppLogger()->flush();      \
        std::abort();                                        \
    } while(0)

#define APP_ASSERT(cond, ...)                                 \
    do {                                                      \
        if (!(cond)) {                                        \
            APP_CRITICAL("Assertion '{}' failed ({}:{}): {}", \
                #cond, __FILE__, __LINE__,                    \
                fmt::format(__VA_ARGS__));                    \
            ::NAMESPACE_NAME::Log::GetAppLogger()->flush();   \
            std::abort();                                     \
        }                                                     \
    } while(0)

#endif //NITRONIC_LOG_H
