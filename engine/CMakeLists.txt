file(GLOB_RECURSE ENGINE_SOURCES src/engine/*.cpp)
add_library(engine ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC include)

if(WIN32)
    target_compile_definitions(engine PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(engine PUBLIC NITRONIC_WITH_DX11=$<BOOL:${NITRONIC_WITH_DX11}>)
target_compile_definitions(engine PUBLIC NITRONIC_WITH_DX12=$<BOOL:${NITRONIC_WITH_DX12}>)
target_compile_definitions(engine PUBLIC NITRONIC_WITH_VULKAN=$<BOOL:${NITRONIC_WITH_VULKAN}>)

if (NVRHI_WITH_VULKAN)
    if (NOT NVRHI_BUILD_SHARED)
        target_link_libraries(engine PRIVATE nvrhi_vk)
    endif()

    if (TARGET Vulkan-Headers)
        target_link_libraries(engine PRIVATE Vulkan-Headers)
    elseif (TARGET Vulkan::Headers)
        target_link_libraries(engine PRIVATE Vulkan::Headers)
    endif()

endif()

target_link_libraries(engine
        PUBLIC glfw
        PUBLIC nvrhi)

if(NITRONIC_WITH_DX11)
    file(GLOB_RECURSE DX11_ENGINE_SOURCES src/platform/dx11/*.cpp)
    target_sources(engine PRIVATE ${DX11_ENGINE_SOURCES})
    if (NOT NVRHI_BUILD_SHARED)
        target_link_libraries(engine PRIVATE nvrhi_d3d11)
    endif()
    target_link_libraries(engine PRIVATE d3d11 dxgi dxguid)
endif()

if(NITRONIC_WITH_DX12)
    file(GLOB_RECURSE DX12_ENGINE_SOURCES src/platform/dx12/*.cpp)
    target_sources(engine PRIVATE ${DX12_ENGINE_SOURCES})
    if (NOT NVRHI_BUILD_SHARED)
        target_link_libraries(engine PRIVATE nvrhi_d3d12)
    endif()
    target_link_libraries(engine PRIVATE d3d12 dxgi dxguid)
endif()

if(NITRONIC_WITH_VULKAN)
    file(GLOB_RECURSE VULKAN_ENGINE_SOURCES src/platform/vulkan/*.cpp)
    target_sources(engine PRIVATE ${VULKAN_ENGINE_SOURCES})
endif()
