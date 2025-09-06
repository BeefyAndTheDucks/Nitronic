cmake_minimum_required(VERSION 3.10)
project(Nitronic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakeDependentOption)

cmake_dependent_option(NITRONIC_WITH_DX11 "Enable the DX11 version of Nitronic" ON "WIN32 AND MSVC" OFF)
cmake_dependent_option(NITRONIC_WITH_DX12 "Enable the DX12 version of Nitronic" ON "WIN32 AND MSVC" OFF)
option(NITRONIC_WITH_VULKAN "Enable the Vulkan version of Donut" ON)

if (WIN32)
    option(NITRONIC_FORCE_DISCRETE_GPU "Declare symbols to make the OS run the app on discrete GPU on laptops" OFF)
endif()

add_subdirectory(external)

add_subdirectory(engine)

add_subdirectory(editor)

add_subdirectory(sandbox)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

