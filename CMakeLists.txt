cmake_minimum_required(VERSION 3.10)
project(Nitronic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakeDependentOption)

set(NITRONIC_WITH_DX11 OFF)
set(NITRONIC_WITH_DX12 OFF)
#cmake_dependent_option(NITRONIC_WITH_DX11 "Enable the DX11 version of Nitronic" ON "WIN32 AND MSVC" OFF)
#cmake_dependent_option(NITRONIC_WITH_DX12 "Enable the DX12 version of Nitronic" ON "WIN32 AND MSVC" OFF)
option(NITRONIC_WITH_VULKAN "Enable the Vulkan version of Nitronic" ON)

if (WIN32)
    option(NITRONIC_FORCE_DISCRETE_GPU "Declare symbols to make the OS run the app on discrete GPU on laptops" OFF)
endif()

add_subdirectory(external)

add_subdirectory(engine)

add_subdirectory(editor)

add_subdirectory(sandbox)

# Define paths
set(SHADER_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders")

if (WIN32)
    set(SHADER_SCRIPT_NAME "Compile.bat")
else()
    set(SHADER_SCRIPT_NAME "Compile.sh")
    message(FATAL_ERROR "Linux unsupported so far...")
endif()

# Add the command to run your script
add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/assets/shaders/shaders_compiled.stamp"
        COMMAND ${SHADER_SCRIPT_NAME}
        WORKING_DIRECTORY "${SHADER_SCRIPT_DIR}"
        COMMENT "Running shader compilation script..."
        VERBATIM
)

# Create a target so this actually runs during the build
add_custom_target(compile_shaders_target ALL
        DEPENDS "${CMAKE_BINARY_DIR}/assets/shaders/shaders_compiled.stamp"
)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Syncing assets to build directory..."
)

# Ensure shaders are compiled BEFORE the copy happens (if needed)
add_dependencies(copy_assets compile_shaders_target)

add_dependencies(engine copy_assets)

#file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

