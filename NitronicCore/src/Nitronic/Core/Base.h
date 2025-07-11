#pragma once

#include "Nitronic/Core/PlatformDetection.h"

#ifdef N_DEBUG
#if defined(N_PLATFORM_WINDOWS)
#define N_DEBUGBREAK() __debugbreak()
#elif defined(N_PLATFORM_LINUX)
#include <signal.h>
#define N_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define N_ENABLE_ASSERTS
#else
#define N_DEBUGBREAK()
#endif

#define N_EXPAND_MACRO(x) x
#define N_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#include "Nitronic/Core/Log.h"
#include "Nitronic/Core/Assert.h"
