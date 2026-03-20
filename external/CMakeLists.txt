set(third_party_folder "Third-Party Libraries")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (NOT TARGET glfw)
    add_subdirectory(GLFW)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder})
    if (TARGET update_mappings)
        set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder})
    endif()
endif()

set(NVRHI_WITH_DX11 "${NITRONIC_WITH_DX11}" CACHE BOOL "" FORCE)
set(NVRHI_WITH_DX12 "${NITRONIC_WITH_DX12}" CACHE BOOL "" FORCE)
set(NVRHI_WITH_VULKAN "${NITRONIC_WITH_VULKAN}" CACHE BOOL "" FORCE)

add_subdirectory(nvrhi)

add_subdirectory(glm)


# imgui

if (NOT TARGET imgui)
    file(GLOB IMGUI_SOURCES imgui/*.cpp)

    if (NITRONIC_WITH_DX11)
        list(APPEND IMGUI_SOURCES imgui/backends/imgui_impl_dx11.cpp)
    endif()

    if (NITRONIC_WITH_DX12)
        list(APPEND IMGUI_SOURCES imgui/backends/imgui_impl_dx12.cpp)
    endif()

    if (NITRONIC_WITH_VULKAN)
        list(APPEND IMGUI_SOURCES imgui/backends/imgui_impl_vulkan.cpp)
    endif()

    add_library(imgui STATIC ${IMGUI_SOURCES} imgui/backends/imgui_impl_glfw.cpp)
    target_include_directories(imgui PUBLIC imgui imgui/backends)
    target_link_libraries(imgui PUBLIC glfw)

    if (NITRONIC_WITH_DX11)
        target_link_libraries(imgui PUBLIC d3d11)
    endif()

    if (NITRONIC_WITH_DX12)
        target_link_libraries(imgui PUBLIC d3d12)
    endif()

    if (NITRONIC_WITH_VULKAN)
        find_package(Vulkan REQUIRED)
        if (TARGET Vulkan-Headers)
            target_link_libraries(imgui PUBLIC Vulkan-Headers Vulkan::Vulkan)
        elseif (TARGET Vulkan::Headers)
            target_link_libraries(imgui PUBLIC Vulkan::Headers Vulkan::Vulkan)
        else()
            target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
        endif()
    endif()

    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder})
    if (TARGET update_mappings)
        set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder})
    endif()
endif()