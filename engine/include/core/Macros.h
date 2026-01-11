//
// Created by simon on 02/09/2025.
//

#ifndef NITRONIC_MACROS_H
#define NITRONIC_MACROS_H

#define NAMESPACE namespace Nitronic

#ifdef NITRONIC_WITH_VULKAN
    #define CREATE_BACKEND_FUNCTION_VK(retType, name, ...) retType name##Vk(__VA_ARGS__);
    #define CREATE_BACKEND_CASE_VK(name, ...) \
        case RenderingBackend::Vulkan: \
            return name##Vk(__VA_ARGS__);
#else
    #define CREATE_BACKEND_FUNCTION_VK(retType, name, ...)
    #define CREATE_BACKEND_CASE_VK(name, ...)
#endif

#ifdef NITRONIC_WITH_DX11
    #define CREATE_BACKEND_FUNCTION_DX11(retType, name, ...) retType name##DX11(__VA_ARGS__);
    #define CREATE_BACKEND_CASE_DX11(name, ...) \
        case RenderingBackend::DirectX11: \
            return name##DX11(__VA_ARGS__);
#else
    #define CREATE_BACKEND_FUNCTION_DX11(retType, name, ...)
    #define CREATE_BACKEND_CASE_DX11(name, ...)
#endif

#ifdef NITRONIC_WITH_DX12
    #define CREATE_BACKEND_FUNCTION_DX12(retType, name, ...) retType name##DX12(__VA_ARGS__);
    #define CREATE_BACKEND_CASE_DX12(name, ...) \
        case RenderingBackend::DirectX12: \
            return name##DX12(__VA_ARGS__);
#else
    #define CREATE_BACKEND_FUNCTION_DX12(retType, name, ...)
    #define CREATE_BACKEND_CASE_DX12(name, ...)
#endif

#define CREATE_BACKEND_FUNCTIONS(retType, name, ...) \
    CREATE_BACKEND_FUNCTION_VK(retType, name, __VA_ARGS__) \
    CREATE_BACKEND_FUNCTION_DX11(retType, name, __VA_ARGS__) \
    CREATE_BACKEND_FUNCTION_DX12(retType, name, __VA_ARGS__)

#define CREATE_BACKEND_SWITCH(functionName, ...) \
    ([&]() { \
        switch (m_Backend) { \
            CREATE_BACKEND_CASE_VK(functionName, __VA_ARGS__) \
            CREATE_BACKEND_CASE_DX11(functionName, __VA_ARGS__) \
            CREATE_BACKEND_CASE_DX12(functionName, __VA_ARGS__) \
            default: return decltype(functionName##Vk(__VA_ARGS__)){}; \
        } \
    }())

#endif //NITRONIC_MACROS_H