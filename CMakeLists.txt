cmake_minimum_required(VERSION 3.10)
project(Nitronic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakeDependentOption)

#set(NITRONIC_WITH_DX11 OFF)
#set(NITRONIC_WITH_DX12 OFF)
cmake_dependent_option(NITRONIC_WITH_DX11 "Enable the DX11 version of Nitronic" ON "WIN32 AND MSVC" OFF)
cmake_dependent_option(NITRONIC_WITH_DX12 "Enable the DX12 version of Nitronic" ON "WIN32 AND MSVC" OFF)
option(NITRONIC_WITH_VULKAN "Enable the Vulkan version of Nitronic" ON)

message("-- Nitronic Graphics APIs: Vulkan - ${NITRONIC_WITH_VULKAN}, DX12 - ${NITRONIC_WITH_DX12}, DX11 - ${NITRONIC_WITH_DX11}")

add_subdirectory(external)

add_subdirectory(engine)

add_subdirectory(editor)

add_subdirectory(sandbox)

# Define paths
set(SHADERS_DIR "shaders")
set(ASSETS_DIR "assets")

set(ALL_SHADER_OUTPUTS "")
function(add_slang_shader SHADER_NAME STAGE ENTRY_POINT)
    set(SLANG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${SHADERS_DIR}/${SHADER_NAME}.slang")
    set(SPIRV_OUT "${CMAKE_BINARY_DIR}/${SHADERS_DIR}/${SHADER_NAME}.${STAGE}.spv")
    set(DXIL_OUT "${CMAKE_BINARY_DIR}/${SHADERS_DIR}/${SHADER_NAME}.${STAGE}.dxil")

    # Get shifts from nvrhi.h in the VulkanBindingOffsets structure.
    add_custom_command(
            OUTPUT "${SPIRV_OUT}" "${DXIL_OUT}"
            COMMAND slangc "${SLANG_SRC}"
                    -target spirv -o "${SPIRV_OUT}"
                    -target dxil -o "${DXIL_OUT}"
                    -profile sm_6_0
                    -stage "${STAGE}"
                    -entry "${ENTRY_POINT}"
                    -I "${SHADERS_DIR}/include"
                    -matrix-layout-column-major
                    -fvk-t-shift 0 0
                    -fvk-s-shift 128 0
                    -fvk-b-shift 256 0
                    -fvk-u-shift 384 0
            DEPENDS "${SLANG_SRC}"
            VERBATIM
            COMMENT "Building Shader: ${SHADER_NAME} [${ENTRY_POINT}]"
    )

    set(ALL_SHADER_OUTPUTS ${ALL_SHADER_OUTPUTS} "${SPIRV_OUT}" "${DXIL_OUT}" PARENT_SCOPE)
endfunction()

add_slang_shader(Basic vertex vertexMain)
add_slang_shader(Basic fragment fragmentMain)

set(CLEANUP_SCRIPT "${CMAKE_BINARY_DIR}/cleanup_shaders.cmake")
file(WRITE "${CLEANUP_SCRIPT}" "
    file(GLOB_RECURSE EXISTING_BINARIES \"${CMAKE_BINARY_DIR}/${SHADERS_DIR}/*.spv\" \"${CMAKE_BINARY_DIR}/${SHADERS_DIR}/*.dxil\")
    set(TRACKED_BINARIES \"${ALL_SHADER_OUTPUTS}\")
    foreach(FILE IN LISTS EXISTING_BINARIES)
        # Convert path to CMake style for reliable comparison
        file(TO_CMAKE_PATH \"\${FILE}\" FILE_PATH)
        if (NOT FILE_PATH IN_LIST TRACKED_BINARIES)
            message(STATUS \"Removing orphaned shader binary: \${FILE_PATH}\")
            file(REMOVE \"\${FILE_PATH}\")
        endif()
    endforeach()
")

add_custom_target(
        compile_shaders ALL DEPENDS ${ALL_SHADER_OUTPUTS}
        COMMAND ${CMAKE_COMMAND} -P "${CLEANUP_SCRIPT}" # Remove shaders with no source file
        COMMENT "Building Shaders..."
)

add_dependencies(engine compile_shaders)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/${ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/${ASSETS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_DIR}"
        "${CMAKE_BINARY_DIR}/${ASSETS_DIR}"
        COMMENT "Syncing assets to build directory..."
)

add_dependencies(compile_shaders copy_assets)
add_dependencies(engine copy_assets)

#file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

