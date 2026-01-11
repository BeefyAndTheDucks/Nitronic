file(GLOB_RECURSE ENGINE_SOURCES src/engine/*.cpp)
add_library(engine ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC include)

if(WIN32)
    target_compile_definitions(engine PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(engine
        PUBLIC glfw
        PUBLIC nvrhi
        PUBLIC glm)

if (NITRONIC_WITH_VULKAN)
    file(GLOB_RECURSE VULKAN_ENGINE_SOURCES src/platform/vulkan/*.cpp)
    target_compile_definitions(engine PUBLIC NITRONIC_WITH_VULKAN)
    target_sources(engine PUBLIC ${VULKAN_ENGINE_SOURCES})

    if (NOT NVRHI_BUILD_SHARED)
        target_link_libraries(engine PUBLIC nvrhi_vk)
    endif()
    if (TARGET Vulkan-Headers)
        target_link_libraries(engine PUBLIC Vulkan-Headers)
    elseif (TARGET Vulkan::Headers)
        target_link_libraries(engine PUBLIC Vulkan::Headers)
    endif()
endif()

if(NITRONIC_WITH_DX11)
    target_compile_definitions(engine PUBLIC NITRONIC_WITH_DX11)
    file(GLOB_RECURSE DX11_ENGINE_SOURCES src/platform/dx11/*.cpp)
    target_sources(engine PUBLIC ${DX11_ENGINE_SOURCES})

    if (NOT NVRHI_BUILD_SHARED)
        target_link_libraries(engine PUBLIC nvrhi_d3d11)
    endif()
    target_link_libraries(engine PUBLIC d3d11 dxgi dxguid)
endif()

if(NITRONIC_WITH_DX12)
    target_compile_definitions(engine PUBLIC NITRONIC_WITH_DX12)
    file(GLOB_RECURSE DX12_ENGINE_SOURCES src/platform/dx12/*.cpp)
    target_sources(engine PUBLIC ${DX12_ENGINE_SOURCES})

    if (NOT NVRHI_BUILD_SHARED)
        target_link_libraries(engine PUBLIC nvrhi_d3d12)
    endif()
    target_link_libraries(engine PUBLIC d3d12 dxgi dxguid)
endif()
