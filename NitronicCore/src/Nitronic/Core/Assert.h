#pragma once

#include "Nitronic/Core/Base.h"
#include "Nitronic/Core/Log.h"
#include <filesystem>

#ifdef N_ENABLE_ASSERTS

#define N_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { N##type##ERROR(msg, __VA_ARGS__); N_DEBUGBREAK(); } }
#define N_INTERNAL_ASSERT_WITH_MSG(type, check, ...) N_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define N_INTERNAL_ASSERT_NO_MSG(type, check) N_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", N_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define N_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define N_INTERNAL_ASSERT_GET_MACRO(...) N_EXPAND_MACRO( N_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, N_INTERNAL_ASSERT_WITH_MSG, N_INTERNAL_ASSERT_NO_MSG) )

#define N_ASSERT(...) N_EXPAND_MACRO( N_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define N_CORE_ASSERT(...) N_EXPAND_MACRO( N_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define N_ASSERT(...)
#define N_CORE_ASSERT(...)
#endif
