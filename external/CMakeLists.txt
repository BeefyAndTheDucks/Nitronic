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
