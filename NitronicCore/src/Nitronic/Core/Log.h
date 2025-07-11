#pragma once

#include "Nitronic/Core/Base.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Nitronic {

	class Log {
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define N_CORE_TRACE(...)    ::Nitronic::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define N_CORE_INFO(...)     ::Nitronic::Log::GetCoreLogger()->info(__VA_ARGS__)
#define N_CORE_WARN(...)     ::Nitronic::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define N_CORE_ERROR(...)    ::Nitronic::Log::GetCoreLogger()->error(__VA_ARGS__)
#define N_CORE_CRITICAL(...) ::Nitronic::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define N_TRACE(...)         ::Nitronic::Log::GetClientLogger()->trace(__VA_ARGS__)
#define N_INFO(...)          ::Nitronic::Log::GetClientLogger()->info(__VA_ARGS__)
#define N_WARN(...)          ::Nitronic::Log::GetClientLogger()->warn(__VA_ARGS__)
#define N_ERROR(...)         ::Nitronic::Log::GetClientLogger()->error(__VA_ARGS__)
#define N_CRITICAL(...)      ::Nitronic::Log::GetClientLogger()->critical(__VA_ARGS__)
