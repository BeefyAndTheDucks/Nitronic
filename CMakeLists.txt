cmake_minimum_required(VERSION 4.0)
project(Nitronic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

add_subdirectory(engine)

add_subdirectory(editor)

add_subdirectory(sandbox)
